// Copyright 2021 Ioannis Angelis <john_agelis@hotmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ============================================================
//
// ChangeLog:
//   0.0.7    linspace, pad_left, pad_right
//            zip_with, zip_to_pair
//            remove zip,
//            GetVectorsSize, map
//            takeFromIdx, Slice, chunks
//            Some
//   0.0.6    mod, findfirst, findall,
//            panic, unwrap_or_panic
//   0.0.5    map, take, filter, var,
//            arange, normalize, abs,
//            zip, pad<vector>
//            operators *,<, > for vector
//            where, argsort
//   0.0.4    remove Particle,
//            println for std::vector<T>
//   0.0.3    vec_from_range
//   0.0.2    pop, drop, readlines
//   0.0.1    Maybe<T>, StringView,
//            Particles_t, Matrix
//
// ============================================================

#ifndef PFT_H_
#define PFT_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring> // for memset
#include <deque>
#include <limits>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#ifdef PFT_USE_ROOT
#include <TLorentzVector.h>
#include <TMath.h>
#endif

// Unsigned
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Signed
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// Floating point
using f32 = float;
using f64 = double;

//////////////////////////////////////////////////
// Operators for vector<T>
//////////////////////////////////////////////////
template <typename T>
std::vector<T> operator*(const T& elem, const std::vector<T>& rhs) {
  std::vector<T> res(rhs.size(), 0);
  for (std::size_t i = 0; i < rhs.size(); ++i) {
    res[i] = elem * rhs[i];
  }
  return res;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& rhs, const T& elem) {
  std::vector<T> res(rhs.size(), 0);
  for (std::size_t i = 0; i < rhs.size(); ++i) {
    res[i] = rhs[i] / elem;
  }
  return res;
}

template <typename T>
std::vector<i32> operator<(const std::vector<T>& lhs, const T& elem) {
  std::vector<i32> res(lhs.size());

  for (std::size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] < elem) {
      res[i] = 1;
    } else {
      res[i] = 0;
    }
  }
  return res;
}

template <typename T>
std::vector<i32> operator>(const T& elem, const std::vector<T>& rhs) {
  return rhs < elem;
}

template <typename T>
std::vector<i32> operator>(const std::vector<T>& lhs, const T& elem) {
  std::vector<i32> res(lhs.size());

  for (std::size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] > elem) {
      res[i] = 1;
    } else {
      res[i] = 0;
    }
  }
  return res;
}

template <typename T>
std::vector<i32> operator<(const T& elem, const std::vector<T>& rhs) {
  return rhs > elem;
}

template <typename T>
std::vector<i32> operator==(const std::vector<T>& lhs, const T& elem) {
  std::vector<i32> ret(lhs.size(), 1);

  for (std::size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != elem) {
      ret[i] = 0;
    }
  }
  return ret;
}
namespace pft {

//////////////////////////////////////////////////
// Slice
//////////////////////////////////////////////////
struct Slice {
  i64 start;
  i64 stop;
  constexpr auto size() const -> std::size_t { return stop - start; }
};

// Maybe and StringView are based on https://github.com/rexim/aids
//////////////////////////////////////////////////
// Maybe
//////////////////////////////////////////////////
template <typename T>
struct Maybe {
  bool has_value;
  T unwrap;

  constexpr Maybe() : has_value(false) {}
  constexpr Maybe(bool f, T& val) : has_value(f), unwrap(val) {}
  constexpr Maybe(bool f, T&& val) : has_value(f), unwrap(std::move(val)) {}

  constexpr bool operator!=(const Maybe<T>& that) const {
    return !(*this == that);
  }

  constexpr bool operator==(const Maybe<T>& that) const {
    if (this->has_value && that.has_value) {
      return this->unwrap == that.unwrap;
    }

    return !this->has_value && !that.has_value;
  }

  template <typename F>
  constexpr auto bind(F&& f) const {
    if (has_value) {
      return Maybe(true, f(unwrap));
    }
    return Maybe();
  }
};

template <typename T>
constexpr auto Some(T m) -> Maybe<T> {
  return {true, m};
}

//////////////////////////////////////////////////
// StringView
//////////////////////////////////////////////////
struct StringView : public std::string_view {

