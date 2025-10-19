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
#include <stdio.h>


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
netcalc_cur_reset(
         netcalc_set_t *               ns,
         netcalc_cur_t *               cur );


static void
netcalc_rec_free(
         netcalc_rec_t *               rec );


static int
netcalc_rec_get(
         netcalc_rec_t *               rec,
         netcalc_net_t **              netp,
         char **                       commentp,
         void **                       datap,
         int *                         flagsp );


static int
netcalc_set_bindex(
         netcalc_set_t *               ns,
         const netcalc_net_t *         key,
         netcalc_recs_t **             basep,
         uint32_t *                    wouldbep );


static void
netcalc_set_debug_print(
         const char *                  prefix,
         int                           count,
         int                           indent,
         int                           maxlen,
         netcalc_net_t *               net,
         const char *                  comment,
         int                           flags );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

_NETCALC_F int
netcalc_cur_first(
         netcalc_cur_t *               cur,
         netcalc_net_t **              netp,
         char **                       commentp,
         void **                       datap,
         int *                         flagsp,
         int *                         depthp )
{
   assert(cur != NULL);
   cur->cur_idx[0]   = 0;
   cur->cur_depth    = 0;
   cur->cur_serial   = cur->cur_set->set_serial;
   return(netcalc_cur_next(cur, netp, commentp, datap, flagsp, depthp));
}


void
netcalc_cur_free(
         netcalc_cur_t *               cur )
{
   assert(cur != NULL);
   free(cur);
   return;
}


int
netcalc_cur_init(
         netcalc_set_t *               ns,
         netcalc_cur_t **              curp )
{
   netcalc_cur_t *   cur;

   assert(ns   != NULL);
   assert(curp != NULL);

   if ((cur = malloc(sizeof(netcalc_cur_t))) == NULL)
      return(NETCALC_ENOMEM);

   netcalc_cur_reset(ns, cur);

   *curp = cur;

   return(0);
}


int
netcalc_cur_next(
         netcalc_cur_t *               cur,
         netcalc_net_t **              netp,
         char **                       commentp,
         void **                       datap,
         int *                         flagsp,
         int *                         depthp )
{
   uint32_t             depth;
   uint32_t             idx;
   netcalc_rec_t *      rec;
   netcalc_recs_t *     recs;
   netcalc_recs_t *     list[129];

   assert(cur != NULL);

   if ((netp))
      *netp = NULL;
   if ((commentp))
      *commentp = NULL;
   if ((datap))
      *datap = NULL;
   if ((flagsp))
      *flagsp = 0;
   if ((depthp))
      *depthp = 0;

   if (cur->cur_serial != cur->cur_set->set_serial)
      return(NETCALC_ESETMOD);

   rec      = NULL;
   recs     = &cur->cur_set->set_recs;

   // fast forward to position
   for(depth = 0; (depth <= cur->cur_depth); depth++)
   {  list[depth] = recs;
      idx         = cur->cur_idx[depth];
      if (depth < cur->cur_depth)
         recs     = &recs->list[idx]->rec_children;
   };

   // retrieve current record
   depth = cur->cur_depth;
   idx   = cur->cur_idx[depth];
   if (idx >= list[depth]->len)
      return(NETCALC_ENOREC);
   rec = list[depth]->list[idx];
   if ((depthp))
      *depthp = (int)cur->cur_depth;

   // increment index
   depth = cur->cur_depth;
   idx   = cur->cur_idx[depth];
   if ((list[depth]->list[idx]->rec_children.len))
   {  cur->cur_depth++;
      cur->cur_idx[cur->cur_depth] = 0;
   } else
   {  cur->cur_idx[depth]++;
      while ( (cur->cur_idx[depth] >= list[depth]->len) && (depth != 0) )
      {  cur->cur_depth--;
         depth = cur->cur_depth;
         cur->cur_idx[depth]++;
      };
   };

   return(netcalc_rec_get(rec, netp, commentp, datap, flagsp));
}


