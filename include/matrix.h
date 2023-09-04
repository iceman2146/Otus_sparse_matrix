#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <utility>

using namespace std;

template <size_t N> class Coordinates {
public:
  int dimensions[N];

public:
  Coordinates() = default;
  Coordinates(const Coordinates<N> &) = default;
  Coordinates(Coordinates<N> &&) = default;
  template <typename... Args> Coordinates(Args... args) {
    const int argc = sizeof...(args);
    int t[argc] = {(args)...};
    memcpy(dimensions, t, sizeof(dimensions));
  }

  bool operator<(const Coordinates<N> &ob) const {

    return lexicographical_compare(dimensions, dimensions + N, ob.dimensions,
                                   ob.dimensions + N);
  }
};

template <size_t N> ostream &operator<<(ostream &s, const Coordinates<N> &ob) {
  cout << "(";
  for (int i = 0; i < N; i++)
    cout << ob.dimensions[i] << ((i < N - 1) ? ";" : ")");
  return s;
}

template <typename T, const T DefVal, size_t N = 2> class Matrix {
  Coordinates<N> key;
  pair<Coordinates<N>, T> DefPair;
  T DefValForRes;
  map<Coordinates<N>, T> data;

  class flat_iterator {
    typename map<Coordinates<N>, T>::iterator it;

  public:
    flat_iterator(typename map<Coordinates<N>, T>::iterator _it) : it(_it) {}
    bool operator!=(const flat_iterator &_it) { return it != _it.it; }
    void operator++() { ++it; }
    auto &operator*() { return *it; }
  };

  class ProxyAtOnce {
    Matrix &self;
    Coordinates<N> key;

  public:
    ProxyAtOnce(Matrix &_matrix, Coordinates<N> _key)
        : self(_matrix), key(_key) {}
    ProxyAtOnce &operator=(T val) {
      if (val != DefVal)
        self.data.try_emplace(key, val);
      else
        self.data.erase(key);

      return *this;
    }

    ProxyAtOnce &operator=(const ProxyAtOnce &ob) {

      *this = static_cast<T>(ob);
      return *this;
    }

    operator T() const { return self.GetRefVal(key); }
  };

  template <int P, typename Dummy = void> class SubProxy {
  private:
    Matrix &self;

  public:
    SubProxy(Matrix &_matrix) : self(_matrix){};
    SubProxy<P + 1> operator[](int i) {
      self.key.dimensions[P + 1] = i;
      return SubProxy<P + 1>(self);
    }
  };

  template <typename Dummy> class SubProxy<N - 2, Dummy> {
    Matrix &self;

  public:
    SubProxy(Matrix &_matrix) : self(_matrix) {}
    ProxyAtOnce operator[](int i) {
      self.key.dimensions[N - 1] = i;
      return ProxyAtOnce(self, self.key);
    }
  };

public:
  size_t size() { return data.size(); }

  template <typename... Args> void SetValue(const T &Value, Args... args);

  template <typename... Args> T &GetValue(Args... args);

  template <typename... Args> T &GetRefVal(Args... args);

  T &GetRefVal(Coordinates<N> key);

  SubProxy<0> operator[](int i) {
    key.dimensions[0] = i;
    return SubProxy<0>(*this);
  }

  flat_iterator begin() { return data.begin(); }
  flat_iterator end() { return data.end(); }
};

template <typename T, T DefVal, size_t N>
template <typename... Args>
T &Matrix<T, DefVal, N>::GetRefVal(Args... args) {
  assert(N == sizeof...(args));
  int realArgs[sizeof...(args)] = {(args)...};
  Coordinates<N> key;
  memcpy(key.dimensions, realArgs, sizeof(realArgs));

  auto it = data.find(key);
  if (it != data.end()) {
    return (*it).second;
  } else {
    DefValForRes = DefVal;
    return DefValForRes;
  }
}

template <typename T, T DefVal, size_t N>
T &Matrix<T, DefVal, N>::GetRefVal(Coordinates<N> key) {
  auto it = data.find(key);
  if (it != data.end()) {
    return (*it).second;
  } else {
    DefValForRes = DefVal;
    return DefValForRes;
  }
}

template <typename T, T DefVal, size_t N>
template <typename... Args>
void Matrix<T, DefVal, N>::SetValue(const T &Value, Args... args) {
  assert(N == sizeof...(args));
  int realArgs[sizeof...(args)] = {(args)...};

  Coordinates<N> key;
  memcpy(key.dimensions, realArgs, sizeof(realArgs));

  auto it = data.find(key);

  if (it != data.end()) {
    if (Value != DefVal)
      (*it).second = Value;
    else
      data.erase(key);
  } else {

    if (Value != DefVal)
      data.try_emplace(key, Value);
  }
}

template <typename T, T DefVal, size_t N>
template <typename... Args>
T &Matrix<T, DefVal, N>::GetValue(Args... args) {
  assert(N == sizeof...(args));
  int realArgs[sizeof...(args)] = {(args)...};

  Coordinates<N> key;
  memcpy(key.dimensions, realArgs, sizeof(realArgs));

  auto it = data.find(key);

  if (it != data.end()) {
    return it->second;
  } else {
    memcpy(DefPair.first.dimensions, realArgs,
           sizeof(DefPair.first.dimensions));
    DefPair.second = DefVal;
    return DefPair.second;
  }
}