  StringView() : std::string_view("") {}
  StringView(const std::string& s) : std::string_view(s) {}
  StringView(const std::string_view& s) : std::string_view(s) {}
  StringView(std::string&& s) : std::string_view(std::move(s)) {}
  StringView(std::string_view&& s) : std::string_view(s) {}
  StringView(const char* s) : std::string_view(s) {}
  StringView(const char* s, std::size_t l) : std::string_view(s, l) {}

  StringView chop(std::size_t n) {
    if (this->size() > n) {
      return this->substr(n);
    } else {
      return {};
    }
  }

  StringView chop_by_delim(char delim) {
    std::size_t i = 0;
    while (i < this->size() && this->data()[i] != delim) {
      ++i;
    }
    StringView result = {this->data(), i};
    this->remove_prefix(i + 1);
    return result;
  }

  std::string as_string() const { return std::string(*this); }
};

//////////////////////////////////////////////////
// Particles struct usefull for Geant4
//////////////////////////////////////////////////
struct Particles_t {
  std::vector<i32> det_id, parent_id, trid, n_secondaries;
  std::vector<f64> times, edep, energy, posX, posY, posZ;
  std::vector<f64> theta, phi, trlen;

  void Reserve(const std::size_t nparticles) {
    det_id.reserve(nparticles);
    parent_id.reserve(nparticles);
    trid.reserve(nparticles);
    times.reserve(nparticles);
    edep.reserve(nparticles);
    energy.reserve(nparticles);
    posX.reserve(nparticles);
    posY.reserve(nparticles);
    posZ.reserve(nparticles);
    theta.reserve(nparticles);
    phi.reserve(nparticles);
    trlen.reserve(nparticles);
    n_secondaries.reserve(nparticles);
  }

  void ClearVecs() {
    det_id.clear();
    parent_id.clear();
    trid.clear();
    times.clear();
    edep.clear();
    energy.clear();
    posX.clear();
    posY.clear();
    posZ.clear();
    theta.clear();
    phi.clear();
    trlen.clear();
    n_secondaries.clear();
  }
};

// StringView utilities
static inline StringView operator""_sv(const char* data, std::size_t count) {
  return {data, count};
}

static inline StringView& trimr(StringView& s) {
  auto i = s.find_last_not_of(" \t\n\r\f\v");
  if (i != std::string_view::npos) {
    s = s.substr(0, i + 1);
  }

  return s;
}

static inline StringView& triml(StringView& s) {
  auto i = s.find_first_not_of(" \t\n\r\f\v");
  if (i != std::string_view::npos) {
    s.remove_prefix(i);
  }

  return s;
}

static inline std::vector<StringView> split_by(StringView view, char delim) {
  view = trimr(view);
  std::vector<StringView> ret;
  while (!view.empty()) {
    auto len = view.find(delim);
    if (len == std::string_view::npos) {
      ret.emplace_back(view);
      break;
    }
    ret.emplace_back(view.substr(0, len));
    view = view.substr(len + 1);
  }
  return ret;
}

static inline std::vector<StringView> split_by(std::string& str, char delim) {
  std::vector<StringView> vec;
  StringView temp{str.data(), str.size()};
  StringView aug = {};
  while (!temp.empty()) {
    aug = temp.chop_by_delim(delim);
    vec.emplace_back(aug.data(), aug.size());
  }
  return vec;
}

static inline StringView cstr_as_sv(const char* cstr) {
  return {cstr, strlen(cstr)};
}

static inline StringView string_as_sv(const std::string& s) {
  return {s.data(), s.length()};
}

static inline i32 to_int(StringView s) { return std::stoi(std::string(s)); }

static inline Maybe<StringView> read_file_as_string_view(const char* filename) {
  FILE* f = fopen(filename, "rb");
  if (f == nullptr) {
    return {};
  }

  int err = fseek(f, 0, SEEK_END);
  if (err < 0) {
    return {};
  }

  long size = ftell(f);
  if (size < 0) {
    return {};
  }

  err = fseek(f, 0, SEEK_SET);
  if (err < 0) {
    return {};
  }

  auto data = malloc(size);
  if (data == nullptr) {
    return {};
  }

  std::size_t read_size = fread(data, 1, size, f);
  if (read_size != (std::size_t)size && ferror(f) != 0) {
    return {};
  }

  fclose(f);
  return {true,
          {static_cast<const char*>(data), static_cast<std::size_t>(size)}};
}

static inline std::vector<StringView> readlines(const char* filename,
                                                const char delim = '\n') {

  auto file   = read_file_as_string_view(filename);
  auto result = split_by(file.unwrap, delim);
  return result;
}

static inline void ignore_header_lines(std::vector<StringView>& vec,
                                       int lines) {
  vec.erase(vec.begin(), vec.begin() + lines);
}

static inline std::vector<float> as_floats(const std::vector<StringView>& vec) {
  std::vector<float> buffer(vec.size());

  for (std::size_t i = 0; i < vec.size(); ++i) {
    buffer[i] = std::stof(std::string(vec[i]));
  }
  return buffer;
}

//////////////////////////////////////////////////
// Matrix
//////////////////////////////////////////////////
template <typename T>
struct Matrix {
  std::size_t rows, cols;
  T* data{nullptr};

