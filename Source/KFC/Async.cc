#include "KFC/Async.h"
#include "KFC/Memory.h"
#include "KFC/Mutex.h"
#include "KFC/Preclude.h"
#include "KFC/Trace.h"
#include <utility>

namespace KFC {

#define KFC_ALREADY_READY reinterpret_cast<KFC::_::Event *>(114514)

static __thread EventLoop *threadLocalEventLoop;

EventLoop::EventLoop()
    : m_current(nullptr), m_head(nullptr), m_tail(&m_head), m_depthFirstInsertPoint(&m_head),
      m_breadthFirstInsertPoint(&m_head), m_port(None), m_executor(None) {}
EventLoop::EventLoop(EventPort &poller)
    : m_current(nullptr), m_head(nullptr), m_tail(&m_head), m_depthFirstInsertPoint(&m_head),
      m_breadthFirstInsertPoint(&m_head), m_port(poller), m_executor(None) {}

EventLoop &EventLoop::current() {
  EventLoop *runLoop = threadLocalEventLoop;
  KFC_CHECK(runLoop, "No EventLoop in current thread");
  return *runLoop;
}

void EventLoop::enter() const {
  KFC_CHECK(threadLocalEventLoop == nullptr, "This thread already has a EventLoop");
  threadLocalEventLoop = const_cast<EventLoop *>(this);
}

void EventLoop::leave() const {
  KFC_CHECK(threadLocalEventLoop == this, "EventLoop leaves a different thread than it enters");
  threadLocalEventLoop = nullptr;
}

const Executor &EventLoop::getExecutor() {
  KFC_IF_SOME_CONST(e, m_executor) { return *e; }
  return *m_executor.emplace(Executor::create(*this));
}

bool EventLoop::poll(const Option<Duration> &timeout) {
  KFC_IF_SOME(p, m_port) {
    // Poll the poller if there is one.
    if (p.poll(timeout)) {
      // Another thread woke up the poller, check for cross-thread events.
      KFC_IF_SOME_CONST(e, m_executor) { (*e).poll(None); }
    } else {
      // Timeout.
      return false;
    }
  }
  else KFC_IF_SOME_CONST(e, m_executor) {
    // No poller, try the executor.
    (*e).poll(timeout);
  }
  else {
    KFC_FATAL("Neither a poller nor an executor is set for the EventLoop");
  }
  // Woken up.
  return true;
}

bool EventLoop::turn() {
  _::Event *event = m_head;
  if (!event) {
    // No event in the queue.
    return false;
  }

  m_head = event->m_next;
  if (m_head) {
    // The next event is not null, update its prev pointer.
    // That's to say, there is more than one event in the queue.
    m_head->m_prev = &m_head;
  }

  if (m_tail == &event->m_next) {
    // The tail is the next pointer of the current event, update it.
    // That's to say, we are removing the last event from the queue.
    m_tail = &m_head;
  }

  // Reset the depth-first insert point.
  m_depthFirstInsertPoint = &m_head;
  if (m_breadthFirstInsertPoint == &event->m_next) {
    // The breadth-first insert point is the next pointer of the current event, update it.
    // That's to say, we are removing the last event from the queue.
    m_breadthFirstInsertPoint = &m_head;
  }

  event->m_next = nullptr;
  event->m_prev = nullptr;

  // Fire the event.
  m_current = event;
  event->m_firing = true;
  event->fire();
  event->m_firing = false;
  m_current = nullptr;

  // Reset the depth-first insert point again because the event might have armed more events.
  m_depthFirstInsertPoint = &m_head;
  return true;
}

namespace _ {
Event::Event() : Event(EventLoop::current()) {}

Event::Event(EventLoop &loop)
    : m_loop(loop), m_prev(nullptr), m_next(nullptr), m_firing(false),
      m_alive(kEventLivenessMagic) {}

Event::~Event() noexcept(false) {
  m_alive = 0;
  // Make sure `m_alive = 0` won't be optimized away.
  std::atomic_signal_fence(std::memory_order_acq_rel);
  disarm();
}

void Event::disarm() {
  KFC_CHECK(&m_loop == threadLocalEventLoop,
            "Disarming an event from a different thread than it was armed");

  if (!m_prev) {
    // Already disarmed.
    return;
  }

  if (m_loop.m_tail == &m_next) {
    // The tail is the next pointer of the current event, update it.
    // That's to say, we are removing the last event from the queue.
    m_loop.m_tail = m_prev;
  }

  if (m_loop.m_depthFirstInsertPoint == &m_next) {
    // The depth-first insert point is the next pointer of the current event, update it.
    // That's to say, we are removing the last event from the queue.
    m_loop.m_depthFirstInsertPoint = m_prev;
  }

  if (m_loop.m_breadthFirstInsertPoint == &m_next) {
    // The breadth-first insert point is the next pointer of the current event, update it.
    // That's to say, we are removing the last event from the queue.
    m_loop.m_breadthFirstInsertPoint = m_prev;
  }

  *m_prev = m_next;
  if (m_next) {
    // The next event is not null, update its prev pointer.
    // That's to say, there is more than one event in the queue.
    m_next->m_prev = m_prev;
  }

  m_prev = nullptr;
  m_next = nullptr;
}

void Event::armDepthFirst() {
  KFC_CHECK(&m_loop == threadLocalEventLoop,
            "Arming an event from a different thread than it was created in");
  KFC_CHECK(m_alive == kEventLivenessMagic, "Arming a destroyed event");

  if (m_prev) {
    // Already armed.
    return;
  }

  // Link the current event to its next event.
  m_next = *m_loop.m_depthFirstInsertPoint;
  if (m_next) {
    // The next event is not null, update its prev pointer.
    // That's to say, there is more than one event in the queue.
    m_next->m_prev = &m_next;
  }

  // Link the current event to its previous event.
  m_prev = m_loop.m_depthFirstInsertPoint;
  *m_prev = this;

  // Update the depth-first insert point.
  m_loop.m_depthFirstInsertPoint = &m_next;

  if (m_loop.m_breadthFirstInsertPoint == m_prev) {
    // The breadth-first insert point is the previous pointer of the current event, update it.
    // That's to say, we are inserting the first event into the queue.
    m_loop.m_breadthFirstInsertPoint = &m_next;
  }

  if (m_loop.m_tail == m_prev) {
    // The tail is the previous pointer of the current event, update it.
    // That's to say, we are inserting the first event into the queue.
    m_loop.m_tail = &m_next;
  }
}

void Event::armBreadthFirst() {
  KFC_CHECK(&m_loop == threadLocalEventLoop,
            "Arming an event from a different thread than it was created in");
  KFC_CHECK(m_alive == kEventLivenessMagic, "Arming a destroyed event");

  if (m_prev) {
    // Already armed.
    return;
  }

  // Link the current event to its next event.
  m_next = *m_loop.m_breadthFirstInsertPoint;
  if (m_next) {
    // The next event is not null, update its prev pointer.
    // That's to say, there is more than one event in the queue.
    m_next->m_prev = &m_next;
  }

  // Link the current event to its previous event.
  m_prev = m_loop.m_breadthFirstInsertPoint;
  *m_prev = this;

  // Update the breadth-first insert point.
  m_loop.m_breadthFirstInsertPoint = &m_next;

  if (m_loop.m_tail == m_prev) {
    // The tail is the previous pointer of the current event, update it.
    // That's to say, we are inserting the first event into the queue.
    m_loop.m_tail = &m_next;
  }
}

void Event::armLast() {
  KFC_CHECK(&m_loop == threadLocalEventLoop,
            "Arming an event from a different thread than it was created in");
  KFC_CHECK(m_alive == kEventLivenessMagic, "Arming a destroyed event");

  if (m_prev) {
    // Already armed.
    return;
  }

  // Link the current event to its next event.
  m_next = *m_loop.m_breadthFirstInsertPoint;
  if (m_next) {
    // The next event is not null, update its prev pointer.
    // That's to say, there is more than one event in the queue.
    m_next->m_prev = &m_next;
  }

  // Link the current event to its previous event.
  m_prev = m_loop.m_tail;
  *m_prev = this;

  // We don't do `m_loop.m_breadthFirstInsertPoint = &m_next` here like that in `armBreadthFirst`,
  // because we want further breadth-first inserts to go before this event.

  if (m_loop.m_tail == m_prev) {
    // The tail is the previous pointer of the current event, update it.
    // That's to say, we are inserting the first event into the queue.
    m_loop.m_tail = &m_next;
  }
}

void wait(OwnPromiseNode &node, PromiseResultBase &result, const WaitScope &scope,
          const Option<Duration> &timeout) {
  EventLoop &loop = scope.m_loop;
  KFC_CHECK(&loop == threadLocalEventLoop, "Waiting in a different thread than the EventLoop");

  _::RootEvent event;
  node->poll(&event);

  for (;;) {
    scope.runOnStackPool([&] {
      while (!event.fired()) {
        if (!loop.turn()) {
          // No more events to process, exit the loop.
          return;
        }
      }
    });

    if (event.fired()) {
      break;
    }

    // Poll the EventLoop for events.
    if (!loop.poll(timeout)) {
      // Timeout.
      return;
    }
  }

  scope.runOnStackPool([&] {
    // Read the result.
    node->read(result);
  });
}

PromiseNode::PollEvent::PollEvent() : m_event(nullptr) {}

void PromiseNode::PollEvent::init(Event *event) {
  if (m_event == KFC_ALREADY_READY) {
    // A new continuation was added to a promise that was already ready.  In this case, we schedule
    // breadth-first, to make it difficult for applications to accidentally starve the event loop
    // by repeatedly waiting on immediate promises.
    if (event) event->armBreadthFirst();
  } else {
    m_event = event;
  }
}

void PromiseNode::PollEvent::arm() {
  KFC_ASSERT(m_event != KFC_ALREADY_READY, "`arm` should be called once");
  if (m_event) {
    // A promise resolved and an event is already waiting on it. In this case, arm in depth-first
    // order so that the event runs immediately after the current one. This way, chained promises
    // are executed together for better cache locality and lower latency.
    m_event->armDepthFirst();
  }
  m_event = KFC_ALREADY_READY;
}

ChainPromiseNode::ChainPromiseNode(OwnPromiseNode inner)
    : m_state(Step1), m_inner(std::move(inner)), m_pollEvent(nullptr) {
  m_inner->poll(this);
}

ChainPromiseNode::~ChainPromiseNode() noexcept(false) = default;

void ChainPromiseNode::poll(Event *event) {
  switch (m_state) {
  case Step1:
    m_pollEvent = event;
    break;
  case Step2:
    m_inner->poll(event);
    break;
  default:
    KFC_UNREACHABLE();
  }
}

void ChainPromiseNode::read(PromiseResultBase &result) noexcept {
  KFC_ASSERT(m_state == Step2);
  m_inner->read(result);
}

Option<Own<Event>> ChainPromiseNode::fire() {
  KFC_ASSERT(m_state == Step1);

  PromiseResult<PromiseBase> intermediate;
  m_inner->read(intermediate);

  if (intermediate.isErr()) {
    // The inner promise failed, adopt it to step 2 as an immediate promise returning the exception.
    Exception &e = intermediate.unwrapErr();
    m_inner = new ImmediatePromiseNode<Void>(PromiseResult<Void>(std::move(e)));
  } else {
    PromiseBase &p = intermediate.unwrap();
    // The inner promise succeeded, adopt it to step 2.
    m_inner = _::PromiseNode::from(std::move(p));
  }

  m_state = Step2;
  m_inner->poll(m_pollEvent);
  return None;
}

XThreadEventBase::XThreadEventBase(PromiseResultBase &result, const Executor &targetExecutor,
                                   EventLoop &loop)
    : Event(loop), m_result(result), m_targetExecutor(targetExecutor) {}

void XThreadEventBase::poll(Event *event) { m_pollEvent.init(event); }

Option<Own<Event>> XThreadEventBase::fire() {
  KFC_IF_SOME(n, m_promiseNode) {
    n->read(m_result);
    m_promiseNode = None;
    goto done;
  }
  KFC_IF_SOME(e, runCatchingExceptions([&] { m_promiseNode = std::move(execute()); })) {
    m_result.assignException(std::move(e));
  }
  KFC_IF_SOME(n, m_promiseNode) {
    n->poll(this);
    return None;
  }

done:
  // TODO
  return None;
}

void XThreadEventBase::done() {
  KFC_ASSERT(&m_targetExecutor == &getCurrentThreadExecutor(),
             "calling `XThreadEventBase::done` from wrong thread.");

  KFC_IF_SOME(e, m_requestExecutor) { e.sendReady(*this); }
}

void XThreadEventBase::setDisconnected() const {
  m_result.assignException(KFC_EXCEPTION(
      kLogicError, "Executor's event loop exited before cross-thread event could complete"));
}

struct XThreadEventBase::DelayedDoneDisposer {
  using ValueType = XThreadEventBase;
  static void dispose(ValueType *t) { t->done(); }
};

} // namespace _

Executor::Executor(EventLoop &loop) : m_shared(loop) {}
// TODO: cancel all cross-thread events
Executor::~Executor() noexcept(false) = default;

Ref<Executor> Executor::create(EventLoop &loop) { return adoptRef(*new Executor(loop)); }

void Executor::poll(Option<Duration> timeout) {
  auto shared = m_shared.lock();
  KFC_IF_SOME_CONST(t, timeout) {
    // Wait for the timeout.
    m_condvar.wait(shared, t);
  }

  // Arm all pending events to the target EventLoop.
  for (auto &event : shared->m_pendingEvents) {
    shared->m_pendingEvents.remove(event);
    event.armBreadthFirst();
  }

  // Arm all ready events to the requesting EventLoop.
  for (auto &event : shared->m_readyEvents) {
    shared->m_readyEvents.remove(event);
    event.m_pollEvent.arm();
  }
}

void Executor::sendPending(_::XThreadEventBase &event, const bool sync) {
  if (sync) {
    // The event is expected to be executed synchronously, check if we are on the Executor's own
    // thread, if so, execute it directly to avoid deadlock, otherwise queue it to the loop.
    if (belongsToCurrentThread()) {
      const auto promiseNode = event.execute();
      KFC_ASSERT(promiseNode == None,
                 "`executeSync` cannot be called on own Executor's thread with "
                 "a promise-returning function");
      return;
    }
  } else {
    // The event is expected to be executed asynchronously, record the requesting Executor to
    // receive the result after the event completes.
    event.m_requestExecutor = const_cast<Executor &>(getCurrentThreadExecutor());
  }

  auto shared = m_shared.lock();
  KFC_IF_SOME(l, shared->m_loop) {
    // Queue the event to the pending queue so that the next call to `Executor::poll` could arm it
    // to the target EventLoop.
    shared->m_pendingEvents.add(event);
    KFC_IF_SOME_CONST(p, l.m_port) {
      // Wake up the port for the next call to `Executor::poll`.
      p.wake();
    }
  }
  else {
    // The EventLoop has exited.
    event.setDisconnected();
    return;
  }

  if (sync) {
    // Wait for the event to complete.
    m_condvar.wait(shared);
  }
}

void Executor::sendReady(_::XThreadEventBase &event) {
  auto shared = m_shared.lock();
  KFC_IF_SOME(l, shared->m_loop) {
    // Queue the event to the ready queue so that the next call to `Executor::poll` could arm it
    // to the requesting EventLoop.
    shared->m_readyEvents.add(event);
    KFC_GIVE_UP_GUARD(shared); // Drop the lock to avoid deadlock.
    KFC_IF_SOME_CONST(p, l.m_port) {
      // Wake up the port for the next call to `Executor::poll`.
      p.wake();
    }
  }
  else {
    KFC_FATAL("The requesting executor has exited its EventLoop without canceling the "
              "cross-thread event. This is yet an undefined behavior, so crash it now");
  }
}

EventLoop &Executor::getEventLoop() {
  KFC_IF_SOME(l, m_shared.lock()->m_loop) { return l; }
  KFC_THROW(kLogicError, "Executor's EventLoop has exited");
}

bool Executor::belongsToCurrentThread() const {
  if (!threadLocalEventLoop) return false;
  KFC_IF_SOME_CONST(e, threadLocalEventLoop->m_executor) { return e.ptr() == this; }
  return false;
}

WaitScope::WaitScope(EventLoop &loop) : m_loop(loop) { m_loop.enter(); }
WaitScope::~WaitScope() { m_loop.leave(); }

template <class Func> void WaitScope::runOnStackPool(Func &&func) const {
  // TODO
  func();
}

Promise<void> yield() { return _::PromiseNode::to<Promise<void>>(new _::YieldPromiseNode()); }

const Executor &getCurrentThreadExecutor() { return EventLoop::current().getExecutor(); }

} // namespace KFC
