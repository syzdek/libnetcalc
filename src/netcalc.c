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

#undef   NETCALC_SHORT_FORMAT
#define  NETCALC_SHORT_FORMAT "012346EeMSZ"

#undef   NETCALC_LONG_OPT
#define  NETCALC_LONG_OPT \
   { "help",            no_argument,         NULL, 'h' }, \
   { "quiet",           no_argument,         NULL, 'q' }, \
   { "silent",          no_argument,         NULL, 'q' }, \
   { "version",         no_argument,         NULL, 'V' }, \
   { "verbose",         no_argument,         NULL, 'v' }, \
   { NULL, 0, NULL, 0 }

#undef   NETCALC_LONG_FORMAT
#define  NETCALC_LONG_FORMAT \
   { "ipv4",            no_argument,         NULL, '4' }, \
   { "ipv6",            no_argument,         NULL, '6' }, \
   { "eui48",           no_argument,         NULL, 'e' }, \
   { "mac",             no_argument,         NULL, 'e' }, \
   { "colon",           no_argument,         NULL, '0' }, \
   { "dash",            no_argument,         NULL, '1' }, \
   { "dot",             no_argument,         NULL, '2' }, \
   { "no-delimiter",    no_argument,         NULL, '3' }


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
netcalc_arguments(
         netcalc_config_t *            cnf,
         int                           argc,
         char * const *                argv );


static int
netcalc_usage(
         netcalc_config_t *            cnf );


static netcalc_widget_t *
netcalc_widget_lookup(
         const char *                  wname,
         int                           exact );


//--------------------//
// widgets prototypes //
//--------------------//
#pragma mark widgets prototypes

static int
netcalc_widget_copyright(
         netcalc_config_t *            cnf );


static int
netcalc_widget_null(
         netcalc_config_t *            cnf );


static int
netcalc_widget_usage(
         netcalc_config_t *            cnf );


static int
netcalc_widget_version(
         netcalc_config_t *            cnf );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

#pragma mark netcalc_widget_map[]
static netcalc_widget_t netcalc_widget_map[] =
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
      .func_exec  = &netcalc_widget_copyright,
      .func_usage = NULL,
   },

   // debug widget
   {  .name       = "debug",
      .desc       = NULL, // displays debugging information
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT, ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &netcalc_widget_debug,
      .func_usage = &netcalc_widget_null,
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
      .func_exec  = &netcalc_widget_usage,
      .func_usage = &netcalc_widget_null,
   },

   // info widget
   {  .name       = "info",
      .desc       = "display IP address information",
      .usage      = "[OPTIONS] <address> [ <address> [ ... <address> ] ]",
      .short_opt  = NETCALC_SHORT_OPT NETCALC_SHORT_FORMAT,
      .long_opt   = NETCALC_LONG( NETCALC_LONG_FORMAT, ),
      .arg_min    = 1,
      .arg_max    = -1,
      .aliases    = NULL,
      .func_exec  = &netcalc_widget_info,
      .func_usage = &netcalc_widget_null,
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
      .func_exec  = &netcalc_widget_version,
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
   netcalc_config_t *   cnf;

   // determine program name
   if ((prog_name = strrchr(argv[0], '/')) != NULL)
      prog_name = &prog_name[1];
   if (!(prog_name))
      prog_name = argv[0];

   // allocate and initialize config memory
   if ((cnf = malloc(sizeof(netcalc_config_t))) == NULL)
   {
      fprintf(stderr, "%s; out of virtual memory\n", prog_name);
      return(1);
   };
   memset(cnf, 0, sizeof(netcalc_config_t));
   cnf->argc         = argc;
   cnf->argv         = argv;
   cnf->prog_name    = prog_name;
   cnf->flags        = NETCALC_DFLTS;

   // skip argument processing if called via alias
   if ((cnf->widget = netcalc_widget_lookup(cnf->prog_name, 1)) != NULL)
   {
      cnf->argc      = argc;
      cnf->argv      = argv;
      cnf->symlinked = 1;
      return(cnf->widget->func_exec(cnf));
   };

   // initial processing of common cli arguments
   if (!(cnf->widget))
   {
      if ((rc = netcalc_arguments(cnf, argc, argv)) != 0)
         return((rc == -1) ? 0 : 1);
      if ((cnf->widget = netcalc_widget_lookup(cnf->argv[0], 0)) == NULL)
      {
         fprintf(stderr, "%s: unknown or ambiguous widget -- \"%s\"\n", cnf->prog_name, cnf->argv[0]);
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(1);
      };
   };

   cnf->flags &= ~cnf->flags_negate;

   // initial processing of widget cli arguments
   if ((rc = netcalc_arguments(cnf, cnf->argc, cnf->argv)) != 0)
      return((rc == -1) ? 0 : 1);

   return(cnf->widget->func_exec(cnf));
}