  constexpr Matrix() : rows(0), cols(0), data(nullptr) {}
  constexpr Matrix(std::size_t r, std::size_t c) : rows(r), cols(c) {
    data = new T[r * c]();
  }
  constexpr Matrix(const Matrix<T>& m) : rows(m.rows), cols(m.cols) {
    data = new T[rows * cols]();
    std::memcpy(data, m.data, sizeof(T) * rows * cols);
  }

  template <std::size_t r, std::size_t c>
  constexpr Matrix(const T (&m)[r][c]) : Matrix(r, c) {
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < cols; ++j) {
        (*this)(i, j) = m[i][j];
      }
    }
  }

  ~Matrix() { delete[] this->data; }

  constexpr T& operator()(std::size_t i, std::size_t j) {
    return data[j + i * cols];
  }
  constexpr T operator()(std::size_t i, std::size_t j) const {
    return data[j + i * cols];
  }

  constexpr Matrix<T>& operator=(const Matrix<T>& a) {
    if (this == &a) {
      return *this;
    }
    if (data != nullptr) {
      delete[] data;
    }
    rows       = a.rows;
    cols       = a.cols;
    this->data = new T[rows * cols]();
    std::memcpy(data, a.data, sizeof(T) * rows * cols);
    return *this;
  }

  constexpr void diagonal(T d = (T)1.0) {
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < cols; ++j) {
        if (i == j) {
          (*this)(i, j) = d;
        }
      }
    }
  }

  constexpr T trace() const {
    T tr = 0;
    if (cols == rows) {
      for (std::size_t i = 0; i < cols; ++i) {
        tr += ((*this)(i, i));
      }
      return tr;
    } else {
      return std::numeric_limits<T>::quiet_NaN();
    }
  }

  constexpr Matrix<T> transpose() const {
    Matrix<T> result(cols, rows);
    for (std::size_t i = 0; i < cols; ++i) {
      for (std::size_t j = 0; j < rows; ++j) {
        result(i, j) = ((*this)(j, i));
      }
    }
    return result;
  }

  constexpr Matrix<T> mult(const Matrix<T>& b) {
    if (cols != b.rows) {
      exit(1);
    }
    Matrix<T> ret(rows, b.cols);
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < b.cols; ++j) {
        for (std::size_t k = 0; k < cols; ++k) {
          ret(i, j) += ((*this)(i, k)) * b(k, j);
        }
      }
    }
    return ret;
  }

  constexpr std::vector<T> getColumn(std::size_t c) const {
    std::vector<T> ret(rows);
    for (std::size_t i = 0; i < rows; ++i) {
      ret[i] = (*this)(i, c);
    }
    return ret;
  }

  constexpr Matrix<T> GetMinor(std::size_t d) const {
    Matrix<T> ret(rows, cols);
    for (std::size_t i = 0; i < d; ++i) {
      ret(i, i) = 1;
    }

    for (std::size_t i = d; i < rows; ++i) {
      for (std::size_t j = d; j < cols; ++j) {
        ret(i, j) = (*this)(i, j);
      }
    }

    return ret;
  }
};

//////////////////////////////////////////////////
// Printers
//////////////////////////////////////////////////
static inline void print1(FILE* stream, char x) { fputc(x, stream); }
static inline void print1(FILE* stream, char* x) {
  fwrite(x, 1, std::strlen(x), stream);
}
static inline void print1(FILE* stream, const char* x) {
  fwrite(x, 1, std::strlen(x), stream);
}

