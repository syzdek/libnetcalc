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
   netcalc_rec_t *   parent;

   while((rec))
   {  while((rec->rec_children_len))
      {  rec->rec_children_len--;
         rec = rec->rec_children[rec->rec_children_len];
      };

      parent = rec->rec_parent;

      if ((rec->rec_comment))
         free(rec->rec_comment);
      if ((rec->rec_children))
         free(rec->rec_children);
      netcalc_free(&rec->rec_net);

      rec = parent;
   };

   return;
}


void
netcalc_set_free(
         netcalc_set_t *               ns )
{
   uint32_t       idx;

   if (!(ns))
      return;

   for(idx = 0; (idx < ns->set_recs_len); idx++)
      netcalc_rec_free(ns->set_recs[idx]);
   free(ns->set_recs);
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

   if ((ns = malloc(sizeof(netcalc_set_t))) == NULL)
      return(NETCALC_ENOMEM);
   memset(ns, 0, sizeof(netcalc_set_t));

   ns->set_flags   = flags;
   ns->set_flags  |= ((flags & NETCALC_AF)) ? 0 : NETCALC_AF;

   *nsp = ns;

   return(0);
}


/* end of source */
