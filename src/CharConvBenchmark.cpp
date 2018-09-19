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

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cmath>
#include <iostream>
#include <random>
#include <rigtorp/CharConv.h>
#include <sstream>

template <typename T> auto generate_ints(int digits) {
  T max = 1;
  for (int i = 0; i < digits; ++i) {
    max *= 10;
  }
  --max;
  const T min = std::is_unsigned_v<T> ? 0 : -max;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<T> dis(min, max);
  std::array<T, 4096> v = {};
  std::generate(v.begin(), v.end(), [&] {
    T res;
    do {
      res = dis(gen);
    } while (std::to_string(res).size() < static_cast<size_t>(digits));
    return res;
  });
  return v;
}

template <typename T> auto generate_strings(int digits) {
  T max = 1;
  for (int i = 0; i < digits; ++i) {
    max *= 10;
  }
  --max;
  const T min = std::is_unsigned_v<T> ? 0 : -max;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<T> dis(min, max);
  std::array<std::string, 4096> v;
  std::generate(v.begin(), v.end(), [&] {
    std::string res;
    do {
      res = std::to_string(dis(gen));
    } while (res.size() < static_cast<size_t>(digits));
    return res;
  });
  return v;
}

static bool to_chars_naive(char *first, char *last[[maybe_unused]],
                           uint32_t v) {
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
  const auto v = generate_ints<int>(state.range(0));
  std::array<char, 16> buf = {};
  int i = 0;
  for (auto _ : state) {
    sprintf(buf.data(), "%i", v[i % v.size()]);
    ++i;
  }
}

static void BM_to_string(benchmark::State &state) {
  const auto v = generate_ints<int>(state.range(0));
  std::string buf;
  int i = 0;
  for (auto _ : state) {
    std::to_string(v[i % v.size()]);
    ++i;
  }
}

static void BM_stringstream(benchmark::State &state) {
  const auto v = generate_ints<int>(state.range(0));
  std::stringstream ss;
  int i = 0;
  for (auto _ : state) {
    ss << v[i % v.size()];
    ++i;
  }
}

static void BM_rigtorp_to_chars_naive(benchmark::State &state) {
  const auto v = generate_ints<int>(state.range(0));
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

template <typename T> static void BM_rigtorp_to_chars(benchmark::State &state) {
  const auto v = generate_ints<T>(state.range(0));
  std::array<char, 32> buf = {};
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
  const auto v = generate_strings<int>(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(atoi(v[i % v.size()].data()));
    ++i;
  }
}

static void BM_strtol(benchmark::State &state) {
  const auto v = generate_strings<long>(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(strtol(v[i % v.size()].data(), nullptr, 10));
    ++i;
  }
}

static void BM_stoi(benchmark::State &state) {
  const auto v = generate_strings<int>(state.range(0));
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::stoi(v[i % v.size()]));
    ++i;
  }
}

static void BM_rigtorp_from_chars_unchecked(benchmark::State &state) {
  const auto v = generate_strings<uint32_t>(state.range(0));
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

template <typename T>
static void BM_rigtorp_from_chars(benchmark::State &state) {
  const auto v = generate_strings<T>(state.range(0));
  T val = 0;
  int i = 0;
  for (auto _ : state) {
    const auto &s = v[i % v.size()];
    benchmark::DoNotOptimize(val);
    benchmark::DoNotOptimize(
        rigtorp::from_chars(s.data(), s.data() + s.size(), val));
    ++i;
  }
}

#if __has_include(<charconv>)

#include <charconv>

template <typename T> static void BM_std_to_chars(benchmark::State &state) {
  const auto v = generate_ints<T>(state.range(0));
  std::array<char, 32> buf = {};
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(buf.data());
    benchmark::DoNotOptimize(
        std::to_chars(buf.data(), buf.data() + buf.size(), v[i % v.size()]));
    benchmark::ClobberMemory();
    ++i;
  }
}

template <typename T> static void BM_std_from_chars(benchmark::State &state) {
  const auto v = generate_strings<T>(state.range(0));
  T val = 0;
  int i = 0;
  for (auto _ : state) {
    const auto &s = v[i % v.size()];
    benchmark::DoNotOptimize(val);
    benchmark::DoNotOptimize(
        std::from_chars(s.data(), s.data() + s.size(), val));
    ++i;
  }
}

#endif

template <int N> static void Digits(benchmark::internal::Benchmark *b) {
  for (int i = 1; i <= N; ++i) {
    b->Arg(i);
  }
}

BENCHMARK(BM_sprintf)->Apply(Digits<9>);
BENCHMARK(BM_to_string)->Apply(Digits<9>);
BENCHMARK(BM_stringstream)->Apply(Digits<9>);
#if __has_include(<charconv>)
BENCHMARK_TEMPLATE(BM_std_to_chars, int32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_std_to_chars, uint32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_std_to_chars, int64_t)->Apply(Digits<19>);
BENCHMARK_TEMPLATE(BM_std_to_chars, uint64_t)->Apply(Digits<19>);
#endif
BENCHMARK(BM_rigtorp_to_chars_naive)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_to_chars, int32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_to_chars, uint32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_to_chars, int64_t)->Apply(Digits<19>);
BENCHMARK_TEMPLATE(BM_rigtorp_to_chars, uint64_t)->Apply(Digits<19>);
BENCHMARK(BM_atoi)->Apply(Digits<9>);
BENCHMARK(BM_strtol)->Apply(Digits<9>);
BENCHMARK(BM_stoi)->Apply(Digits<9>);
#if __has_include(<charconv>)
BENCHMARK_TEMPLATE(BM_std_from_chars, int32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_std_from_chars, uint32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_std_from_chars, int64_t)->Apply(Digits<19>);
BENCHMARK_TEMPLATE(BM_std_from_chars, uint64_t)->Apply(Digits<19>);
#endif
BENCHMARK(BM_rigtorp_from_chars_unchecked)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_from_chars, int32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_from_chars, uint32_t)->Apply(Digits<9>);
BENCHMARK_TEMPLATE(BM_rigtorp_from_chars, int64_t)->Apply(Digits<19>);
BENCHMARK_TEMPLATE(BM_rigtorp_from_chars, uint64_t)->Apply(Digits<19>);

BENCHMARK_MAIN();