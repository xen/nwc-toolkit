AM_CXXFLAGS = -Wall -Weffc++ -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags`

bin_SCRIPTS = \
  nwc-toolkit-config

noinst_SCRIPTS = \
  cgi/cetr-text-extractor.py \
  cgi/text-extractor.py

bin_PROGRAMS = \
  nwc-toolkit-content-extractor \
  nwc-toolkit-duplicate-detector \
  nwc-toolkit-hash-calculator \
  nwc-toolkit-html-parser \
  nwc-toolkit-html-reducer \
  nwc-toolkit-ngram-counter \
  nwc-toolkit-ngram-merger \
  nwc-toolkit-text-extractor \
  nwc-toolkit-text-filter \
  nwc-toolkit-unicode-normalizer

nwc_toolkit_content_extractor_SOURCES = nwc-toolkit-content-extractor.cc
nwc_toolkit_content_extractor_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_duplicate_detector_SOURCES = nwc-toolkit-duplicate-detector.cc
nwc_toolkit_duplicate_detector_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_hash_calculator_SOURCES = nwc-toolkit-hash-calculator.cc
nwc_toolkit_hash_calculator_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_html_parser_SOURCES = nwc-toolkit-html-parser.cc
nwc_toolkit_html_parser_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_html_reducer_SOURCES = nwc-toolkit-html-reducer.cc
nwc_toolkit_html_reducer_LDADD = ../lib/libnwc-toolkit.a

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
