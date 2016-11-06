#include "util.h"

std::random_device rd;
std::default_random_engine rand_eng(rd());

std::string random_name()
{
    char alpha[27] = "abcdefghijklmnopqrstuvwxyz";

    const int len = random_num(5, 9);
    char buf[20] = {0};

    for (int i = 0; i < len; i++) {
        int c = random_num<int>(0, 27);
        buf[i] = alpha[c];
    }
    buf[len] = 0;
    return std::string(buf) + std::to_string(random_num<int>(1000, 9999));
}

std::string get_env_uri(const char *name, const std::string &def)
{
    char *temp = getenv(name);

    if (temp != NULL) {
        return temp;
    }

    return def;
}
