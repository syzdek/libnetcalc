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
#define __TESTS_NETCALC_STRFNET_C 1
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
#define PROGRAM_NAME "test-strfnet"


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
#pragma mark - Datatypes

typedef struct _test_fmt testfmt_t;
struct _test_fmt
{
   const char *            fmt_input;
   const char *            fmt_results;
   const char * const *    fmts;
};


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
         testfmt_t *                   dat );


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


testfmt_t test_fmts[] =
{
   {  .fmt_input     = "::/0",
      .fmt_results   = NULL,
      .fmts          = (const char * const[])
                     {  "%20-a",
                        "%-20Z10a",
                        NULL
                     },
   },

   {  .fmt_input     = "2001:db8::/32",
      .fmt_results   = "==2001:db8::/32       ==",
      .fmts          = (const char * const[])
                     {  "==%-20zsd==",
                        "==%zsa%-10c==",
                        "==%zsa/%-9C==",
                        "==%zsa%c       ==",
                        NULL
                     },
   },

   {  .fmt_input     = "2001:db8::/32",
      .fmt_results   = "==       2001:db8::/32==",
      .fmts          = (const char * const[])
                     {  "==%20zsd==",
                        "==%17zsa%c==",
                        "==%17zsa/%C==",
                        "==       %zsa%c==",
                        NULL
                     },
   },

   {  .fmt_input     = "[fe80::216:3eff:fefe:b381%eth0]/64:80",
      .fmt_results   = "[fe80::216:3eff:fefe:b381%eth0]/64:80",
      .fmts          = (const char * const[])
                     {  "%szd",
                        "[%zsa%i]%c%p",
                        NULL
                     },
   },

   {  .fmt_input     = "[fe80::216:3eff:fefe:b381%eth0]/64:80",
      .fmt_results   = "[fe80:0000:0000:0000:0216:3eff:fefe:b381%eth0]/64:80",
      .fmts          = (const char * const[])
                     {  "%d",
                        "[%a%i]%c%p",
                        "[%ZSa%i]%c%p",
                        "[%a%%%I]%c%p",
                        "[%a%i]/%C%p",
                        "[%a%i]%c:%P",
                        "[%a%%%I]/%C:%P",
                        NULL
                     },
   },

   // end of data
   {  .fmt_input     = NULL,
      .fmt_results   = NULL,
      .fmts          = NULL
   }
};


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
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
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

   for(pos = 0; ((test_fmts[pos].fmt_input)); pos++)
      errors += my_test(&test_fmts[pos]);

   my_info("%i errors\n", errors);

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
         testfmt_t *                   dat )
{
   int               rc;
   int               idx;
   int               errs;
   size_t            len;
   netcalc_net_t *   net;
   char              buff[NETCALC_ADDRESS_LENGTH];

   net   = NULL;
   errs  = 0;

   if (!(dat->fmt_results))
   {
      my_info("testing \"%s\" with invalid formats ...\n",  dat->fmt_input);
   } else
   {
      my_info("testing \"%s\" to\n",  dat->fmt_input);
      my_info("        \"%s\" ...\n", dat->fmt_results);
   };

   rc = netcalc_initialize(&net, dat->fmt_input, 0);
   if ((rc))
   {  my_info("        netcalc_initialize():  %i (%s)\n", rc, netcalc_strerror(rc));
      my_verbose("\n");
      return(1);
   };

   for(idx = 0; ((dat->fmts[idx])); idx++)
   {
      my_verbose("        format: \"%s\" ...\n", dat->fmts[idx]);
      len = netcalc_strfnet(net, buff, sizeof(buff), dat->fmts[idx], 0);
      if ((dat->fmt_results))
      {  if (!(len))
         {  if (!(verbose))
               my_info("        format: \"%s\" ...\n", dat->fmts[idx]);
            my_info("           output:        <NULL>\n");
            my_info("           expected:      %s\n", dat->fmt_results);
            errs++;
         } else if ((strcmp(dat->fmt_results, buff)))
         {  if (!(verbose))
               my_info("        format: \"%s\" ...\n", dat->fmts[idx]);
            my_info("           output:        %s\n", buff);
            my_info("           expected:      %s\n", dat->fmt_results);
            errs++;
         };
      } else
      { if ((len))
         {  if (!(verbose))
               my_info("        format: \"%s\" ...\n", dat->fmts[idx]);
            my_info("           output:        %s\n", buff);
            my_info("           expected:      <NULL>\n");
            errs++;
         }
      };
   };

   netcalc_free(net);

   my_verbose("\n");

   return(errs);
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
