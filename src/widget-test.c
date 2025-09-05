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

#define MY_CMP_LT             0x01
#define MY_CMP_LE             0x02
#define MY_CMP_EQ             0x03
#define MY_CMP_GE             0x04
#define MY_CMP_GT             0x05
#define MY_CMP_NE             0x06
#define MY_CMP_WITHIN         0x07
#define MY_CMP_WITHIN_EQ      0x08
#define MY_CMP_CONTAINS       0x09
#define MY_CMP_CONTAINS_EQ    0x0a


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
my_widget_test(
         my_config_t *                 cnf )
{
   int                  idx;
   int                  rc;
   int                  flags;
   int                  family;
   int                  cmp_op;
   int                  cmp_flags;
   int                  families[3];
   size_t               len;
   size_t               size;
   netcalc_net_t **     nets;
   const char *         op_str;
   const char *         strs[3];

   flags  = cnf->flags;
   flags &= ~cnf->flags_negate;

   strs[0]  = cnf->argv[0];
   strs[1]  = cnf->argv[2];
   strs[2]  = NULL;
   op_str   = cnf->argv[1];

   if      (!(strcmp("<",    op_str))) { cmp_op = MY_CMP_LT; }
   else if (!(strcmp("<=",   op_str))) { cmp_op = MY_CMP_LE; }
   else if (!(strcmp("=",    op_str))) { cmp_op = MY_CMP_EQ; }
   else if (!(strcmp(">=",   op_str))) { cmp_op = MY_CMP_GE; }
   else if (!(strcmp(">",    op_str))) { cmp_op = MY_CMP_GT; }
   else if (!(strcmp("<>",   op_str))) { cmp_op = MY_CMP_NE; }
   else if (!(strcmp("<<",   op_str))) { cmp_op = MY_CMP_WITHIN; }
   else if (!(strcmp("<<=",  op_str))) { cmp_op = MY_CMP_WITHIN_EQ; }
   else if (!(strcmp(">>=",  op_str))) { cmp_op = MY_CMP_CONTAINS_EQ; }
   else if (!(strcmp(">>",   op_str))) { cmp_op = MY_CMP_CONTAINS; }
   else if (!(strcmp("_lt",  op_str))) { cmp_op = MY_CMP_LT; }
   else if (!(strcmp("_le",  op_str))) { cmp_op = MY_CMP_LE; }
   else if (!(strcmp("_eq",  op_str))) { cmp_op = MY_CMP_EQ; }
   else if (!(strcmp("_ge",  op_str))) { cmp_op = MY_CMP_GE; }
   else if (!(strcmp("_gt",  op_str))) { cmp_op = MY_CMP_GT; }
   else if (!(strcmp("_ne",  op_str))) { cmp_op = MY_CMP_NE; }
   else if (!(strcmp("_llt", op_str))) { cmp_op = MY_CMP_WITHIN; }
   else if (!(strcmp("_lle", op_str))) { cmp_op = MY_CMP_WITHIN_EQ; }
   else if (!(strcmp("_gge", op_str))) { cmp_op = MY_CMP_CONTAINS_EQ; }
   else if (!(strcmp("_ggt", op_str))) { cmp_op = MY_CMP_CONTAINS; }
   else
   {  fprintf(stderr, "%s: unknown operator '%s'\n", my_prog_name(cnf), cnf->argv[1]);
      return(1);
   };

   // allocates memory
   len   = 3;
   size  = sizeof(netcalc_net_t *) * len;
   if ((nets = malloc(size)) == NULL)
   {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
      return(1);
   };
   memset(nets, 0, size);

   // process network arguments
   for(idx = 0; (idx < 2); idx++)
   {  // parse address
      if ((rc = netcalc_init(&nets[idx], strs[idx], flags)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), strs[idx], netcalc_strerror(rc));
         my_nets_free(nets);
         return(1);
      };
      // convert address to prefix address family
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY, &family);
      if ( ((cnf->net_prefix)) && (cnf->net_prefix_family != family) )
      {  if ((rc = netcalc_convert(nets[idx], cnf->net_prefix_family, cnf->net_prefix)) != 0)
         {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), strs[idx], netcalc_strerror(rc));
            my_nets_free(nets);
            return(1);
         };
      };
      // obtain family
      netcalc_get_field(nets[idx], NETCALC_FLD_FAMILY, &families[idx]);
   };

   // verify families
   if (families[0] != families[1])
   {  fprintf(stderr, "%s: address family mismatch\n", my_prog_name(cnf));
      my_nets_free(nets);
      return(1);
   };

   switch(cmp_op)
   {  case MY_CMP_WITHIN:
      case MY_CMP_WITHIN_EQ:
      case MY_CMP_CONTAINS_EQ:
      case MY_CMP_CONTAINS:
         cmp_flags = NETCALC_FLG_NETWORK;
         if ((netcalc_verify(nets[0], NETCALC_TYPE_NETWORK)))
         {  fprintf(stderr, "%s: %s: not a network address\n", my_prog_name(cnf), cnf->argv[0]);
            my_nets_free(nets);
            return(1);
         };
         if ((netcalc_verify(nets[1], NETCALC_TYPE_NETWORK)))
         {  fprintf(stderr, "%s: %s: not a network address\n", my_prog_name(cnf), cnf->argv[2]);
            my_nets_free(nets);
            return(1);
         };
         break;

      case MY_CMP_LT:
      case MY_CMP_LE:
      case MY_CMP_EQ:
      case MY_CMP_GE:
      case MY_CMP_GT:
      case MY_CMP_NE:
      default:
         cmp_flags = NETCALC_FLG_PORT | NETCALC_FLG_IFACE;
         break;
   };

   rc = netcalc_cmp(nets[0], nets[1], cmp_flags);

   my_nets_free(nets);

   switch(cmp_op)
   {  case MY_CMP_LT:            return( (rc < 0)                                   ? 0 : 2 );
      case MY_CMP_LE:            return( ((rc < 0) || (!(rc)))                      ? 0 : 2 );
      case MY_CMP_EQ:            return( (rc == 0)                                  ? 0 : 2 );
      case MY_CMP_GE:            return( ((rc > 0) || (!(rc)))                      ? 0 : 2 );
      case MY_CMP_GT:            return( (rc > 0)                                   ? 0 : 2 );
      case MY_CMP_NE:            return( (!(rc))                                    ? 0 : 2 );
      case MY_CMP_WITHIN:        return( (rc == NETCALC_CMP_SUBNET)                 ? 0 : 2 );
      case MY_CMP_WITHIN_EQ:     return( ((rc == NETCALC_CMP_SUBNET) || (!(rc)))    ? 0 : 2 );
      case MY_CMP_CONTAINS_EQ:   return( ((rc == NETCALC_CMP_SUPERNET) || (!(rc)))  ? 0 : 2 );
      case MY_CMP_CONTAINS:      return( (rc == NETCALC_CMP_SUPERNET)               ? 0 : 2 );
      default:                   break;
   };

   return(2);
}


