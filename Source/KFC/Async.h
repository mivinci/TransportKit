#pragma once

#include "KFC/Condvar.h"
#include "KFC/Exception.h"
#include "KFC/Function.h"
#include "KFC/List.h"
#include "KFC/Mutex.h"
#include "KFC/Option.h"
#include "KFC/Own.h"
#include "KFC/Ref.h"
#include "KFC/Result.h"
#include "KFC/Time.h"

namespace KFC {
template <class T> class Promise;
class EventLoop;
class WaitScope;
class Executor;

namespace _ {
// An event that is scheduled in a EventLoop.
class Event {
public:
  // ZSBD
  explicit Event();
  explicit Event(EventLoop &);
  virtual ~Event() noexcept(false);
  KFC_DISALLOW_COPY_AND_MOVE(Event)
  // Arm the event in depth-first order.
  void armDepthFirst();
  // Arm the event in breadth-first order.
  void armBreadthFirst();
  // Arm the event to the end of the event queue.
  void armLast();
  // Disarm the event.
  void disarm();

protected:
  // Fire the event. Possibly return the ownership to the caller, the EventLoop fired the
  // event. In other words, `fire` should return `None` if it doesn't need the caller to destroy
  // the event, which, in most cases, happens when the event is not heap-allocated.
  virtual Option<Own<Event>> fire() = 0;

private:
  friend class KFC::EventLoop;
  EventLoop &m_loop;
  Event **m_prev;
  Event *m_next;
  bool m_firing;

  static constexpr uint32_t kEventLivenessMagic = 0xdeadc0de;
  uint32_t m_alive;
};

class PromiseNode;
template <class> class PromiseResult;

// clang-format off
template <class T> struct ReducePromise { typedef T Type; };
template <class U> struct ReducePromise<KFC::Promise<U>> { typedef U Type; };
// clang-format on

template <class T> using ReducedPromise = Promise<typename ReducePromise<T>::Type>;
using OwnPromiseNode = Own<PromiseNode /* , PromiseDisposer */>;

class PromiseResultBase {
public:
  virtual ~PromiseResultBase() noexcept(false) = default;
  virtual void assignException(Exception &&) noexcept = 0;
  template <class T> PromiseResult<T> &as() {
    KFC_STATIC_ASSERT_IS_BASE_OF(PromiseResultBase, PromiseResult<T>);
    return *static_cast<PromiseResult<T> *>(this);
  }
  template <class T> KFC_NODISCARD const PromiseResult<T> &as() const {
    KFC_STATIC_ASSERT_IS_BASE_OF(PromiseResultBase, PromiseResult<T>);
    return *static_cast<const PromiseResult<T> *>(this);
  }
};

template <class T>
class PromiseResult final : public Result<T, Exception>, public PromiseResultBase {
public:
  explicit PromiseResult() : Result<T, Exception>() {}
  explicit PromiseResult(T &&t) : Result<T, Exception>(std::move(t)) {}
  explicit PromiseResult(Exception &&e) : Result<T, Exception>(std::move(e)) {}
  void assignException(Exception &&e) noexcept override {
    this->Result<T, Exception>::operator=(std::move(e));
  }
};

template <>
class PromiseResult<void> final : public Result<void, Exception>, public PromiseResultBase {
public:
  explicit PromiseResult() : Result<void, Exception>() {}
  explicit PromiseResult(Exception &&e) : Result<void, Exception>(std::move(e)) {}
  void assignException(Exception &&e) noexcept override {
    this->Result<void, Exception>::operator=(std::move(e));
  }
};

class PromiseResolverBase {
public:
  virtual ~PromiseResolverBase() = default;
  virtual bool isWaiting() = 0;
};

template <class T> class PromiseResolver : public PromiseResolverBase {
public:
  // Resolve the promise with the given result.
  virtual void resolve(_::PromiseResult<T> &&result) = 0;
};

class PromiseNode {
public:
  virtual ~PromiseNode() noexcept(false) = default;
  // Read the result of the promise.
  virtual void read(PromiseResultBase &result) noexcept = 0;
  // Arm the given event when ready.
  virtual void poll(Event *event) = 0;
  // Extract the node from the given promise.
  template <class T> static OwnPromiseNode from(T &&promise) { return std::move(promise.m_node); }
  // Extract the node reference from the given promise.
  template <class T> static PromiseNode &from(T &promise) { return *promise.m_node; }
  // Convert the node to a promise.
  template <class T> static T to(OwnPromiseNode &&node) { return T(false, std::move(node)); }

protected:
  class PollEvent {
  public:
    explicit PollEvent();
    void init(Event *event);
    void arm();

