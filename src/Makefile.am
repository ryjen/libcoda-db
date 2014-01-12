
lib_LTLIBRARIES = libarg3db-0.5.la

libarg3db_0_5_la_SOURCES = base_query.cpp column.cpp delete_query.cpp modify_query.cpp mysql_column.cpp mysql_db.cpp \
	mysql_resultset.cpp mysql_row.cpp mysql_statement.cpp resultset.cpp row.cpp schema.cpp schema_factory.cpp select_query.cpp \
	sql_value.cpp sqldb.cpp sqlite3_column.cpp sqlite3_db.cpp sqlite3_resultset.cpp sqlite3_row.cpp sqlite3_statement.cpp \
	where_clause.cpp

pkginclude_HEADERS = base_query.h base_record.h bindable.h column.h config.h delete_query.h exception.h modify_query.h mpl.h mysql_column.h \
	mysql_db.h mysql_resultset.h mysql_row.h mysql_statement.h resultset.h row.h schema.h schema_factory.h select_query.h sql_value.h \
	sqldb.h sqlite3_column.h sqlite3_db.h sqlite3_resultset.h sqlite3_row.h sqlite3_statement.h statement.h variant.h where_clause.h

AM_CPPFLAGS = -std=c++11 -stdlib=libc++ -Wall -Werror
