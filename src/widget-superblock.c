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
#define __SRC_WIDGET_SUPERBLOCK_C 1


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
my_widget_superblock(
         my_config_t *                 cnf )
{
   int                  idx;
   int                  rc;
   int                  family;
   int                  af_ipv6;
   int                  af_other;
   int                  cidr;
   int                  flags;
   size_t               size;
   netcalc_net_t *      superblock;
   netcalc_net_t **     nets;
   my_rec_t **          recs;
   my_len_t             lens;

   cnf->flags &= ~cnf->flags_negate;
   family      = 0;
   af_ipv6     = 0;
   af_other    = 0;

   // allocates memory
   size = sizeof(netcalc_net_t *) * ((size_t)cnf->argc + 2);
   if ((nets = malloc(size)) == NULL)
   {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
      return(1);
   };
   memset(nets, 0, size);

   // process network arguments
   for(idx = 0; (idx < cnf->argc); idx++)
   {  if ((rc = netcalc_init(&nets[idx], cnf->argv[idx], cnf->flags)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
         my_nets_free(nets);
         return(1);
      };
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY, &family);

      // convert address to prefix address family
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY, &family);
      if ( ((cnf->net_prefix)) && (cnf->net_prefix_family != family) )
      {  if ((rc = netcalc_convert(nets[idx], cnf->net_prefix_family, cnf->net_prefix)) != 0)
         {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->argv[idx], netcalc_strerror(rc));
            my_nets_free(nets);
            return(1);
         };
         family = cnf->net_prefix_family;
      };

      if (family == NETCALC_AF_INET6)
         af_ipv6 = family;
      else if (family != NETCALC_AF_INET)
         af_other = family;
   };

   // normalize address families
   if ((af_ipv6))
   {  af_other = 0;
      for(idx = 0; (idx < cnf->argc); idx++)
         netcalc_convert(nets[idx], NETCALC_AF_INET6, NULL);
   };
   if ((af_other))
   {  my_nets_free(nets);
      fprintf(stderr, "%s: %s: unsupported address families\n", my_prog_name(cnf), cnf->argv[idx]);
      return(1);
   };

   // calculate superblock and free networks
   rc = netcalc_superblock(&superblock, (const netcalc_net_t * const *)nets, cnf->argc);
   for(idx = 0; ((nets[idx])); idx++)
      netcalc_free(nets[idx]);
   free(nets);
   nets = NULL;
   if (rc != NETCALC_SUCCESS)
   {  fprintf(stderr, "%s: netcalc_superblock(): %s\n", my_prog_name(cnf), netcalc_strerror(rc));
      return(1);
   };
   netcalc_get_field(superblock, NETCALC_FLD_CIDR, &cidr);

   if ((recs = my_recs_alloc(cnf, (size_t)(cidr+4))) == NULL)
   { netcalc_free(superblock);
      return(1);
   };
   recs[0]->net = superblock;
   my_rec_process(cnf, recs[0]);

   if ((cnf->verbose))
   {  for(idx = 1; (cidr > 0); idx++)
      {  cidr--;
         if ((rc = netcalc_dup(&recs[idx]->net, superblock)) != NETCALC_SUCCESS)
         {  fprintf(stderr, "%s: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
            my_recs_free(recs);
         };
         netcalc_network_mask(recs[idx]->net, NULL, cidr);
         my_rec_process(cnf, recs[idx]);
      };
   };


   my_recs_lengths(recs, &lens);

   flags = recs[0]->family;
   if (!(cnf->quiet))
      my_rec_summary_ip(NULL, &lens, flags);
   for(idx = 0; ((recs[idx]->net)); idx++)
      my_rec_summary_ip(recs[idx], &lens, flags);

   my_recs_free(recs);

   return(0);
}


/* end of source */
