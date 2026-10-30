/*******************************************************************\

Module: SHA-256 message digest

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// SHA-256 message digest (FIPS 180-4)

#include "sha256.h"

#include <array>
#include <cstdint>
#include <istream>
#include <iterator>

namespace
{
class sha256t
{
public:
  sha256t()
    : state{
        {0x6a09e667,
         0xbb67ae85,
         0x3c6ef372,
         0xa54ff53a,
         0x510e527f,
         0x9b05688c,
         0x1f83d9ab,
         0x5be0cd19}}
  {
  }

  void update(const std::string &data)
  {
    for(const char c : data)
    {
      block[block_size++] = static_cast<uint8_t>(c);
      if(block_size == block.size())
      {
        process_block();
        block_size = 0;
      }
    }
    length_in_bytes += data.size();
  }

  std::string hex_digest()
  {
    const uint64_t length_in_bits = length_in_bytes * 8;

    // pad with a single 1 bit, then zeros until there are 8 bytes left for
    // the message length
    block[block_size++] = 0x80;
    if(block_size > 56)
    {
      while(block_size < block.size())
        block[block_size++] = 0;
      process_block();
      block_size = 0;
    }
    while(block_size < 56)
      block[block_size++] = 0;
    for(int i = 7; i >= 0; --i)
      block[block_size++] = static_cast<uint8_t>(length_in_bits >> (i * 8));
    process_block();

    static const char hex_digits[] = "0123456789abcdef";
    std::string result;
    for(const uint32_t word : state)
    {
      for(int i = 28; i >= 0; i -= 4)
        result += hex_digits[(word >> i) & 0xf];
    }
    return result;
  }

private:
  std::array<uint32_t, 8> state;
  std::array<uint8_t, 64> block{};
  std::size_t block_size = 0;
  uint64_t length_in_bytes = 0;

  static uint32_t rotr(uint32_t x, unsigned n)
  {
    return (x >> n) | (x << (32 - n));
  }

  void process_block()
  {
    static const std::array<uint32_t, 64> k = {
      {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
       0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
       0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
       0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
       0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
       0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
       0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
       0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
       0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
       0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
       0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2}};

    std::array<uint32_t, 64> w;
    for(std::size_t i = 0; i < 16; ++i)
    {
      w[i] = (uint32_t(block[i * 4]) << 24) |
             (uint32_t(block[i * 4 + 1]) << 16) |
             (uint32_t(block[i * 4 + 2]) << 8) | uint32_t(block[i * 4 + 3]);
    }
    for(std::size_t i = 16; i < 64; ++i)
    {
      const uint32_t s0 =
        rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
      const uint32_t s1 =
        rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];

    for(std::size_t i = 0; i < 64; ++i)
    {
      const uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
      const uint32_t ch = (e & f) ^ (~e & g);
      const uint32_t t1 = h + s1 + ch + k[i] + w[i];
      const uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
      const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
      const uint32_t t2 = s0 + maj;

      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
  }
};
} // namespace

std::string sha256_hex(const std::string &data)
{
  sha256t sha256;
  sha256.update(data);
  return sha256.hex_digest();
}

std::string sha256_hex(std::istream &in)
{
  const std::string data{
    std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
  return sha256_hex(data);
}
