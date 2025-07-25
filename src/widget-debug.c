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
#define __SRC_WIDGET_DEBUG_C 1


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
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static void
my_widget_debug_print(
         const char *                  fld,
         const char *                  val );


static void
my_widget_debug_print_bytes(
         const char *                  fld,
         uint8_t *                     val,
         size_t                        len );


static void
my_widget_debug_print_hex(
         const char *                  fld,
         int                           val );


static void
netcalc_widget_debug_print_int(
         const char *                  fld,
         int                           val );


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
my_widget_debug(
         my_config_t *                 cnf )
{
   int                  idx;
   int                  rc;
   int                  ival;
   int                  family;
   int                  errs;
   netcalc_net_t *      net;
   const char *         str;
   void *               ptr;
   char                 buff[512];

   errs  = 0;

   // process network arguments
   for(idx = 0; (idx < cnf->argc); idx++)
   {
      printf("processing \"%s\" ...\n", cnf->argv[idx]);
      rc = netcalc_initialize(&net, cnf->argv[idx], cnf->flags);
      my_widget_debug_print( "status", netcalc_strerror(rc) );
      netcalc_widget_debug_print_int( "return code", rc );
      if ((rc))
      {
         errs++;
         printf("\n");
         continue;
      };

      // print address family
      netcalc_get_field(net, NETCALC_FLD_FAMILY, &family);
      switch(family)
      {  case NETCALC_AF_EUI48:  str = "EUI48";    break;
         case NETCALC_AF_EUI64:  str = "EUI64";    break;
         case NETCALC_AF_INET:   str = "IPv4";     break;
         case NETCALC_AF_INET6:  str = "IPv6";     break;
         default:                str = "unknown";  break;
      };
      snprintf(buff, sizeof(buff), "%s (%04x)", str, (unsigned)family );
      my_widget_debug_print( "Family", buff );

      netcalc_get_field(net, NETCALC_FLD_FLAGS,  &ival);
      my_widget_debug_print_hex("Flags",    (ival & ~NETCALC_AF));

      str = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
      my_widget_debug_print("Address",  str);

      str = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_NETMASK, cnf->flags);
      my_widget_debug_print("Netmask",  str);

      str = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_WILDCARD, cnf->flags);
      my_widget_debug_print("Wildcard",  str);

      str = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_NETWORK, cnf->flags);
      my_widget_debug_print("Network",  str);

      str = netcalc_ntop(net, NULL, 0, NETCALC_TYPE_BROADCAST, cnf->flags);
      my_widget_debug_print("Broadcast",  str);

      netcalc_get_field(net, NETCALC_FLD_CIDR,        &ival);
      netcalc_widget_debug_print_int("Prefix length", ival);

      netcalc_get_field(net, NETCALC_FLD_PORT,  &ival);
      netcalc_widget_debug_print_int("Port",    ival);

      netcalc_get_field(net, NETCALC_FLD_ADDRLEN,  &ival);
      netcalc_get_field(net, NETCALC_FLD_ADDR,     &ptr);
      my_widget_debug_print_bytes("Binary Address", ptr, ival);
      free(ptr);

      if (family == NETCALC_AF_INET6)
      {
         netcalc_get_field(net, NETCALC_FLD_SCOPE_NAME,  &ptr);
         my_widget_debug_print("Scope Name", (((ptr)) ? (char *)ptr : ""));
         free(ptr);
      };

      netcalc_free(net);

      printf("\n");
   };

   printf("%i errors\n", errs);

   return( ((errs)) ? 1 : 0 );
}


void
my_widget_debug_print(
         const char *                  fld,
         const char *                  val )
{
   char  buff[512];
   snprintf(buff, sizeof(buff), "%s:", fld);
   printf("   %-15s %s\n", buff, val);
   return;
}


void
my_widget_debug_print_bytes(
         const char *                  fld,
         uint8_t *                     val,
         size_t                        len )
{
   size_t   idx;
   size_t   pos;
   char     buff[1024];
   char     map[] = "0123456789abcdef";

   for(idx = 0, pos = 0; (idx < len); idx++)
   {
      buff[pos++] = map[ (val[idx] >> 4) & 0x0f ];
      buff[pos++] = map[ (val[idx] >> 0) & 0x0f ];
      if ((idx % 2) == 1)
         buff[pos++] = ' ';
      if (pos >= (sizeof(buff)-5))
         idx = len;
   };
   buff[pos] = '\0';

   my_widget_debug_print(fld, buff);

   return;
}


void
my_widget_debug_print_hex(
         const char *                  fld,
         int                           val )
{
   char buff[32];
   snprintf(buff, sizeof(buff), "%08x", (unsigned)val);
   my_widget_debug_print(fld, buff);
   return;
}


void
netcalc_widget_debug_print_int(
         const char *                  fld,
         int                           val )
{
   char buff[16];
   snprintf(buff, sizeof(buff), "%i", val);
   my_widget_debug_print(fld, buff);
   return;
}


/* end of source */
