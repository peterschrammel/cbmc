/*******************************************************************\

Module: Unit tests of SHA-256

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// SHA-256 Unit Tests

#include <util/sha256.h>

#include <testing-utils/use_catch.h>

#include <sstream>

TEST_CASE("SHA-256 test vectors", "[core][util][sha256]")
{
  // FIPS 180-4 examples, including messages whose padding needs an
  // additional block
  REQUIRE(
    sha256_hex("") ==
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  REQUIRE(
    sha256_hex("abc") ==
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  REQUIRE(
    sha256_hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
    "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
  REQUIRE(
    sha256_hex(std::string(64, 'a')) ==
    "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
  REQUIRE(
    sha256_hex(std::string(1000000, 'a')) ==
    "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST_CASE("SHA-256 of a stream", "[core][util][sha256]")
{
  std::istringstream in("abc");
  REQUIRE(
    sha256_hex(in) ==
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}
