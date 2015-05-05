
SOURCES = arg3.test.cpp base_record.test.cpp column.test.cpp db.test.cpp delete_query.test.cpp modify_query.test.cpp \
	resultset.test.cpp row.test.cpp schema.test.cpp schema_factory.test.cpp select_query.test.cpp sql_value.test.cpp \
	where_clause.test.cpp sqlite3_statement.test.cpp mysql_column.test.cpp mysql_db.test.cpp mysql_row.test.cpp \
	mysql_resultset.test.cpp mysql_binding.test.cpp mysql_statement.test.cpp

AM_CPPFLAGS = @AM_CPPFLAGS@ -I../src -Ivendor/bandit -Wall -Werror -I../libs/variant/src

check_PROGRAMS = arg3mysql_test arg3sqlite_test

arg3mysql_test_SOURCES = $(SOURCES)

arg3mysql_test_CPPFLAGS = -DTEST_MYSQL $(AM_CPPFLAGS)

arg3mysql_test_LDADD = ../src/libarg3db.la ../libs/variant/src/libarg3variant.la


arg3sqlite_test_SOURCES = $(SOURCES)

arg3sqlite_test_CPPFLAGS = -DTEST_SQLITE $(AM_CPPFLAGS)

arg3sqlite_test_LDADD = ../src/libarg3db.la ../libs/variant/src/libarg3variant.la

check: arg3mysql_test arg3sqlite_test
	./arg3mysql_test
	./arg3sqlite_test