static inline void print1(FILE* stream, u32 x) { fprintf(stream, "%u", x); }
static inline void print1(FILE* stream, u64 x) { fprintf(stream, "%lu", x); }
// static inline void print1(FILE* stream, std::size_t x) { fprintf(stream, "%zu", x); }

static inline void print1(FILE* stream, i16 x) { fprintf(stream, "%hd", x); }
static inline void print1(FILE* stream, i32 x) { fprintf(stream, "%d", x); }
static inline void print1(FILE* stream, i64 x) { fprintf(stream, "%ld", x); }

static inline void print1(FILE* stream, f32 f) { fprintf(stream, "%8.4f", f); }
static inline void print1(FILE* stream, f64 f) { fprintf(stream, "%8.4f", f); }

static inline void print1(FILE* stream, StringView view) {
  fwrite(view.data(), 1, view.size(), stream);
}

// NOTE: forward declare to use Maybe<std::vector>
template <typename T>
static inline void print1(FILE* stream, const std::vector<T>& v);

template <typename T>
static inline void print1(FILE* stream, const Maybe<T>& m) {
  print1(stream, "Maybe{ ");
  print1(stream, m.has_value);
  print1(stream, ", ");
  print1(stream, m.unwrap);
  print1(stream, " }");
}

template <typename T, typename U>
static inline void print1(FILE* stream, const std::pair<T, U>& pr) {
  print1(stream, "(");
  print1(stream, pr.first);
  print1(stream, ", ");
  print1(stream, pr.second);
  print1(stream, ")");
}

template <typename T>
static inline void print1(FILE* stream, const std::vector<T>& v) {
  const std::size_t n = v.size();
  fprintf(stream, "vector(size=%zu) ", v.size());
  print1(stream, "{");
  for (std::size_t i = 0; i < n - 1; ++i) {
    print1(stream, v[i]);
    print1(stream, ", ");
  }
  print1(stream, v[n - 1]);
  print1(stream, "}");
}

// TODO: make pretty printer for matrices
template <typename T>
void print1(FILE* stream, const Matrix<T>& mat) {
  for (std::size_t i = 0; i < mat.rows; ++i) {
    for (std::size_t j = 0; j < mat.cols; ++j) {
      print1(stream, mat(i, j));
      print1(stream, "  ");
    }
    fprintf(stream, "\n");
  }
}

template <typename... Types>
static inline void print(FILE* stream, Types... args) {
  (print1(stream, args), ...);
}

template <typename... Types>
static inline void println(FILE* stream, Types... args) {
  print(stream, args...);
  print1(stream, '\n');
}

// stolen from https://github.com/rexim/aids/blob/master/aids.hpp
template <typename... Args>
[[noreturn]] void panic(Args... args) {
  println(stderr, args...);
  exit(1);
}

// stolen from https://github.com/rexim/aids/blob/master/aids.hpp
template <typename T, typename... Args>
T unwrap_or_panic(Maybe<T> maybe, Args... args) {
  if (!maybe.has_value) {
    panic(args...);
  }

  return maybe.unwrap;
}

//////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////
// http://reedbeta.com/blog/python-like-enumerate-in-cpp17/
template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T&& iterable) {
  struct iterator {
    std::size_t i;
    TIter iter;
    bool operator!=(const iterator& other) const { return iter != other.iter; }
    void operator++() {
      ++i;
      ++iter;
    }
    auto operator*() const { return std::tie(i, *iter); }
  };
  struct iterable_wrapper {
    T iterable;
    auto begin() { return iterator{0, std::begin(iterable)}; }
    auto end() { return iterator{0, std::end(iterable)}; }
  };
  return iterable_wrapper{std::forward<T>(iterable)};
}

template <typename F, typename T, typename U>
auto zip_with(F&& fn, const std::vector<T>& a, const std::vector<U>& b)
    -> std::vector<decltype(fn(a[0], b[0]))> {
  const std::size_t n = std::min(a.size(), b.size());
  std::vector<decltype(fn(a[0], b[0]))> ret(n);
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = fn(a[i], b[i]);
  }
  return ret;
}

template <typename T, typename U>
auto zip_to_pair(const std::vector<T>& a, const std::vector<U>& b) {
  auto MakePair = [](const T& x, const U& y) { return std::make_pair(x, y); };

  return zip_with(MakePair, a, b);
}

template <typename T, typename R, typename FoldOp>
R foldl(const R& i, const std::vector<T>& xs, FoldOp fn) {
  auto ret = i;
  for (const auto& x : xs) {
    ret = fn(ret, x);
  }
  return ret;
}

