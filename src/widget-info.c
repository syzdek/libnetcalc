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
#define __SRC_WIDGET_INFO_C 1


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

static int
netcalc_widget_info_eui48(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets );


static int
netcalc_widget_info_eui64(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets );


static void
netcalc_widget_info_free(
         netcalc_net_t **              nets );


static int
netcalc_widget_info_ip(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets );


static int
netcalc_widget_info_ip_verbose(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets );


static void
netcalc_widget_info_print(
         const char *                  fld,
         const char *                  val );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

const char *      prog_name;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
netcalc_widget_info(
         netcalc_config_t *            cnf )
{
   int                  idx;
   int                  rc;
   int                  net_family;
   int                  ival;
   size_t               len;
   size_t               size;
   netcalc_net_t **     nets;

   prog_name   = netcalc_prog_name(cnf);
   net_family  = 0;

   // allocates memory
   len   = (size_t)cnf->argc + 2;
   size  = sizeof(netcalc_net_t *) * len;
   if ((nets = malloc(size)) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", prog_name);
      return(1);
   };
   memset(nets, 0, size);

   // process network arguments
   for(idx = 0; (idx < cnf->argc); idx++)
   {
      if ((rc = netcalc_initialize(&nets[idx], cnf->argv[idx], cnf->flags)) != NETCALC_SUCCESS)
      {
         fprintf(stderr, "%s: %s: %s\n", prog_name, cnf->argv[idx], netcalc_strerror(rc));
         netcalc_widget_info_free(nets);
         return(1);
      };
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY, &ival);
      if (!(net_family))
         net_family = ival;
      if (net_family != ival)
      {
         fprintf(stderr, "%s: incompatible address families provided\n", prog_name);
         netcalc_widget_info_free(nets);
         return(1);
      };
   };

   // print address family information
   switch(net_family)
   {
      case NETCALC_AF_EUI48:  rc = netcalc_widget_info_eui48( cnf, nets); break;
      case NETCALC_AF_EUI64:  rc = netcalc_widget_info_eui64( cnf, nets); break;
      case NETCALC_AF_INET:   rc = netcalc_widget_info_ip(    cnf, nets); break;
      case NETCALC_AF_INET6:  rc = netcalc_widget_info_ip(    cnf, nets); break;
      default:
         fprintf(stderr, "%s: unknown or unsupported address family\n", prog_name);
         rc = 1;
         break;
   };

   netcalc_widget_info_free(nets);

   return(rc);
}


int
netcalc_widget_info_eui48(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets )
{
   int                  idx;
   const char *         net_addr_str;

   assert(cnf  != NULL);
   assert(nets != NULL);

   for(idx = 0; ((nets[idx])); idx++)
   {
      netcalc_widget_info_print("Family",  "EUI48");

      net_addr_str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
      netcalc_widget_info_print("Address",  net_addr_str);

      printf("\n");
   };

   return(0);
}


int
netcalc_widget_info_eui64(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets )
{
   int                  idx;
   const char *         net_addr_str;

   assert(cnf  != NULL);
   assert(nets != NULL);

   for(idx = 0; ((nets[idx])); idx++)
   {
      netcalc_widget_info_print("Family",  "EUI64");

      net_addr_str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
      netcalc_widget_info_print("Address",  net_addr_str);

      printf("\n");
   };

   return(0);
}


void
netcalc_widget_info_free(
         netcalc_net_t **              nets )
{
   int      pos;
   if (!(nets))
      return;
   for(pos = 0; ((nets[pos])); pos++)
      netcalc_free(nets[pos]);
   free(nets);
   return;
}


