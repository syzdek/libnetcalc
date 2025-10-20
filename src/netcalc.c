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
#define __SRC_NETCALC_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include "netcalc-utility.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
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

#undef   NETCALC_SHORT_OPT
#define  NETCALC_SHORT_OPT "hqVv"

#undef   NETCALC_SHORT_FAMILY
#define  NETCALC_SHORT_FAMILY "46C:Eep:"

#undef   NETCALC_SHORT_FILE
#define  NETCALC_SHORT_FILE "cf:"

#undef   NETCALC_SHORT_FORMAT
#define  NETCALC_SHORT_FORMAT "0123MSZ" NETCALC_SHORT_FAMILY

#undef   NETCALC_LONG_OPT
#define  NETCALC_LONG_OPT \
   { "help",            no_argument,         NULL, 'h' }, \
   { "quiet",           no_argument,         NULL, 'q' }, \
   { "silent",          no_argument,         NULL, 'q' }, \
   { "version",         no_argument,         NULL, 'V' }, \
   { "verbose",         no_argument,         NULL, 'v' }, \
   { NULL, 0, NULL, 0 }

#undef   NETCALC_LONG_FAMILY
#define  NETCALC_LONG_FAMILY \
   { "ipv4",            no_argument,         NULL, '4' }, \
   { "ipv6",            no_argument,         NULL, '6' }, \
   { "eui48",           no_argument,         NULL, 'e' }, \
   { "eui64",           no_argument,         NULL, 'E' }, \
   { "mac",             no_argument,         NULL, 'e' },

#undef   NETCALC_LONG_FILE
#define  NETCALC_LONG_FILE \
   { "continue",        no_argument,         NULL, 'c' },

#undef   NETCALC_LONG_FORMAT
#define  NETCALC_LONG_FORMAT \
   NETCALC_LONG_FAMILY \
   { "colon",           no_argument,         NULL, '0' }, \
   { "dash",            no_argument,         NULL, '1' }, \
   { "dot",             no_argument,         NULL, '2' }, \
   { "no-delimiter",    no_argument,         NULL, '3' },


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#undef   NETCALC_LONG
#define  NETCALC_LONG(...) (const struct option []) { __VA_ARGS__ NETCALC_LONG_OPT }


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

extern int
main(
         int                           argc,
         char **                       argv );


static int
my_arguments(
         my_config_t *                 cnf,
         int                           argc,
         char * const *                argv );


void
my_free(
         my_config_t *                 cnf );


static int
my_usage(
         my_config_t *                 cnf );


static int
my_usage_import(
         my_config_t *                 cnf );


static my_widget_t *
my_widget_lookup(
         const char *                  wname,
         int                           exact );


//--------------------//
// widgets prototypes //
//--------------------//
#pragma mark widgets prototypes

static int
my_widget_copyright(
         my_config_t *                 cnf );


static int
my_widget_null(
         my_config_t *                 cnf );


static int
my_widget_syntaxes(
         my_config_t *                 cnf );


static int
my_widget_usage(
         my_config_t *                 cnf );


static int
my_widget_version(
         my_config_t *                 cnf );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