template <typename T>
static inline T sum(const std::vector<T>& xs) {
  return std::accumulate(std::cbegin(xs), std::cend(xs), T());
  // return foldl(T(), xs, [](const T& a, const T& b) -> T { return a + b; });
}

template <typename T>
static inline double mean(const std::vector<T>& xs) {
  return double(sum(xs) / (xs.size()));
}

// using the two pass formula
template <typename T>
static inline double var(const std::vector<T>& xs) {
  const std::size_t n = xs.size();
  if (n < std::size_t(2)) {
    fprintf(stderr, "Need atleast 2 elements for variance\n");
    return 0;
  }
  double sum_squares = 0.0, squared_sum = 0.0;
  auto xs_mean = mean(xs);
  auto pred    = [&sum_squares, &squared_sum, &xs_mean](const T& x) {
    squared_sum += x - xs_mean;
    sum_squares += (x - xs_mean) * (x - xs_mean);
  };
  std::for_each(std::cbegin(xs), std::end(xs), pred);
  const auto N = (double)n;
  return (sum_squares - squared_sum * squared_sum / N) / (N - 1);
}

template <typename T>
static inline T stdev(const std::vector<T>& xs) {
  return sqrt(var(xs));
}

template <typename... Types>
static inline std::size_t GetVectorsSize(const std::vector<Types>&... vs) {
  constexpr const auto nArgs = sizeof...(Types);
  const std::size_t sizes[]  = {vs.size()...};
  if (nArgs > 1) {
    for (std::size_t i = 1; i < nArgs; ++i) {
      if (sizes[0] == sizes[i]) {
        continue;
      }
      pft::panic("Vectors have different lengths");
    }
  }
  return sizes[0];
}

template <typename F, typename T>
static inline auto map(F&& fn, const std::vector<T>& input)
    -> std::vector<decltype(fn(input[0]))> {
  const auto size = input.size();
  std::vector<decltype(fn(input[0]))> ret(size);
  std::transform(input.begin(), input.end(), ret.begin(), fn);
  return ret;
}

template <typename F, typename... Types>
static inline auto map(F&& fn, const std::vector<Types>&... input)
    -> std::vector<decltype(fn(input[0]...))> {
  const auto size = GetVectorsSize(input...);
  std::vector<decltype(fn(input[0]...))> ret(size);
  for (std::size_t i = 0; i < size; ++i) {
    ret[i] = fn(input[i]...);
  }
  return ret;
}

template <typename F, typename T>
std::vector<T> filter(F&& fn, const std::vector<T>& v) {
  const auto n = v.size();
  std::vector<T> ret;
  ret.reserve(n);
  for (auto&& val : v) {
    if (fn(val)) {
      ret.emplace_back(val);
    }
  }
  return ret;
}

template <typename T>
void pop(std::vector<T>& vec, std::size_t elements) {
  vec.erase(vec.begin(), vec.begin() + elements);
}

template <typename T>
void drop(std::vector<T>& vec, std::size_t elements) {
  for (std::size_t i = 0; i < elements; ++i) {
    vec.pop_back();
  }
}

template <typename T>
static inline std::vector<T> take(const std::vector<T>& vec,
                                  std::size_t elements) {
  return {vec.begin(), vec.begin() + elements};
}

template <typename T>
static inline std::vector<T> take(const std::vector<T>& vec,
                                  const Slice& slice) {
  return std::vector<T>{
      vec.begin() + slice.start,
      vec.begin() + (slice.stop >= vec.size() ? vec.size() : slice.stop)};
}

template <typename T>
static inline std::vector<T> take(const std::vector<T>& vec,
                                  const std::vector<int>& indices) {
  const std::size_t n = indices.size();
  std::vector<T> ret;
  ret.reserve(n);

  auto l = [&vec](const int& i) { return vec[i]; };
  std::transform(indices.begin(), indices.end(), std::back_inserter(ret), l);
  return ret;
}

template <typename T>
static inline std::vector<T> takeFromIdx(const std::vector<T>& vec,
                                         const std::vector<i32>& keep_indices) {
  const std::size_t n = keep_indices.size();
  std::vector<T> ret;
  ret.reserve(n);

  for (std::size_t i = 0; i < n; ++i) {
    if (keep_indices[i]) {
      ret.emplace_back(vec[i]);
    }
  }
  return ret;
}

