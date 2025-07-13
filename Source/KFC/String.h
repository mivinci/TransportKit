#pragma once

#include "KFC/Preclude.h"
#include <cstddef>
#include <string>

KFC_NAMESPACE_BEG
using String = std::string;

class StringView {
public:
  StringView() : m_ptr(nullptr), m_size(0) {}
  StringView(std::nullptr_t) : m_ptr(nullptr), m_size(0) {}
  StringView(const char *ptr) : m_ptr(ptr), m_size(::strlen(ptr)) {}
  StringView(const char *ptr, const size_t size) : m_ptr(ptr), m_size(size) {}
  StringView(const char *start, const char *end)
      : m_ptr(start), m_size(end <= start ? 0 : end - start) {}
  StringView(const String &str) : m_ptr(str.data()), m_size(str.size()) {}

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

  KFC_NODISCARD bool empty() const { return m_size == 0; }
  KFC_NODISCARD size_t size() const { return m_size; }
  KFC_NODISCARD const char *data() const { return m_ptr; }
  KFC_NODISCARD Iterator begin() const { return Iterator(m_ptr); }
  KFC_NODISCARD Iterator end() const { return Iterator(m_ptr + m_size); }
  KFC_NODISCARD String toString() const { return {m_ptr, m_size}; }

  KFC_NODISCARD size_t find(char c) const;
  KFC_NODISCARD size_t find(const char *str) const;
  KFC_NODISCARD size_t find(const String &str) const;
  KFC_NODISCARD size_t find(const StringView &str) const;

  KFC_NODISCARD size_t rfind(char c) const;
  KFC_NODISCARD size_t rfind(const char *str) const;
  KFC_NODISCARD size_t rfind(const String &str) const;
  KFC_NODISCARD size_t rfind(const StringView &str) const;

  KFC_NODISCARD StringView slice(std::size_t start, std::size_t end = INT64_MAX) const;

  StringView &operator=(const char *ptr) {
    m_ptr = ptr;
    m_size = strlen(ptr);
    return *this;
  }

  bool operator==(const StringView &other) const {
    if (m_size != other.m_size) return false;
    return ::strncmp(m_ptr, other.m_ptr, m_size) == 0;
  }

  bool operator==(const char *other) const { return *this == StringView(other); }

  char operator[](size_t index) const;

private:
  const char *m_ptr;
  size_t m_size;
};

template <class T> String joinStringArray(T &&arr, const char *delim) {
  const size_t arrSize = KFC::size(arr);
  const size_t delimSize = strlen(delim);
  size_t size = 0;

  for (size_t i = 0; i < arrSize; ++i) {
    size += arr[i].size();
  }
  size += (arrSize - 1) * delimSize;

  String res;
  res.resize(size);

  char *ptr = res.data();
  for (size_t i = 0; i < arrSize; ++i) {
    if (i != 0) {
      memcpy(ptr, delim, delimSize);
      ptr += delimSize;
    }
    memcpy(ptr, arr[i].data(), arr[i].size());
    ptr += arr[i].size();
  }
  return res;
}

KFC_NAMESPACE_END
