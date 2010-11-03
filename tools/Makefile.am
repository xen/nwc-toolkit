AM_CXXFLAGS = -Wall -Weffc++ -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags`

bin_SCRIPTS = \
  nwc-toolkit-config

noinst_SCRIPTS = \
  cgi/text-extractor.py

bin_PROGRAMS = \
  nwc-toolkit-html-digest \
  nwc-toolkit-html-parser \
  nwc-toolkit-ngram-counter \
  nwc-toolkit-ngram-merger \
  nwc-toolkit-text-extractor \
  nwc-toolkit-text-filter \
  nwc-toolkit-unicode-normalizer

nwc_toolkit_html_digest_SOURCES = nwc-toolkit-html-digest.cc
nwc_toolkit_html_digest_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_html_parser_SOURCES = nwc-toolkit-html-parser.cc
nwc_toolkit_html_parser_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_ngram_counter_SOURCES = nwc-toolkit-ngram-counter.cc
nwc_toolkit_ngram_counter_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_ngram_merger_SOURCES = nwc-toolkit-ngram-merger.cc
nwc_toolkit_ngram_merger_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_text_extractor_SOURCES = nwc-toolkit-text-extractor.cc
nwc_toolkit_text_extractor_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_text_filter_SOURCES = nwc-toolkit-text-filter.cc
nwc_toolkit_text_filter_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_unicode_normalizer_SOURCES = nwc-toolkit-unicode-normalizer.cc
nwc_toolkit_unicode_normalizer_LDADD = ../lib/libnwc-toolkit.a

EXTRA_DIST = \
  $(bin_SCRIPTS) \
  $(noinst_SCRIPTS)