template <typename T>
std::vector<T> vec_from_range(i64 low, i64 high) {
  std::size_t elements = 0;
  if (low < 0) {
    elements = high - low + 1;
  } else {
    elements = high + low + 1;
  }
  std::vector<T> v(elements);
  i64 i = low;
  for (auto& x : v) {
    x = (T)i;
    ++i;
  }
  return v;
}

template <typename T>
static inline std::vector<T> arange(T start, T stop, T step = 1) {
  // TODO: this is bad if T=unsigned type
  const auto n = std::abs(stop - start);
  std::vector<T> values;
  values.reserve(n);

  if (step > 0) {
    for (T value = start; value < stop; value += step) {
      values.emplace_back(value);
    }
  } else {
    for (T value = start; value > stop; value += step) {
      values.emplace_back(value);
    }
  }

  return values;
}

template <typename T>
static inline std::vector<T> linspace(T start, T stop, std::size_t num = 50,
                                      bool endpoint = true) {

  if (num == 0) {
    return std::vector<T>(1, 0);
  }

  if (num == 1) {
    std::vector<T> values = {start};
    return values;
  }

  if (endpoint) {
    if (num == 2) {
      std::vector<T> values = {start, stop};
      return values;
    }

    std::vector<T> values(num, 0);
    values[0]       = start;
    values[num - 1] = stop;
    const T step    = (stop - start) / static_cast<T>(num - 1);

    for (std::size_t i = 1; i < num - 1; ++i) {
      values[i] = start + static_cast<T>(i) * step;
    }
    return values;
  }

  if (num == 2) {
    const T step          = (stop - start) / num;
    std::vector<T> values = {start, start + step};
    return values;
  }
  std::vector<T> values(num, 0);
  values[0] = start;

  const T step = (stop - start) / static_cast<T>(num);

  for (std::size_t i = 1; i < num; ++i) {
    values[i] = start + static_cast<T>(i) * step;
  }

  return values;
}

template <typename ContainerIn,
          typename ContainerOut = std::vector<ContainerIn>>
static inline ContainerOut chunks(std::size_t n, const ContainerIn& xs) {
  const auto input_size = xs.size();
  auto ids              = pft::arange<i64>(0, (i64)input_size, n);
  auto N                = input_size / n;

  N += input_size % n != 0;
  ids.push_back(input_size);

  ContainerOut ret(N);
  std::transform(ret.begin(), ret.end(), ret.begin(),
                 [i = 0, &xs, &ids](auto) mutable {
                   return take(xs, pft::Slice{ids[i], ids[i++ + 1]});
                 });
  return ret;
}

template <typename T>
static inline auto norm(const std::vector<T>& xs) {
  auto sum2 =
      foldl(T(), xs, [](const T& a, const T& b) -> T { return a + b * b; });
  return std::sqrt(sum2);
}

template <typename T>
static inline auto normalise(const std::vector<T>& xs) {
  const auto d = norm(xs);
  return map([&d](const T& a) -> T { return a / d; }, xs);
}

template <typename T>
static inline auto abs(std::vector<T>& vec) {
  auto abs_lambda = [](auto x) { return std::abs(x); };
  return map(abs_lambda, vec);
}

template <typename T>
std::vector<T> pad_right(const std::vector<T>& in, std::size_t padwidth = 1,
                         T pad_value = 0) {
  auto out_size = padwidth + in.size();
  std::vector<T> out(out_size, pad_value);
  std::transform(in.begin(), in.end(), out.begin(),
                 [](const auto& x) { return x; });
  return out;
}

template <typename T>
std::vector<T> pad_left(const std::vector<T>& in, std::size_t padwidth = 1,
                        T pad_value = 0) {
  auto out = pad_right(in, padwidth, pad_value);
  std::rotate(out.begin(), out.end() - padwidth, out.end());
  return out;
}

template <typename T>
std::vector<T> pad(const std::vector<T>& in, std::size_t padwidth = 1,
                   T pad_value = 0) {
  auto out_size = 2 * padwidth + in.size();
  std::vector<T> out(out_size, pad_value);
  std::transform(in.begin(), in.end(), out.begin() + padwidth,
                 [](const auto& x) { return x; });

  return out;
}

