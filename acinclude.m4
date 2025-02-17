#
#   Netcalc
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
#   acinclude.m4 - custom m4 macros used by configure.ac
#

# AC_NETCALC_LEGACY()
# ______________________________________________________________________________
AC_DEFUN([AC_NETCALC_LEGACY],[dnl
   enableval=""
   AC_ARG_ENABLE(
      legacy,
      [AS_HELP_STRING([--enable-legacy], [build legacy utility])],
      [ ELEGACY=$enableval ],
      [ ELEGACY=$enableval ]
   )

   if test "x${ELEGACY}" == "xno";then
      ENABLE_LEGACY="skip"
   else
      ENABLE_LEGACY="build"
   fi

   AM_CONDITIONAL([ENABLE_LEGACY],  [test "$ENABLE_LEGACY"  = "build"])
   AM_CONDITIONAL([DISABLE_LEGACY], [test "$ENABLE_LEGACY" != "build"])
])dnl


# AC_NETCALC_UTILITY()
# ______________________________________________________________________________
AC_DEFUN([AC_NETCALC_UTILITY],[dnl
   enableval=""
   AC_ARG_ENABLE(
      utility,
      [AS_HELP_STRING([--disable-utility], [do not install netcalc])],
      [ EUTILITY=$enableval ],
      [ EUTILITY=$enableval ]
   )

   if test "x${EUTILITY}" == "xno";then
      ENABLE_UTILITY="skip"
   else
      ENABLE_UTILITY="install"
   fi

   AM_CONDITIONAL([ENABLE_UTILITY],  [test "$ENABLE_UTILITY"  = "install"])
   AM_CONDITIONAL([DISABLE_UTILITY], [test "$ENABLE_UTILITY" != "install"])
])dnl


# AC_NETCALC_DOCS()
# ______________________________________________________________________________
AC_DEFUN([AC_NETCALC_DOCS],[dnl
   enableval=""
   AC_ARG_ENABLE(
      extra-docs,
      [AS_HELP_STRING([--enable-extra-docs], [install extra documentation])],
      [ EDOCUMENATION=$enableval ],
      [ EDOCUMENATION=$enableval ]
   )

   if test "x${EDOCUMENATION}" == "xyes";then
      ENABLE_DOCUMENATION="install"
   else
      ENABLE_DOCUMENATION="skip"
   fi

   AM_CONDITIONAL([ENABLE_DOCUMENATION],  [test "$ENABLE_DOCUMENATION"  = "install"])
   AM_CONDITIONAL([DISABLE_DOCUMENATION], [test "$ENABLE_DOCUMENATION" != "install"])
])dnl


# AC_NETCALC_LIBRARY()
# ______________________________________________________________________________
AC_DEFUN([AC_NETCALC_LIBRARY],[dnl

   # prerequists
   AC_REQUIRE([AC_NETCALC_UTILITY])

   enableval=""
   AC_ARG_ENABLE(
      libnetcalc,
      [AS_HELP_STRING([--disable-libnetcalc], [do not install libnetcalc])],
      [ ELIBRARY=$enableval ],
      [ ELIBRARY=$enableval ]
   )

   if test "x${ELIBRARY}" == "xno";then
      if test "x${ENABLE_UTILITY}" == "xinstall"; then
         ENABLE_LIBRARY="build"
      else
         ENABLE_LIBRARY="skip"
      fi
   else
      ENABLE_LIBRARY="install"
   fi

   AM_CONDITIONAL([ENABLE_LIBRARY_LA],  [test "$ENABLE_LIBRARY"  = "install"])
   AM_CONDITIONAL([DISABLE_LIBRARY_LA], [test "$ENABLE_LIBRARY" != "install"])
   AM_CONDITIONAL([ENABLE_LIBRARY_A],  [test "$ENABLE_LIBRARY"  = "build"])
   AM_CONDITIONAL([DISABLE_LIBRARY_A], [test "$ENABLE_LIBRARY" != "build"])
])dnl

# end of m4 file
