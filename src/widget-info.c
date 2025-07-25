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


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static int
my_widget_info_eui(
         my_config_t *                 cnf,
         my_rec_t **                   recs );


static int
my_widget_info_ip(
         my_config_t *                 cnf,
         my_rec_t **                   recs,
         int                           family_any );


static int
my_widget_info_ip_verbose(
         my_config_t *                 cnf,
         my_rec_t **                   recs );


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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
my_widget_info(
         my_config_t *                 cnf )
{
   int                  idx;
   int                  rc;
   int                  net_family;
   int                  ival;
   int                  family_any;
   my_rec_t **          recs;

   net_family  = 0;
   family_any  = 0;

   cnf->flags &= ~cnf->flags_negate;

   // allocates memory
   if ((recs = my_recs_alloc(cnf, (size_t)cnf->argc + 2)) == NULL)
   {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
      return(1);
   };

   // process network arguments
   for(idx = 0; (idx < cnf->argc); idx++)
   {
      if ((rc = netcalc_initialize(&recs[idx]->net, cnf->argv[idx], cnf->flags)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
         my_recs_free(recs);
         return(1);
      };

      // convert address to prefix address family
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_FAMILY, &ival);
      if ( ((cnf->net_prefix)) && (cnf->net_prefix_family != ival) )
      {  if ((rc = netcalc_convert(recs[idx]->net, cnf->net_prefix_family, cnf->net_prefix)) != 0)
         {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
            my_recs_free(recs);
            return(1);
         };
      };
      my_rec_process(cnf, recs[idx]);

      if (!(net_family))
      {  if ((recs[idx]->family & (NETCALC_AF_INET | NETCALC_AF_INET6)))
            net_family = NETCALC_AF_INET | NETCALC_AF_INET6;
         else if ((recs[idx]->family & (NETCALC_AF_EUI48 | NETCALC_AF_EUI64)))
            net_family = NETCALC_AF_EUI48 | NETCALC_AF_EUI64;
         else
            net_family = recs[idx]->family;
      };
      if (!(net_family & recs[idx]->family))
         family_any = 1;
   };

   // print address family information
   if ((family_any))
   {  rc  = 0;
      rc += my_widget_info_eui( cnf, recs);
      rc += my_widget_info_ip(  cnf, recs, family_any);
   }
   else if ((net_family & (NETCALC_AF_EUI48 | NETCALC_AF_EUI64)))
      rc = my_widget_info_eui( cnf, recs);
   else if ((net_family & (NETCALC_AF_INET | NETCALC_AF_INET6)))
      rc = my_widget_info_ip( cnf, recs, family_any);
   else
   {  fprintf(stderr, "%s: unknown or unsupported address family\n", my_prog_name(cnf));
      rc = 1;
   };

   my_recs_free(recs);

   return(((rc)) ? 1 : 0);
}


