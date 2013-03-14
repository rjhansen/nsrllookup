EXTRA_DIST = common.hpp win32.hpp unix.hpp README.W32
bin_PROGRAMS=nsrllookup
nsrllookup_SOURCES = nsrllookup.cc common.cc parse_options.cc query_server.cc
nsrllookup_CPPFLAGS = -DPKGDATADIR="\"$(pkgdatadir)\"" -DPACKAGE_VERSION="\"$(PACKAGE_VERSION)\"" -DPACKAGE_URL="\"$(PACKAGE_URL)\"" -DPACKAGE_BUGREPORT="\"$(PACKAGE_BUGREPORT)\""
