/*
 *  Linux Frame Buffer Device Configuration
 *
 *  (C) Copyright 1995-1998 by Geert Uytterhoeven
 *		       (Geert.Uytterhoeven@cs.kuleuven.ac.be)
 *
 *  --------------------------------------------------------------------------
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of the Linux
 *  distribution for more details.
 */


%{

#define YYSTYPE		long

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <linux/fb.h>
#include "fbset.h"

extern int yylex(void);
extern void yyerror(const char *s);
extern int line;


static struct VideoMode VideoMode;

static void ClearVideoMode(void)
{
    memset(&VideoMode, 0, sizeof(VideoMode));
    VideoMode.accel_flags = FB_ACCELF_TEXT;
}

%}

%start file

%token MODE GEOMETRY TIMINGS HSYNC VSYNC CSYNC GSYNC EXTSYNC BCAST LACED DOUBLE
       NONSTD GRAYSCALE
       ENDMODE ACCEL RGBA FRACTION POLARITY BOOLEAN STRING NUMBER

%%

file	  : vmodes
	  ;


vmodes	  : /* empty */
	  | vmodes vmode
	  ;

vmode	  : MODE STRING geometry timings options ENDMODE
	    {
		VideoMode.name = (char *)$2;
		AddVideoMode(&VideoMode);
		ClearVideoMode();
	    }
	  ;

geometry  : GEOMETRY NUMBER NUMBER NUMBER NUMBER NUMBER
	    {
		ClearVideoMode();
		VideoMode.xres = $2;
		VideoMode.yres = $3;
		VideoMode.vxres = $4;
		VideoMode.vyres = $5;
		VideoMode.depth = $6;
	    }
	  ;

timings	  : TIMINGS NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER
	    {
		VideoMode.pixclock = $2;
		VideoMode.left = $3;
		VideoMode.right = $4;
		VideoMode.upper = $5;
		VideoMode.lower = $6;
		VideoMode.hslen = $7;
		VideoMode.vslen = $8;
	    }
	  ;

options	  : /* empty */
	  | options accel
	  | options rgba
	  | options hsync
	  | options vsync
	  | options csync
	  | options gsync
	  | options extsync
	  | options bcast
	  | options laced
	  | options double
	  | options nonstd
	  | options grayscale
	  ;

accel	  : ACCEL BOOLEAN
	    {
		VideoMode.accel = $2;
	    }
	  ;

grayscale  : GRAYSCALE BOOLEAN
	    {
		    /* VideoMode.grayscale = $2; */
	    }
	  ;

nonstd	  : NONSTD NUMBER
	    {
		    /* VideoMode.nonstd = $2; */
	    }
	  ;

rgba	  : RGBA FRACTION ',' FRACTION ',' FRACTION ',' FRACTION
	    {
		VideoMode.rgba[0] = $2;
		VideoMode.rgba[1] = $4;
		VideoMode.rgba[2] = $6;
		VideoMode.rgba[3] = $8;
	    }
	  ;

hsync	  : HSYNC POLARITY
	    {
		VideoMode.hsync = $2;
	    }
	  ;

vsync	  : VSYNC POLARITY
	    {
		VideoMode.vsync = $2;
	    }
	  ;

csync	  : CSYNC POLARITY
	    {
		VideoMode.csync = $2;
	    }
	  ;

gsync	  : GSYNC POLARITY
	    {
		VideoMode.gsync = $2;
	    }
	  ;

extsync	  : EXTSYNC BOOLEAN
	    {
		VideoMode.extsync = $2;
	    }
	  ;

bcast	  : BCAST BOOLEAN
	    {
		VideoMode.bcast = $2;
	    }
	  ;

laced	  : LACED BOOLEAN
	    {
		VideoMode.laced = $2;
	    }
	  ;

double	  : DOUBLE BOOLEAN
	    {
		VideoMode.dblscan = $2;
	    }
	  ;

%%
