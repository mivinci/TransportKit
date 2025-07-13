#include "KFC/String.h"

#include <vector>

namespace KFC {

std::size_t StringView::find(const char c) const {
  for (size_t i = 0; i < m_size; ++i) {
    if (m_ptr[i] == c) return i;
  }
  return String::npos;
}

std::size_t StringView::find(const char *str) const { return find(StringView(str)); }

std::size_t StringView::find(const String &str) const { return find(StringView(str)); }

std::size_t StringView::find(const StringView &str) const {
  if (str.m_size == 0) return 0;
  if (m_size < str.m_size) return String::npos;

  // Preprocess the pattern to compute the longest prefix suffix (LPS) array
  std::vector<std::size_t> lps(str.m_size, 0);
  std::size_t len = 0;
  for (std::size_t i = 1; i < str.m_size;) {
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
  std::size_t i = 0;  // index for the main string
  std::size_t j = 0;  // index for the pattern string
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
  auto signed_start = static_cast<int64_t>(start);
  auto signed_end = static_cast<int64_t>(end);

  if (signed_start < 0) {
    signed_start = static_cast<int64_t>(m_size) + signed_start;
    if (signed_start < 0) signed_start = 0;
  }

  if (signed_end < 0) {
    signed_end = static_cast<int64_t>(m_size) + signed_end;
    if (signed_end < 0) signed_end = 0;
  }

  if (signed_start > m_size) signed_start = static_cast<int64_t>(m_size);
  if (signed_end > m_size) signed_end = static_cast<int64_t>(m_size);

  return StringView(m_ptr + signed_start, m_ptr + signed_end);
}

}  // namespace KFC