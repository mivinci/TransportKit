#include "KFC/URL.h"

namespace KFC {
Result<URL, URLParseError> URL::parse(const String &rawURL) {
  if (rawURL.empty()) return KFC_ERR(URLParseError::InvalidURL);
  URL url(rawURL);
  StringView view(url.m_rawURL);
  // TODO: parse

  return KFC_OK(url);
}

}  // namespace KFC