template <typename T>
std::vector<T> where(const std::vector<i32>& c, const std::vector<T>& v1,
                     const std::vector<T>& v2) {
  const std::size_t n = c.size();
  std::vector<T> ret(n);
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = (c[i] != 0 ? v1[i] : v2[i]);
  }

  return ret;
}

template <typename T>
std::vector<T> where(const std::vector<i32>& c, const std::vector<T>& v1,
                     T v2) {
  const std::size_t n = c.size();
  std::vector<T> ret(n);
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = (c[i] != 0 ? v1[i] : v2);
  }

  return ret;
}

template <typename T>
std::vector<T> where(const std::vector<i32>& c, T v1,
                     const std::vector<T>& v2) {
  const std::size_t n = c.size();
  std::vector<T> ret(n);
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = (c[i] != 0 ? v1 : v2[i]);
  }

  return ret;
}

template <typename T>
std::vector<T> where(const std::vector<i32>& c, T v1, T v2) {
  const std::size_t n = c.size();
  std::vector<T> ret(n);
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = (c[i] != 0 ? v1 : v2);
  }

  return ret;
}

template <typename T>
std::vector<int> argsort(const std::vector<T>& xs) {
  auto idx     = arange<int>(0, xs.size());
  const auto f = [&xs](std::size_t i1, std::size_t i2) {
    return xs[i1] < xs[i2];
  };
  std::sort(idx.begin(), idx.end(), f);

  return idx;
}

//////////////////////////////////////////////////
// Arg Parse
//////////////////////////////////////////////////
struct ArgOption {
  std::string short_op;
  std::string long_op;
  std::string msg;
  bool accepts_value;
};

struct AParse {
  using Value = std::string;
  std::size_t nArgs;
  std::deque<char*> args;
  std::vector<ArgOption> flags;
  std::map<std::string, std::pair<ArgOption, std::string>> arg_table;

  AParse(int argc, char** a) : nArgs(argc) {

    for (std::size_t i = 0; i < nArgs; ++i) {
      args.push_back(a[i]);
    }
  }

  void PrintArgv() {
    for (std::size_t i = 1; i < nArgs; ++i) {
      // printf("%s\n", args[i]);
    }
  }

  void Parse() {
    if (nArgs == 1) {
      PrintUsage();
      return;
    }

    for (auto [i, arg] : ::pft::enumerate(args)) {
      for (std::size_t j = 0; j < flags.size(); ++j) {
        if (arg == flags[j].short_op || arg == flags[j].long_op) {
          if (flags[j].accepts_value) {
            if (i + 1 >= nArgs) {
              fprintf(stderr, "Missing argument for flag `%s` \n", arg);
              return;
            } else {
              arg_table.insert({flags[j].short_op, {flags[j], args[i + 1]}});
              args.pop_front();
            }
          } else {
            arg_table.insert({flags[j].short_op, {flags[j], ""}});
            args.pop_front();
            fprintf(stderr, "TODO: handle non-accepting value args\n");
          }
        }
      }
    }

    // fprintf(stdout, "[DBG]: %zu\n", options.size());
  }

  void Add(ArgOption opt) { flags.push_back(opt); }

  void PrintUsage() {
    for (auto& op : flags) {
      println(stdout, op.short_op, ", ", op.long_op, "\t:", op.msg);
    }
  }

  Maybe<std::string> value_of(std::string flag) {
    // TODO: check if flag exist
    auto val = arg_table.find(flag);

    if (val != arg_table.end()) {
      println(stdout, "[DBG]: ", val->second.second.c_str());
      return {val->second.first.accepts_value, val->second.second};
    } else {
      return {false, ""};
    }
  }
};

// sane mod function
template <typename T>
T mod(T a, T b) {
  return (a % b + b) % b;
}

// return the index
template <typename T, typename Op>
Maybe<i64> findfirst(Op&& fn, const std::vector<T>& h) {
  auto res = std::find(h.begin(), h.end(), fn);
  if (res != h.end()) {
    return {1, std::distance(h.begin(), res)};
  } else {
    return {0, -1};
  }
}

// return the indices
template <typename T, typename Op>
std::vector<i64> findall(Op&& fn, const std::vector<T>& h) {
  const auto n = h.size();
  std::vector<i64> ret;
  ret.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (fn(h[i])) {
      ret.emplace_back(i);
    }
  }
  return ret;
}

} // namespace pft

#endif // PFT_H_
