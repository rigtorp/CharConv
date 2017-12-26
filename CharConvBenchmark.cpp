/*
Copyright (c) 2017 Erik Rigtorp <erik@rigtorp.se>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "CharConv.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>

auto generate_ints(int digits) {
  if (digits > 9) {
    std::abort();
  }
  int max = 1;
  for (int i = 0; i < digits; ++i) {
    max *= 10;
  }
  --max;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(-max, max);
  std::array<int, 4096> v = {};
  std::generate(v.begin(), v.end(), [&] { return dis(gen); });
  return v;
}

auto generate_strings(int digits) {
  if (digits > 9) {
    std::abort();
  }
  int max = 1;
  for (int i = 0; i < digits; ++i) {
    max *= 10;
  }
  --max;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(-max, max);
  std::array<std::string, 4096> v;
  std::generate(v.begin(), v.end(), [&] {
    std::array<char, 32> b = {};
    sprintf(b.data(), "%i", dis(gen));
    return b.data();
  });
  return v;
}

static bool to_chars_naive(char *first, char *last, uint32_t v) {
  char *p = first;
  while (v >= 10) {
    const auto q = v / 10;
    const auto r = static_cast<uint32_t>(v % 10);
    *p++ = r + '0';
    v = q;
  }
  *p++ = v + '0';
  std::reverse(first, p);
  return true;
}

bool from_chars_unchecked(const char *first, const char *last,
                          uint32_t &value) {
  uint32_t res = 0;
  while (first != last) {
    res *= 10;
    res += *first - '0';
    first++;
  }
  value = res;
  return true;
}

static void BM_sprintf(benchmark::State &state) {
  const auto v = generate_ints(state.range(0));
  std::array<char, 16> buf = {};
  int i = 0;
  for (auto _ : state) {
    sprintf(buf.data(), "%i", v[i % v.size()]);
    ++i;
  }
}

static void BM_to_string(benchmark::State &state) {
  const auto v = generate_ints(state.range(0));
  std::string buf;
  int i = 0;
  for (auto _ : state) {
    std::to_string(v[i % v.size()]);
    ++i;
  }
}

static void BM_stringstream(benchmark::State &state) {
  const auto v = generate_ints(state.range(0));
  std::stringstream ss;
  int i = 0;
  for (auto _ : state) {
    ss << v[i % v.size()];
    ++i;
  }
}

static void BM_to_chars_naive(benchmark::State &state) {
  const auto v = generate_ints(state.range(0));
  std::array<char, 16> buf = {};
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(buf.data());
    benchmark::DoNotOptimize(
        to_chars_naive(buf.data(), buf.data() + buf.size(), v[i % v.size()]));
    benchmark::ClobberMemory();
    ++i;
  }
}

static void BM_to_chars(benchmark::State &state) {
  const auto v = generate_ints(state.range(0));
  std::array<char, 16> buf = {};
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(buf.data());
    benchmark::DoNotOptimize(rigtorp::to_chars(
        buf.data(), buf.data() + buf.size(), v[i % v.size()]));
    benchmark::ClobberMemory();
    ++i;
  }
}

static void BM_atoi(benchmark::State &state) {
  auto v = generate_strings(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(atoi(v[i % v.size()].data()));
    ++i;
  }
}

static void BM_strtol(benchmark::State &state) {
  auto v = generate_strings(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(strtol(v[i % v.size()].data(), nullptr, 10));
    ++i;
  }
}

static void BM_stoi(benchmark::State &state) {
  auto v = generate_strings(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::stoi(v[i % v.size()]));
    ++i;
  }
}

static void BM_from_chars_unchecked(benchmark::State &state) {
  auto v = generate_strings(state.range(0));
  uint32_t val = 0;
  int i = 0;
  for (auto _ : state) {
    const auto &s = v[i % v.size()];
    benchmark::DoNotOptimize(val);
    benchmark::DoNotOptimize(
        from_chars_unchecked(s.data(), s.data() + s.size(), val));
    ++i;
  }
}

static void BM_from_chars(benchmark::State &state) {
  auto v = generate_strings(state.range(0));
  int32_t val = 0;
  int i = 0;
  for (auto _ : state) {
    const auto &s = v[i % v.size()];
    benchmark::DoNotOptimize(val);
    benchmark::DoNotOptimize(
        rigtorp::from_chars(s.data(), s.data() + s.size(), val));
    ++i;
  }
}

BENCHMARK(BM_sprintf)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_to_string)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_stringstream)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_to_chars_naive)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_to_chars)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_atoi)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_strtol)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_stoi)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_from_chars_unchecked)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK(BM_from_chars)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Arg(6)
    ->Arg(7)
    ->Arg(8)
    ->Arg(9);

BENCHMARK_MAIN();