  private:
    Event *m_event;
  };
};

template <class T> class ImmediatePromiseNode final : public PromiseNode {
public:
  explicit ImmediatePromiseNode(PromiseResult<T> &&result) : m_result(std::move(result)) {}
  void read(PromiseResultBase &result) noexcept override { result.as<T>() = std::move(m_result); }
  void poll(Event *event) override {
    if (event) event->armBreadthFirst();
  }

private:
  PromiseResult<T> m_result;
};

// Transform Promise<T> to Promise<U> through a function. `TransformPromiseNode` is the key that
// makes `then` possible.
template <class U, class T, class Func, class ErrFunc>
class TransformPromiseNode final : public PromiseNode {
public:
  explicit TransformPromiseNode(OwnPromiseNode &&dep, Func &&func, ErrFunc &&errFunc)
      : m_func(std::move(func)), m_errFunc(std::move(errFunc)), m_dep(std::move(dep)) {}
  ~TransformPromiseNode() noexcept(false) override { dropDep(); }
  void poll(Event *event) override { m_dep->poll(event); }
  void read(PromiseResultBase &result) noexcept override {
    KFC_IF_SOME(e, KFC::runCatchingExceptions([&] { tryRead(result); })) {
      result.as<U>() = forward(std::move(e));
    }
    else {
      dropDep();
    }
  }

private:
  void tryRead(PromiseResultBase &result) {
    PromiseResult<T> depResult;
    m_dep->read(depResult);
    KFC_IF_SOME(e, depResult.takeErr()) {
      auto errFuncResult =
          FunctionCaller<KFC::FixVoid<KFC::ReturnType<ErrFunc, Exception>>, Exception>::apply(
              m_errFunc, std::move(e));
      result.as<U>() = forward(std::move(errFuncResult));
    }
    else {
      U funcResult = FunctionCaller<U, T>::apply(m_func, std::move(depResult.unwrap()));
      result.as<U>() = forward(std::move(funcResult));
    }
  }
  void dropDep() { m_dep = nullptr; }
  PromiseResult<U> forward(U &&value) { return PromiseResult<U>(std::move(value)); }
  PromiseResult<U> forward(KFC::Exception &&exception) {
    return PromiseResult<U>(std::move(exception));
  }

  Func m_func;
  ErrFunc m_errFunc;
  OwnPromiseNode m_dep;
};

class ChainPromiseNode final : public PromiseNode, public Event {
public:
  explicit ChainPromiseNode(OwnPromiseNode inner);
  ~ChainPromiseNode() noexcept(false) override;
  void read(PromiseResultBase &result) noexcept override;
  void poll(Event *event) override;

private:
  Option<Own<Event>> fire() override;

  enum State {
    Step1 = 1,
    Step2,
  };

