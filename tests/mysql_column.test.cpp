#include "config.h"

#undef VERSION

#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql_column.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

shared_ptr<column_impl> get_results_column(int index)
{
    auto rs = mysql_testdb.execute("select * from users");

    auto i = rs.begin();

    if (index >= i->size())
        throw database_exception("invalid column index");

    auto c = i->co1umn(index);

    return c.impl();
}

shared_ptr<column_impl> get_stmt_column(int index)
{
    select_query query(&mysql_testdb, "users");

    auto rs = query.execute();

    auto i = rs.begin();

    if (index >= i->size())
        throw database_exception("invalid column index");

    auto c = i->co1umn(index);

    return c.impl();
}


template<typename T>
void test_copy_column(std::function<shared_ptr<column_impl>(int)> funk)
{
    auto f1 = funk(0);

    auto f2 = funk(1);

    T c2 (*static_pointer_cast<T>(f1));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.to_value() == f1->to_value(), IsTrue());

    c2 = *static_pointer_cast<T>(f2);

    Assert::That(c2.is_valid(), IsTrue());

    AssertThat(c2.to_value() == f2->to_value(), IsTrue());
}

template<typename T>
void test_move_column(std::function < shared_ptr<column_impl>(int)> funk)
{
    auto f1 = funk(0);

    auto f2 = funk(1);

    auto f1Value = f1->to_value();

    auto f2Value = f2->to_value();

    T c2 (std::move(*static_pointer_cast<T>(f1)));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.to_value() == f1Value, IsTrue());

    c2 = std::move(*static_pointer_cast<T>(f2));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.to_value() == f2Value, IsTrue());
}

go_bandit([]()
{

    describe("mysql column", []()
    {
        before_each([]()
        {
            mysql_testdb.setup();

            user user1(&mysql_testdb);

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();


            user user2(&mysql_testdb);

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([]()
        {
            mysql_testdb.teardown();
        });

        describe("is copyable", []()
        {
            if (mysql_testdb.cache_level() != sqldb::CACHE_NONE)
            {
                it("as cached results", []()
                {
                    test_copy_column<mysql_cached_column>(get_stmt_column);
                });
            }
            else
            {
                it("as statement results", []()
                {
                    test_copy_column<mysql_stmt_column>(get_stmt_column);
                });

                it("as results", []()
                {
                    test_copy_column<mysql_column>(get_results_column);
                });
            }

        });

        describe("is movable", []()
        {

            if (mysql_testdb.cache_level() != sqldb::CACHE_NONE)
            {
                it("as cached results", []()
                {
                    test_move_column<mysql_cached_column>(get_stmt_column);
                });

            }
            else
            {
                it("as statement results", []()
                {
                    test_move_column<mysql_stmt_column>(get_stmt_column);
                });

                it("as results", []()
                {
                    test_move_column<mysql_column>(get_results_column);
                });
            }


        });
    });

});

#endif
