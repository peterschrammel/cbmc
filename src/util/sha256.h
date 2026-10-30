/*******************************************************************\

Module: SHA-256 message digest

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// SHA-256 message digest (FIPS 180-4)

#ifndef CPROVER_UTIL_SHA256_H
#define CPROVER_UTIL_SHA256_H

#include <iosfwd>
#include <string>

/// \return the SHA-256 digest of \p data as a lower-case hex string
std::string sha256_hex(const std::string &data);

/// \return the SHA-256 digest of all bytes that can be read from \p in
///   as a lower-case hex string
std::string sha256_hex(std::istream &in);

#endif // CPROVER_UTIL_SHA256_H
