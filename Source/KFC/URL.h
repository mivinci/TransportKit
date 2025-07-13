#pragma once

#include <utility>

#include "KFC/Option.h"
#include "KFC/Result.h"
#include "KFC/String.h"

namespace KFC {

enum class URLParseError : int {
  InvalidURL,
};

class URLQuery {};

class URL {
 public:
  // Parse a raw URL string and return a `URL` object or `URLParseError`.
  // The general form represented in the URL is:
  //   <scheme>://<user>:<password>@<host>:<port>/<path>?<query>#<anchor>
  static Result<URL, URLParseError> parse(const String &rawURL);

  explicit URL() : m_port(0) {}

  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView rawURL() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView rawQuery() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView origin() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView scheme() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView user() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView password() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView host() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView path() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION StringView anchor() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION short port() const;
  KFC_NODISCARD KFC_CONSTEXPR_FUNCTION const URLQuery &query() const;

 private:
  explicit URL(String href) : m_rawURL(std::move(href)), m_port(0) {}

  String m_rawURL;
  StringView m_rawQuery;
  StringView m_origin;
  StringView m_scheme;
  StringView m_user;
  StringView m_password;
  StringView m_host;
  StringView m_path;
  StringView m_anchor;
  short m_port;
  Option<URLQuery> m_query;
};

}  // namespace KFC
