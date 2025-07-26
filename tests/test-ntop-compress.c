/*
 *  Network Calculations Library
 *  Copyright (C) 2025 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define __TESTS_NETCALC_NTOP_SUPPRESS_C 1
#include "libnetcalc-tests.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <netcalc.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "netcalc-ntop-compress"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

extern int
main(
         int                           argc,
         char *                        argv[] );


static void
my_info(
         const char *                  fmt,
         ... );


static int
my_test(
         testdata_t *                  dat );


static void
my_verbose(
         const char *                  fmt,
         ... );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static int verbose   = 0;
static int quiet     = 0;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
main(
         int                           argc,
         char *                        argv[] )
{
   int               c;
   int               opt_index;
   int               pos;
   int               errors;

   // getopt options
   static const char *  short_opt = "hqVv";
   static struct option long_opt[] =
   {  {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {  switch(c)
      {  case -1:       /* no more arguments */
         case 0:        /* long options toggles */
            break;

         case 'h':
            printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
            printf("       %s [OPTIONS] <string> <string> ... <string> \n", PROGRAM_NAME);
            printf("OPTIONS:\n");
            printf("  -h, --help                print this help and exit\n");
            printf("  -q, --quiet, --silent     do not print messages\n");
            printf("  -V, --version             print version number and exit\n");
            printf("  -v, --verbose             print verbose messages\n");
            printf("\n");
            return(0);

         case 'q':
            quiet++;
            break;

         case 'V':
            printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
            printf("Written by David M. Syzdek.\n");
            return(0);

         case 'v':
            verbose++;
            break;

         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);

         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
      };
   };

   errors = 0;

   for(pos = 0; ((test_data[pos].addr_str)); pos++)
      if ((my_test(&test_data[pos])))
         errors++;

   return( ((errors)) ? 1 : 0 );
}


void
my_info(
         const char *                  fmt,
         ... )
{
   va_list  args;
   if ((quiet))
      return;
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
   return;
}


int
my_test(
         testdata_t *                  dat )
{
   netcalc_net_t *   net;
   int               rc;
   int               flags;
   const char *      net_addr;
   const char *      dat_addr;

   if (!(dat->addr_ip_compress))
      return(0);

   net      = NULL;
   dat_addr = dat->addr_ip_compress;
   flags    = NETCALC_FLG_COMPR | NETCALC_FLG_IFACE | NETCALC_FLG_CIDR | NETCALC_FLG_PORT;

   my_info("checking: \"%s\" ...\n", dat->addr_str);

   rc = netcalc_initialize(&net, dat->addr_str, 0);
   my_verbose("   status:        %s\n", netcalc_strerror(rc));
   my_verbose("   return code:   %i\n", rc);
   if ((rc))
   {  my_verbose("\n");
      return(0);
   };

   // check address
   if ((dat_addr))
   {  net_addr = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, flags );
      if ((strcmp(dat_addr, net_addr)))
      {
         printf("   address:       %s\n", net_addr);
         printf("   expected:      %s\n", dat_addr);
         return(1);
      };
      my_verbose("   address:       %s\n", net_addr);
   };

   netcalc_free(net);

   my_verbose("\n");

   return(0);
}


void
my_verbose(
         const char *                  fmt,
         ... )
{
   va_list  args;
   if (!(verbose))
      return;
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
   return;
}


/* end of source */