void
netcalc_cur_reset(
         netcalc_set_t *               ns,
         netcalc_cur_t *               cur )
{
   assert(ns   != NULL);
   assert(cur  != NULL);

   memset(cur, 0, sizeof(netcalc_cur_t));
   cur->cur_serial   = ns->set_serial;
   cur->cur_set      = ns;

   return;
}


void
netcalc_rec_free(
         netcalc_rec_t *               rec )
{
   uint32_t                   rec_idx;
   netcalc_rec_t *            recs[129];

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
netcalc_rec_get(
         netcalc_rec_t *               rec,
         netcalc_net_t **              netp,
         char **                       commentp,
         void **                       datap,
         int *                         flagsp )
{
   int                  rc;
   netcalc_buff_t       nbuff;
   netcalc_net_t *      net;

   assert(rec != NULL);

   net = &nbuff.buff_net;

   if ((datap))
      *datap = rec->rec_data;
   if ((flagsp))
      *flagsp = rec->rec_flags;

   if ( ((netp)) && ((net)) )
   {  memset(net, 0, sizeof(netcalc_net_t));
      memcpy(&net->net_addr, &rec->rec_addr, sizeof(netcalc_addr_t));
      net->net_cidr  = rec->rec_cidr;
      net->net_flags = rec->rec_flags;
      if ((rc = netcalc_dup(netp, net)) != NETCALC_SUCCESS)
      {  if ((datap))
            *datap = NULL;
         if ((flagsp))
            *flagsp = 0;
         return(rc);
      };
   };

   if ( ((commentp)) && ((rec->rec_comment)) )
   {  if ((*commentp = strdup(rec->rec_comment)) == NULL)
      {  if ((datap))
            *datap = NULL;
         if ((flagsp))
            *flagsp = 0;
         if ((netp))
         {  netcalc_free(*netp);
            *netp = NULL;
         };
         return(NETCALC_ENOMEM);
      };
   };

   return(0);
}


// returns code which describes the key's relation to the record specified
// by 'wouldbe'
//    NETCALC_IDX_BEFORE   - key is before specified record
//    NETCALC_IDX_SUPERNET - key is supernet of specified record
//    NETCALC_IDX_SAME     - key is same as specified record
//    NETCALC_IDX_SUBNET   - key is subnet of specified record
//    NETCALC_IDX_AFTER    - key is after specified record
//    NETCALC_IDX_ERROR    - an error was detected
int
netcalc_set_bindex(
         netcalc_set_t *               ns,
         const netcalc_net_t *         key,
         netcalc_recs_t **             basep,
         uint32_t *                    wouldbep )
{
   int                  rc;
   int32_t              low;
   int32_t              mid;
   int32_t              high;
   netcalc_rec_t *      rec;
   netcalc_recs_t *     base;

   assert(ns         != NULL);
   assert(key        != NULL);
   assert(basep      != NULL);
   assert(*basep     != NULL);
   assert(wouldbep   != NULL);

   base = *basep;
   if (base->len == 0)
   {  if ((wouldbep))
         *wouldbep = 0;
      return(NETCALC_IDX_INSERT);
   };

   low  = 0;
   high = (int32_t)(base->len - 1);

   rc = NETCALC_IDX_SAME;
   while ( rc != NETCALC_IDX_ERROR )
   {  mid   = (low+high) / 2;
      rec   = base->list[mid];
      rc    = netcalc_addr_cmp(&key->net_addr, key->net_cidr, &rec->rec_addr, rec->rec_cidr);
      switch(rc)
      {  case NETCALC_CMP_BEFORE:
            if (low == mid)
            {  *wouldbep = (uint32_t)mid;
               return(NETCALC_CMP_BEFORE);
            };
            high = mid;
            break;

         case NETCALC_CMP_SUPERNET:
            if (low == mid)
            {  *wouldbep = (uint32_t)mid;
               return(NETCALC_CMP_SUPERNET);
            };
            high = mid;
            break;

         case NETCALC_CMP_SAME:
            *wouldbep = (uint32_t)mid;
            return(NETCALC_IDX_SAME);

         case NETCALC_CMP_SUBNET:
            if (!(rec->rec_children.len))
            {  *wouldbep = (uint32_t)mid;
               return(NETCALC_IDX_SUBNET);
            };
            *basep   = &rec->rec_children;
            base     = *basep;
            low      = 0;
            high     = (int32_t)(base->len - 1);
            break;

         case NETCALC_CMP_AFTER:
            if (high == mid)
            {  *wouldbep = (uint32_t)mid;
               return(NETCALC_CMP_AFTER);
            };
            low = (mid < high) ? mid + 1 : mid;
            break;

         default:
            break;
      };
   };

   return(NETCALC_IDX_ERROR);
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
   netcalc_buff_t       nbuff;

   assert(ns   != NULL);
   assert(net  != NULL);

   if (!( (net->net_flags & NETCALC_AF) | (ns->set_flags & NETCALC_AF) ))
      return(NETCALC_EINVAL);

   if ((netcalc_verify(net, NETCALC_TYPE_NETWORK)))
      return(NETCALC_EINVAL);

   // adjust nbuff
   memcpy(&nbuff.buff_net.net_addr, &net->net_addr, sizeof(netcalc_addr_t));
   nbuff.buff_net.net_cidr       = net->net_cidr;
   nbuff.buff_net.net_port       = net->net_port;
   nbuff.buff_net.net_flags      = net->net_flags;
   nbuff.buff_net.net_scope_name = NULL;
   net                           = &nbuff.buff_net;
   netcalc_addr_convert_inet6(&nbuff.buff_net.net_addr, (nbuff.buff_net.net_flags & NETCALC_AF));

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
   rc = netcalc_set_bindex(ns, net, &base, &wouldbe);
   switch(rc)
   {  case NETCALC_IDX_AFTER:
         wouldbe++;
      case NETCALC_IDX_BEFORE:
      case NETCALC_IDX_INSERT:
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
         base->list[wouldbe]  = rec;
         base->len++;
         ns->set_serial++;
         return(NETCALC_SUCCESS);

      case NETCALC_IDX_SAME:
         netcalc_rec_free(rec);
         return(NETCALC_EEXISTS);

      case NETCALC_IDX_SUBNET:
         // check if set allows supernets/subnets
         if ((ns->set_flags & NETCALC_FLG_UNIQ))
         {  netcalc_rec_free(rec);
            return(NETCALC_ESUBNET);
         };
         // allocate memory for list of children of record
         size = sizeof(netcalc_rec_t *) * 2;
         if ((base->list[wouldbe]->rec_children.list = malloc(size)) == NULL)
         {  netcalc_rec_free(rec);
            return(NETCALC_ENOMEM);
         };
         // save record as subnet
         base->list[wouldbe]->rec_children.size     = 1;
         base->list[wouldbe]->rec_children.len      = 1;
         base->list[wouldbe]->rec_children.list[0]  = rec;
         base->list[wouldbe]->rec_children.list[1]  = NULL;
         return(NETCALC_SUCCESS);

      case NETCALC_IDX_SUPERNET:
         // check if set allows supernets/subnets
         if ((ns->set_flags & NETCALC_FLG_UNIQ))
         {  netcalc_rec_free(rec);
            return(NETCALC_ESUPERNET);
         };
         // determine number of subnets for the the record is supernet
         count    = 1;
         rc       = NETCALC_CMP_SUPERNET;
         while((rc == NETCALC_CMP_SUPERNET) && ((count+wouldbe) < base->len))
         {  child = base->list[count+wouldbe];
            rc    = netcalc_addr_cmp(&rec->rec_addr, rec->rec_cidr, &child->rec_addr, child->rec_cidr);
            if (rc == NETCALC_CMP_SUPERNET)
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
         ns->set_serial++;
         return(NETCALC_SUCCESS);

      default:
         break;
   };

   netcalc_rec_free(rec);

   return(NETCALC_EUNKNOWN);
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
netcalc_set_debug(
         netcalc_set_t *               ns,
         const char *                  prefix )
{
   int               rc;
   int               flags;
   int               depth;
   int               maxdepth;
   uint32_t          count;
   char *            comment;
   netcalc_net_t *   net;
   netcalc_cur_t *   cur;
   netcalc_cur_t     cbuff;

   assert(ns != NULL);

   netcalc_cur_reset(ns, &cbuff);

   cur   = &cbuff;
   count = 0;

   // determine max depth
   if ((rc = netcalc_set_stats(ns, NULL, &maxdepth, NULL)) != 0)
   {  fprintf(stderr, "netcalc_set_maxdepth(): %s\n", netcalc_strerror(rc));
      return;
   };

   // print records
   if ((rc = netcalc_cur_first(cur, &net, &comment, NULL, &flags, &depth)) != 0)
   {  fprintf(stderr, "netcalc_cur_first(): %s\n", netcalc_strerror(rc));
      netcalc_cur_free(cur);
      return;
   };
   netcalc_set_debug_print(prefix, count++, depth, maxdepth, net, comment, flags);
   if ((net))
      netcalc_free(net);
   if ((comment))
      free(comment);
   while((rc = netcalc_cur_next(cur, &net, &comment, NULL, &flags, &depth)) == 0)
   {  netcalc_set_debug_print(prefix, count++, depth, maxdepth, net, comment, flags);
      if ((net))
         netcalc_free(net);
      if ((comment))
         free(comment);
   };

   if (rc != NETCALC_ENOREC)
      fprintf(stderr, "netcalc_cur_next(): %s\n", netcalc_strerror(rc));

   return;
}


void
netcalc_set_debug_print(
         const char *                  prefix,
         int                           count,
         int                           depth,
         int                           maxdepth,
         netcalc_net_t *               net,
         const char *                  comment,
         int                           flags )
{
   char        addr[NETCALC_ADDRESS_LENGTH];
   char        str[NETCALC_ADDRESS_LENGTH*3];

   if (!(net))
      return;
   netcalc_ntop(net, addr, sizeof(addr), NETCALC_TYPE_ADDRESS, NETCALC_FLG_CIDR);

   prefix   = ((prefix))   ? prefix    : "";
   comment  = ((comment))  ? comment   : "";
   maxdepth = (maxdepth * 3) + 44;

   snprintf(str, sizeof(str), "%*s%s", (depth*3), "", addr);

   printf("%s%5i: %-*s 0x%x %s\n", prefix, count, maxdepth, str, flags, comment);

   return;
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


int
netcalc_set_stats(
         netcalc_set_t *               ns,
         size_t *                      numelep,
         int *                         maxdepthp,
         int *                         familiesp )
{
   int               rc;
   int               depth;
   int               maxdepth;
   int               families;
   int               flags;
   size_t            numele;
   netcalc_cur_t *   cur;
   netcalc_cur_t     cbuff;

   assert(ns != NULL);

   netcalc_cur_reset(ns, &cbuff);
   cur = &cbuff;

   // loops through set and gathers stats
   if ((rc = netcalc_cur_first(cur, NULL, NULL, NULL, &flags, &depth)) != 0)
   {  if (rc == NETCALC_ENOREC)
      {  if ((numelep))
            *numelep = 0;
         if ((maxdepthp))
            *maxdepthp = 0;
         if ((familiesp))
            *familiesp = 0;
         return(0);
      };
      return(rc);
   };
   numele   = 1;
   families = flags & NETCALC_AF;
   maxdepth = depth;
   while((rc = netcalc_cur_next(cur, NULL, NULL, NULL, &flags, &depth)) == 0)
   {  numele++;
      families |= flags & NETCALC_AF;
      maxdepth  = (depth > maxdepth) ? depth : maxdepth;
   };
   if (rc != NETCALC_ENOREC)
      return(rc);

   if ((numelep))
      *numelep = numele;
   if ((maxdepthp))
      *maxdepthp = maxdepth;
   if ((familiesp))
      *familiesp = families;

   return(0);
}


/* end of source */
