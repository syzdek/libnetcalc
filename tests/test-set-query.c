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
#include <stdlib.h>

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


static int
my_pass(
         const char *                  name,
         const char * const *          data,
         const testquery_t *           queries );


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

   // check set 2
   if ((my_pass("set2", test_set2, test_query2)))
      errs++;

   return( ((errs)) ? 1 : 0 );
}


int
my_pass(
         const char *                  name,
         const char * const *          data,
         const testquery_t *           queries )
{
   int               rc;
   int               errs;
   int               flags;
   size_t            idx;
   netcalc_set_t *   ns;
   netcalc_net_t *   net;
   netcalc_net_t *   exp;
   netcalc_net_t *   res;
   netcalc_cur_t *   cur;
   char *            comment;
   char              str[NETCALC_ADDRESS_LENGTH+64];
   char              exp_str[NETCALC_ADDRESS_LENGTH];
   char              res_str[NETCALC_ADDRESS_LENGTH];

   errs  = 0;
   flags = NETCALC_FLG_SUPR | NETCALC_FLG_COMPR | NETCALC_FLG_CIDR;

   printf("testing %s ...\n", name);
   printf("   loading set ...\n");

   if ((rc = netcalc_set_init(&ns, NULL, 0)) != 0)
   {  printf("%s: %s\n", PROGRAM_NAME, netcalc_strerror(rc));
      return(1);
   };

   // add addresses to sets
   for(idx = 0; ((data[idx])); idx++)
   {  if ((verbose))
         printf("      adding %s ...\n", data[idx]);
      snprintf(str, sizeof(str), "test data %s (%zu)", data[idx], idx);
      if ((rc = netcalc_set_add_str(ns, data[idx], str, NULL, 0)) != 0)
      {  printf("%s: netcalc_set_add_str(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         continue;
      };
   };

   // print set data
   if ((verbose))
   {  idx = 0;
      printf("   set data ...\n");
      if ((rc = netcalc_cur_init(ns, &cur)) != 0)
      {  netcalc_set_free(ns);
         fprintf(stderr, "%s: netcalc_cur_init(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         return(errs);
      };
      if ((rc = netcalc_cur_first(cur, &net, &comment, NULL, NULL, NULL)) != 0)
      {  netcalc_set_free(ns);
         netcalc_cur_free(cur);
         fprintf(stderr, "%s: netcalc_cur_first(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         return(errs);
      };
      printf(  "      %3zu: %-25s comment: %s\n",
               idx,
               netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, flags),
               ( ((comment)) ? comment : "n/a")
            );
      netcalc_free(net);
      if ((comment))
         free(comment);
      while((rc = netcalc_cur_next(cur, &net, &comment, NULL, NULL, NULL)) == 0)
      {  idx++;
         printf(  "      %3zu: %-25s comment: %s\n",
                  idx,
                  netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, flags),
                  ( ((comment)) ? comment : "n/a")
               );
         netcalc_free(net);
         if ((comment))
            free(comment);
      };
      if (rc != NETCALC_ENOREC)
      {  netcalc_set_free(ns);
         netcalc_cur_free(cur);
         fprintf(stderr, "%s: netcalc_cur_next(): %s\n", PROGRAM_NAME, netcalc_strerror(rc));
         errs++;
         return(errs);
      };
      netcalc_cur_free(cur);
   };

   // test queries
   printf("   testing queries ...\n");
   for(idx = 0; ((queries[idx].query_addr)); idx++)
   {  printf(  "      test %s (should %s) ...\n",
               queries[idx].query_addr,
               ( ((queries[idx].query_exp)) ? "pass" : "fail")
            );

      // generate netcalc_net_t for expected response
      exp         = NULL;
      exp_str[0]  = '\0';
      if ((queries[idx].query_exp))
      {  if ((rc = netcalc_init(&exp, queries[idx].query_exp, flags)) != 0)
         {  fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, queries[idx].query_addr, netcalc_strerror(rc));
            errs++;
            return(errs);
         };
         netcalc_ntop(exp, exp_str, sizeof(exp_str), NETCALC_TYPE_ADDRESS, flags);
      };

      // query for matching network
      res = NULL;
      rc  = netcalc_set_query_str(ns, queries[idx].query_addr, &res, &comment, NULL, NULL);
      res_str[0] = '\0';
      if ((res))
         netcalc_ntop(res, res_str, sizeof(res_str), NETCALC_TYPE_ADDRESS, flags);

      // verify query which should pass had result
      if ( ((exp)) && ((rc)) )
      {  printf(  "%s: %s: expected %s: netcalc_set_add_str(): %s\n",
                  PROGRAM_NAME,
                  queries[idx].query_addr,
                  exp_str,
                  netcalc_strerror(rc)
               );
         netcalc_free(exp);
         errs++;
         continue;
      };

      // verify query which should fail did not have a result
      if ( (!(exp)) && (!(rc)) )
      {  printf(  "%s: %s: should fail but matched %s\n",
                  PROGRAM_NAME,
                  queries[idx].query_addr,
                  netcalc_ntop(res, 0, 0, NETCALC_TYPE_ADDRESS, flags)
               );
         errs++;
         netcalc_free(res);
         continue;
      };

      // continue if no result was returned
      if (!(res))
      {  if ((exp))
            netcalc_free(exp);
         continue;
      };

      // verify that expected result and result networks match
      rc = netcalc_cmp(exp, res, NETCALC_FLG_NETWORK);
      if (rc != NETCALC_CMP_SAME)
      {  printf(  "%s: %s: expected network %s and matched %s\n",
                  PROGRAM_NAME,
                  queries[idx].query_addr,
                  exp_str,
                  res_str
               );
         errs++;
         netcalc_free(res);
         netcalc_free(exp);
         continue;
      };

      // verify that expected result and result addresses match
      rc = netcalc_cmp(exp, res, 0);
      if (rc != NETCALC_CMP_SAME)
      {  printf(  "%s: %s: expected address %s and matched %s\n",
                  PROGRAM_NAME,
                  queries[idx].query_addr,
                  exp_str,
                  res_str
               );
         printf("%s: compare result: %s\n", PROGRAM_NAME, netcalc_strcmp(rc));
         errs++;
         netcalc_free(res);
         netcalc_free(exp);
         continue;
      };

      // free resources
      netcalc_free(res);
      netcalc_free(exp);
   };

   netcalc_set_free(ns);

   return(errs);
}


/* end of source */