  State m_state;
  // In Step1, a PromiseNode for a Promise<T>.
  // In Step2, a PromiseNode for a T.
  OwnPromiseNode m_inner;
  Event *m_pollEvent;
};

class YieldPromiseNode final : public PromiseNode {
public:
  void read(PromiseResultBase &result) noexcept override {
    result.as<Void>() = PromiseResult<Void>(Void{});
  }
  void poll(Event *event) override {
    if (event) event->armBreadthFirst();
  }
};

template <class T, class Adapter>
class AdapterPromiseNode final : public PromiseNode, private PromiseResolver<T> {
public:
  template <class... Args>
  explicit AdapterPromiseNode(Args &&...args)
      : m_adapter(static_cast<PromiseResolver<T> &>(*this), std::forward<Args>(args)...) {}
  bool isWaiting() override { return m_result.isEmpty(); }
  void poll(Event *event) override { m_pollEvent.init(event); }
  void wake() { m_pollEvent.arm(); }
  void resolve(PromiseResult<T> &&result) override {
    if (!isWaiting()) return;
    m_result = std::move(result);
    wake();
  }
  void read(PromiseResultBase &result) noexcept override {
    KFC_CHECK(!isWaiting(), "Promise is not ready");
    result.as<T>() = std::move(m_result);
  }

private:
  Adapter m_adapter;
  PollEvent m_pollEvent;
  PromiseResult<T> m_result;
};

class PromiseBase {
public:
  explicit PromiseBase() = default;
  explicit PromiseBase(OwnPromiseNode &&node) : m_node(std::move(node)) {}

private:
  template <class> friend class KFC::Promise;
  friend class PromiseNode;
  OwnPromiseNode m_node;
};

// Cross-thread event, scheduled by an Executor.
class XThreadEventBase : public PromiseNode, private Event {
public:
  explicit XThreadEventBase(PromiseResultBase &result, const Executor &targetExecutor,
                            EventLoop &loop);

protected:
  // Execute the underlying function. If the function returns a promise, returns the inner promise
  // node, otherwise returns None.
  virtual Option<OwnPromiseNode> execute() = 0;

  void poll(Event *event) override;

private:
  Option<Own<Event>> fire() override;
  void done();
  void setDisconnected() const;

  PollEvent m_pollEvent;
  Option<OwnPromiseNode> m_promiseNode;
  PromiseResultBase &m_result;

  const Executor &m_targetExecutor;     // The executor executing this event.
  Option<Executor &> m_requestExecutor; // The executor requesting this event.

  ListLink<XThreadEventBase> m_link;

  struct DelayedDoneDisposer;
  friend KFC::Executor;
};

template <class Func, class T = FixVoid<ReturnType<Func, void>>>
class XThreadEvent final : public XThreadEventBase {
public:
  explicit XThreadEvent(Func &&func, const Executor &targetExecutor, EventLoop &loop)
      : XThreadEventBase(m_result, targetExecutor, loop), m_func(std::move(func)) {}

  void read(PromiseResultBase &result) noexcept override { result.as<T>() = std::move(m_result); }

  Option<OwnPromiseNode> execute() override {
    m_result = FunctionCaller<T, void>::apply(m_func);
    return None;
  }

private:
  friend Executor;
  Func m_func;
  PromiseResult<T> m_result;
};

template <class Func, class T>
class XThreadEvent<Func, Promise<T>> final : public XThreadEventBase {
public:
  explicit XThreadEvent(Func &&func, const Executor &targetExecutor, EventLoop &loop)
      : XThreadEventBase(m_result, targetExecutor, loop), m_func(std::move(func)) {}

  void read(PromiseResultBase &result) noexcept override {
    result.as<Promise<T>>() = std::move(m_result);
  }

  Option<OwnPromiseNode> execute() override {
    OwnPromiseNode node = PromiseNode::from(m_func());
    KFC_CHECK(!!node, "Promise returned by async function must not be empty");
    return std::move(node);
  }

private:
  friend Executor;
  Func m_func;
  Promise<T> m_result;
};

class RootEvent final : public Event {
public:
  explicit RootEvent() : m_fired(false) {};
  KFC_NODISCARD bool fired() const { return m_fired; }

