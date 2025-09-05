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
#define __LIB_LIBNETCALC_LSETS_C 1
#include "libnetcalc.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros


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
netcalc_rec_free(
         netcalc_rec_t *               rec );


extern int
netcalc_set_bindex(
         netcalc_set_t *               ns,
         const netcalc_net_t *         key,
         const netcalc_recs_t *        base,
         uint32_t *                    wouldbep );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions


void
netcalc_rec_free(
         netcalc_rec_t *               rec )
{
   uint32_t                   rec_idx;
   netcalc_rec_t *            recs[128];

   assert(rec != NULL);

   rec_idx        = 0;
   recs[rec_idx]  = rec;

   while(rec)
   {  if ((rec->rec_children.len))
      {  recs[++rec_idx] = rec->rec_children.list[--rec->rec_children.len];
         rec = recs[rec_idx];
         continue;
      };

      if ((rec->rec_children.list))
         free(rec->rec_children.list);

      if ((rec->rec_comment))
         free(rec->rec_comment);

      free(rec);

      rec = ((rec_idx)) ? recs[--rec_idx] : NULL;
   };

   return;
}


int
netcalc_set_bindex(
         netcalc_set_t *               ns,
         const netcalc_net_t *         key,
         const netcalc_recs_t *        base,
         uint32_t *                    wouldbep )
{
   int                  rc;
   int32_t              low;
   int32_t              mid;
   int32_t              high;
   netcalc_rec_t *      rec;
   uint32_t             nel;

   assert(ns         != NULL);
   assert(key        != NULL);
   assert(base       != NULL);
   assert(wouldbep   != NULL);

   nel = base->len;

   if (nel == 0)
   {  if ((wouldbep))
         *wouldbep = 0;
      return(NETCALC_IDX_INSERT);
   };

   low  = 0;
   high = (int32_t)(nel - 1);
   mid  = high / 2;

   while ( (mid >= low) && (mid <= high) && (high != low) )
   {  rec   = base->list[mid];
      rc    = netcalc_addr_cmp(&key->net_addr, key->net_cidr, &rec->rec_addr, rec->rec_cidr);
      switch(rc)
      {  case NETCALC_CMP_SAME:
            *wouldbep = (uint32_t)mid;
            return(NETCALC_IDX_SAME);

         case NETCALC_CMP_SUPERNET:
            high = mid;
            break;

         case NETCALC_CMP_SUBNET:
            *wouldbep = (uint32_t)mid;
            return(NETCALC_IDX_SUBNET);

         default:
            if (rc < 0)
               high = mid - 1;
            else
               low = mid + 1;
            break;
      };
      mid = (high + low) / 2;
   };

   rec   = base->list[mid];
   rc    = netcalc_addr_cmp(&key->net_addr, key->net_cidr, &rec->rec_addr, rec->rec_cidr);
   switch(rc)
   {  case NETCALC_CMP_SAME:
         *wouldbep = (uint32_t)mid;
         return(NETCALC_IDX_SAME);

      case NETCALC_CMP_SUPERNET:
         *wouldbep = (uint32_t)mid;
         return(NETCALC_IDX_SUPERNET);

      case NETCALC_CMP_SUBNET:
         *wouldbep = (uint32_t)mid;
         return(NETCALC_IDX_SUBNET);

      default:
         break;
   };

   *wouldbep = (uint32_t)((rc < 0) ? mid : mid+1);

   return(NETCALC_IDX_INSERT);
}


