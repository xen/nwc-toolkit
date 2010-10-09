AM_CXXFLAGS = -Wall -Weffc++ -std=c++0x -I../include `icu-config --cppflags`
AM_LDFLAGS = `icu-config --ldflags`

bin_SCRIPTS = \
	nwc-toolkit-config

bin_PROGRAMS = \
	nwc-toolkit-extract-text

nwc_toolkit_extract_text_SOURCES = nwc-toolkit-extract-text.cc
nwc_toolkit_extract_text_LDADD = ../lib/libnwc-toolkit.a
