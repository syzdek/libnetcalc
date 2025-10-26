#
#   Network Calculations Library
#   Copyright (C) 2025 David M. Syzdek <david@syzdek.net>.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of David M. Syzdek nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M SYZDEK BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
#
#   @configure_input@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					=
DIST_SUBDIRS				=

# GNU make options
V					= 0

# directory locations
moddir					= $(pkglibexecdir)/modules
win64_builddir				= build-win64-x86
win64_distdir				= $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-win64-x86
abs_win64_distdir			= $(abs_builddir)/$(win64_distdir)

# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 \
					  -DHAVE_CONFIG_H=1 \
					  -I$(top_builddir)/include \
					  -I$(top_srcdir)/include \
					  -DLOCALSTATEDIR="\"$(localstatedir)\"" \
					  -DRUNSTATEDIR="\"$(runstatedir)\"" \
					  -DSYSCONFDIR="\"$(sysconfdir)\""
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
AM_MAKEINFOFLAGS                        = --no-split
DEFS					=


# custom directories


# automake targets
check_PROGRAMS				=
doc_DATA				= AUTHORS.md \
					  ChangeLog.md \
					  COPYING.md \
					  NEWS.md \
					  Notes.md \
					  README.md \
					  TODO.md
include_HEADERS				=
lib_LTLIBRARIES				=
lib_LIBRARIES				=
man_MANS				=
info_TEXINFOS				=
noinst_LTLIBRARIES			=
noinst_LIBRARIES			=
noinst_HEADERS				= include/netcalc.h
noinst_PROGRAMS				=
bin_PROGRAMS				=
bin_SCRIPTS				=
pkgdata_DATA				=
sbin_SCRIPTS				=
sbin_PROGRAMS				=
mod_LTLIBRARIES				=
EXTRA_PROGRAMS				= src/netcalc \
					  src/netcalc-dmstools \
					  tests/test-ntop-4mapped6 \
					  tests/test-ntop-broadcast \
					  tests/test-ntop-compress \
					  tests/test-ntop-dflt \
					  tests/test-ntop-len \
					  tests/test-ntop-network \
					  tests/test-ntop-suppress \
					  tests/test-parse \
					  tests/test-set-debug \
					  tests/test-set-query \
					  tests/test-set-sort \
					  tests/test-strfnet \
					  tests/test-verify
EXTRA					=


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				=
TESTS					= tests/test-ntop-4mapped6 \
					  tests/test-ntop-broadcast \
					  tests/test-ntop-compress \
					  tests/test-ntop-dflt \
					  tests/test-ntop-len \
					  tests/test-ntop-network \
					  tests/test-ntop-suppress \
					  tests/test-parse \
					  tests/test-set-debug \
					  tests/test-set-query \
					  tests/test-set-sort \
					  tests/test-strfnet \
					  tests/test-verify
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= $(doc_DATA) \
					  $(noinst_HEADERS) \
					  doc/man/netcalc.1.in \
					  doc/man/netcalc-info.1.in \
					  doc/man/netcalc-printf.1.in \
					  doc/man/netcalc-test.1.in \
					  lib/libnetcalc/libnetcalc.sym
