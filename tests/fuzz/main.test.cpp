#include <iostream>
#include "../db.test.h"
#include "../util.h"
#include "mysql/session.h"

using namespace coda::db;

typedef std::vector<std::string> arguments;

int perform_set(test::user &u, const arguments &args)
{
    if (args.size() < 3) {
        std::cerr << "Invalid set syntax\n";
        return 1;
    }
    u.set(args[2], args[3]);
    return 0;
}

int perform_delete(test::user &u, const arguments &args)
{
    u.remove();
    return 0;
}


int perform_save(test::user &u, const arguments &args)
{
    u.save();
    return 0;
}

int perform_refresh(test::user &u, const arguments &args)
{
    u.refresh();
    return 0;
}

int perform_find_by_id(test::user &u, const arguments &args)
{
    auto other = u.find_by_id(stol(args[2]));
    return 0;
}

int perform_find_all(test::user &u, const arguments &args)
{
    for (auto &o : u.find_all()) {
        if (!o->is_valid()) {
            continue;
        }
    }
    return 0;
}

int perform_find_by(test::user &u, const arguments &args)
{
    for (auto &o : u.find_by(args[2], args[3])) {
        if (!o->is_valid()) {
            continue;
        }
    }
    return 0;
}

namespace grammar
{
    typedef int (*func)(test::user &, const std::vector<std::string> &);

    const struct {
        const char *command;
        func func;
    } commands[] = {"set",        &perform_set,        "rem",      &perform_delete,
                    "save",       &perform_save,       "refresh",  &perform_refresh,
                    "find_by_id", &perform_find_by_id, "find_all", &perform_find_all,
                    "find_by",    &perform_find_by,    NULL,       NULL};

    func find(const std::string &value)
    {
        for (int i = 0; commands[i].command != NULL; i++) {
            if (value == commands[i].command) {
                return commands[i].func;
            }
        }
        return nullptr;
    }
}

int main(int argc, char *argv[])
{
    coda::db::mysql::initialize();
    auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost:3306/test");
    auto fuzz_session = coda::db::sqldb::create_session(uri_s);
    test::user u(fuzz_session);

    if (argc <= 1) {
        std::cerr << "No arguments.\n";
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);

    auto func = grammar::find(args[1]);

    if (!func) {
        return 1;
    }

    func(u, args);

    return 0;
}