int
netcalc_widget_info_ip(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets )
{
   int                  family;
   int                  idx;
   int                  len_address;
   int                  len_broadcast;
   int                  len_netmask;
   int                  len_network;
   int                  len_wildcard;
   long long unsigned   subnet_count;
   size_t               nets_len;
   size_t               size;
   my_info_t *          recs;
   char                 subnet_count_str[32];

   assert(cnf  != NULL);
   assert(nets != NULL);

   if ((cnf->verbose))
      return(netcalc_widget_info_ip_verbose(cnf, nets));

   len_address    = (int)strlen("Address");;
   len_broadcast  = (int)strlen("Broadcast");;
   len_netmask    = (int)strlen("Netmask");;
   len_network    = (int)strlen("Network");;
   len_wildcard   = (int)strlen("Wildcard");

   netcalc_get_field(nets[0], NETCALC_FLD_FAMILY, &family);

   for(nets_len = 0; ((nets[nets_len])); nets_len++);

   if (nets_len > 1)
      nets_len++;
   size = sizeof(my_info_t) * nets_len;
   if ((recs = malloc(size)) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", cnf->prog_name);
      return(1);
   };
   memset(recs, 0, size);

   for(idx = 0; ((nets[idx])); idx++)
   {
      netcalc_ntop(nets[idx], recs[idx].address,   sizeof(((my_info_t *)0)->address),    NETCALC_TYPE_ADDRESS,   cnf->flags);
      netcalc_ntop(nets[idx], recs[idx].network,   sizeof(((my_info_t *)0)->network),    NETCALC_TYPE_NETWORK,   NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR));
      netcalc_ntop(nets[idx], recs[idx].broadcast, sizeof(((my_info_t *)0)->broadcast),  NETCALC_TYPE_BROADCAST, NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR));
      netcalc_ntop(nets[idx], recs[idx].netmask,   sizeof(((my_info_t *)0)->netmask),    NETCALC_TYPE_NETMASK,   cnf->flags);
      netcalc_ntop(nets[idx], recs[idx].wildcard,  sizeof(((my_info_t *)0)->wildcard),   NETCALC_TYPE_WILDCARD,  cnf->flags);
      netcalc_get_field(nets[idx], NETCALC_FLD_CIDR, &recs[idx].cidr);

      len_address    = (int)(((int)strlen(recs[idx].address)   > len_address)    ? strlen(recs[idx].address)   : len_address);
      len_broadcast  = (int)(((int)strlen(recs[idx].broadcast) > len_broadcast)  ? strlen(recs[idx].broadcast) : len_broadcast);
      len_netmask    = (int)(((int)strlen(recs[idx].netmask)   > len_netmask)    ? strlen(recs[idx].netmask)   : len_netmask);
      len_network    = (int)(((int)strlen(recs[idx].network)   > len_network)    ? strlen(recs[idx].network)   : len_network);
      len_wildcard   = (int)(((int)strlen(recs[idx].wildcard)  > len_wildcard)   ? strlen(recs[idx].wildcard)  : len_wildcard);
   };

   printf(
      "%-*s  %-*s  %-*s  %-*s  %-*s  %-4s  %s\n",
      len_address,      "Address",
      len_network,      "Network",
      len_broadcast,    "Broadcast",
      len_netmask,      "Netmask",
      len_wildcard,     "Wildcard",
      "CIDR",
      ( (family == NETCALC_AF_INET) ? "/32s" : "/64s" )
   );

   for(idx = 0; ((nets[idx])); idx++)
   {
      switch(family)
      {
         case NETCALC_AF_INET:  subnet_count =        0x100000000LLU >> recs[idx].cidr; break;
         case NETCALC_AF_INET6: subnet_count = 0x8000000000000000LLU >> (recs[idx].cidr -1); break;
         default:               subnet_count = 0LLU; break;
      };
      if ( (family == NETCALC_AF_INET6) && (recs[idx].cidr == 0) )
         subnet_count = 0;
      if ( ((subnet_count)) && (recs[idx].cidr <= 64) )
         snprintf(subnet_count_str, sizeof(subnet_count), "%llu", subnet_count);
      else
         snprintf(subnet_count_str, sizeof(subnet_count), "n/a");

      printf(
         "%-*s  %-*s  %-*s  %-*s  %-*s  %-4i  %s\n",
         len_address,      recs[idx].address,
         len_network,      recs[idx].network,
         len_broadcast,    recs[idx].broadcast,
         len_netmask,      recs[idx].netmask,
         len_wildcard,     recs[idx].wildcard,
         recs[idx].cidr,
         subnet_count_str
      );
   };

   free(recs);

   return(0);
}


int
netcalc_widget_info_ip_verbose(
         netcalc_config_t *            cnf,
         netcalc_net_t **              nets )
{
   int                  idx;
   int                  ival;
   int                  family;
   int                  cidr;
   int                  flags;
   const char *         str;
   char                 buff[512];

   assert(cnf  != NULL);
   assert(nets != NULL);

   for(idx = 0; ((nets[idx])); idx++)
   {
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY,   &family);
      netcalc_get_field(nets[idx], NETCALC_FLD_CIDR,     &cidr);

      if (idx < cnf->argc)
      {
         str = (family == NETCALC_AF_INET) ? "IPv4 String" : "IPv6 String";
         netcalc_widget_info_print(str, cnf->argv[idx]);
      } else
      {
         printf("%s SUPERBLOCK\n", (family == NETCALC_AF_INET) ? "IPv4" : "IPv6");
      };

      flags = NETCALC_UNSET(cnf->flags, (NETCALC_FLG_PORT | NETCALC_FLG_CIDR | NETCALC_FLG_IFACE));
      str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_ADDRESS, flags);
      netcalc_widget_info_print("Host Address", str);

      str = NULL;
      netcalc_get_field(nets[idx], NETCALC_FLD_SCOPE_NAME, &str);
      if ((str))
         netcalc_widget_info_print("Scope Name", str);

      flags = NETCALC_UNSET(cnf->flags, (NETCALC_FLG_PORT | NETCALC_FLG_CIDR));
      str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_NETWORK, flags);
      netcalc_widget_info_print("Network Address", str);

      flags = NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR);
      str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_BROADCAST, flags);
      netcalc_widget_info_print("Broadcast Address", str);

      str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_NETMASK, cnf->flags);
      netcalc_widget_info_print("Netmask", str);

      str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_WILDCARD, cnf->flags);
      netcalc_widget_info_print("Wildcard", str);

      snprintf(buff, sizeof(buff), "%i", cidr);
      netcalc_widget_info_print("CIDR", buff);

      netcalc_get_field(nets[idx], NETCALC_FLD_PORT, &ival);
      if ((ival))
      {
         snprintf(buff, sizeof(buff), "%i", ival);
         netcalc_widget_info_print("Port", buff);
      };

      if ( ((family == NETCALC_AF_INET6) && (cidr < 128)) || ((family == NETCALC_AF_INET) && (cidr < 32)) )
      {
         str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_FIRST, cnf->flags);
         netcalc_widget_info_print("Usable Range", str);
         str = netcalc_ntop(nets[idx], NULL, 0, NETCALC_TYPE_LAST, cnf->flags);
         netcalc_widget_info_print(NULL, str);
      };

      printf("\n");
   };

   return(0);
}



void
netcalc_widget_info_print(
         const char *                  fld,
         const char *                  val )
{
   char  buff[512];
   buff[0] = '\0';
   if ((fld))
      snprintf(buff, sizeof(buff), "%s:", fld);
   printf("%-20s %s\n", buff, val);
   return;
}


/* end of source */