  Option<Own<Event>> fire() override {
    m_fired = true;
    return None;
  }

private:
  friend class KFC::WaitScope;
  bool m_fired;
};

class PropagateException {
public:
  Exception operator()(Exception &&exception) const { return std::move(exception); }
  Exception operator()(const Exception &exception) const { return exception; }
};

void wait(OwnPromiseNode &node, PromiseResultBase &result, const WaitScope &scope,
          const Option<Duration> &timeout);

template <class T> OwnPromiseNode maybeChain(OwnPromiseNode &&node, Promise<T> *) {
  return new ChainPromiseNode(std::move(node));
}
template <class T> OwnPromiseNode maybeChain(OwnPromiseNode &&node, T *) { return std::move(node); }

template <class T> T &&maybeReturnVoid(PromiseResult<T> &&result) {
  return std::move(result.unwrap());
}
inline void maybeReturnVoid(PromiseResult<Void> &&result) { KFC_DISCARD(result.unwrap()); }

// Create a promise that owns an adapter. The adapter's constructor must take PromiseResolver<T>& as
// the first argument and resolve the promise at some point. `args` will be forwarded to construct
// the adapter. The adapter will be destroyed when the owning promise is destroyed.
template <class T, class Adapter, class... Args>
_::ReducedPromise<T> createAdaptedPromise(Args &&...args) {
  _::OwnPromiseNode intermediate(
      new _::AdapterPromiseNode<T, Adapter>(std::forward<Args>(args)...));
  return _::PromiseNode::to<_::ReducedPromise<T>>(
      _::maybeChain(std::move(intermediate), static_cast<T *>(nullptr)));
}

} // namespace _

// Evaluates to the type of Promise for the result of calling Func with argument T. If the result of
// calling Func is of type U, PromiseForResult evaluates to Promise<U>. If the result of calling
// Func is of type Promise<U>, PromiseForResult reduces to Promise<U>. This is useful for chaining
// promises.
template <class Func, class T>
using PromiseForResult = Promise<typename _::ReducePromise<ReturnType<Func, T>>::Type>;

template <class T> class Promise : public _::PromiseBase {
public:
  // Construct a promise already fulfilled with the given result.
  explicit Promise(_::PromiseResult<T> &&result)
      : PromiseBase(Own<_::ImmediatePromiseNode<T>>(new _::ImmediatePromiseNode<T>(result))) {}
  // Construct a promise from a PromiseNode.
  explicit Promise(bool, _::OwnPromiseNode &&node) : PromiseBase(std::move(node)) {}
  // Chain a function to the promise. The function will be called when the promise is fulfilled.
  // The function should be callable with T and return a value of type U. The returned promise will
  // be fulfilled with the result of calling the function.
  template <class Func, class ErrFunc = _::PropagateException>
  PromiseForResult<Func, T> then(Func &&func, ErrFunc errFunc = _::PropagateException()) {
    using U = ReturnType<Func, T>;
    _::OwnPromiseNode intermediate =
        new _::TransformPromiseNode<FixVoid<U>, FixVoid<T>, Func, ErrFunc>(
            std::move(m_node), std::forward<Func>(func), std::forward<ErrFunc>(errFunc));
    return _::PromiseNode::to<_::ReducedPromise<U>>(
        _::maybeChain(std::move(intermediate), static_cast<FixVoid<U> *>(nullptr)));
  }

  // Wait for the promise to be fulfilled and return the result. A `timeout` can be specified
  // to limit the maximum amount of time to wait. If the timeout expires, a `Timeout` exception
  // will be thrown.
  T wait(WaitScope &scope, Option<Duration> timeout = None) {
    _::PromiseResult<FixVoid<T>> result;
    _::wait(m_node, result, scope, timeout);
    return _::maybeReturnVoid(std::move(result));
  }

  // Discard the result of the promise and return a promise fulfilled when the original promise is
  // fulfilled.
  Promise<void> discard() {
    return then([](T &&) {});
  }

private:
  friend _::PromiseNode;
};

template <class T> struct PromiseResolverPair {
  _::ReducedPromise<T> promise;
  Own<_::PromiseResolver<T>> resolver;
};

// An interface for polling for events.
class EventPort {
public:
  virtual ~EventPort() noexcept(false) = default;
  // Check if any events have arrived. If so, add them into the event queue of the
  // underlying EventLoop and return false. If no events have arrived, the call to `poll` will block
  // until a call to `wake` from another thread which will cause `poll` return true. A `timeout`
  // can be specified to limit the maximum amount of time to block. If the timeout expires, `poll`
  // will return false.
  virtual bool poll(Option<Duration> timeout) = 0;
  // `wake` can be called from another thread to wake up the port that is polling for events or
  // a timeout. If called when the port is not polling, the next call to `poll` will return
  // immediately.
  virtual void wake() const = 0;
};

// Schedule events to a EventLoop from another thread.
class Executor final : public RefCounted<Executor> {
public:
  static Ref<Executor> create(EventLoop &loop);

