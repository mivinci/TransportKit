#pragma once
#include "Assert.h"
#include "KFC/Preclude.h"

namespace KFC {
// A link for `KFC::List`.
template <class T> class ListLink;

// An iterator for `KFC::List`.
template <class T, ListLink<T> T::*link> class ListIterator;

// Unlike `std::list`, `KFC::List` does not do memory allocation.
template <class T, ListLink<T> T::*link> class List {
public:
  using Iterator = ListIterator<T, link>;
  explicit List() : m_head(nullptr), m_tail(&m_head), m_count(0) {}
  KFC_NODISCARD bool empty() const { return m_head == nullptr; }
  KFC_NODISCARD size_t size() const { return m_count; }

  // Adds t to the back of the list.
  void add(T &t) {
    // points the `m_next` of the last element to t
    *m_tail = &t;
    // points the `m_prev` of t to the `m_next` of the last element
    (t.*link).m_prev = m_tail;
    // updates m_tail to point to the `m_next` of t
    m_tail = &(t.*link).m_next;
    ++m_count;
  }

  // Adds t to the front of the list.
  void addFront(T &t) {
    // points the `m_next` of t to the first element
    (t.*link).m_next = m_head;
    // points the `m_prev` of t to the `m_head` pointer
    (t.*link).m_prev = &m_head;
    // if the list is not empty, points the `m_prev` of the first element to the
    // `m_next` of t, otherwise updates m_tail to point to the `m_next` of t
    if (m_head) {
      (m_head->*link).m_prev = &(t.*link).m_next;
    } else {
      m_tail = &(t.*link).m_next;
    }
    // updates m_head to point to t
    m_head = &t;
    ++m_count;
  }

  // Removes t from the list. Safe to call while iterating over the list.
  void remove(T &t) {
    if (!(t.*link).isLinked()) KFC_THROW(KFC::Exception::Kind::Logic, "Remove unlinked element");
    // points the `m_next` of the previous
    *(t.*link).m_prev = (t.*link).m_next;
    // if t is not the last element, points the `m_prev` of the next to the
    // `m_prev` of t, otherwise updates m_tail to point to the `m_prev` of t
    auto &next = (t.*link).m_next;
    if (next) {
      (next->*link).m_prev = (t.*link).m_prev;
    } else {
      if (m_tail != &(t.*link).m_next)
        KFC_THROW(KFC::Exception::Kind::Logic, "Remove element from wrong list");
      m_tail = (t.*link).m_prev;
    }
    // clears the links of t
    (t.*link).m_next = nullptr;
    (t.*link).m_prev = nullptr;
    --m_count;
  }

  // ReSharper disable once CppMemberFunctionMayBeStatic
  Iterator end() { return Iterator(nullptr); }
  Iterator begin() { return Iterator(m_head); }
  T &front() { return *m_head; }

private:
  T *m_head;  // points to the first element
  T **m_tail; // points to the `m_next` of the last element
  size_t m_count;
};

template <class T> class ListLink {
public:
  explicit ListLink() : m_next(nullptr), m_prev(nullptr) {}
  KFC_NODISCARD bool isLinked() const { return m_prev != nullptr; }

private:
  template <class U, ListLink<U> U::*link> friend class List;
  template <class U, ListLink<U> U::*link> friend class ListIterator;
  T *m_next;  // points to the next element
  T **m_prev; // points to the `m_next` of the previous element
};

template <class T, ListLink<T> T::*link> class ListIterator {
public:
  explicit ListIterator() : m_current(nullptr), m_next(nullptr) {}
  bool operator==(const ListIterator &other) const { return m_current == other.m_current; }
  bool operator!=(const ListIterator &other) const { return m_current != other.m_current; }

  T &operator*() const {
    KFC_CHECK(m_current, "deref end of list");
    return *m_current;
  }
  T *operator->() const {
    KFC_CHECK(m_current, "deref end of list");
    return m_current;
  }

  ListIterator &operator++() {
    m_current = m_next;
    m_next = m_current ? (m_current->*link).m_next : nullptr;
    return *this;
  }

private:
  friend class List<T, link>;
  explicit ListIterator(T *ptr) : m_current(ptr), m_next(ptr ? (ptr->*link).m_next : nullptr) {}

  T *m_current;
  T *m_next;
};

} // namespace KFC
