
bin_PROGRAMS = arg3db_test

arg3db_test_SOURCES = arg3.test.cpp base_record.test.cpp column.test.cpp db.test.cpp delete_query.test.cpp modify_query.test.cpp \
	schema.test.cpp select_query.test.cpp sql_value.test.cpp

AM_CPPFLAGS = -std=c++11 -stdlib=libc++ -Ivendor

arg3db_test_LDADD = ../libarg3db-0.5.la