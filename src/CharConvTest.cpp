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

#include "../include/CharConv.h"
#include <catch2/catch.hpp>

template <typename T> bool check_to_chars(T value, std::string_view expected) {
  using namespace rigtorp;
  std::array<char, 32> buf = {};
  const auto res = to_chars(buf.begin(), buf.end(), value);
  return res.ec == std::errc{} && (res.ptr == buf.end() || *res.ptr == 0) &&
         buf.data() == expected;
}

TEST_CASE("to_chars") {
  using namespace rigtorp;

  SECTION("min") {
    CHECK(check_to_chars(std::numeric_limits<int32_t>::min(), "-2147483648"));
    CHECK(check_to_chars(std::numeric_limits<uint32_t>::min(), "0"));
  }

  SECTION("max") {
    CHECK(check_to_chars(std::numeric_limits<int32_t>::max(), "2147483647"));
    CHECK(check_to_chars(std::numeric_limits<uint32_t>::max(), "4294967295"));
  }

  SECTION("0") {
    CHECK(check_to_chars<int32_t>(0, "0"));
    CHECK(check_to_chars<uint32_t>(0, "0"));
  }

  SECTION("log10") {
    int val = 1;
    for (int i = 0; i < 10; ++i) {
      val *= 10;
      CHECK(check_to_chars<int32_t>(val, std::to_string(val)));
      CHECK(check_to_chars<int32_t>(val - 1, std::to_string(val - 1)));
      CHECK(check_to_chars<int32_t>(-val, std::to_string(-val)));
      CHECK(check_to_chars<int32_t>(-val + 1, std::to_string(-val + 1)));
      CHECK(check_to_chars<uint32_t>(val, std::to_string(val)));
      CHECK(check_to_chars<uint32_t>(val - 1, std::to_string(val - 1)));
    }
  }

  SECTION("overflow") {
    std::array<char, 32> buf = {};
    auto res = to_chars(buf.begin(), buf.begin() + 3, 1000);
    CHECK(res.ec == std::errc::value_too_large);
    CHECK(res.ptr == buf.begin() + 3);
    CHECK(std::string("") == buf.data());
    res = to_chars(buf.begin(), buf.begin() + 4, 1000);
    CHECK(res.ec == std::errc{});
    CHECK(res.ptr == buf.begin() + 4);
    CHECK(std::string("1000") == buf.data());
  }
}

template <typename T> bool check_from_chars(T expected, std::string_view s) {
  T value;
  auto r = rigtorp::from_chars(s.begin(), s.end(), value);
  return r.ec == std::errc{} && r.ptr == s.end() && value == expected;
}

TEST_CASE("from_chars") {
  using namespace rigtorp;

  SECTION("min") {
    CHECK(check_from_chars(std::numeric_limits<int32_t>::min(), "-2147483648"));
    CHECK(check_from_chars(std::numeric_limits<uint32_t>::min(), "0"));
  }

  SECTION("max") {
    CHECK(check_from_chars(std::numeric_limits<int32_t>::max(), "2147483647"));
    CHECK(check_from_chars(std::numeric_limits<uint32_t>::max(), "4294967295"));
  }

  SECTION("0") {
    CHECK(check_from_chars<int32_t>(0, "0"));
    CHECK(check_from_chars<uint32_t>(0, "0"));
  }

  SECTION("log10") {
    int val = 1;
    for (int i = 0; i < 10; ++i) {
      val *= 10;
      CHECK(check_from_chars<int32_t>(val, std::to_string(val)));
      CHECK(check_from_chars<int32_t>(val - 1, std::to_string(val - 1)));
      CHECK(check_from_chars<int32_t>(-val, std::to_string(-val)));
      CHECK(check_from_chars<int32_t>(-val + 1, std::to_string(-val + 1)));
      CHECK(check_from_chars<uint32_t>(val, std::to_string(val)));
      CHECK(check_from_chars<uint32_t>(val - 1, std::to_string(val - 1)));
    }
  }

  SECTION("zero padded") {
    CHECK(check_from_chars(std::numeric_limits<int32_t>::min(),
                           "-000000000002147483648"));
    CHECK(check_from_chars(std::numeric_limits<uint32_t>::min(),
                           "0000000000000000000000"));
    CHECK(check_from_chars(std::numeric_limits<int32_t>::max(),
                           "0000000000002147483647"));
    CHECK(check_from_chars(std::numeric_limits<uint32_t>::max(),
                           "0000000000004294967295"));
  }

  SECTION("invalid") {
    int32_t i = 999;
    std::string_view s;

    s = "";
    auto r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin());
    CHECK(i == 999);

    s = "*";
    r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin());
    CHECK(i == 999);

    s = "-";
    r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin() + 1);
    CHECK(i == 999);

    s = "-*";
    r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin() + 1);
    CHECK(i == 999);

    s = "2147483648";
    r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::result_out_of_range);
    CHECK(r.ptr == s.end());
    CHECK(i == 999);

    s = "-2147483649";
    r = from_chars(s.begin(), s.end(), i);
    CHECK(r.ec == std::errc::result_out_of_range);
    CHECK(r.ptr == s.end());
    CHECK(i == 999);

    uint32_t u = 888;
    s = "";
    r = from_chars(s.begin(), s.end(), u);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin());
    CHECK(u == 888);

    s = "*";
    r = from_chars(s.begin(), s.end(), u);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin());
    CHECK(u == 888);

    s = "-*";
    r = from_chars(s.begin(), s.end(), u);
    CHECK(r.ec == std::errc::invalid_argument);
    CHECK(r.ptr == s.begin());
    CHECK(u == 888);

    s = "4294967296";
    r = from_chars(s.begin(), s.end(), u);
    CHECK(r.ec == std::errc::result_out_of_range);
    CHECK(r.ptr == s.end() - 1);
    CHECK(u == 888);
  }
}

TEST_CASE("all", "[.]") {
  for (int32_t i = std::numeric_limits<int32_t>::min();
       i < std::numeric_limits<int32_t>::max(); ++i) {
    if (!check_to_chars(i, std::to_string(i))) {
      FAIL(i);
    }
    if (!check_from_chars(i, std::to_string(i))) {
      FAIL(i);
    }
  }
  for (uint32_t i = std::numeric_limits<uint32_t>::min();
       i < std::numeric_limits<uint32_t>::max(); ++i) {
    if (!check_to_chars(i, std::to_string(i))) {
      FAIL(i);
    }
    if (!check_from_chars(i, std::to_string(i))) {
      FAIL(i);
    }
  }
}
