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
#define __SRC_WIDGET_PRINTF_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include "netcalc-utility.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _my_info_t
{
   int                  cidr;
   int                  __pad;
   netcalc_net_t *      net;
   char                 address[NETCALC_ADDRESS_LENGTH];
   char                 network[NETCALC_ADDRESS_LENGTH];
   char                 broadcast[NETCALC_ADDRESS_LENGTH];
   char                 netmask[NETCALC_ADDRESS_LENGTH];
   char                 wildcard[NETCALC_ADDRESS_LENGTH];
} my_info_t;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
netcalc_widget_printf(
         netcalc_config_t *            cnf )
{
   int                  idx;
   int                  rc;
   int                  flags;
   size_t               len;
   size_t               size;
   netcalc_net_t **     nets;
   char                 buff[512];

   flags  = cnf->flags;
   flags &= ~cnf->flags_negate;

   // allocates memory
   len   = (size_t)cnf->argc;
   size  = sizeof(netcalc_net_t *) * len;
   if ((nets = malloc(size)) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", netcalc_prog_name(cnf));
      return(1);
   };
   memset(nets, 0, size);

   // process network arguments
   for(idx = 1; (idx < cnf->argc); idx++)
   {
      if ((rc = netcalc_initialize(&nets[idx-1], cnf->argv[idx], cnf->flags)) != NETCALC_SUCCESS)
      {
         fprintf(stderr, "%s: %s: %s\n", netcalc_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
         netcalc_nets_free(nets);
         return(1);
      };
   };

   // print address family information
   for(idx = 0; ((nets[idx])); idx++)
   {
      if (!(netcalc_strfnet(nets[idx], buff, sizeof(buff), cnf->argv[0], flags)))
      {
         fprintf(stderr, "%s: invalid format syntax or format character\n", netcalc_prog_name(cnf));
         return(1);
      };
      printf("%s\n", buff);
   };

   netcalc_nets_free(nets);

   return(0);
}


int
netcalc_widget_printf_usage(
         netcalc_config_t *            cnf )
{
   assert(cnf != NULL);
   printf("\n");
   printf("FORMAT:\n");
   printf("  %%%%    is replaced by '%%'.\n");
   printf("  %%A    is replaced by address using default notation.\n");
   printf("  %%a    is replaced by address.\n");
   printf("  %%b    is replaced by broadcast.\n");
   printf("  %%C    is replaced by CIDR without delimiter.\n");
   printf("  %%c    is conditionally replaced by CIDR with delimiter.\n");
   printf("  %%F    is replaced by family.\n");
   printf("  %%f    is replaced by first usable address.\n");
   printf("  %%I    is conditionally replaced by interface without delimiter.\n");
   printf("  %%i    is conditionally replaced by interface with delimiter.\n");
   printf("  %%l    is replaced by first usable address.\n");
   printf("  %%N    is replaced by netmask.\n");
   printf("  %%n    is replaced by network address.\n");
   printf("  %%P    is replaced by port number without delimiter.\n");
   printf("  %%p    is conditionally replaced by port number with delimiter.\n");
   printf("  %%W    is replaced by wildcard.\n");
   printf("\n");
   printf("FORMAT MODIFIERS:\n");
   printf("  %%-    left justify padded fields\n");
   printf("  %%##   pad field to ## number of spaces\n");
   printf("  %%B    do not use delimiters (EUI48 and EUI64)\n");
   printf("  %%H    use dash delimiters (EUI48 and EUI64)\n");
   printf("  %%L    use colon delimiters (EUI48 and EUI64)\n");
   printf("  %%M    disable IPv4-mapped IPv6 address\n");
   printf("  %%m    enable IPv4-mapped IPv6 address\n");
   printf("  %%O    use dot delimiters (EUI48 and EUI64)\n");
   printf("  %%S    disable zero suppression\n");
   printf("  %%s    enable zero suppression\n");
   printf("  %%Z    disable zero compression\n");
   printf("  %%z    enable zero compression\n");
   printf("\n");
   printf("EXAMPLES:\n");
   printf("  $ %s \"%%-20zsa %%-4C %%P\" [2001:db8:f:300::54f]/64:443  203.0.113.1:143\n", netcalc_prog_name(cnf));
   printf("  2001:db8:f:300::54f  64   443\n");
   printf("  203.0.113.1          128  143\n");
   printf("\n");
   printf("  $ %s \"%%n%%c\" [fe80::216:3eff:fe2a:bab8%%eth0]/64\n", netcalc_prog_name(cnf));
   printf("  fe80::/64\n");
   printf("\n");
   printf("  $ %s \"%%b\" fe80::216:3eff:fe2a:bab8/64\n", netcalc_prog_name(cnf));
   printf("  fe80:0:0:0:ffff:ffff:ffff:ffff\n");
   printf("\n");
   return(0);
}


/* end of source */