CLEANFILES				= $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  $(pkgdata_DATA) \
					  $(bin_SCRIPTS) \
					  $(sbin_SCRIPTS) \
					  $(EXTRA_PROGRAMS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.txz \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings \
					  --enable-utility \
					  --enable-libnetcalc \
					  --enable-extra-docs \
					  CFLAGS="$(CFLAGS)" \
					  CPPFLAGS="$(CPPFLAGS)" \
					  LDFLAGS="$(LDFLAGS)"


# conditional build
if ENABLE_LEGACY
   noinst_PROGRAMS			+= src/netcalc-dmstools
endif
if ENABLE_LIBRARY_A
   noinst_LIBRARIES 			+= lib/libnetcalc.a
endif
if ENABLE_LIBRARY_LA
   include_HEADERS 			+= include/netcalc.h
   lib_LTLIBRARIES 			+= lib/libnetcalc.la
endif
if ENABLE_UTILITY
   bin_PROGRAMS				+= src/netcalc
   man_MANS				+= doc/man/netcalc.1
   man_MANS				+= doc/man/netcalc-info.1
   man_MANS				+= doc/man/netcalc-printf.1
   man_MANS				+= doc/man/netcalc-test.1
endif


# macros for lib/libnetcalc.a
lib_libnetcalc_a_DEPENDENCIES		= Makefile \
					  config.h
lib_libnetcalc_a_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libnetcalc
lib_libnetcalc_a_SOURCES		= include/netcalc.h \
					  lib/libnetcalc/libnetcalc.h \
					  lib/libnetcalc/laddr.c \
					  lib/libnetcalc/lmisc.c \
					  lib/libnetcalc/lnets.c \
					  lib/libnetcalc/lsets.c \
					  lib/libnetcalc/lvars.c


# macros for lib/libnetcalc.la
lib_libnetcalc_la_DEPENDENCIES		= Makefile \
					  config.h \
					  lib/libnetcalc/libnetcalc.sym
lib_libnetcalc_la_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libnetcalc
lib_libnetcalc_la_LDFLAGS		= -rpath'$(libdir)' \
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
					  -export-symbols $(srcdir)/lib/libnetcalc/libnetcalc.sym
lib_libnetcalc_la_SOURCES		= $(lib_libnetcalc_a_SOURCES)


# macros for src/netcalc
src_netcalc_DEPENDENCIES		= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
src_netcalc_CPPFLAGS			= -DPROGRAM_NAME="\"netcalc\"" $(AM_CPPFLAGS)
src_netcalc_LDADD			= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
src_netcalc_SOURCES			= src/netcalc-utility.h \
					  src/netcalc.c \
					  src/widget-debug.c \
					  src/widget-info.c \
					  src/widget-match.c \
					  src/widget-printf.c \
					  src/widget-sort.c \
					  src/widget-superblock.c \
					  src/widget-test.c \
					  src/widget-tree.c


# macros for src/netcalc-dmstools
src_netcalc_dmstools_DEPENDENCIES	= Makefile config.h
src_netcalc_dmstools_SOURCES		= src/netcalc-dmstools.c


# macros for tests/test-ntop-4mapped6
tests_test_ntop_4mapped6_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_4mapped6_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_4mapped6_SOURCES	= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-4mapped6.c


# macros for tests/test-ntop-broadcast
tests_test_ntop_broadcast_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_broadcast_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_broadcast_SOURCES	= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-broadcast.c


# macros for tests/test-ntop-compress
tests_test_ntop_compress_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_compress_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_compress_SOURCES	= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-compress.c


# macros for tests/test-ntop-dflt
tests_test_ntop_dflt_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_dflt_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_dflt_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-dflt.c


# macros for tests/test-ntop-len
tests_test_ntop_len_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_len_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_len_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-ntop-len.c


# macros for tests/test-ntop-network
tests_test_ntop_network_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_network_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_network_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-network.c


# macros for tests/test-ntop-suppress
tests_test_ntop_suppress_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_suppress_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_ntop_suppress_SOURCES	= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-ntop-suppress.c


# macros for tests/test-parse
tests_test_parse_DEPENDENCIES		= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_parse_LDADD			= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_parse_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-parse.c


# macros for tests/test-set-debug
tests_test_set_debug_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_debug_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_debug_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-set-data.c \
					  tests/test-set-debug.c


# macros for tests/test-set-query
tests_test_set_query_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_query_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_query_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-set-data.c \
					  tests/test-set-query.c


# macros for tests/test-set-sort
tests_test_set_sort_DEPENDENCIES	= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_sort_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_set_sort_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-set-data.c \
					  tests/test-set-sort.c


# macros for tests/test-parse
tests_test_strfnet_DEPENDENCIES		= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_strfnet_LDADD		= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_strfnet_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-strfnet.c


# macros for tests/test-verify
tests_test_verify_DEPENDENCIES		= Makefile config.h \
					  $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_verify_LDADD			= $(noinst_LIBRARIES) $(lib_LTLIBRARIES)
tests_test_verify_SOURCES		= tests/libnetcalc-tests.h \
					  tests/test-data.c \
					  tests/test-verify.c


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =
include contrib/bindletools/build-aux/makefile-subst.am
include contrib/bindletools/build-aux/makefile-version.am


# custom targets
.PHONY: git-clean mingw32

git-clean:
	git fsck --full --unreachable
	git gc --auto --aggressive


mingw32: $(win64_distdir).zip


$(win64_distdir).zip: $(win64_builddir)/netcalc-built
	@rm -Rf "$(@)" "$(win64_distdir)"
	make -C "$(win64_builddir)" install
	@rm -f "$(win64_distdir)/libnetcalc.la"
	zip -r "$(@)" "$(win64_distdir)"
	@rm -Rf "$(win64_distdir)"
	@touch "$(@)"


$(win64_builddir)/netcalc-built: $(win64_builddir)/Makefile
	@rm -f "$(@)"
	make -C "$(win64_builddir)"
	@touch "$(@)"


$(win64_builddir)/Makefile: Makefile
	@rm -Rf "$(win64_builddir)" "$(win64_distdir)"
	@mkdir -p "$(win64_builddir)"
	cd "$(win64_builddir)" && "$(abs_top_srcdir)/configure" \
		--host=x86_64-w64-mingw32 \
		--prefix="$(abs_win64_distdir)" \
		--bindir="$(abs_win64_distdir)" \
		--docdir="$(abs_win64_distdir)" \
		--libdir="$(abs_win64_distdir)" \
		--includedir="$(abs_win64_distdir)" \
		--enable-strictwarnings \
		--disable-debug \
		--enable-libnetcalc \
		--enable-extra-docs \
		--enable-utility \
		--disable-legacy \
		--enable-static \
		CFLAGS="-Wno-implicit-fallthrough"
	@touch "$(@)"


doc/man/netcalc.1: Makefile $(srcdir)/doc/man/netcalc.1.in
	@$(do_subst_dt)


doc/man/netcalc-info.1: Makefile $(srcdir)/doc/man/netcalc-info.1.in
	@$(do_subst_dt)


doc/man/netcalc-printf.1: Makefile $(srcdir)/doc/man/netcalc-printf.1.in
	@$(do_subst_dt)


doc/man/netcalc-test.1: Makefile $(srcdir)/doc/man/netcalc-test.1.in
	@$(do_subst_dt)


# local targets
install-exec-local:

install-data-local:

install-data-hook:

uninstall-local:

clean-local:
	rm -Rf $(win64_builddir)
	rm -Rf $(PACKAGE_TARNAME)-*-win64-x86 $(PACKAGE_TARNAME)-*-win64-x86.zip

distclean-local:
	rm -fR $(srcdir)/autom4te.cache

# end of automake file
