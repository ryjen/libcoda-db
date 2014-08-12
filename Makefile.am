
SUBDIRS = src tests

include $(top_srcdir)/Makefile.am.coverage

ACLOCAL_AMFLAGS = -I m4

BUILT_SOURCES = module-update

module-update:
	git submodule update --init --recursive
