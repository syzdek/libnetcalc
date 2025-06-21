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
#define __TESTS_NETCALC_INITIALIZE_C 1
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

#include <netcalc.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "netcalc-initialize"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

int
main(
         int                           argc,
         char *                        argv[] );


int
my_test(
         testdata_t *                  dat );


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
   uint32_t          family;
   testdata_t        dat;

   // getopt options
   static const char *  short_opt = "46EehqVv";
   static struct option long_opt[] =
   {
      {"ipv4",             no_argument,       NULL, '4' },
      {"ipv6",             no_argument,       NULL, '6' },
      {"eui48",            no_argument,       NULL, 'e' },
      {"eui64",            no_argument,       NULL, 'E' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   family = NETCALC_AF;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case '4': family = NETCALC_AF_INET;    break;
         case '6': family = NETCALC_AF_INET;    break;
         case 'E': family = NETCALC_AF_EUI64;   break;
         case 'e': family = NETCALC_AF_EUI48;   break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("       %s [OPTIONS] <string> <string> ... <string> \n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -4, --ipv4                test for IPv4 address\n");
         printf("  -6, --ipv6                test for IPv6 address\n");
         printf("  -e, --eui48               test for EUI48 string/MAC address\n");
         printf("  -E, --eui64               test for EUI64 string\n");
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

   if (optind == argc)
   {
      for(pos = 0; ((test_data[pos].addr_str)); pos++)
         if ((test_data[pos].addr_flgs & family))
            if ((my_test(&test_data[pos])))
               errors++;
   } else
   {
      dat.addr_flgs  = family;
      dat.addr_port  = MY_NOPORT;
      dat.addr_cidr  = MY_NOCIDR;
      dat.addr_fail  = 0;
      for(pos = optind; (pos < argc); pos++)
      {
         dat.addr_str   = argv[pos];
         if ((my_test(&dat)))
            errors++;
      };
   };

   return( ((errors)) ? 1 : 0 );
}


int
my_test(
         testdata_t *                  dat )
{
   netcalc_net_t *   net;
   int               rc;
   int               net_cidr;
   int               net_family;
   int               net_port;
   int               dat_cidr;
   int               dat_family;
   int               dat_port;

   net = NULL;

   dat_cidr       = (int)dat->addr_cidr;
   dat_family     = (int)(dat->addr_flgs & NETCALC_AF);
   dat_port       = (int)dat->addr_port;

   if (!(quiet))
      printf("checking: \"%s\" (should %s) ...\n", dat->addr_str, ( ((dat->addr_fail)) ? "fail" : "pass" ) );
   rc = netcalc_initialize(&net, dat->addr_str, dat->addr_flgs);
   if ( ((rc)) && (dat->addr_fail == MY_PASS) )
   {
      if (!(quiet))
         printf("   failure code:  %i (%s)\n", rc, netcalc_strerror(rc));
      if ((verbose))
         printf("\n");
      return(1);
   };
   if ( (!(rc)) && (dat->addr_fail != MY_PASS) )
   {
      if (!(quiet))
         printf("   success code:  %i (%s)\n", rc, netcalc_strerror(rc));
      if ((verbose))
         printf("\n");
      netcalc_free(net);
      return(1);
   };
   if ((verbose))
   {
         printf("   status:        %s\n", netcalc_strerror(rc));
         printf("   return code:   %i\n", rc);
   };
   if ((rc))
   {
      if ((verbose))
         printf("\n");
      return(0);
   };

   // check family
   netcalc_get_field(net, NETCALC_FLD_FAMILY, &net_family);
   if ( ((dat_family)) && (net_family != dat_family) )
   {
         printf("   family:        %08x\n", net_family);
         printf("   expected:      %08x\n", dat_family);
         return(1);
   };
   if ((verbose))
      printf("   family:        %08x\n", net_family);

   // check port
   if (dat_port != MY_NOPORT)
   {
      netcalc_get_field(net, NETCALC_FLD_PORT, &net_port);
      if (net_port != dat_port)
      {
            printf("   port:          %i\n", net_port);
            printf("   expected:      %i\n", dat_port);
            return(1);
      };
      if ((verbose))
         printf("   port:          %08x\n", net_port);
   };

   // check cidr
   if (dat_cidr != MY_NOCIDR)
   {
      netcalc_get_field(net, NETCALC_FLD_CIDR, &net_cidr);
      if (net_cidr != dat_cidr)
      {
            printf("   cidr:          %i\n", net_cidr);
            printf("   expected:      %i\n", dat_cidr);
            return(1);
      };
      if ((verbose))
         printf("   cidr:          %08x\n", net_cidr);
   };

   netcalc_free(net);

   if ((verbose))
         printf("\n");

   return(0);
}


/* end of source */
