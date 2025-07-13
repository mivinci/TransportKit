#pragma once

#include <cstddef>
#include <string>

#include "TX/Assert.h"

namespace TX {
class String;

class StringView {
 public:
  explicit StringView() : m_ptr(nullptr), m_size(0) {}
  explicit StringView(std::nullptr_t) : m_ptr(nullptr), m_size(0) {}
  explicit StringView(const char *ptr) : m_ptr(ptr), m_size(::strlen(ptr)) {}
  explicit StringView(const char *ptr, const size_t size) : m_ptr(ptr), m_size(size) {}
  explicit StringView(const char *start, const char *end)
      : m_ptr(start), m_size(end <= start ? 0 : end - start) {}
  explicit StringView(const std::string &str) : m_ptr(str.data()), m_size(str.size()) {}

  class Iterator {
   public:
    explicit Iterator(const char *ptr) : m_ptr(const_cast<char *>(ptr)) {}
    char &operator*() const { return *m_ptr; }
    bool operator!=(const Iterator &other) const { return m_ptr != other.m_ptr; }
    bool operator==(const Iterator &other) const { return m_ptr == other.m_ptr; }
    Iterator &operator++() {
      ++m_ptr;
      return *this;
    }
    Iterator &operator--() {
      --m_ptr;
      return *this;
    }

   private:
    char *m_ptr;
  };

  TX_NODISCARD bool empty() const { return m_size == 0; }
  TX_NODISCARD size_t size() const { return m_size; }
  TX_NODISCARD const char *data() const { return m_ptr; }
  TX_NODISCARD Iterator begin() const { return Iterator(m_ptr); }
  TX_NODISCARD Iterator end() const { return Iterator(m_ptr + m_size); }
  TX_NODISCARD std::string toString() const { return {m_ptr, m_size}; }

  TX_NODISCARD std::size_t find(char c) const;
  TX_NODISCARD std::size_t find(const StringView &str) const;

  TX_NODISCARD StringView slice(std::size_t start, std::size_t end = INT64_MAX) const;

  StringView &operator=(const char *ptr) {
    m_ptr = ptr;
    m_size = ::strlen(ptr);
    return *this;
  }

  bool operator==(const StringView &other) const {
    if (m_size != other.m_size) return false;
    return ::strncmp(m_ptr, other.m_ptr, m_size) == 0;
  }

  bool operator==(const char *other) const { return *this == StringView(other); }

  char operator[](const size_t index) const {
    TX_CHECK(index < m_size, "Index out of range");
    return m_ptr[index];
  }

 private:
  const char *m_ptr;
  size_t m_size;
};

}  // namespace TX
