#pragma once
#include <algorithm>
#include <iostream>
#include <map>

template <size_t N>
class Coordinates
{
public:
    int dimensions[N];

public:
    Coordinates() = default;
    Coordinates(const Coordinates<N> &) = default;
    Coordinates(Coordinates<N> &&) = default;
    template <typename... Args>
    Coordinates(Args... args)
    {
        const int count = sizeof...(args);
        int cap[count] = {(args)...};
        memcpy(dimensions, cap, sizeof(dimensions));
    }

    bool operator<(const Coordinates<N> &coord) const
    {

        return std::lexicographical_compare(dimensions, dimensions + N, coord.dimensions,
                                            coord.dimensions + N);
    }
};

template <size_t N>
std::ostream &operator<<(std::ostream &str, const Coordinates<N> &coord)
{
    std::cout << "(";
    for (int i = 0; i < N; i++)
        std::cout << coord.dimensions[i] << ((i < N - 1) ? ";" : ")");
    return str;
}