#include "KFC/String.h"
#include "KFC/Assert.h"

#include <vector>

namespace KFC {

char StringView::operator[](const size_t index) const {
  KFC_CHECK(index < m_size, "Index out of range");
  return m_ptr[index];
}

size_t StringView::find(const char c) const {
  for (size_t i = 0; i < m_size; ++i) {
    if (m_ptr[i] == c) return i;
  }
  return String::npos;
}

size_t StringView::find(const char *str) const { return find(StringView(str)); }

size_t StringView::find(const String &str) const { return find(StringView(str)); }

size_t StringView::find(const StringView &str) const {
  if (str.m_size == 0) return 0;
  if (m_size < str.m_size) return String::npos;

  // Preprocess the pattern to compute the longest prefix suffix (LPS) array
  std::vector<size_t> lps(str.m_size, 0);
  size_t len = 0;
  for (size_t i = 1; i < str.m_size;) {
    if (str.m_ptr[i] == str.m_ptr[len]) {
      lps[i++] = ++len;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i++] = 0;
      }
    }
  }

  // Perform the KMP search
  size_t i = 0; // index for the main string
  size_t j = 0; // index for the pattern string
  while (i < m_size && j < str.m_size) {
    if (m_ptr[i] == str.m_ptr[j]) {
      i++;
      j++;
    } else {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }
  return (j == str.m_size) ? (i - j) : String::npos;
}

size_t StringView::rfind(const char c) const {
  for (size_t i = m_size; i > 0; --i) {
    if (m_ptr[i - 1] == c) return i - 1;
  }
  return String::npos;
}

size_t StringView::rfind(const char *str) const { return rfind(StringView(str)); }

size_t StringView::rfind(const String &str) const { return rfind(StringView(str)); }

size_t StringView::rfind(const StringView &str) const {
  if (str.m_size == 0) return m_size;
  if (m_size < str.m_size) return String::npos;

  // Preprocess the pattern to compute the longest prefix suffix (LPS) array
  std::vector<size_t> lps(str.m_size, 0);
  size_t len = 0;
  for (size_t i = 1; i < str.m_size;) {
    if (str.m_ptr[i] == str.m_ptr[len]) {
      lps[i++] = ++len;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i++] = 0;
      }
    }
  }

  // Perform the reverse KMP search
  size_t i = m_size;     // index for the main string
  size_t j = str.m_size; // index for the pattern string
  size_t last_match_pos = String::npos;

  while (i > 0) {
    if (m_ptr[i - 1] == str.m_ptr[j - 1]) {
      i--;
      j--;
      if (j == 0) {
        last_match_pos = i;
        j = str.m_size; // Reset to find earlier matches
      }
    } else {
      if (j != str.m_size) {
        j = lps[j];
      } else {
        i--;
      }
    }
  }

  return last_match_pos;
}

/*
 * StringView s("12345678");
 * s.slice(0, 1) -> "1"
 * s.slice(0, 2) -> "12"
 * s.slice(0, 8) -> "12345678"
 * s.slice(0, 9) -> "12345678"
 *
 * s.slice(0, -1) -> s.slice(0, 7) -> "1234567"
 * s.slice(0, -2) -> s.slice(0, 6) -> "123456"
 * s.slice(0, -8) -> s.slice(0, 0) -> ""
 * s.slice(0, -9) -> s.slice(0, 0) -> ""
 *
 * s.slice(0) -> "12345678"
 * s.slice(1) -> "2345678"
 * s.slice(2) -> "345678"
 * s.slice(8) -> ""
 * s.slice(9) -> ""
 *
 * s.slice(-1) -> s.slice(7) -> "8"
 * s.slice(-2) -> s.slice(6) -> "78"
 * s.slice(-8) -> s.slice(0) -> "12345678"
 * s.slice(-9) -> index out of range
 *
 * s.slice(-1, 0) -> s.slice(7, 0) -> ""
 * s.slice(-2, 0) -> s.slice(6, 0) -> ""
 * s.slice(-2, -1) -> s.slice(6, 7) -> "7"
 * s.slice(-5, 7) -> s.slice(3, 7) -> "4567"
 */
StringView StringView::slice(const std::size_t start, const std::size_t end) const {
  auto signedStart = static_cast<int64_t>(start);
  auto signedEnd = static_cast<int64_t>(end);

  if (signedStart < 0) {
    signedStart = static_cast<int64_t>(m_size) + signedStart;
    if (signedStart < 0) signedStart = 0;
  }

  if (signedEnd < 0) {
    signedEnd = static_cast<int64_t>(m_size) + signedEnd;
    if (signedEnd < 0) signedEnd = 0;
  }

  if (signedStart > m_size) signedStart = static_cast<int64_t>(m_size);
  if (signedEnd > m_size) signedEnd = static_cast<int64_t>(m_size);

  return StringView(m_ptr + signedStart, m_ptr + signedEnd);
}

} // namespace KFC