int
my_widget_info_eui(
         my_config_t *                 cnf,
         my_rec_t **                   recs)
{
   int                  idx;
   int                  family;
   int                  rc;
   int                  ival;
   int                  bit;
   int                  pos;
   char                 buff[512];
   const char *         str;
   netcalc_net_t *      dup;

   assert(cnf  != NULL);
   assert(recs != NULL);

   for(idx = 0; ((recs[idx]->net)); idx++)
   {
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_FAMILY,   &family);
      if ( (family != NETCALC_AF_EUI48) && (family != NETCALC_AF_EUI64) )
         continue;

      if ((rc = netcalc_dup(&dup, recs[idx]->net)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
         return(rc);
      };

      snprintf(
         buff, sizeof(buff),
         "%s (%s)",
         cnf->argv[idx] ,
         ((family == NETCALC_AF_EUI48) ? "EUI48" : "EUI64")
      );
      printf("%s\n", buff);
      for(pos = 0; (pos < (int)strlen(buff)); pos++)
         printf("-");
      printf("\n");

      if (family == NETCALC_AF_EUI48)
      {
         str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
         netcalc_widget_info_print("EUI48",  str);
         if ((rc = netcalc_convert(dup, NETCALC_AF_EUI64, NULL)) == 0)
         {  str = netcalc_ntop(dup, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
            netcalc_widget_info_print("Modified EUI64",  str);
         };
      } else if ((rc = netcalc_convert(dup, NETCALC_AF_EUI48, NULL)) == 0)
      {  str = netcalc_ntop(dup, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
         netcalc_widget_info_print("EUI48",  str);
         str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
         netcalc_widget_info_print("Modified EUI64",  str);
      } else
      {  str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
         netcalc_widget_info_print("EUI64",  str);
      };

      netcalc_get_field(recs[idx]->net, NETCALC_FLD_UL,       &ival);
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_UL_BIT,   &bit);
      str   = (ival == NETCALC_UL_LOCAL)
            ? "(locally administered)"
            : "(universally administered)";
      snprintf(buff, sizeof(buff), "%i %s", bit, str);
      netcalc_widget_info_print("U/L Bit", buff);

      netcalc_get_field(recs[idx]->net, NETCALC_FLD_IG,       &ival);
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_IG_BIT,   &bit);
      str   = (ival == NETCALC_IG_UNICAST)
            ? "(unicast)"
            : "(multicast)";
      snprintf(buff, sizeof(buff), "%i %s", bit, str);
      netcalc_widget_info_print("I/G Bit", buff);

      if ((rc = netcalc_convert(dup, NETCALC_AF_INET6, NULL)) == 0)
      {  str = netcalc_ntop(dup, NULL, 0, NETCALC_TYPE_ADDRESS, cnf->flags);
         netcalc_widget_info_print("IPv6 SLAAC Address",  str);
      };

      netcalc_free(dup);

      printf("\n\n");
   };

   return(0);
}


int
my_widget_info_ip(
         my_config_t *                 cnf,
         my_rec_t **                   recs,
         int                           family_any )
{
   int                  flags;
   int                  idx;
   size_t               size;
   netcalc_net_t **     mets;
   my_len_t             lens;

   assert(cnf  != NULL);
   assert(recs != NULL);

   // calculate superblock
   if ( (cnf->argc > 1) && (!(family_any)) )
   {
      size = sizeof(netcalc_net_t *) * ((size_t)(cnf->argc+1));
      if ((mets = malloc(size)) == NULL)
      {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
         return(1);
      };
      memset(mets, 0, size);
      for(idx = 0; (idx < cnf->argc); idx++)
         mets[idx] = recs[idx]->net;
      netcalc_superblock(&recs[cnf->argc]->net, mets, cnf->argc);
      recs[cnf->argc]->ip_superblock = 1;
      my_rec_process(cnf, recs[cnf->argc]);
      free(mets);
   };

   if ( ((cnf->verbose)) || ((family_any)) )
      return(my_widget_info_ip_verbose(cnf, recs));

   my_recs_lengths(recs, &lens);

   flags  = (cnf->argc > 1) ? MY_FLG_SHOW_ADDR : 0;
   flags |= recs[0]->family;
   if (!(cnf->quiet))
      my_rec_summary_ip(NULL, &lens, flags);

   for(idx = 0; ((recs[idx]->net)); idx++)
      my_rec_summary_ip(recs[idx], &lens, flags);

   return(0);
}