  class Shared {
  public:
    Shared(EventLoop &loop) : m_loop(loop) {} // NOLINT(*-explicit-constructor)

  private:
    friend Executor;
    friend _::XThreadEventBase;
    Option<EventLoop &> m_loop;
    List<_::XThreadEventBase, &_::XThreadEventBase::m_link> m_pendingEvents;
    List<_::XThreadEventBase, &_::XThreadEventBase::m_link> m_readyEvents;
  };

  explicit Executor(EventLoop &loop);
  ~Executor() override;

  template <class Func> PromiseForResult<Func, void> executeSync(Func &&func);
  template <class Func> PromiseForResult<Func, void> executeAsync(Func &&func) {
    auto event = new _::XThreadEvent<Func>(std::forward<Func>(func, *this, getEventLoop()));
    sendPending(event, false);
    return _::PromiseNode::to<PromiseForResult<Func, void>>(std::move(event));
  }

private:
  friend EventLoop;
  friend _::XThreadEventBase;
  // Called by the underlying EventLoop to check if any cross-thread events have arrived. If so, add
  // them into the event queue.
  void poll(Option<Duration> timeout);
  void sendPending(_::XThreadEventBase &event, bool sync = false);
  void sendReady(_::XThreadEventBase &event);
  EventLoop &getEventLoop();
  KFC_NODISCARD bool belongsToCurrentThread() const;

  Condvar m_condvar;
  Mutex<Shared> m_shared;
};

class EventLoop final {
public:
  // Get the EventLoop for the current thread.
  static EventLoop &current();

  explicit EventLoop();
  explicit EventLoop(EventPort &port);

  // Get the executor for this event loop. An executor is created for an EventLoop on first call to
  // `getExecutor`.
  const Executor &getExecutor();
  // Fire the next event in the event queue, if any.
  bool turn();
  // Check if any events have arrived. If so, add them into the event queue. First try the EventPort
  // for in-thread events, then the Executor for cross-thread events.
  bool poll(const Option<Duration> &timeout);

private:
  friend class _::Event;
  friend class Executor;
  friend class WaitScope;

  void enter() const;
  void leave() const;

  _::Event *m_current;
  _::Event *m_head;
  _::Event **m_tail;
  _::Event **m_depthFirstInsertPoint;
  _::Event **m_breadthFirstInsertPoint;

  Option<EventPort &> m_port;
  Option<Ref<Executor>> m_executor;
};

class WaitScope {
public:
  explicit WaitScope(EventLoop &loop);
  ~WaitScope();
  KFC_DISALLOW_COPY_AND_MOVE(WaitScope)

private:
  friend void _::wait(_::OwnPromiseNode &node, _::PromiseResultBase &result, const WaitScope &scope,
                      const Option<Duration> &timeout);

  template <class Func> void runOnStackPool(Func &&func) const;

  EventLoop &m_loop;
};

Promise<void> yield();

template <class Func> PromiseForResult<Func, void> evalNow(Func &&func) {
  PromiseForResult<Func, void> promise = nullptr;
  KFC_IF_SOME(e, runCatchingExceptions([&] { promise = func(); })) { promise = std::move(e); }
  return promise;
}

template <class Func> PromiseForResult<Func, void> evalLater(Func &&func) {
  return yield().then(std::forward<Func>(func));
}

const Executor &getCurrentThreadExecutor();
} // namespace KFC
