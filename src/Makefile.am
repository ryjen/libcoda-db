
lib_LTLIBRARIES = libarg3db.la

libarg3db_la_LDFLAGS = $(LIBARG3DB_LA_LDFLAGS) -version-info 0:0:0

libarg3db_la_LIBADD = ../libs/variant/src/libarg3variant.la

libarg3db_la_SOURCES = base_query.cpp column.cpp delete_query.cpp modify_query.cpp mysql_column.cpp mysql_binding.cpp mysql_db.cpp \
	mysql_resultset.cpp mysql_row.cpp mysql_statement.cpp resultset.cpp row.cpp schema.cpp schema_factory.cpp select_query.cpp \
	sql_value.cpp sqldb.cpp sqlite3_column.cpp sqlite3_db.cpp sqlite3_resultset.cpp sqlite3_row.cpp sqlite3_statement.cpp \
	where_clause.cpp

libarg3db_la_CXXFLAGS = $(LIBARG3DB_LA_CXXFLAGS) $(COVERAGE_CFLAGS) -I../libs/variant/src

pkginclude_HEADERS = base_query.h base_record.h bindable.h column.h delete_query.h exception.h modify_query.h \
	mysql_binding.h mysql_column.h mysql_db.h mysql_resultset.h mysql_row.h mysql_statement.h resultset.h row.h schema.h schema_factory.h \
	select_query.h sql_value.h sqldb.h sqlite3_column.h sqlite3_db.h sqlite3_resultset.h sqlite3_row.h sqlite3_statement.h \
	statement.h where_clause.h
