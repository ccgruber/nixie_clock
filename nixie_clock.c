// ************************************************************************************
// *           Userland driver for the 8 digits Nixie Clock/Counter from              *
// *  http://www.kosbo.com/index.php?option=com_content&view=article&id=51&Itemid=60  *
// *                                  Version 0.4                                     *
// *                     Copyright by Christian C. Gruber 2011                        *
// ************************************************************************************

#include <stdio.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

main( int argc, char *argv[] )
{
  // Three command line arguments are required
  // Usage information
  if ( argc != 5 )
  {
    fprintf(stderr,"Usage: %s [UTC led device] [nixie tubes device] [delay in us] [min for the nixie dance] (Example: %s /dev/led/gpio11 /dev/ttyu0 1000 30) \n", argv[0], argv[0] );
  }
  // Starting
  else
  {
    int dance = atoi(argv[4]);
    int delay = atoi(argv[3]);
    fprintf(stderr,"%s Version 0.4\nCopyright 2011 Christian C. Gruber <cg@chilia.com>\nUTC LED device       = %s\nNixie tubes device   = %s\nDelay                = %d [us]\nDance time           = 23, 43 and %d [min]\n", argv[0], argv[1], argv[2], delay, dance);
    FILE *output;
    FILE* led;
    FILE* port;
    int offset;
    char *state;
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;  
    state = "init";
    offset = 0;
    port = fopen(argv[2], "wb+" );
    for (;;)
    {
      gettimeofday(&tv,&tz);
      tm=localtime(&tv.tv_sec);
      // Tick
      if ( tv.tv_usec <= delay )
      {
        // Output offset in ns between the 20th and 25th second 
        if ( ( tm->tm_sec > 20 ) && ( tm->tm_sec < 25 ) )
        {
          if ( offset == 0 )
          {
          fprintf(port,"%08s\n", "        ");
          }
          else if ( offset > 99999999 )
          {
          fprintf(port,"%08s\n", "99999999");
          }
          else
          {
          fprintf(port,"%08d\n", offset);
          }
        }
        // Output a between the 40th and 45th second
        else if ( ( tm->tm_sec > 40 ) && ( tm->tm_sec < 45 ) )
        {
          fprintf(port,"%02d %02d %02d\n", tm->tm_mday, tm->tm_mon, tm->tm_year - 100 );
        }
        // Output time
        else
        {
          fprintf(port, "%02d %02d %02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
        }
        // Timing statistics go to stderr
        fprintf(stderr,"The %02d:%02d:%02d tick is %04d us late but within a %d us range (offset: %08d, %s).\r", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, delay, offset, state);
        // Light red led when ntpd is not running
        if ( offset == 0 )
        {
          state="off";
          led = fopen(argv[1], "w" );
          fprintf(led,"1");
          fclose(led);
        }
        // Wild blink red led during synchronisation
        else if ( offset > delay * 1000 )
        {
          state="syncing";
          led = fopen(argv[1], "w" );
          fprintf(led,"f1");
          fclose(led);
        }
        // Blink led synchronised to UTC time tick
        else if ( offset <= delay * 1000 )
        {
          state="synced";
          led = fopen(argv[1], "w" );
          fprintf(led,"1");
          fclose(led);
          usleep(1000);
          led = fopen(argv[1], "w" );
          fprintf(led,"0");
          fclose(led);
        }  
        // Get ntpd offset
        output = popen(" printf '%08s' `ntptime | grep interval | cut -d' ' -f4 | sed 's/-//g' | awk '{ print $1 * 1000 }'` ", "r");
        fscanf( output, "%d", & offset );
        pclose(output);
        // This half a second sleep is important for cpu load descrase ...
        usleep(500000);
        // Do the nixie dance here
        while ( ( tm->tm_min == 23 ) | ( tm->tm_min == 43 ) | ( tm->tm_min == dance ) )
        {
          gettimeofday(&tv,&tz);
          tm=localtime(&tv.tv_sec);
          fprintf(stderr,"The %02d:%02d is Nixie dance time for one minute ...                                                       \r", tm->tm_hour, tm->tm_min);
          led = fopen(argv[1], "w" );
          fprintf(led,"1");
          fclose(led);
          int i = 0;
          int wait = 30000;
          while (i <= 9 )
          {
            fprintf(port,"%d       \n", i);
            usleep(wait);
            fprintf(port," %d      \n", i);
            usleep(wait);
            fprintf(port,"  %d     \n", i);
            usleep(wait);
            fprintf(port,"   %d    \n", i);
            usleep(wait);
            fprintf(port,"    %d   \n", i);
            usleep(wait);
            fprintf(port,"     %d  \n", i);
            usleep(wait);
            fprintf(port,"      %d \n", i);
            usleep(wait);
            fprintf(port,"       %d\n", i);
            usleep(wait);
            fprintf(port,"      %d \n", i);
            usleep(wait);
            fprintf(port,"     %d  \n", i);
            usleep(wait);
            fprintf(port,"    %d   \n", i);
            usleep(wait);
            fprintf(port,"   %d    \n", i);
            usleep(wait);
            fprintf(port,"  %d     \n", i);
            usleep(wait);
            fprintf(port," %d      \n", i);
            usleep(wait);
            fprintf(port,"%d       \n", i);
            usleep(wait);
            i++;
          }
          i=9;
          while (i >= 0 )
          {
            fprintf(port,"%d       \n", i);
            usleep(wait);
            fprintf(port," %d      \n", i);
            usleep(wait);
            fprintf(port,"  %d     \n", i);
            usleep(wait);
            fprintf(port,"   %d    \n", i);
            usleep(wait);
            fprintf(port,"    %d   \n", i);
            usleep(wait);
            fprintf(port,"     %d  \n", i);
            usleep(wait);
            fprintf(port,"      %d \n", i);
            usleep(wait);
            fprintf(port,"       %d\n", i);
            usleep(wait);
            fprintf(port,"      %d \n", i);
            usleep(wait);
            fprintf(port,"     %d  \n", i);
            usleep(wait);
            fprintf(port,"    %d   \n", i);
            usleep(wait);
            fprintf(port,"   %d    \n", i);
            usleep(wait);
            fprintf(port,"  %d     \n", i);
            usleep(wait);
            fprintf(port," %d      \n", i);
            usleep(wait);
            fprintf(port,"%d       \n", i);
            usleep(wait);
            i--;
          }
          led = fopen(argv[1], "w" );
          fprintf(led,"0");
          fclose(led);
        }
      }
    }
  fclose(port);
  }
  return 0;
}
