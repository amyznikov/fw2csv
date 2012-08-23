/*
 * fw2csv.c
 *
 *  Created on: Jan 26, 2012
 *      Author: amyznikov
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <libgen.h>

#define MAX_STOPS 10000
#define MAX_LINE  10000

static int stops[MAX_STOPS];
static int num_stops = 0;

static void usage( int argc, char * argv[] )
{
  printf("\n");
  printf("Stream converter for fixed width tables to CSV\n");
  printf("A.A. Myznikov\n");
  printf("Feb 28 2012\n");
  printf("\n");

  printf("USAGE:\n");
  printf("  %s [OPTIONS] [FILE]\n\n", basename(argv[0]));

  printf("OPTIONS:\n");
  printf("  --stops=<stop-list>\n");
  printf("  -d <output delimiter>, default is '|'\n");
  printf("  -header <number of first heading lines to skip>, optional\n");

  printf("\n");
  printf("The stop-list is a coma-separated list of zero-based column indexes to insert separators\n\n");
  printf("For example\n");
  printf(" %s --stops=5,8,12 -d '|' mytable.dat\n", basename(argv[0]));
  printf("Will insert delimiter character '|' in positions 5, 8 and 12 of each line of original stream\n\n");
}

static char getcharacter( const char * s )
{
  char ch;

  if ( s[0] != '\\' )
  {
    ch = s[0];
  }
  else
  {
    switch ( s[1] )
    {
    case 't':
      ch = '\t';
      break;
    case 'n':
      ch = '\n';
      break;
    default:
      ch = s[1];
      break;
    }
  }
  return ch;
}


static int parse_stops( char * s )
{
  char * endptr = 0;
  char * p;
  static const char delims[] = ",";

  p = strtok(s, delims);
  while ( p && num_stops < MAX_STOPS )
  {
    if ( sscanf(p, "%u", &stops[num_stops]) != 1 )
    {
      return -1;
    }

    if ( num_stops && stops[num_stops] <= stops[num_stops - 1] )
    {
      return -1;
    }

   ++num_stops;
    p = strtok(NULL, delims);
  }

  return num_stops;
}

int main( int argc, char *argv[] )
{
  char line[MAX_LINE] = {0}; /* buffer for input line */
  char od = '|'; /* output delimiter */
  size_t n, headlines = 0; /* header lines counters */

  char filename[PATH_MAX] = { 0 }; /* input file name */
  FILE * fp = stdin;

  int i;

  for ( i = 1; i < argc; ++i )
  {
    if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0 )
    {
      usage(argc, argv);
      return 0;
    }

    if ( strncmp(argv[i], "--stops=", 8) == 0 )
    {
      if ( parse_stops(argv[i] + 8) < 0 )
      {
        fprintf(stderr, "invalid syntax in --stops argument\n");
        return -1;
      }
    }
    else if ( strcmp(argv[i], "-d") == 0 )
    {
      if ( ++i >= argc || !( od = getcharacter(argv[i]) ) )
      {
        fprintf(stderr, "invalid value of argument '%s'\n", argv[i - 1]);
        return -1;
      }
    }
    else if ( strcmp(argv[i], "-header") == 0 )
    {
      if ( ++i >= argc || sscanf(argv[i], "%zu", &headlines) != 1 )
      {
        fprintf(stderr, "invalid value of argument '%s'\n", argv[i - 1]);
        return -1;
      }
    }
    else if ( !*filename )
    {
      strncpy(filename, argv[i], sizeof( filename ) - 1);
    }
    else
    {
      fprintf(stderr, "invalid argument '%s'\n", argv[i]);
      return -1;
    }
  }

  if ( *filename && !( fp = fopen(filename, "rb") ) )
  {
    fprintf(stderr, "can't open '%s': %s\n", filename, strerror(errno));
    return -1;
  }

  n = 0;
  while ( fgets(line, sizeof( line ) - 1, stdin) )
  {
    if ( ++n < headlines || num_stops < 1 )
    {
      printf("%s", line);
    }
    else
    {
      size_t stop = 0;
      size_t pos = 0;

      size_t len = strlen(line);
      for ( pos = 0; pos < len; ++pos )
      {
        if ( pos == stops[stop] )
        {
          putc(od,stdout);
          ++stop;
        }

        putc(line[pos],stdout);
      }
    }
  }

  if ( fp != stdin )
  {
    fclose(fp);
  }

  return 0;
}