int
netcalc_arguments(
         netcalc_config_t *            cnf,
         int                           argc,
         char * const *                argv )
{
   int                        c;
   int                        opt_index;
   const struct option *      long_opt;
   const char *               short_opt;
   const char *               str;
   const netcalc_widget_t *   widget;

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
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case '0':
         case '1':
         case '2':
         case '3':
            if ((cnf->flags & (NETCALC_FLG_COLON | NETCALC_FLG_DASH | NETCALC_FLG_DOT | NETCALC_FLG_NODELIM)))
            {  fprintf(stderr, "%s: incompatible options `--colon', `--dash', `--dot' and `--no-delimiter'\n", netcalc_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
               return(1);
            }
            if (c == '0') cnf->flags |= NETCALC_FLG_COLON;
            if (c == '1') cnf->flags |= NETCALC_FLG_DASH;
            if (c == '2') cnf->flags |= NETCALC_FLG_DOT;
            if (c == '3') cnf->flags |= NETCALC_FLG_NODELIM;
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
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", netcalc_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
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
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", netcalc_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_INET6;
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
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", netcalc_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
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
            {  fprintf(stderr, "%s: incompatible options `%s' and `-%c'\n", netcalc_prog_name(cnf), str, c);
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
               return(1);
            };
            cnf->flags |= NETCALC_AF_EUI48;
            break;

         case 'h':
            netcalc_usage(cnf);
            return(-1);

         case 'q':
            cnf->quiet = 1;
            if ((cnf->verbose))
            {  fprintf(stderr, "%s: incompatible options `-q' and `-v'\n", netcalc_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n",  netcalc_prog_name(cnf));
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
            netcalc_widget_version(cnf);
            return(-1);

         case 'v':
            cnf->verbose++;
            if ((cnf->quiet))
            {
               fprintf(stderr, "%s: incompatible options `-q' and `-v'\n", netcalc_prog_name(cnf));
               fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
               return(1);
            };
            break;

         case 'Z':
            cnf->flags_negate |= NETCALC_FLG_COMPR;
            break;

         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
            return(1);

         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", netcalc_prog_name(cnf), c);
            fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
            return(1);
      };
   };

   cnf->argc   = (argc - optind);
   cnf->argv   = &argv[optind];

   if (!(widget))
   {
      if (cnf->argc < 1)
      {
         fprintf(stderr, "%s: missing required argument\n", netcalc_prog_name(cnf));
         fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
         return(1);
      };
      return(0);
   };

   if (cnf->argc < widget->arg_min)
   {
      fprintf(stderr, "%s: missing required argument\n", netcalc_prog_name(cnf));
      fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
      return(1);
   };
   if ( (cnf->argc > widget->arg_max) && (widget->arg_max >= widget->arg_min) )
   {
      fprintf(stderr, "%s: unknown argument -- `%s'\n", netcalc_prog_name(cnf), cnf->argv[widget->arg_max]);
      fprintf(stderr, "Try `%s --help' for more information.\n", netcalc_prog_name(cnf));
      return(1);
   };

   return(0);
}


char *
netcalc_prog_name(
         netcalc_config_t *            cnf )
{
   static char    buff[512];

   const char * prog_name;

   prog_name   = ((cnf->prog_name))
               ? cnf->prog_name
               : PROGRAM_NAME;

   if ( (!(cnf->widget)) || ((cnf->symlinked)) )
   {
      snprintf(buff, sizeof(buff), "%s", prog_name);
      return(buff);
   };

   snprintf(buff, sizeof(buff), "%s %s", prog_name, cnf->widget->name);

   return(buff);
}


int
netcalc_usage(
         netcalc_config_t *            cnf )
{
   size_t                     pos;
   const char *               widget_name;
   const char *               widget_help;
   const char *               short_opt;
   const netcalc_widget_t *   widget;

   assert(cnf != NULL);

   widget_name  = (!(cnf->widget)) ? "widget"               : cnf->widget->name;
   short_opt    = ((cnf->widget))  ? cnf->widget->short_opt : NETCALC_SHORT_OPT;
   short_opt    = ((short_opt))    ? short_opt              : NETCALC_SHORT_OPT;
   widget_help  = "";
   if ((cnf->widget))
      widget_help = ((cnf->widget->usage)) ? cnf->widget->usage : "";

   if ((widget = cnf->widget) == NULL)
   {
      printf("Usage: %s %s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);
      printf("       %s-%s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);
      printf("       %s%s [OPTIONS]%s\n",  PROGRAM_NAME, widget_name, widget_help);
   } else if (cnf->symlinked == 0)
   {
      widget_name = (widget->alias_idx == -1) ? widget_name : widget->aliases[widget->alias_idx];
      printf("Usage: %s %s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);
   }
   else
   {
      printf("Usage: %s [OPTIONS]%s\n", cnf->prog_name, widget_help);
   };
   printf("OPTIONS:\n");
   if ((strchr(short_opt, '4'))) printf("  -4, --ipv4                input is IPv4\n");
   if ((strchr(short_opt, '6'))) printf("  -6, --ipv6                input is IPv6\n");
   if ((strchr(short_opt, 'E'))) printf("  -E, --eui64               input is EUI64\n");
   if ((strchr(short_opt, 'e'))) printf("  -e, --eui48, --mac        input is EUI48\n");
   if ((strchr(short_opt, 'h'))) printf("  -h, --help                print this help and exit\n");
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
   {
      printf("WIDGETS:\n");
      for(pos = 0; netcalc_widget_map[pos].name != NULL; pos++)
      {
         widget = &netcalc_widget_map[pos];
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


netcalc_widget_t *
netcalc_widget_lookup(
         const char *                  wname,
         int                           exact )
{
   int                        x;
   int                        y;
   size_t                     len;
   size_t                     wname_len;
   const char *               alias;
   netcalc_widget_t *         match;
   netcalc_widget_t *         widget;

   // strip program prefix from widget name
   len = strlen(PROGRAM_NAME);
   if (!(strncasecmp(wname, PROGRAM_NAME, len)))
      wname = &wname[len];
   if (wname[0] == '-')
      wname = &wname[1];
   if (!(wname[0]))
      return(NULL);

   match       = NULL;
   wname_len   = strlen(wname);

   // loop through widgets looking for match
   for(x = 0; ((netcalc_widget_map[x].name)); x++)
   {
      // check widget
      widget = &netcalc_widget_map[x];
      if (widget->func_exec == NULL)
         continue;
      widget->alias_idx = -1;

      // compare widget name for match
      if (!(strncmp(widget->name, wname, wname_len)))
      {
         if (widget->name[wname_len] == '\0')
            return(widget);
         if ( ((match)) && (match != widget) )
            return(NULL);
         if (exact == 0)
            match = widget;
      };

      if (!(widget->aliases))
         continue;

      for(y = 0; ((widget->aliases[y])); y++)
      {
         alias = widget->aliases[y];
         if (!(strncmp(alias, wname, wname_len)))
         {
            if (alias[wname_len] == '\0')
            {
               widget->alias_idx = y;
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
// widgets functions //
//-------------------//
#pragma mark widgets functions

int
netcalc_widget_copyright(
         netcalc_config_t *            cnf )
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
netcalc_widget_null(
         netcalc_config_t *            cnf )
{
   printf("\n");
   if ((cnf))
      return(0);
   return(0);
}


int
netcalc_widget_usage(
         netcalc_config_t *            cnf )
{
   cnf->widget = NULL;
   netcalc_usage(cnf);
   return(0);
}


int
netcalc_widget_version(
         netcalc_config_t *            cnf )
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