int
my_widget_info_ip_verbose(
         my_config_t *                 cnf,
         my_rec_t **                   recs )
{
   int                  rc;
   int                  idx;
   int                  ival;
   int                  family;
   int                  cidr;
   int                  flags;
   int                  pos;
   const char *         str;
   char                 buff[512];
   netcalc_net_t *      eui;

   assert(cnf  != NULL);
   assert(recs != NULL);

   for(idx = 0; ((recs[idx]->net)); idx++)
   {
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_FAMILY,   &family);
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_CIDR,     &cidr);

      if ( (family != NETCALC_AF_INET) && (family != NETCALC_AF_INET6) )
         continue;

      if ((rc = netcalc_dup(&eui, recs[idx]->net)) != 0)
      {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
         return(rc);
      };

      snprintf(
         buff, sizeof(buff),
         "%s (%s)",
         ((idx < cnf->argc) ? cnf->argv[idx] : "SUPERBLOCK"),
         ((family == NETCALC_AF_INET) ? "IPv4" : "IPv6")
      );
      printf("%s\n", buff);
      for(pos = 0; (pos < (int)strlen(buff)); pos++)
         printf("-");
      printf("\n");

      flags = NETCALC_UNSET(cnf->flags, (NETCALC_FLG_PORT | NETCALC_FLG_CIDR | NETCALC_FLG_IFACE));
      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ADDRESS, flags);
      netcalc_widget_info_print("Host Address", str);

      netcalc_get_field(recs[idx]->net, NETCALC_FLD_IPV4_MAPPED, &ival);
      if ( ((ival)) && (!(cnf->flags & NETCALC_FLG_V4MAPPED)) )
      {  flags  = NETCALC_UNSET(cnf->flags, (NETCALC_FLG_PORT | NETCALC_FLG_CIDR | NETCALC_FLG_IFACE));
         flags |= NETCALC_FLG_V4MAPPED;
         str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ADDRESS, flags);
         netcalc_widget_info_print("IPv4-mapped Address", str);
      };

      str = NULL;
      netcalc_get_field(recs[idx]->net, NETCALC_FLD_SCOPE_NAME, &str);
      if ((str))
         netcalc_widget_info_print("Scope Name", str);

      flags = NETCALC_UNSET(cnf->flags, (NETCALC_FLG_PORT | NETCALC_FLG_CIDR));
      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_NETWORK, flags);
      netcalc_widget_info_print("Network Address", str);

      flags = NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR);
      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_BROADCAST, flags);
      netcalc_widget_info_print("Broadcast Address", str);

      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_NETMASK, cnf->flags);
      netcalc_widget_info_print("Netmask", str);

      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_WILDCARD, cnf->flags);
      netcalc_widget_info_print("Wildcard", str);

      snprintf(buff, sizeof(buff), "%i", cidr);
      netcalc_widget_info_print("CIDR", buff);

      netcalc_get_field(recs[idx]->net, NETCALC_FLD_PORT, &ival);
      if ((ival))
      {
         snprintf(buff, sizeof(buff), "%i", ival);
         netcalc_widget_info_print("Port", buff);
      };

      if ( ((family == NETCALC_AF_INET6) && (cidr < 128)) || ((family == NETCALC_AF_INET) && (cidr < 32)) )
      {
         flags |= NETCALC_FLG_SUPR;
         flags = NETCALC_UNSET(flags, (NETCALC_FLG_COMPR | NETCALC_FLG_PORT | NETCALC_FLG_CIDR | NETCALC_FLG_IFACE));
         str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_FIRST, flags);
         netcalc_widget_info_print("Usable Range", str);
         str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_LAST, flags);
         netcalc_widget_info_print(NULL, str);
      };

      if ((rc = netcalc_convert(eui, NETCALC_AF_EUI48, NULL)) == 0)
      {  str = netcalc_ntop(eui, NULL, 0, NETCALC_TYPE_ADDRESS, 0);
         netcalc_widget_info_print("MAC Address", str);
         if ((rc = netcalc_convert(eui, NETCALC_AF_EUI64, NULL)) == 0)
         {  str = netcalc_ntop(eui, NULL, 0, NETCALC_TYPE_ADDRESS, 0);
            netcalc_widget_info_print("Modified EUI64", str);
         };
      };

      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ARPA_HOST, 0);
      netcalc_widget_info_print("DNS ARPA Host", str);

      str = netcalc_ntop(recs[idx]->net, NULL, 0, NETCALC_TYPE_ARPA_ZONE, 0);
      netcalc_widget_info_print("DNS ARPA Zone", str);

      str = netcalc_ntop(recs[idx]->net, buff, sizeof(buff), NETCALC_TYPE_ARPA_REC, 0);
      netcalc_strlcat(buff, " IN PTR", sizeof(buff));
      netcalc_widget_info_print("DNS ARPA RR", buff);

      netcalc_free(eui);

      printf("\n\n");
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
