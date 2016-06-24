#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#define BENCHPRESS_CONFIG_MAIN
#include <benchpress/benchpress.hpp>
#include "db.test.h"
#include "log.h"
#include "random.test.h"

using namespace arg3::db;

void benchmark_insert(insert_query &insert, const std::shared_ptr<arg3::db::session> &session)
{
        insert.values(random_name(), random_name(), random_num<int>(-123012, 1231232));

        if (!insert.execute()) {
            arg3::db::log::error("unable to prepare test: %s", session->last_error().c_str());
        }
}

BENCHMARK("insert", [](benchpress::context* context)
{
        register_test_sessions();

        current_session = sqldb::create_session("file://testdb.db");

        setup_current_session();

        arg3::db::insert_query query(current_session);

        query.flags(insert_query::Batch);

        query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

        context->reset_timer();

        for(size_t i = 0; i < context->num_iterations(); i++) {
            benchmark_insert(query, current_session);
        }

        context->stop_timer();

        teardown_current_session();
});
