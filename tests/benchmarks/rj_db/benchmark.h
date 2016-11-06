#ifndef RJ_DB_BENCHMARK_H
#define RJ_DB_BENCHMARK_H

#include <benchpress/benchpress.hpp>
#include "insert_query.h"
#include "record.h"
#include "session.h"

extern std::shared_ptr<rj::db::session> current_session;

void benchmark_insert(rj::db::insert_query &insert, const std::shared_ptr<rj::db::session> &session);

void benchmark_select(const std::string &tableName);

void benchmark_setup(const rj::db::uri &uri_s);

void benchmark_teardown();

void benchmark_populate(benchpress::context *context);

void sqlite_setup();

void sqlite_teardown();

void mysql_setup();

void mysql_teardown();

void postgres_setup();

void postgres_teardown();

class user : public rj::db::record<user>
{
   public:
    constexpr static const char *const TABLE_NAME = "users";

    using rj::db::record<user>::record;

    user(const std::shared_ptr<rj::db::session> &sess = current_session) : record(sess->get_schema(TABLE_NAME))
    {
    }

    /*!
     * required constructor
     */
    user(const std::shared_ptr<rj::db::schema> &schema) : record(schema)
    {
    }

    std::string to_string()
    {
        std::ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};

#endif
