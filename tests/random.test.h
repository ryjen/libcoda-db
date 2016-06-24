#ifndef ARG3_DB_RANDOM_TEST_H_
#define ARG3_DB_RANDOM_TEST_H_

#include <string>
#include <random>

std::string random_name();

extern std::default_random_engine rand_eng;

template <typename T>
T random_num(T min, T max)
{
    std::uniform_int_distribution<T> uniform_dist(min, max);
    return uniform_dist(rand_eng);
}

#endif
