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
#define __TESTS_NETCALC_SET_SORT_C 1
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
#define PROGRAM_NAME "netcalc-set-sort"


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


int
my_pass(
         const char *                  name,
         const char * const *          data,
         size_t                        datalen,
         size_t                        offset );


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
   size_t            datalen;
   size_t            pass;
   size_t            errs;

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

   errs = 0;

   // check set 1
   for(datalen = 0; ((test_set1[datalen])); datalen++);
   for(pass = 0; (pass < datalen); pass++)
      if ((my_pass("set1", test_set1, datalen, pass)))
         errs++;

   // check set 2
   for(datalen = 0; ((test_set2[datalen])); datalen++);
   for(pass = 0; (pass < datalen); pass++)
      if ((my_pass("set2", test_set2, datalen, pass)))
         errs++;

   return( ((errs)) ? 1 : 0 );
}


int
my_pass(
         const char *                  name,
         const char * const *          data,
         size_t                        datalen,
         size_t                        offset )
{
   int               rc;
   int               errs;
   int               flags;
   size_t            idx;
   size_t            dup;
   netcalc_set_t *   ns;
   netcalc_cur_t *   cur;
   netcalc_net_t *   prev;
   netcalc_net_t *   net;
   char              prev_str[NETCALC_ADDRESS_LENGTH];
   char              net_str[NETCALC_ADDRESS_LENGTH];

   errs  = 0;
   flags = NETCALC_FLG_SUPR | NETCALC_FLG_COMPR | NETCALC_FLG_CIDR;

   printf("testing %s ...\n", name);
   printf("   %s starting pass %zu ...\n", name, offset);

   if ((rc = netcalc_set_init(&ns, NULL, 0)) != 0)
   {  printf("%s: %s\n", PROGRAM_NAME, netcalc_strerror(rc));
      return(1);
   };

   // add addresses to sets
   for(idx = offset; (idx < datalen); idx++)
   {  printf("      adding %s ...\n", data[idx]);
      if ((rc = netcalc_set_add_str(ns, data[idx], NULL, NULL, 0)) != 0)
      {  printf("%s: netcalc_set_add_str(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         continue;
      };
      for(dup = offset; (dup <= idx); dup++)
      {  if ((rc = netcalc_set_add_str(ns, data[dup], NULL, NULL, 0)) != NETCALC_EEXISTS)
         {  printf("%s: %s: adding duplicate: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
         if ((rc = netcalc_set_query_str(ns, data[dup], NULL, NULL, NULL, NULL)) != 0)
         {  printf("%s: %s: netcalc_set_query_str: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
      };
   };
   for(idx = 0; (idx < offset); idx++)
   {  printf("      adding %s ...\n", data[idx]);
      if ((rc = netcalc_set_add_str(ns, data[idx], NULL, NULL, 0)) != 0)
      {  printf("%s: netcalc_set_add_str(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         continue;
      };
      for(dup = offset; (dup < datalen); dup++)
      {  if ((rc = netcalc_set_add_str(ns, data[dup], NULL, NULL, 0)) != NETCALC_EEXISTS)
         {  printf("%s: %s: adding duplicate: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
         if ((rc = netcalc_set_query_str(ns, data[dup], NULL, NULL, NULL, NULL)) != 0)
         {  printf("%s: %s: netcalc_set_query_str: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
      };
      for(dup = 0; (dup <= idx); dup++)
      {  if ((rc = netcalc_set_add_str(ns, data[dup], NULL, NULL, 0)) != NETCALC_EEXISTS)
         {  printf("%s: %s: adding duplicate: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
         if ((rc = netcalc_set_query_str(ns, data[dup], NULL, NULL, NULL, NULL)) != 0)
         {  printf("%s: %s: netcalc_set_query_str: %s\n", PROGRAM_NAME, data[dup], netcalc_strerror(rc));
            errs++;
            continue;
         };
      };
   };

   // initialize cursor
   if ((rc = netcalc_cur_init(ns, &cur)) != 0)
   {  fprintf(stderr, "%s: %s\n", PROGRAM_NAME, netcalc_strerror(rc));
      netcalc_set_free(ns);
      errs++;
      return(errs);
   };

   if ((rc = netcalc_cur_first(cur, &prev, NULL, NULL, NULL, NULL)) != 0)
   {  fprintf(stderr, "%s: %s\n", PROGRAM_NAME, netcalc_strerror(rc));
      netcalc_cur_free(cur);
      netcalc_set_free(ns);
      return(1);
   };

   while(netcalc_cur_next(cur, &net, NULL, NULL, NULL, NULL) == 0)
   {  netcalc_ntop(prev, prev_str, sizeof(prev_str), NETCALC_TYPE_ADDRESS, flags);
      netcalc_ntop(net,  net_str,  sizeof(net_str),  NETCALC_TYPE_ADDRESS, flags);
      printf("      checking order of %s and %s ...\n", prev_str, net_str);
      switch(netcalc_net_cmp(prev, net, NETCALC_FLG_NETWORK))
      {  case NETCALC_CMP_SUPERNET:
         case NETCALC_CMP_BEFORE:
            break;

         case NETCALC_CMP_AFTER:
            fprintf(stderr, "%s: %s added after %s\n", PROGRAM_NAME, prev_str, net_str);
            errs++;
            break;

         case NETCALC_CMP_SAME:
            fprintf(stderr, "%s: %s duplicated\n", PROGRAM_NAME, prev_str);
            errs++;
            break;

         case NETCALC_CMP_SUBNET:
            fprintf(stderr, "%s: %s added as subet of %s\n", PROGRAM_NAME, prev_str, net_str);
            errs++;
            break;

         default:
            fprintf(stderr, "%s: unknown error comparing %s and %s\n", PROGRAM_NAME, prev_str, net_str);
            errs++;
            break;
      };
      netcalc_net_free(prev);
      prev  = net;
      net   = NULL;
   };
   netcalc_net_free(prev);
   prev = NULL;

   netcalc_cur_free(cur);
   netcalc_set_free(ns);

   return(errs);
}


/* end of source */
