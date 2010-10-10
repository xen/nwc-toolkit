AM_CXXFLAGS = -Wall -Weffc++ -std=c++0x -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags`

bin_SCRIPTS = \
  nwc-toolkit-config

bin_PROGRAMS = \
  nwc-toolkit-text-extractor \
  nwc-toolkit-text-filter \
  nwc-toolkit-unicode-normalizer

nwc_toolkit_text_extractor_SOURCES = nwc-toolkit-text-extractor.cc
nwc_toolkit_text_extractor_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_text_filter_SOURCES = nwc-toolkit-text-filter.cc
nwc_toolkit_text_filter_LDADD = ../lib/libnwc-toolkit.a

nwc_toolkit_unicode_normalizer_SOURCES = nwc-toolkit-unicode-normalizer.cc
nwc_toolkit_unicode_normalizer_LDADD = ../lib/libnwc-toolkit.a

EXTRA_DIST = \
  nwc-toolkit-config
