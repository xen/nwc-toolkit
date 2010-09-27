AM_CXXFLAGS = -Wall -Weffc++ -std=c++0x -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags` -liconv

TESTS = \
	test-char-cond \
	test-char-filter \
	test-char-table \
	test-char-type \
	test-character-encoding \
	test-character-reference \
	test-darts \
	test-iconv \
	test-int-traits \
	test-string \
	test-string-builder \
	test-string-hash \
	test-string-pool

noinst_PROGRAMS = $(TESTS)

test_char_cond_SOURCES = test-char-cond.cc
test_char_cond_LDADD = ../lib/libnwc-toolkit.a

test_char_filter_SOURCES = test-char-filter.cc
test_char_filter_LDADD = ../lib/libnwc-toolkit.a

test_char_table_SOURCES = test-char-table.cc
test_char_table_LDADD = ../lib/libnwc-toolkit.a

test_char_type_SOURCES = test-char-type.cc
test_char_type_LDADD = ../lib/libnwc-toolkit.a

test_character_encoding_SOURCES = test-character-encoding.cc
test_character_encoding_LDADD = ../lib/libnwc-toolkit.a

test_character_reference_SOURCES = test-character-reference.cc
test_character_reference_LDADD = ../lib/libnwc-toolkit.a

test_darts_SOURCES = test-darts.cc
test_darts_LDADD = ../lib/libnwc-toolkit.a

test_iconv_SOURCES = test-iconv.cc
test_iconv_LDADD = ../lib/libnwc-toolkit.a

test_int_traits_SOURCES = test-int-traits.cc
test_int_traits_LDADD = ../lib/libnwc-toolkit.a

test_string_SOURCES = test-string.cc
test_string_LDADD = ../lib/libnwc-toolkit.a

test_string_builder_SOURCES = test-string-builder.cc
test_string_builder_LDADD = ../lib/libnwc-toolkit.a

test_string_hash_SOURCES = test-string-hash.cc
test_string_hash_LDADD = ../lib/libnwc-toolkit.a

test_string_pool_SOURCES = test-string-pool.cc
test_string_pool_LDADD = ../lib/libnwc-toolkit.a
