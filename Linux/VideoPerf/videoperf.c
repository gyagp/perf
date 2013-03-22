/*
 Copyright (C) 2010 Intel Corporation.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 It is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this software; if not, write to the Free
 Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 02111-1307 USA.
                                                                                        
 Authors:                                                                               
 	Wei, Zhang <wei.z.zhang@intel.com>

Use the X Damage Extension to track modified regions of drawables,
Capture window XDamageNotify to calculate media player framerate

Modify:
	Min, Li < min.a.li@intel.com>
add variance function
add auto set DISPLAY and Get Window ID 

Note: The media player should be played full-screen to avoid the 
      impact from other control widget.

*/

#include <math.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#define  MAX_LEN 1000

static int term_flag = 0;

void term_handler (int signo)
{
	term_flag = 1;
}

struct evt_proc_arg
{
	Display *pd;
	int dmgevt;
	int verbose;
	long *pcount;
};

void * evt_proc (void *arg)
{
	XEvent e;
	struct evt_proc_arg *p = (struct evt_proc_arg *) arg;

	while (1)
	{
		XNextEvent (p->pd, &e);

		if (e.type == (p->dmgevt + XDamageNotify))
    		{
     			 (*(p->pcount))++;
			
			 if (p->verbose)
     			 {
				XDamageNotifyEvent *dev;
				dev = (XDamageNotifyEvent *) & e;
				printf ("Area(x,y,w,h)=(%d %d %d %d),\t", dev->area.x,
				dev->area.y, dev->area.width, dev->area.height);
				printf ("Geometry(x,y,w,h)=(%d %d %d %d),\t", dev->geometry.x,
				dev->geometry.y, dev->geometry.width,dev->geometry.height);
				static int i = 0;
				printf ("damage event received %d\n", i++);
			}
		}
	}	
}

void  variance( long x[], int n, double * var )
{
	int i=0;
	double averx, sumx=0;
	double sum_t =0;

	for( i=0; i<n; i++ )
	{
		sumx+=x[i];
	}

	averx=sumx/n;

	for( i=0; i<n; i++ )
	{
		sum_t += (x[i] -averx)*(x[i] -averx);
	}

	*var=sum_t/(n-1);
	return;
}


int main (int argc, char *argv[])
{
	char *p= NULL;
	Drawable win;
	Display *display;
	pthread_t tid;
	long timer_n_frames = 0;
	int damage_evt, damage_err;
	int verbose = 0;
	double timeout = 30;
	double interval = 0;
	double f_time = 0;
	int f_sum = 0;
	long value_x[MAX_LEN]={0,};
	int i=0;
	double var=0;
	int ch;
	opterr = 0;

	while ((ch = getopt (argc, argv, "t:hv")) != EOF)
	{
		switch (ch)
		{
			case 'v':
			verbose = 1;
			break;
			
			case 't':
			sscanf (optarg, "%lf", &timeout);
			break;

			case 'h':
			printf ("%s -t %s -v\n", argv[0], "[Test Seconds]");
			return 0;

			default:
			printf ("%s -t %s -v\n", argv[0], "[Test Seconds]");
			return 1;
		}	
	}

	//auto set DISPLAY
	//printf("Set DISPLAY\n");
	setenv("DISPLAY", ":0.0", 1);

	display = XOpenDisplay(NULL);
	
	if(display == NULL)
	{
		printf ("\nFailed to connect to Display\n");
		return -1;
	}

	win = (long) RootWindow(display, DefaultScreen(display));
	//printf("window ID=%x\n", win);

	if( timeout > MAX_LEN)
	{	
                                                              
		printf("\n timeout outof max value,Failed\n ");
		return -1;
	}

	/* create damage to target window (media player)*/
	Damage damage = XDamageCreate (display, win,XDamageReportRawRectangles);

	/* check X damage extension */
	if (!XDamageQueryExtension (display, &damage_evt, &damage_err))
	{
		printf ("not support damage\n");
		return -1;
	}	

	signal (SIGINT, term_handler);

	/* capture XDamageNotify to calculate framerate */
	struct evt_proc_arg epa;
	epa.pd = display;
	epa.pcount = &timer_n_frames;
	epa.dmgevt = damage_evt;
	epa.verbose = verbose;
	
	if (pthread_create (&tid, NULL, evt_proc, (void *) &epa))
	{
		printf ("cannot create thread to watch event, exit");
		exit (1);
	}

	while (0 == term_flag)
	{
		if (f_time >= timeout && 0 != timeout)
		{
			break;
		}

		/* framerate calculation */
		timer_n_frames = 0;
		sleep (1);
	
		//f_time++;
		f_sum += timer_n_frames;
		
		printf("*** FPS: %d *** ", timer_n_frames);
		value_x[(int)f_time]=timer_n_frames;
		f_time++;
	
		if (0 != timeout)
		{
			printf ("[%d/%d]\n", (int) f_time, (int) timeout);
		}
		else
		{
			printf ("\n");
		}
	}

	printf("\n**************\nAverage: %.1f\n**************\n",f_sum / f_time);

	//get variance
	variance( value_x, timeout, &var );
	printf("\n**************\nVariance: %.2f\n**************\n", var);

	/* clean */
	//XCloseDisplay(display);
	return 0;
}
