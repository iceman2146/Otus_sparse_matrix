#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <iterator>
#include <utility>
#include "Coordinate_struct.h"

template <typename T, const T DefVal, size_t N = 2>
class Matrix
{
  Coordinates<N> key;
  std::pair<Coordinates<N>, T> DefPair;
  T DefValForRes;
  std::map<Coordinates<N>, T> data;

public:
  size_t size() { return data.size(); }
  template <typename... Args>
  void SetValue(const T &Value, Args... args)
  {
    assert(N == sizeof...(args));
    int realArgs[sizeof...(args)] = {(args)...};

    Coordinates<N> key;
    memcpy(key.dimensions, realArgs, sizeof(realArgs));

    auto iter = data.find(key);

    if (iter != data.end())
    {
      if (Value != DefVal)
        (*iter).second = Value;
      else
        data.erase(key);
    }
    else
    {

      if (Value != DefVal)
        data.try_emplace(key, Value);
    }
  }
  template <typename... Args>
  T &GetValue(Args... args)
  {
    assert(N == sizeof...(args));
    int realArgs[sizeof...(args)] = {(args)...};

    Coordinates<N> key;
    memcpy(key.dimensions, realArgs, sizeof(realArgs));

    auto iter = data.find(key);

    if (iter != data.end())
    {
      return iter->second;
    }
    else
    {
      memcpy(DefPair.first.dimensions, realArgs,
             sizeof(DefPair.first.dimensions));
      DefPair.second = DefVal;
      return DefPair.second;
    }
  }
  template <typename... Args>
  T &GetRefVal(Args... args)
  {
    assert(N == sizeof...(args));
    int realArgs[sizeof...(args)] = {(args)...};
    Coordinates<N> key;
    memcpy(key.dimensions, realArgs, sizeof(realArgs));

    auto iter = data.find(key);
    if (iter != data.end())
    {
      return (*iter).second;
    }
    else
    {
      DefValForRes = DefVal;
      return DefValForRes;
    }
  }

  T &GetRefVal(Coordinates<N> key)
  {
    auto iter = data.find(key);
    if (iter != data.end())
    {
      return (*iter).second;
    }
    else
    {
      DefValForRes = DefVal;
      return DefValForRes;
    }
  }

  class ProxyAtOnce
  {
    Matrix &self;
    Coordinates<N> key;

  public:
    ProxyAtOnce(Matrix &_matrix, Coordinates<N> _key)
        : self(_matrix), key(_key) {}
    ProxyAtOnce &operator=(T val)
    {
      if (val != DefVal)
        self.data.try_emplace(key, val);
      else
        self.data.erase(key);

      return *this;
    }

    ProxyAtOnce &operator=(const ProxyAtOnce &coord)
    {

      *this = static_cast<T>(coord);
      return *this;
    }

    operator T() const { return self.GetRefVal(key); }
  };

  template <int P, typename Dummy = void>
  class SubProxy
  {
  private:
    Matrix &self;

  public:
    SubProxy(Matrix &_matrix) : self(_matrix){};
    SubProxy<P + 1> operator[](int i)
    {
      self.key.dimensions[P + 1] = i;
      return SubProxy<P + 1>(self);
    }
  };

  template <typename Dummy>
  class SubProxy<N - 2, Dummy>
  {
    Matrix &self;

  public:
    SubProxy(Matrix &_matrix) : self(_matrix) {}
    ProxyAtOnce operator[](int i)
    {
      self.key.dimensions[N - 1] = i;
      return ProxyAtOnce(self, self.key);
    }
  };

  SubProxy<0> operator[](int i)
  {
    key.dimensions[0] = i;
    return SubProxy<0>(*this);
  }
  class flat_iterator
  {
    typename std::map<Coordinates<N>, T>::iterator iter;

  public:
    flat_iterator(typename std::map<Coordinates<N>, T>::iterator other) : iter(other) {}
    bool operator!=(const flat_iterator &other) { return iter != other.iter; }
    void operator++() { ++iter; }
    auto &operator*() { return *iter; }
  };
  flat_iterator begin() { return data.begin(); }
  flat_iterator end() { return data.end(); }
};
