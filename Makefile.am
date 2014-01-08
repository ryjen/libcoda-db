
lib_LTLIBRARIES = libarg3db-0.5.la

libarg3db_0_5_la_SOURCES = base_query.cpp column.cpp delete_query.cpp modify_query.cpp mysql_column.cpp mysql_db.cpp \
	mysql_resultset.cpp mysql_row.cpp mysql_statement.cpp resultset.cpp row.cpp schema.cpp schema_factory.cpp select_query.cpp \
	sql_value.cpp sqldb.cpp sqlite3_column.cpp sqlite3_db.cpp sqlite3_resultset.cpp sqlite3_row.cpp sqlite3_statement.cpp \
	where_clause.cpp

AM_CPPFLAGS = -std=c++11 -stdlib=libc++

SUBDIRS = tests

all-local:
	./tests/arg3db_test