int
netcalc_set_add(
         netcalc_set_t *               ns,
         netcalc_net_t *               net,
         const char *                  comment,
         void *                        data,
         int                           flags )
{
   int                  rc;
   uint32_t             idx;
   uint32_t             count;
   uint32_t             off;
   size_t               size;
   void *               ptr;
   netcalc_recs_t *     base;
   uint32_t             wouldbe;
   netcalc_rec_t *      rec;
   netcalc_rec_t *      child;

   assert(ns   != NULL);
   assert(net  != NULL);

   if (!( (net->net_flags & NETCALC_AF) | (ns->set_flags & NETCALC_AF) ))
      return(NETCALC_EINVAL);

   if ((netcalc_verify(net, NETCALC_TYPE_NETWORK)))
      return(NETCALC_EINVAL);

   // add information to record
   if ((rec = malloc(sizeof(netcalc_rec_t))) == NULL)
      return(NETCALC_ENOMEM);
   memset(rec, 0, sizeof(netcalc_rec_t));
   memcpy(&rec->rec_addr.addr8, &net->net_addr.addr8, sizeof(netcalc_addr_t));
   rec->rec_cidr   = net->net_cidr;
   rec->rec_data   = data;
   rec->rec_flags  = ns->set_flags & ~(NETCALC_AF | NETCALC_USR);
   rec->rec_flags |= net->net_flags & NETCALC_AF;
   rec->rec_flags |= flags & NETCALC_USR;
   if ((comment))
   {  if ((rec->rec_comment = strdup(comment)) == NULL)
      {  netcalc_rec_free(rec);
         return(NETCALC_ENOMEM);
      };
   };

   base = &ns->set_recs;
   while ((rc = netcalc_set_bindex(ns, net, base, &wouldbe)) != NETCALC_IDX_SAME)
   {  switch(rc)
      {  case NETCALC_IDX_INSERT:
            // allocate memory for net record pointer
            size = (base->size + 2) * sizeof(netcalc_rec_t *);
            if ((ptr = realloc(base->list, size)) == NULL)
            {  netcalc_rec_free(rec);
               return(NETCALC_ENOMEM);
            };
            base->size++;
            base->list              = ptr;
            base->list[base->size]  = NULL;
            // shift records down list
            for(idx = base->len; (idx > wouldbe); idx--)
               base->list[idx] = base->list[idx-1];
            // record new record
            base->len++;
            base->list[wouldbe]  = rec;
            return(NETCALC_SUCCESS);

         case NETCALC_IDX_SUBNET:
            if ((ns->set_flags & NETCALC_FLG_UNIQ))
            {  netcalc_rec_free(rec);
               return(NETCALC_ESUBNET);
            };
            base = &base->list[wouldbe]->rec_children;
            break;

         case NETCALC_IDX_SUPERNET:
            if ((ns->set_flags & NETCALC_FLG_UNIQ))
            {  netcalc_rec_free(rec);
               return(NETCALC_ESUPERNET);
            };
            count   = 0;
            rc    = NETCALC_CMP_SUPERNET;
            while((rc == NETCALC_CMP_SUPERNET) && ((count+wouldbe+1) < base->len))
            {  child = base->list[count+wouldbe+1];
               rc    = netcalc_addr_cmp(&rec->rec_addr, rec->rec_cidr, &child->rec_addr, child->rec_cidr);
               count++;
            };
            size = ((size_t)count+1) * sizeof(netcalc_rec_t *);
            if ((rec->rec_children.list = malloc(size)) == NULL)
            {  netcalc_rec_free(rec);
               return(NETCALC_ENOMEM);
            };
            memset(rec->rec_children.list, 0, size);
            for(off = 0; (off < count); off++)
               rec->rec_children.list[off]   = base->list[wouldbe+off];
            rec->rec_children.list[off]      = NULL;
            rec->rec_children.size           = count;
            rec->rec_children.len            = count;
            base->list[wouldbe]              = rec;
            if (count > 1)
            {  count--;
               for(off = 1; ((wouldbe+count+off) < base->len); off++)
                  base->list[wouldbe+off] = base->list[wouldbe+off+count];
               base->len -= count;
            };
            return(NETCALC_SUCCESS);

         default:
            netcalc_rec_free(rec);
            return(NETCALC_EUNKNOWN);
      };
   };

   netcalc_rec_free(rec);

   return(NETCALC_EEXISTS);
}


int
netcalc_set_add_str(
         netcalc_set_t *               ns,
         const char *                  address,
         const char *                  comment,
         void *                        data,
         int                           flags )
{
   int                  rc;
   netcalc_buff_t       b;

   assert(ns      != NULL);
   assert(address != NULL);

   if ((rc = netcalc_parse(&b, address, ns->set_flags)) != 0)
      return(rc);

   return(netcalc_set_add(ns, &b.buff_net, comment, data, flags));
}


void
netcalc_set_free(
         netcalc_set_t *               ns )
{
   uint32_t       idx;

   if (!(ns))
      return;

   for(idx = 0; (idx < ns->set_recs.len); idx++)
      netcalc_rec_free(ns->set_recs.list[idx]);

   if ((ns->set_recs.list))
      free(ns->set_recs.list);

   free(ns);

   return;
}


int
netcalc_set_init(
         netcalc_set_t **              nsp,
         int                           flags )
{
   netcalc_set_t *      ns;

   assert(nsp != NULL);
   assert((flags & ~NETCALC_FLGS_SET) == 0);

   if ((ns = malloc(sizeof(netcalc_set_t))) == NULL)
      return(NETCALC_ENOMEM);
   memset(ns, 0, sizeof(netcalc_set_t));

   ns->set_flags      = flags;
   ns->set_flags     |= ((flags & NETCALC_AF)) ? 0 : NETCALC_AF;

   *nsp = ns;

   return(0);
}


/* end of source */
