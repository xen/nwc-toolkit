AM_CXXFLAGS = -Wall -Weffc++ -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags` -liconv

TESTS = \
	test-iconv

noinst_PROGRAMS = $(TESTS)

test_iconv_SOURCES = test-iconv.cc
test_iconv_LDADD = ../lib/libnwc.a