int
my_widget_test_arguments(
         my_config_t *                 cnf,
         int                           argc,
         char * const *                argv )
{
   int      idx;
   char *   op_str;

   assert(cnf != NULL);

   for(idx = 1; (idx < argc); idx++)
   {  op_str = argv[idx];
      if (  (!(strcmp("-lt",  op_str))) || (!(strcmp("-le",  op_str))) ||
            (!(strcmp("-eq",  op_str))) || (!(strcmp("-ne",  op_str))) ||
            (!(strcmp("-ge",  op_str))) || (!(strcmp("-gt",  op_str))) ||
            (!(strcmp("-llt", op_str))) || (!(strcmp("-lle", op_str))) ||
            (!(strcmp("-gge", op_str))) || (!(strcmp("-ggt", op_str))) )
      {  op_str[0] = '_';
         return(0);
      };
   };

   return(0);
}


int
my_widget_test_usage(
         my_config_t *                 cnf )
{
   assert(cnf != NULL);
   printf("OPERATORS:\n");
   printf("  <      is less than\n");
   printf("  <=     is less than or equal\n");
   printf("  =      equals\n");
   printf("  >=     is greater than or equal\n");
   printf("  >      is greater than\n");
   printf("  <>     is not equal\n");
   printf("  <<     is contained within\n");
   printf("  <<=    is contained within or equals\n");
   printf("  >>=    contains or equals\n");
   printf("  >>     contains\n");
   printf("  -lt    is less than\n");
   printf("  -le    is less than or equal\n");
   printf("  -eq    equals\n");
   printf("  -ge    is greater than or equal\n");
   printf("  -gt    is greater than\n");
   printf("  -ne    is not equal\n");
   printf("  -llt   is contained within\n");
   printf("  -lle   is contained within or equals\n");
   printf("  -gge   contains or equals\n");
   printf("  -ggt   contains\n");
   printf("EXAMPLES:\n");
   printf("  $ %s -p 2001:db8:f:300::/64 [2001:db8:f:300::54f]/64:443 << 203.0.113.1:143\n", my_prog_name(cnf));
   printf("  2001:db8:f:300::54f  64   443\n");
   printf("  203.0.113.1          128  143\n");
   printf("\n");
   printf("  $ %s \"%%n%%c\" [fe80::216:3eff:fe2a:bab8%%eth0]/64\n", my_prog_name(cnf));
   printf("  fe80::/64\n");
   printf("\n");
   printf("  $ %s \"%%b\" fe80::216:3eff:fe2a:bab8/64\n", my_prog_name(cnf));
   printf("  fe80:0:0:0:ffff:ffff:ffff:ffff\n");
   printf("\n");
   return(0);
}


/* end of source */
