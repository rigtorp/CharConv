# CharConv.h

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/rigtorp/HashMap/master/LICENSE)
[![Build Status](https://travis-ci.org/rigtorp/CharConv.svg?branch=master)](https://travis-ci.org/rigtorp/CharConv)

Fast integer to string and string to integer conversion functions.

## Usage

`CharConv.h` exposes two functions: `to_chars` and `from_chars`. 
The API is similar, but not identical to that of 
[to_chars](http://en.cppreference.com/w/cpp/utility/to_chars)
and
[from_chars](http://en.cppreference.com/w/cpp/utility/from_chars)
in the C++17 standard.

- `to_chars_result to_chars(char *first, char *last, int32_t value) noexcept`
- `to_chars_result to_chars(char *first, char *last, uint32_t value) noexcept`
- `to_chars_result to_chars(char *first, char *last, int64_t value) noexcept`
- `to_chars_result to_chars(char *first, char *last, uint64_t value) noexcept`

  Converts `value` into characters in base 10.  

- `from_chars_result from_chars(const char *first, const char *last, int32_t &value) noexcept`
- `from_chars_result from_chars(const char *first, const char *last, uint32_t &value) noexcept`
- `from_chars_result from_chars(const char *first, const char *last, int64_t &value) noexcept`
- `from_chars_result from_chars(const char *first, const char *last, uint64_t &value) noexcept`

  Converts the character sequence `[first, last]` representing
  a base 10 number to an integer value.

## Example

```cpp
using namespace rigtorp;

std::array<char, 32> buf = {};
to_chars(buf.begin(), buf.end(), 123));
std::cout << buf.data();

int result;
from_chars(buf.begin(), buf.end(), result);
std::cout << result; 
```

## Benchmark

Running on Intel(R) Xeon(R) CPU E5-2620 v4 @ 2.10GHz:

```
2017-12-26 15:08:09
Run on (32 X 3000 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 256K (x16)
  L3 Unified 20480K (x2)
-----------------------------------------------------------------
Benchmark                          Time           CPU Iterations
-----------------------------------------------------------------
BM_sprintf/1                      73 ns         73 ns    9625985
BM_sprintf/2                      75 ns         75 ns    9284410
BM_sprintf/3                      77 ns         77 ns    9135939
BM_sprintf/4                      79 ns         79 ns    8868328
BM_sprintf/5                      80 ns         80 ns    8716034
BM_sprintf/6                      82 ns         82 ns    8561608
BM_sprintf/7                      83 ns         83 ns    8426476
BM_sprintf/8                      85 ns         85 ns    8219398
BM_sprintf/9                      87 ns         87 ns    8016940
BM_to_string/1                    76 ns         76 ns    9159664
BM_to_string/2                    79 ns         79 ns    8852605
BM_to_string/3                    81 ns         81 ns    8612168
BM_to_string/4                    83 ns         83 ns    8388007
BM_to_string/5                    85 ns         85 ns    8289128
BM_to_string/6                    86 ns         86 ns    8124345
BM_to_string/7                    87 ns         87 ns    8049160
BM_to_string/8                    89 ns         89 ns    7870350
BM_to_string/9                    90 ns         90 ns    7744079
BM_stringstream/1                 34 ns         34 ns   20586984
BM_stringstream/2                 38 ns         38 ns   18778128
BM_stringstream/3                 39 ns         39 ns   17165452
BM_stringstream/4                 43 ns         43 ns   16696859
BM_stringstream/5                 45 ns         45 ns   16127302
BM_stringstream/6                 47 ns         47 ns   15600041
BM_stringstream/7                 51 ns         51 ns   10000000
BM_stringstream/8                 51 ns         51 ns   13379508
BM_stringstream/9                 55 ns         55 ns   12981355
BM_to_chars_naive/1               12 ns         12 ns   58830826
BM_to_chars_naive/2               14 ns         14 ns   51283290
BM_to_chars_naive/3               15 ns         15 ns   47083964
BM_to_chars_naive/4               16 ns         16 ns   43907170
BM_to_chars_naive/5               17 ns         17 ns   41087032
BM_to_chars_naive/6               18 ns         18 ns   38062908
BM_to_chars_naive/7               19 ns         19 ns   36324383
BM_to_chars_naive/8               21 ns         21 ns   33114607
BM_to_chars_naive/9               22 ns         22 ns   31397235
BM_to_chars/1                      9 ns          9 ns   77857609
BM_to_chars/2                     10 ns         10 ns   71753282
BM_to_chars/3                     11 ns         11 ns   54759461
BM_to_chars/4                     12 ns         12 ns   56404502
BM_to_chars/5                     14 ns         14 ns   50743796
BM_to_chars/6                     15 ns         15 ns   47294406
BM_to_chars/7                     16 ns         16 ns   43201370
BM_to_chars/8                     18 ns         18 ns   39207877
BM_to_chars/9                     20 ns         20 ns   35792902
BM_atoi/1                         14 ns         14 ns   48904147
BM_atoi/2                         16 ns         16 ns   42837308
BM_atoi/3                         18 ns         18 ns   39150309
BM_atoi/4                         20 ns         20 ns   35951644
BM_atoi/5                         21 ns         21 ns   33153980
BM_atoi/6                         23 ns         23 ns   30722978
BM_atoi/7                         24 ns         24 ns   28734136
BM_atoi/8                         26 ns         26 ns   26651732
BM_atoi/9                         28 ns         28 ns   25124905
BM_strtol/1                       14 ns         15 ns   48394136
BM_strtol/2                       16 ns         16 ns   42688678
BM_strtol/3                       18 ns         18 ns   39113890
BM_strtol/4                       20 ns         20 ns   35940516
BM_strtol/5                       21 ns         21 ns   33045007
BM_strtol/6                       23 ns         23 ns   30704735
BM_strtol/7                       24 ns         24 ns   28600808
BM_strtol/8                       26 ns         26 ns   26730526
BM_strtol/9                       28 ns         28 ns   25050583
BM_stoi/1                         16 ns         16 ns   44890941
BM_stoi/2                         17 ns         17 ns   40451418
BM_stoi/3                         19 ns         19 ns   37593651
BM_stoi/4                         20 ns         20 ns   34442293
BM_stoi/5                         22 ns         22 ns   31927689
BM_stoi/6                         24 ns         24 ns   29465662
BM_stoi/7                         25 ns         25 ns   27752163
BM_stoi/8                         27 ns         27 ns   25848348
BM_stoi/9                         29 ns         29 ns   24381987
BM_from_chars_unchecked/1          4 ns          4 ns  162408633
BM_from_chars_unchecked/2          5 ns          5 ns  132550992
BM_from_chars_unchecked/3          6 ns          6 ns  113085260
BM_from_chars_unchecked/4          7 ns          7 ns   96808997
BM_from_chars_unchecked/5          8 ns          8 ns   89169648
BM_from_chars_unchecked/6          8 ns          8 ns   82540712
BM_from_chars_unchecked/7          9 ns          9 ns   77455690
BM_from_chars_unchecked/8         10 ns         10 ns   73136554
BM_from_chars_unchecked/9         10 ns         10 ns   68730597
BM_from_chars/1                    9 ns          9 ns   78818376
BM_from_chars/2                   10 ns         10 ns   71176282
BM_from_chars/3                   11 ns         11 ns   63257070
BM_from_chars/4                   12 ns         12 ns   56995175
BM_from_chars/5                   14 ns         14 ns   50584861
BM_from_chars/6                   15 ns         15 ns   47135520
BM_from_chars/7                   16 ns         16 ns   43457090
BM_from_chars/8                   17 ns         17 ns   40382499
BM_from_chars/9                   19 ns         19 ns   37375537
```

## About

This project was created by [Erik Rigtorp](http://rigtorp.se)
<[erik@rigtorp.se](mailto:erik@rigtorp.se)>.