#pragma mark netcalc_widget_map[]
static my_widget_t my_widget_map[] =
{
   // copyright widget
   {  .name       = "copyright",
      .desc       = "display copyright",
      .usage      = NULL,
      .short_opt  = NULL,
      .long_opt   = NULL,
      .arg_min    = 0,
      .arg_max    = 0,
      .aliases    = NULL,
      .func_exec  = &my_widget_copyright,
      .func_usage = NULL,
   },

   // debug widget
   {  .name       = "debug",
      .desc       = NULL, // displays debugging information
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_debug,
      .func_usage = &my_widget_null,
   },

   // default widget
   {  .name       = "default",
      .desc       = NULL,
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT,
      .long_opt   = NETCALC_LONG( ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_info,
      .func_usage = &my_widget_null,
   },

   // help widget
   {  .name       = "help",
      .desc       = "display help",
      .usage      = NULL,
      .short_opt  = NULL,
      .long_opt   = NULL,
      .arg_min    = 0,
      .arg_max    = 0,
      .aliases    = (const char *[]){ "usage", NULL },
      .func_exec  = &my_widget_usage,
      .func_usage = &my_widget_null,
   },

   // info widget
   {  .name       = "info",
      .desc       = "display IP address information",
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_info,
      .func_usage = &my_widget_null,
   },

   // match widget
   {  .name       = "match",
      .desc       = "finds matching record within sorted networks",
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FILE NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FILE NETCALC_LONG_FORMAT ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_match,
      .func_usage = &my_usage_import,
   },

   // printf widget
   {  .name       = "printf",
      .desc       = "display formatted IP address information",
      .usage      = "[OPTIONS] <fmt> <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT ),
      .arg_min    = 2,
      .arg_max    = -1,
      .aliases    = (const char * const[]) { "print", NULL },
      .func_exec  = &my_widget_printf,
      .func_usage = &my_widget_printf_usage,
   },

   // superblock widget
   {  .name       = "superblock",
      .desc       = "display superblocks containing IP addresses",
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = (const char * const[]) { "super", "supernet", NULL },
      .func_exec  = &my_widget_superblock,
      .func_usage = &my_widget_null,
   },

   // sort widget
   {  .name       = "sort",
      .desc       = "sorts networks",
      .usage      = "[OPTIONS] [ <address> [ <address> [ ... <address> ] ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FILE NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FILE NETCALC_LONG_FORMAT ),
      .arg_min    = 0,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_sort,
      .func_usage = &my_usage_import,
   },

   // syntaxes widget
   {  .name       = "syntaxes",
      .desc       = "display supported address syntaxes",
      .usage      = "[OPTIONS]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FAMILY,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FAMILY ),
      .arg_min    = 0,
      .arg_max    = 0,
      .aliases    = NULL,
      .func_exec  = &my_widget_syntaxes,
      .func_usage = &my_widget_null,
   },

   // test widget
   {  .name       = "test",
      .desc       = "compares two IP addresses or networks",
      .usage      = "[OPTIONS] <address> <operator> <address>",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FAMILY,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT ),
      .arg_min    = 3,
      .arg_max    = 3,
      .aliases    = NULL,
      .func_exec  = &my_widget_test,
      .func_usage = &my_widget_test_usage,
      .func_args  = &my_widget_test_arguments,
   },

   // tree widget
   {  .name       = "tree",
      .desc       = "sorts networks and display is a tree structure",
      .usage      = "[OPTIONS] [ <address> [ <address> [ ... <address> ] ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FILE NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FILE NETCALC_LONG_FORMAT ),
      .arg_min    = 0,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &my_widget_tree,
      .func_usage = &my_usage_import,
   },

   // version widget
   {  .name       = "version",
      .desc       = "display version",
      .usage      = NULL,
      .short_opt  = NULL,
      .long_opt   = NULL,
      .arg_min    = 0,
      .arg_max    = 0,
      .aliases    = NULL,
      .func_exec  = &my_widget_version,
      .func_usage = NULL,
   },

   {  .name       = NULL,
      .desc       = NULL,
      .usage      = NULL,
      .short_opt  = NULL,
      .long_opt   = NULL,
      .arg_min    = 0,
      .arg_max    = 0,
      .aliases    = NULL,
      .func_exec  = NULL,
      .func_usage = NULL,
   },
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
main(
         int                           argc,
         char **                       argv )
{
   int                  rc;
   const char *         prog_name;
   my_config_t *        cnf;
   netcalc_net_t *      net;

   // determine program name
   if ((prog_name = strrchr(argv[0], '/')) != NULL)
      prog_name = &prog_name[1];
   if (!(prog_name))
      prog_name = argv[0];

   // allocate and initialize config memory
   if ((cnf = malloc(sizeof(my_config_t))) == NULL)
   {  fprintf(stderr, "%s; out of virtual memory\n", prog_name);
      return(1);
   };
   memset(cnf, 0, sizeof(my_config_t));
   cnf->argc         = argc;
   cnf->argv         = argv;
   cnf->prog_name    = prog_name;
   cnf->flags        = NETCALC_DFLTS;
   cnf->in_fd        = -1;
   cnf->in_filename  = NULL;

   // check for symlink alias
   if ((cnf->widget = my_widget_lookup(cnf->prog_name, 1)) != NULL)
      cnf->symlinked = 1;

   // processing common cli arguments
   if (!(cnf->widget))
   {  if ((rc = my_arguments(cnf, argc, argv)) != 0)
      {  my_free(cnf);
         return((rc == -1) ? 0 : 1);
      };
      if ((cnf->widget = my_widget_lookup(cnf->argv[0], 0)) == NULL)
      {  if (netcalc_init(&net, cnf->argv[0], cnf->flags) != NETCALC_SUCCESS)
         {  fprintf(stderr, "%s: unknown or ambiguous widget -- \"%s\"\n", cnf->prog_name, cnf->argv[0]);
            fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
            my_free(cnf);
            return(1);
         };
         netcalc_free(net);
         cnf->argv--;
         cnf->argc++;
         cnf->symlinked = 1;
         cnf->widget    = my_widget_lookup("default", 0);
      };
   };

   // process arguments using widget specific function
   if ((cnf->widget->func_args))
   {  if ((rc = cnf->widget->func_args(cnf, cnf->argc, cnf->argv)) != 0)
      {  my_free(cnf);
         return((rc == -1) ? 0 : 1);
      };
   };

   // processing widget cli arguments
   if ((rc = my_arguments(cnf, cnf->argc, cnf->argv)) != 0)
   {  my_free(cnf);
      return((rc == -1) ? 0 : 1);
   };

   // adjust flags
   cnf->flags &= ~cnf->flags_negate;

   // opens input file
   if ((cnf->in_filename))
   {  cnf->in_fd = STDIN_FILENO;
      if ((strcmp("-", cnf->in_filename)))
      {  if ((cnf->in_fd = open(cnf->in_filename, O_RDONLY)) == -1)
         {  fprintf(stderr, "%s: %s: %s\n", my_prog_name(cnf), cnf->in_filename, strerror(errno));
            my_free(cnf);
            return(1);
         };
      };
   };

   // process network prefix
   if ((cnf->net_prefix_str))
   {  if ((rc = netcalc_init(&cnf->net_prefix, cnf->net_prefix_str, cnf->flags)) != NETCALC_SUCCESS)
      {  fprintf(stderr, "%s: network prefix: %s\n", my_prog_name(cnf), netcalc_strerror(rc));
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         my_free(cnf);
         return(1);
      };
      netcalc_get_field(cnf->net_prefix, NETCALC_FLD_FAMILY, &cnf->net_prefix_family);
   };

   rc = cnf->widget->func_exec(cnf);

   my_free(cnf);

   return(rc);
}


int
my_arguments(
         my_config_t *                 cnf,
         int                           argc,
         char * const *                argv )
{
   int                        c;
   int                        opt_index;
   const struct option *      long_opt;
   const char *               short_opt;
   const char *               str;
   const my_widget_t *        widget;

   widget         = cnf->widget;
   optind         = 1;
   opt_index      = 0;
   short_opt      = ( ((widget)) && ((widget->short_opt)) )
                  ? widget->short_opt
                  : "+" NETCALC_SHORT_OPT;
   long_opt       = ( ((widget)) && ((widget->long_opt)) )
                  ? cnf->widget->long_opt
                  : NETCALC_LONG();

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {  switch(c)
      {  case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case '0':
         case '1':
         case '2':
         case '3':
            if ((cnf->flags & NETCALC_DELIM))
            {  fprintf(stderr, "%s: incompatible options `--colon', `--dash', `--dot' and `--no-delimiter'\n", my_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            }
            if (c == '0') cnf->flags |= NETCALC_FLG_DELIM_COLON;
            if (c == '1') cnf->flags |= NETCALC_FLG_DELIM_DASH;
            if (c == '2') cnf->flags |= NETCALC_FLG_DELIM_DOT;
            if (c == '3') cnf->flags |= NETCALC_FLG_DELIM_NODELIM;
            break;

         case '4':
            switch(cnf->flags & NETCALC_AF)
            {  case 0:                 str = NULL; break;
               case NETCALC_AF_EUI48:  str = "-e"; break;
               case NETCALC_AF_EUI64:  str = "-E"; break;
               case NETCALC_AF_INET:   str = NULL; break;
               case NETCALC_AF_INET6:  str = "-6"; break;
               default:                str = "--"; break;
            };
            if ((str))
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", my_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_INET;
            break;

         case '6':
            switch(cnf->flags & NETCALC_AF)
            {  case 0:                 str = NULL; break;
               case NETCALC_AF_EUI48:  str = "-e"; break;
               case NETCALC_AF_EUI64:  str = "-E"; break;
               case NETCALC_AF_INET:   str = "-4"; break;
               case NETCALC_AF_INET6:  str = NULL; break;
               default:                str = "--"; break;
            };
            if ((str))
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", my_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_INET6;
            break;

         case 'C':
            if ((cnf->net_prefix_str))
            {  fprintf(stderr, "%s: incompatible options `-c' and `-p'\n", my_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            if (!(strcasecmp(optarg, "eui48")))
               cnf->net_prefix_family = NETCALC_AF_EUI48;
            else if (!(strcasecmp(optarg, "eui64")))
               cnf->net_prefix_family = NETCALC_AF_EUI64;
            else if (!(strcasecmp(optarg, "ipv4")))
               cnf->net_prefix_family = NETCALC_AF_INET;
            else if (!(strcasecmp(optarg, "ipv6")))
               cnf->net_prefix_family = NETCALC_AF_INET6;
            else
            {  fprintf(stderr, "%s: unknown address family `%s'\n", my_prog_name(cnf), optarg);
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            break;

         case 'c':
            cnf->cont = 1;
            break;

         case 'E':
            switch(cnf->flags & NETCALC_AF)
            {  case 0:                 str = NULL; break;
               case NETCALC_AF_EUI48:  str = "-e"; break;
               case NETCALC_AF_EUI64:  str = NULL; break;
               case NETCALC_AF_INET:   str = "-4"; break;
               case NETCALC_AF_INET6:  str = "-6"; break;
               default:                str = "--"; break;
            };
            if ((str))
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", my_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_EUI64;
            break;

         case 'e':
            switch(cnf->flags & NETCALC_AF)
            {  case 0:                 str = NULL; break;
               case NETCALC_AF_EUI48:  str = NULL; break;
               case NETCALC_AF_EUI64:  str = "-E"; break;
               case NETCALC_AF_INET:   str = "-4"; break;
               case NETCALC_AF_INET6:  str = "-6"; break;
               default:                str = "--"; break;
            };
            if ((str))
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", my_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_EUI48;
            break;

         case 'f':
            cnf->in_filename = optarg;
            break;

         case 'h':
            my_usage(cnf);
            return(-1);

         case 'p':
            cnf->net_prefix_str = optarg;
            break;

         case 'q':
            cnf->quiet = 1;
            if ((cnf->verbose))
            {  fprintf(stderr, "%s: incompatible options `-q' and `-v'\n", my_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n",  my_prog_name(cnf));
               return(1);
            };
            break;

         case 'M':
            cnf->flags |= NETCALC_FLG_V4MAPPED;
            break;

         case 'S':
            cnf->flags_negate |= NETCALC_FLG_SUPR;
            break;

         case 'V':
            my_widget_version(cnf);
            return(-1);

         case 'v':
            cnf->verbose++;
            if ((cnf->quiet))
            {  fprintf(stderr, "%s: incompatible options `-q' and `-v'\n", my_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
               return(1);
            };
            break;

         case 'Z':
            cnf->flags_negate |= NETCALC_FLG_COMPR;
            break;

         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
            return(1);

         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", my_prog_name(cnf), c);
            fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
            return(1);
      };
   };

   cnf->argc   = (argc - optind);
   cnf->argv   = &argv[optind];

   if (!(widget))
   {  if (cnf->argc < 1)
      {  fprintf(stderr, "%s: missing required argument\n", my_prog_name(cnf));
         fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
         return(1);
      };
      return(0);
   };

   if (cnf->argc < widget->arg_min)
   {  fprintf(stderr, "%s: missing required argument\n", my_prog_name(cnf));
      fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
      return(1);
   };
   if ( (cnf->argc > widget->arg_max) && (widget->arg_max >= widget->arg_min) )
   {  fprintf(stderr, "%s: unknown argument -- `%s'\n", my_prog_name(cnf), cnf->argv[widget->arg_max]);
      fprintf(stderr, "Try `%s --help' for more information.\n", my_prog_name(cnf));
      return(1);
   };

   return(0);
}


void
my_free(
         my_config_t *                 cnf )
{
   if (!(cnf))
      return;
   if ((cnf->net_prefix))
      netcalc_free(cnf->net_prefix);
   free(cnf);
   return;
}


int
my_netcalc_init(
         my_config_t *                 cnf,
         netcalc_net_t **              netp,
         const char *                  addr )
{
   int                     rc;
   int                     family;
   const netcalc_net_t *   prefix;
   netcalc_net_t *         net;

   assert(cnf  != NULL);
   assert(netp != NULL);
   assert(addr != NULL);

   // parse address string
   if ((rc = netcalc_init(&net, addr, cnf->flags)) != NETCALC_SUCCESS)
      return(rc);
   if ( (!(cnf->net_prefix)) && (!(cnf->net_prefix_family)) )
   {  *netp = net;
      return(0);
   };

   // determines address family and if prefix should be used when converting
   netcalc_get_field(net, NETCALC_FLD_FAMILY, &family);
   if (cnf->net_prefix_family == family)
   {  *netp = net;
      return(0);
   };
   prefix = (cnf->net_prefix_family == NETCALC_AF_INET6)
            ? cnf->net_prefix
            : NULL;

   // convert address
   if ((rc = netcalc_convert(net, cnf->net_prefix_family, prefix)) != 0)
   {  netcalc_free(net);
      return(rc);
   };

   *netp = net;

   return(0);
}


void
my_nets_free(
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


char *
my_prog_name(
         my_config_t *                 cnf )
{
   static char    buff[512];

   const char * prog_name;

   prog_name   = ((cnf->prog_name))
               ? cnf->prog_name
               : PROGRAM_NAME;

   if ( (!(cnf->widget)) || ((cnf->symlinked)) )
   {  snprintf(buff, sizeof(buff), "%s", prog_name);
      return(buff);
   };

   snprintf(buff, sizeof(buff), "%s %s", prog_name, cnf->widget->name);

   return(buff);
}


int
my_set_import(
         my_config_t *                 cnf,
         netcalc_set_t *               ns )
{
   int                  rc;
   static char          buff[256];
   ssize_t              len;
   size_t               buff_len;
   size_t               bol;        // beginning of line
   size_t               eoa;        // end of address
   size_t               boc;        // beginning of comment
   size_t               eol;        // end of line
   size_t               pos;
   size_t               line;
   const char *         address;
   const char *         comment;
   netcalc_net_t *      net;

   assert(cnf != NULL);
   assert(ns  != NULL);

   line = 0;

   if (cnf->in_fd == -1)
      return(0);

   if ((len = read(cnf->in_fd, buff, (sizeof(buff)-1))) == -1)
   {  fprintf(stderr, "%s: read(): %s\n", my_prog_name(cnf), strerror(errno));
      return(1);
   };
   buff[len]   = '\0';
   buff_len    = (size_t)len;

   while(buff_len > 0)
   {  line++;
      for(bol = 0;   ((buff[bol] == ' ')  || (buff[bol] == '\t')); bol++);
      for(eoa = bol; ((buff[eoa] != '\n') && (buff[eoa] != '\0') && (buff[eoa] != ' ') && (buff[eoa] != '\t')); eoa++);
      for(boc = eoa; ((buff[boc] == ' ')  || (buff[boc] == '\t')); boc++);
      for(eol = boc; ((buff[eol] != '\n') && (buff[eol] != '\0')); eol++);
      buff[eoa]   = '\0';
      buff[eol]   = '\0';
      address     = &buff[bol];
      comment     = ((buff[boc])) ? &buff[boc] : NULL;

      // add address to set
      net = NULL;
      if ((rc = my_netcalc_init(cnf, &net, address)) == NETCALC_SUCCESS)
      {  rc = netcalc_set_add(ns, net, comment, NULL, 0);
         netcalc_free(net);
      };
      if (rc != 0)
      {  fprintf(stderr, "%s: %s: %zu: %s: %s\n", my_prog_name(cnf), cnf->in_filename, line, address, netcalc_strerror(rc));
         if (!(cnf->cont))
            return(1);
      };

      // shift buffer
      for(pos = 0; ((pos+eol+1) < buff_len); pos++)
         buff[pos] = buff[pos+eol+1];
      buff_len -= (eol+1);

      // fill buffer
      if ((len = read(cnf->in_fd, &buff[buff_len], (sizeof(buff)-1-buff_len))) == -1)
      {  fprintf(stderr, "%s: read(): %s\n", my_prog_name(cnf), strerror(errno));
         return(1);
      };
      buff_len += (size_t)len;
      buff[buff_len] = '\0';
   };

   return(0);
}


int
my_usage(
         my_config_t *                 cnf )
{
   size_t                     pos;
   const char *               widget_name;
   const char *               widget_help;
   const char *               short_opt;
   const my_widget_t *        widget;

   assert(cnf != NULL);

   widget_name  = (!(cnf->widget)) ? "widget"               : cnf->widget->name;
   short_opt    = ((cnf->widget))  ? cnf->widget->short_opt : NETCALC_SHORT_OPT;
   short_opt    = ((short_opt))    ? short_opt              : NETCALC_SHORT_OPT;
   widget_help  = "";
   if ((cnf->widget))
      widget_help = ((cnf->widget->usage)) ? cnf->widget->usage : "";

   if ((widget = cnf->widget) == NULL)
   {  printf("Usage: %s [OPTIONS] <address> [ <address> [ ... <address> ] ]\n", PROGRAM_NAME);
      printf("       %s [OPTIONS] %s %s\n", PROGRAM_NAME, widget_name, widget_help);
      printf("       %s-%s %s\n", PROGRAM_NAME, widget_name, widget_help);
      printf("       %s%s %s\n",  PROGRAM_NAME, widget_name, widget_help);
      printf("       net%s %s\n", widget_name, widget_help);
   } else if (cnf->symlinked == 0)
   {  widget_name = (widget->alias_idx == -1) ? widget_name : widget->aliases[widget->alias_idx];
      printf("Usage: %s [OPTIONS] %s %s\n", PROGRAM_NAME, widget_name, widget_help);
   }
   else
   {  printf("Usage: %s %s\n", cnf->prog_name, widget_help);
   };
   printf("OPTIONS:\n");
   if ((strchr(short_opt, '4'))) printf("  -4, --ipv4                input is IPv4\n");
   if ((strchr(short_opt, '6'))) printf("  -6, --ipv6                input is IPv6\n");
   if ((strchr(short_opt, 'C'))) printf("  -C family                 convert input to address family\n");
   if ((strchr(short_opt, 'E'))) printf("  -E, --eui64               input is EUI64\n");
   if ((strchr(short_opt, 'e'))) printf("  -e, --eui48, --mac        input is EUI48\n");
   if ((strchr(short_opt, 'f'))) printf("  -f file                   input file\n");
   if ((strchr(short_opt, 'h'))) printf("  -h, --help                print this help and exit\n");
   if ((strchr(short_opt, 'p'))) printf("  -p net, --prefix=net      use prefix when converting families\n");
   if ((strchr(short_opt, 'q'))) printf("  -q, --quiet, --silent     do not print messages\n");
   if ((strchr(short_opt, 'M'))) printf("  -M                        display with IPv4-mapped IPv6 address\n");
   if ((strchr(short_opt, 'S'))) printf("  -S                        display without zero suppression\n");
   if ((strchr(short_opt, 'V'))) printf("  -V, --version             print version number and exit\n");
   if ((strchr(short_opt, 'v'))) printf("  -v, --verbose             print verbose messages\n");
   if ((strchr(short_opt, 'Z'))) printf("  -Z                        display without zero compression\n");
   if ((strchr(short_opt, '0'))) printf("  --colon                   display using colon delimiters\n");
   if ((strchr(short_opt, '1'))) printf("  --dash                    display using dash delimiters\n");
   if ((strchr(short_opt, '2'))) printf("  --dot                     display using dot delimiters\n");
   if (!(cnf->widget))
   {  printf("WIDGETS:\n");
      for(pos = 0; my_widget_map[pos].name != NULL; pos++)
      {  widget = &my_widget_map[pos];
         if ((widget->desc))
            printf("  %-25s %s\n", widget->name, widget->desc);
      };
      printf("\n");
      return(0);
   };

   if ((cnf->widget))
      if ((cnf->widget->func_usage))
         cnf->widget->func_usage(cnf);

   return(0);
}


int
my_usage_import(
         my_config_t *                 cnf )
{
   assert(cnf != NULL);
   printf("FILE FORMAT:\n");
   printf("  address\n");
   printf("  address comment\n");
   printf("FILE EXAMPLE:\n");
   printf("  2001:db8:d::/56\n");
   printf("  203.0.113.0/24 documentation subnet per RFC5737\n");
   printf("  2001:db8::/32         documentation subnet per RFC3849");
   printf("  203.0.113.196/30\n");
   printf("  203.0.113.192/29\n");
   printf("  203.0.113.198/31  example point to point\n");
   printf("  2001:db8:d:a::/64\n");
   printf("  2001:db8:d::/48\n");
   printf("\n");
   return(0);
}


my_widget_t *
my_widget_lookup(
         const char *                  wname,
         int                           exact )
{
   int                        x;
   int                        y;
   size_t                     len;
   size_t                     wname_len;
   const char *               alias;
   my_widget_t *              match;
   my_widget_t *              widget;

   // strip program prefix from widget name
   len = strlen(PROGRAM_NAME);
   if (!(strncasecmp(wname, PROGRAM_NAME, len)))
   {
      wname = &wname[len];
      if (wname[0] == '-')
         wname = &wname[1];
   } else
   {
      len = strlen("net");
      if (!(strncasecmp(wname, "net", len)))
         wname = &wname[len];
   };
   if (!(wname[0]))
      return(NULL);

   match       = NULL;
   wname_len   = strlen(wname);

   // loop through widgets looking for match
   for(x = 0; ((my_widget_map[x].name)); x++)
   {  // check widget
      widget = &my_widget_map[x];
      if (widget->func_exec == NULL)
         continue;
      widget->alias_idx = -1;

      // compare widget name for match
      if (!(strncmp(widget->name, wname, wname_len)))
      {  if (widget->name[wname_len] == '\0')
            return(widget);
         if ( ((match)) && (match != widget) )
            return(NULL);
         if (exact == 0)
            match = widget;
      };

      if (!(widget->aliases))
         continue;

      for(y = 0; ((widget->aliases[y])); y++)
      {  alias = widget->aliases[y];
         if (!(strncmp(alias, wname, wname_len)))
         {  if (alias[wname_len] == '\0')
            {  widget->alias_idx = y;
               return(widget);
            };
            if ( ((match)) && (match != widget) )
               return(NULL);
            if (exact == 0)
               match = widget;
         };
      };
   };

   return((exact == 0) ? match : NULL);
}


//-------------------//
// record functions //
//-------------------//
#pragma mark record functions

extern void
my_rec_length(
         my_rec_t *                    rec,
         my_len_t *                    l )
{
   assert(rec  != NULL);
   assert(l    != NULL);
   l->address        = ((int)strlen(rec->address)        > l->address)        ? (int)strlen(rec->address)       : l->address;
   l->ip_broadcast   = ((int)strlen(rec->ip_broadcast)   > l->ip_broadcast)   ? (int)strlen(rec->ip_broadcast)  : l->ip_broadcast;
   l->ip_netmask     = ((int)strlen(rec->ip_netmask)     > l->ip_netmask)     ? (int)strlen(rec->ip_netmask)    : l->ip_netmask;
   l->ip_network     = ((int)strlen(rec->ip_network)     > l->ip_network)     ? (int)strlen(rec->ip_network)    : l->ip_network;
   l->ip_wildcard    = ((int)strlen(rec->ip_wildcard)    > l->ip_wildcard)    ? (int)strlen(rec->ip_wildcard)   : l->ip_wildcard;
   return;
}


void
my_rec_summary_ip(
         my_rec_t *                    r,
         my_len_t *                    l,
         int                           flags )
{
   int               family;
   const char *      subnets;

   assert(l != NULL);

   family   = ((r))
            ? r->family
            : (flags & NETCALC_AF);
   subnets  = ((r))
            ? r->ip_subnets
            : ( (family == NETCALC_AF_INET) ? "/32s" : "/64s" );

   if ((flags & MY_FLG_SHOW_ADDR))
      printf("%-*s  ", l->address, (((r)) ? r->address : "Address"));
   printf(
      "%-*s  %-*s  %-*s  %-*s  %-4s  %s\n",
      l->ip_network,    (((r)) ? r->ip_network     : "Network"),
      l->ip_broadcast,  (((r)) ? r->ip_broadcast   : "Broadcast"),
      l->ip_netmask,    (((r)) ? r->ip_netmask     : "Netmask"),
      l->ip_wildcard,   (((r)) ? r->ip_wildcard    : "Wildcard"),
                        (((r)) ? r->ip_cidr_str    : "CIDR"),
                        subnets
   );

   return;
}


void
my_rec_process(
         my_config_t *                 cnf,
         my_rec_t *                    rec )
{
   int                  ip_superblock;
   long long unsigned   lluval;
   netcalc_net_t *      n;

   assert(cnf != NULL);
   assert(rec != NULL);

   if (!(rec->net))
      return;
   n              = rec->net;
   ip_superblock  = rec->ip_superblock;

   memset(rec, 0, sizeof(my_rec_t));
   rec->net             = n;
   rec->ip_superblock   = ip_superblock;

   netcalc_get_field(n, NETCALC_FLD_FAMILY, &rec->family);
   if (!(rec->ip_superblock))
      netcalc_ntop(n, rec->address, sizeof(((my_rec_t *)0)->address), NETCALC_TYPE_ADDRESS, cnf->flags);
   else
      netcalc_strlcpy(rec->address, "SUPERBLOCK", sizeof(((my_rec_t *)0)->address));

   if ( (rec->family == NETCALC_AF_INET) || (rec->family == NETCALC_AF_INET6) )
   {  netcalc_ntop(n, rec->ip_network,    sizeof(((my_rec_t *)0)->ip_network),    NETCALC_TYPE_NETWORK,   NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR));
      netcalc_ntop(n, rec->ip_broadcast,  sizeof(((my_rec_t *)0)->ip_broadcast),  NETCALC_TYPE_BROADCAST, NETCALC_UNSET(cnf->flags, NETCALC_FLG_CIDR));
      netcalc_ntop(n, rec->ip_netmask,    sizeof(((my_rec_t *)0)->ip_netmask),    NETCALC_TYPE_NETMASK,   cnf->flags);
      netcalc_ntop(n, rec->ip_wildcard,   sizeof(((my_rec_t *)0)->ip_wildcard),   NETCALC_TYPE_WILDCARD,  cnf->flags);

      netcalc_get_field(n, NETCALC_FLD_CIDR, &rec->ip_cidr);
      snprintf(rec->ip_cidr_str, sizeof(((my_rec_t *)0)->ip_cidr_str), "%i", rec->ip_cidr);

      // calculate subnets
      if (rec->family == NETCALC_AF_INET6)
      {  if (!(rec->ip_cidr))
         {  netcalc_strlcat(rec->ip_subnets, "18446744073709551616", sizeof(((my_rec_t *)0)->ip_subnets));
         } else if (rec->ip_cidr <= 64)
         {  lluval =  0x8000000000000000LLU >> (rec->ip_cidr - 1);
            snprintf(rec->ip_subnets, sizeof(((my_rec_t *)0)->ip_subnets), "%llu", lluval);
         } else
         {  snprintf(rec->ip_subnets, sizeof(((my_rec_t *)0)->ip_subnets), "n/a");
         };
      } else if (rec->family == NETCALC_AF_INET)
      {  lluval =  0x100000000LLU >> rec->ip_cidr;
         snprintf(rec->ip_subnets, sizeof(((my_rec_t *)0)->ip_subnets), "%llu", lluval);
      };
   };

   return;
}


my_rec_t **
my_recs_alloc(
         my_config_t *                 cnf,
         size_t                        len )
{
   size_t            idx;
   size_t            size;
   my_rec_t *        rec;
   my_rec_t **       recs;

   assert(cnf != NULL);

   size = (len + 2) * sizeof(my_rec_t *);
   if ((recs = malloc(size)) == NULL)
   {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
      return(NULL);
   };
   memset(recs, 0, size);

   for(idx = 0; (idx < len); idx++)
   {  if ((rec = malloc(sizeof(my_rec_t))) == NULL)
      {  fprintf(stderr, "%s: out of virtual memory\n", my_prog_name(cnf));
         my_recs_free(recs);
         return(NULL);
      };
      memset(rec, 0, sizeof(my_rec_t));
      recs[idx] = rec;
   };

   return(recs);
}


void
my_recs_free(
         my_rec_t **                   recs )
{
   size_t               idx;
   assert(recs != NULL);
   for(idx = 0; ((recs[idx])); idx++)
   {  if ((recs[idx]->net))
         netcalc_free(recs[idx]->net);
      free(recs[idx]);
   };
   free(recs);
   return;
}


void
my_recs_lengths(
         my_rec_t **                   recs,
         my_len_t *                    l )
{
   size_t            idx;
   my_rec_t *        rec;

   assert(l != NULL);

   memset(l, 0, sizeof(my_len_t));
   l->address        = (int)strlen("Address");;
   l->ip_broadcast   = (int)strlen("Broadcast");;
   l->ip_netmask     = (int)strlen("Netmask");;
   l->ip_network     = (int)strlen("Network");;
   l->ip_wildcard    = (int)strlen("Wildcard");

   if (!(recs))
      return;

   for(idx = 0; ((recs[idx])); idx++)
   {  rec = recs[idx];
      if (!(rec->net))
         continue;
      my_rec_length(rec, l);
   };

   return;
}


//-------------------//
// widgets functions //
//-------------------//
#pragma mark widgets functions

int
my_widget_copyright(
         my_config_t *                 cnf )
{
   const char * pkg_name;
   pkg_name = ((cnf)) ? PACKAGE_NAME : PACKAGE_NAME;
   printf("\n");
   printf("%s\n", pkg_name);
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("All rights reserved.\n");
   printf("\n");
   printf("Redistribution and use in source and binary forms, with or without\n");
   printf("modification, are permitted provided that the following conditions are\n");
   printf("met:\n");
   printf("\n");
   printf("   * Redistributions of source code must retain the above copyright\n");
   printf("     notice, this list of conditions and the following disclaimer.\n");
   printf("   * Redistributions in binary form must reproduce the above copyright\n");
   printf("     notice, this list of conditions and the following disclaimer in the\n");
   printf("     documentation and/or other materials provided with the distribution.\n");
   printf("   * Neither the name of David M. Syzdek nor the\n");
   printf("     names of its contributors may be used to endorse or promote products\n");
   printf("     derived from this software without specific prior written permission.\n");
   printf("\n");
   printf("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS\n");
   printf("IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,\n");
   printf("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n");
   printf("PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR\n");
   printf("ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n");
   printf("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n");
   printf("SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n");
   printf("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n");
   printf("LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n");
   printf("OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF\n");
   printf("SUCH DAMAGE.\n");
   printf("\n");
   return(0);
}


static int
my_widget_null(
         my_config_t *                 cnf )
{
   printf("\n");
   if ((cnf))
      return(0);
   return(0);
}


int
my_widget_syntaxes(
         my_config_t *                 cnf )
{
   int   family;

   assert(cnf != NULL);

   family = ((cnf->flags & NETCALC_AF))
          ? ((cnf->flags & NETCALC_AF))
          : NETCALC_AF;

   printf("\n");
   printf("Address Syntaxes\n");
   printf("----------------\n");
   printf("  AF     Syntax                         Example\n");
   printf("  ------ ------------------------------ --------------------------------------\n");
   if (family & NETCALC_AF_EUI48)
      printf("  EUI-48 <addr>                         01:23:45:67:89:ab\n");
   if (family & NETCALC_AF_EUI64)
      printf("  EUI-64 <addr>                         0123.4567.89ab.cdef\n");
   if (family & NETCALC_AF_INET)
   {  printf("  IPv4   <addr>                         203.0.113.42\n");
      printf("  IPv4   <addr>/<cidr>                  203.0.113.42/29\n");
      printf("  IPv4   <addr>:<port>                  203.0.113.42:8080\n");
      printf("  IPv4   <addr>/<cidr>:<port>           203.0.113.42/29:8080\n");
   };
   if (family & NETCALC_AF_INET6)
   {  printf("  IPv6   <addr>                         fe80::216:3eff:fefe:b381\n");
      printf("  IPv6   <addr>/<cidr>                  fe80::216:3eff:fefe:b381/64\n");
      printf("  IPv6   <addr>%%<iface>                 fe80::216:3eff:fefe:b381%%eth0\n");
      printf("  IPv6   [<addr>]                       [fe80::216:3eff:fefe:b381]\n");
      printf("  IPv6   [<addr>]/<cidr>                [fe80::216:3eff:fefe:b381]/64\n");
      printf("  IPv6   [<addr>]:<port>                [fe80::216:3eff:fefe:b381]:8080\n");
      printf("  IPv6   [<addr>]/<cidr>:<port>         [fe80::216:3eff:fefe:b381]/64:8080\n");
      printf("  IPv6   [<addr>%%<iface>]               [fe80::216:3eff:fefe:b381%%eth0]\n");
      printf("  IPv6   [<addr>%%<iface>]/<cidr>        [fe80::216:3eff:fefe:b381%%eth0]/64\n");
      printf("  IPv6   [<addr>%%<iface>]:<port>        [fe80::216:3eff:fefe:b381%%eth0]:8080\n");
      printf("  IPv6   [<addr>%%<iface>]/<cidr>:<port> [fe80::216:3eff:fefe:b381%%eth0]/64:8080\n");
   };
   printf("\n");
   if (family & NETCALC_AF_EUI48)
   {  printf("EUI-48 Formats\n");
      printf("--------------\n");
      printf("   Using colon delimiter:             01:23:45:67:89:ab\n");
      printf("   Using dash delimiter:              01-23-45-67-89-ab\n");
      printf("   Using dot delimiter:               0123.4567.89ab\n");
      printf("   Without delimiter:                 0123456789ab\n");
      printf("\n");
   };
   if (family & NETCALC_AF_EUI64)
   {  printf("EUI-64 Formats\n");
      printf("--------------\n");
      printf("   Using colon delimiter:             01:23:45:67:89:ab:cd:ef\n");
      printf("   Using dash delimiter:              01-23-45-67-89-ab-cd-ef\n");
      printf("   Using dot delimiter:               0123.4567.89ab.cdef\n");
      printf("   Without delimiter:                 0123456789abcdef\n");
      printf("\n");
   };
   if (family & NETCALC_AF_INET)
   {  printf("IPv4 Formats\n");
      printf("-------------\n");
      printf("   Full Address:                      203.000.113.042\n");
      printf("   With Zero Suppression:             203.0.113.42\n");
      printf("\n");
   };
   if (family & NETCALC_AF_INET6)
   {  printf("IPv6 Formats\n");
      printf("-------------\n");
      printf("   Full Address:                       2001:0db8:000f:0300:0000:0000:cb00:712a\n");
      printf("   With Zero Compression:              2001:0db8:000f:0300::cb00:712a\n");
      printf("   With Zero Suppression:              2001:db8:f:300:0:0:cb00:712a\n");
      printf("   With Compression and Suppression:   2001:db8:f:300::cb00:712a\n");
      printf("   IPv4-mapped Addresses:\n");
      printf("     With Zero Compression:            2001:0db8:000f:0300::203.000.113.042\n");
      printf("     With Zero Suppression:            2001:db8:f:300:0:0:203.0.113.42\n");
      printf("     With Compression and Suppression: 2001:db8:f:300::203.0.113.42\n");
      printf("\n");
   };

   return(0);
}


int
my_widget_usage(
         my_config_t *                 cnf )
{
   cnf->widget = NULL;
   my_usage(cnf);
   return(0);
}


int
my_widget_version(
         my_config_t *                 cnf )
{
   const char * prog_name;
   prog_name = ((cnf)) ? cnf->prog_name : PROGRAM_NAME;
   printf("%s (%s) %s\n", prog_name, PACKAGE_NAME, PACKAGE_VERSION);
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("All rights reserved.\n");
   printf("\n");
   return(0);
}


/* end of source */
