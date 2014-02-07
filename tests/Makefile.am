
SOURCES = arg3.test.cpp base_record.test.cpp column.test.cpp db.test.cpp delete_query.test.cpp modify_query.test.cpp \
	schema.test.cpp select_query.test.cpp sql_value.test.cpp 

AM_CPPFLAGS = -std=c++11 -stdlib=libc++ -Ivendor -Wall -Werror

pkginclude_HEADERS = db.test.h

noinst_PROGRAMS = arg3mysql_test arg3sqlite_test

arg3mysql_test_SOURCES = $(SOURCES)

arg3mysql_test_CPPFLAGS = -DTEST_MYSQL $(AM_CPPFLAGS)

arg3mysql_test_LDADD = ../src/libarg3db-0.5.la

arg3sqlite_test_SOURCES = $(SOURCES)

arg3sqlite_test_CPPFLAGS = -DTEST_SQLITE $(AM_CPPFLAGS)

arg3sqlite_test_LDADD = ../src/libarg3db-0.5.la


all-local: arg3mysql_test$(EXEEXT) arg3sqlite_test$(EXEEXT)
	./arg3mysql_test
	./arg3sqlite_test