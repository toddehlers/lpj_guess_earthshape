//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////
// BENGRAPH
// Library of functions to facilitate production of PostScript graphics code
// Copyright 1996-2007 Ben Smith, all rights reserved
//
// Address enquiries to:
//   Dr B Smith
//   Dept of Physical Geography and Ecosystems Analysis
//   University of Lund
//   Geocentrum II
//   22362 Lund
//   Sweden
//   E-mail: ben.smith@nateko.lu.se
//
//   http://www.nateko.lu.se/personal/benjamin.smith/software

#include <bengraph.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

// GLOBAL VARIABLE DECLARATIONS

FILE* out;
double grey; 
double lwd;
int transp;
int pageorientation;
double pagewidth;	// in ps points = 594.1 for A4
location boxbl,boxtr;	// bounding box
char filename[MAXBGPATH];
char pstitle[MAXBGFILE];
char tempfil[]="post.xxbg";  // temporary postscript file
linedef line_style;
textdef text_style;
markerdef marker_style;
patterndef pattern_style;
long colour_style; // default colour in hexadecimal RGB format 0xRRGGBB
FILE *tdf;
int dataopen=0;	// whether temporary data file open
double mindx,mindy,minde,maxdx,maxdy,maxde;
int seriesid;
double pagescale;  // scaler from page units to ps points
scaledef graphscale; // scaler from graph units to page units
location graphbl;
location graphtr;			// corners of the graph area corresponding to graphscale
xypair autotic;			// not a location, but the automatic tic interval
xypair minortic;    // interval between minor tics
int usermarker; 		// flag to say user has explicitly chosen a marker symbol
int userline;       // flag
int userpattern;    // flag
int usernumformat;
int usercolour; // flag
double outlinepats;		// flag for outlining of histogram bars (-ve = no outline, +ve = linewidth)
int logerror;				// flag
axisdef xaxispars;
axisdef yaxispars;
ticdef tic_style;
ticdef minortic_style;
labeldef xlabel_style;
labeldef ylabel_style;
errordef error_style;
legenddef legend_style;
arrowdef arrow_style;
numformat num_style;
int axinverse;			 // if true, user x-axis is vertical, user y-axis horizontal
int wantminortics;	// whether wanted or not
int iseps;
datascaledef datascale_curr;
colourscaledef colourscale_curr; // normal colour scale
colourscaledef colourscale_curr_dual; // secondary (negative) colour scale
colourkeydef* pcolourkey;
int ncolourkey; // number of classes in current colour key


// Colourdefs

flexcolourdef fcd_red[]={
	
	0.95,0.95,0.95,0,
	1.0,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_green[]={
	
	0.95,0.95,0.95,0,
	0,1.0,0,1,
 
	-1,-1,-1,-1
};

flexcolourdef fcd_blue[]={
	
	0.95,0.95,0.95,0,
	0,0,1.0,1,

	-1,-1,-1,-1
};

flexcolourdef fcd_grey[]={
	
	0.95,0.95,0.95,0,
	0,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_redviolet[]={

	1,0,0,0,
	1,0.6,0,0.1,
	1,1,0,0.2,
	0.75,1,0,0.3,
	0,1,0,0.35,
	0,1,0.7,0.4,
	0,1,1,0.5,
	0,0.6,1,0.67,
	0,0,1,0.75,
	0.5,0,1,0.83,
	1,0,1,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_violetred[]={

	1,0,1,0,
	0.5,0,1,0.17,
	0,0,1,0.25,
	0,0.6,1,0.33,
	0,1,1,0.5,
	0,1,0.7,0.6,
	0,1,0,0.65,
	0.75,1,0,0.7,
	1,1,0,0.8,
	1,0.6,0,0.9,
	1,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_redblue[]={

	1,0,0,0,
	1,0.6,0,0.133,
	1,1,0,0.27,
	0.75,1,0,0.4,
	0,1,0,0.47,
	0,1,0.7,0.53,
	0,1,1,0.67,
	0,0.6,1,0.89,
	0,0,1,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_bluered[]={
	
	0,0,1,0,
	0,0.6,1,0.11,
	0,1,1,0.33,
	0,1,0.7,0.47,
	0,1,0,0.53,
	0.75,1,0,0.6,
	1,1,0,0.73,
	1,0.6,0,0.867,
	1,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_redcyan[]={
	
	1,0,0,0,
	0.9,0.9,0.9,0.5,
	0,1,1,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_cyanred[]={
	
	0,1,1,0,
	0.9,0.9,0.9,0.5,
	1,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_greyred[]={
	
	0.9,0.9,0.9,0,
	0.7,0.7,0.7,0.4,
	1,0,0,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_greycyan[]={
	
	0.9,0.9,0.9,0,
	0.7,0.7,0.7,0.4,
	0,1,1,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_cool[]={
	
	0.3,1.0,0.3,0,
	0.0,0.9,0.7,0.2,
	0.6,1,1.0,0.4,
	0.0,0.7,1.0,0.6,
	0.1,0.3,1.0,0.8,
	0.4,0,0.8,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_warm[]={
	
	1.0,1.0,0.5,0,
	1.0,0.8,0.0,0.2,
	1.0,0.55,0.0,0.4,
	1.0,0.0,0.0,0.55,
	0.8,0.0,0.6,0.7,
	0.4,0,0.2,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_neegreen[]={
	
	0.66,1,0,0,
	0,0.86,0.22,0.33,
	0,0.64,0.27,0.67,
	0,0.32,0.38,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_neered[]={
	
	1,0.9,0,0,
	1,0.45,0,0.35,
	1,0,0,0.5,
	0.38,0,0.32,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_altitude[]={
	
	0,0.6,0.29,0,
	0.6,0.8,0.4,0.15,
	1,0.97,0.6,0.3,
	0.99,0.64,0.5,0.5,
	0.6,0.3,0.33,0.7,
	0.75,0.15,0.6,0.85,
	0.8,0.8,0.8,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_depth[]={

	0.8,0.95,1,0,
	0,0.75,0.95,0.4,
	0,0.4,0.52,0.8,
	0,0.2,0.27,1.0,
	-1,-1,-1,-1
};

flexcolourdef fcd_soil[]={

	1,0.9,0.82,0,
	1,0.8,0.3,0.3,
	0.32,0.32,0.38,0.7,
	0.32,0.45,0.76,1.0,
	-1,-1,-1,-1
};

flexcolourdef fcd_vegetation[]={

	1,0.9,0.8,0,
	0.9,0.66,0.6,0.2,
	0.79,0.76,0.49,0.4,
	0.62,0.67,0,0.6,
	0.3,0.4,0,0.8,
	0,0.3,0.5,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_fire[]={

	1,1,0.5,0,
	1,0,0,0.5,
	0.6,0,0.4,0.7,
	0,0.4,0.8,1.0,
	-1,-1,-1,-1
};

flexcolourdef fcd_wet[]={

	0.6,1,0.8,0,
	0.3,0.5,1.0,0.55,
	0.1,0.1,0.8,0.8,
	0.2,0.2,0.4,1,
	-1,-1,-1,-1
};

flexcolourdef fcd_dry[]={

	0.66,0.76,0.4,0,
	0.9,0.66,0.6,0.6,
	1,0.95,0.9,1.0,
	-1,-1,-1,-1
};



// FUNCTIONS

void HLStoRGB(int H, int L, int S,double& red,double& green,double& blue) {

	// H = hue (range 0-360, 0=red,120=green,240=blue)
	// L = luminance (range 0-100)
	// S = saturation (range 0-100)
	// red, green, blue - 0-1 range
	
	const int LSMAX=100;
	const int LM=LSMAX/2;
	const int CMAX=255;
	int OffsetLightness;
	int OffsetSaturation;
	int r=0,g=0,b=0;
    
	if (L<0) L=0;
	else if (L>LSMAX) L=LSMAX;
	
	if (S<0) S=0;
	else if (S>LSMAX) S=LSMAX;
	
	if (H < 0) H = 360-(-H%360);
	else H = H  % 360;

	if (H < 120) r = CMAX * (120 - H)  / 60;
	else if (H > 240) r = CMAX * (H - 240) / 60;
	if (r>CMAX) r=CMAX;

	if (H < 240) {
		g = CMAX * (120 - fabs(H -  120)) / 60;
		if (g>CMAX) g=CMAX;
	}
	
	if (H >  120) {
		b = CMAX * (120 - fabs(H - 240)) / 60;
		if (b>CMAX) b=CMAX;
	}

	r=r*float(S)/LSMAX;
	g=g*float(S)/LSMAX;
	b=b*float(S)/LSMAX;
	OffsetSaturation = (CMAX+1)/2 * (LSMAX - S) / LSMAX;
	r += OffsetSaturation;
	g += OffsetSaturation;
	b += OffsetSaturation;

	r = r * ((LM - fabs(L - LM)) / LM);
	g = g * ((LM - fabs(L - LM)) / LM);
	b = b * ((LM - fabs(L - LM)) / LM);

	if (L > LM) {
		OffsetLightness = CMAX * (L - LM) / LM;
		r += OffsetLightness;
		g += OffsetLightness;
		b += OffsetLightness;
	}
    
	// Convert RGB to 0-1 scale

	red=double(r)/255.0;
	green=double(g)/255.0;
	blue=double(b)/255.0;
}

void huetorgb(double hue,double& red,double& green,double& blue) {

	// Returns RGB from a hue between 0 (red) and 360
		
	HLStoRGB(hue,50,100,red,green,blue);
	return;
}

void hlstorgb(double hue,double light,double sat,double& red,double& green,double& blue) {

	HLStoRGB(hue,light*100.0,sat*100.0,red,green,blue);
	return;
}


int fnsplit(char* filename,char* drive,char* dir,char* file,char* ext) {

	*drive=0;
	int slashplace=-1,i,flags=0;
	for (i=0;i<strlen(filename);i++)
		if (filename[i]=='/') slashplace=i;
	if (slashplace>=0) {
		filename[slashplace]=0;
		strcpy(dir,filename);
		filename[slashplace]='/';
		strcpy(file,filename+slashplace+1);
		flags|=DIRBG;
	}
	else {
		*dir=0;
		strcpy(file,filename);
	}
	if (*file) {
		slashplace=-1;
		for (i=0;i<strlen(file);i++)
			if (file[i]=='.') slashplace=i;
		if (slashplace>=0) {
			file[slashplace]=0;
			strcpy(ext,file+slashplace+1);
			if (*ext) flags|=EXTBG;
		}
	}
	for (i=0;i<strlen(filename);i++)
		if (filename[i]=='*' || filename[i]=='?') flags|=WILDCARDSBG;
	return flags;

}


int strcmpi(char* w1,char* w2) {

	char c1,c2;
	while (1) {
		c1=*w1++;
		c2=*w2++;
		if (!c1 && !c2) return 0;
		if (!c1 || !c2) return 1;
		if (c1>='a' && c1<='z') c1-=32;
		if (c2>='a' && c2<='z') c2-=32;
		if (c1!=c2) return 1;
	}
}


void psout(char* str) {

	char buf[MAXBGSTR];
	char* bufptr=buf;
	while (*str) {
		if (*str=='%') {
			*bufptr++='%';
			*bufptr++='%';
		}
		else *bufptr++=*str;
		str++;
	}
	*bufptr=0;
	fprintf(out,buf);
	return;
}

void psout(double d) {

	fprintf(out,"%3.1f",d);
	return;
}

void psout(int i) {

	fprintf(out,"%0d",i);
	return;
}

void pc(double x) {

	// prints a single number with one decimal place (no space at end)

	psout(x);
	return;

}


void pc(double h,double v,commandtype c) {

	// prints PS point coordinates, a command (unless NONE specified)
	// a newline is printed unless c=NONE

	pc(h);
	psout(" ");
	pc(v);
	psout(" ");
	switch (c) {
		case NONE: break;
		case MOV: psout("moveto\n"); break;
		case LIN: psout("lineto\n"); break;
	};
	return;

}


void psheader() {

	// prints header for each postscript file (with text handling subroutines)
	// note: bounding box must be specified at end of file

//	psout("save\n\n");
	psout("/mc { moveto } def\n\n");
	psout("/text { mc { true textseg } forall } def\n\n");
	psout("/show_text {\n");
	psout(" /ptsz exch def /str exch def\n");
	psout(" currentpoint\n");
	psout(" /yb exch def /xl exch def\n");
	psout(" str stringwidth\n");
	psout(" pop xl add /xr exch def\n");
	psout(" /yt yb ptsz .8 mul sub def\n");
	psout(" xl yb mc str show\n");
	psout("} def\n\n");
	psout("/textseg {\n");
	psout(" /draw exch def\n");
	psout(" /seg exch def\n");
	psout(" seg (save) eq { gsave } if\n");
	psout(" seg (rest) eq { grestore } if\n");
	psout(" seg type /arraytype eq {\n");
	psout("  seg 3 get findfont seg 2 get scalefont setfont\n");
	psout("  0 seg 1 get rmoveto\n");
	psout("  seg 4 get draw { seg 2 get show_text } ");
	psout("{ stringwidth rmoveto } ifelse\n");
	psout(" } if\n");
	psout("} def\n\n");
	psout("/cent_text {\n");
	psout(" gsave\n");
	psout(" /just exch def\n");
	psout(" /degrees exch def\n");
	psout(" mc\n");
	psout(" currentpoint translate\n");
	psout(" /str exch def\n");
	psout(" str { false textseg } forall\n");
	psout(" currentpoint\n");
	psout(" degrees rotate\n");
	psout(" 0 mul\n");
	psout(" exch\n");
	psout(" just mul\n");
	psout(" exch\n");
	psout(" translate\n");
	psout(" 0 0 moveto\n");
	psout(" str { true textseg } forall\n");
	psout(" grestore\n");
	psout("} def\n\n");
	return;

}


int orientation(int i) {

	// sets page orientation (returning old orientation)
	// 0 = portrait, 1=landscape, A4 pages assumed

	int oldi=pageorientation;
	if (oldi==i) return i;

	if (oldi) {
		pc(0,pagewidth,NONE);
		psout("translate\n");
		psout("-90 rotate\n");
	}
	else {
		pc(pagewidth,0,NONE);
		psout("translate\n");
		psout("90 rotate\n");
	}
	pageorientation=i;
	return oldi;

}



void next_page() {

	// executes a "showpage", i.e. form-feed
	// no output unless this is called (but automatically called by bgend)

	if (!iseps) psout("showpage\n");
	return;

}


int bgend() {

	// ends BenGraph
	// prints current page, sets bounding box for eps (NB: meaningless
	// if more than one page, i.e. if nextpage was called before here).

	/* Error handling: 0 no error
										 1 cannot open or write output file (*fil from bgstart)
											 or temporary input file */

	FILE* in;
	char ch;
	char command[100]="rm ";
	next_page();
	fclose(out);

	// read in temporary file, and write out again, specifying bounding box

	in=fopen(tempfil,"rt");
	out=fopen(filename,"wt");
	if (!in || !out) return 1;
	psout("%!PS-Adobe-3.0\n");
	psout("%%Creator: BenGraph\n");
	psout("%%Title: ");
	psout(pstitle);
	psout("\n");
//	psout("%%BoundingBox: 156 27 571 675\n");
//	psout("%%BoundingBox: 27 23 675 438\n");

 /* psout("%%BoundingBox: ");
	pc(boxbl.h,boxbl.v,NONE);
	pc(boxtr.h,boxtr.v,NONE);
	psout("\n");*/
  psheader();
	ch=fgetc(in);
	while (!feof(in)) {
		fputc(ch,out);
		ch=fgetc(in);
  }
	fclose(in);
	fclose(out);
	
	strcat(command,tempfil);
	system(command);
	
	return 0;
}


int bgstart(char* fil,double user_pagewidth) {

	// initialises bengraph:
	//    opens filnam as eps output file
	//		writes ps header to filnam

	// check if filnam has an extension and, if not, add one (.eps)

	/* Error handling: 0 no error
                     1 cannot open temporary file for output */

	char drive[MAXBGDRIVE];
	char dir[MAXBGDIR];
	char file[MAXBGFILE];
	char ext[MAXBGEXT];
	int flags,i;

	flags=fnsplit(fil,drive,dir,file,ext);

	*filename=0;
	if (flags&DRIVEBG) strcat(filename,drive);
	if (flags&DIRBG) strcat(filename,dir);
	strcat(filename,file);

	if(!(flags & EXTBG)) {
		if (iseps)
			strcat(filename,".eps");
		else
			strcat(filename,".ps");
	}
	else {
		strcat(filename,".");
		strcat(filename,ext);
	}

	for (i=0;i<strlen(file);i++)
		pstitle[i]=toupper(file[i]);
	pstitle[i]=0;

	out=fopen(tempfil,"wt");
	if (!out) return 1;

	// set defaults

	psout("0 0 0 setrgbcolor\n");
	grey=0.0;
	set_grey_internal(grey);
	transp=0;
	pageorientation=0;  // portrait
	lwd=DEFAULT_LINEWIDTH;
	pagewidth=user_pagewidth;
	boxbl.h=user_pagewidth;
	boxbl.v=user_pagewidth*10.0;
	boxtr.h=0.0;
	boxtr.v=0.0;
	datascale_curr.nclass=1;
	datascale_curr.zeroclass=false;
	datascale_curr.minval=0.0;
	datascale_curr.interval=1.0;
	datascale_curr.dual=false;
	datascale_curr.logarithmic=false;

	colourscale_curr.pflexcolours=NULL;
	
	ncolourkey=0;
	pcolourkey=NULL;

	return 0;

}


void bb(double h,double v) {

	// updates bounding box if necessary
	// half of current line width added outwards (since most
	// calling functions potentially use a line of current width)

	double w=lwd/2.0;
	if (h+w>boxtr.h) boxtr.h=h+w;
	if (h-w<boxbl.h) boxbl.h=h-w;
	if (v+w>boxtr.v) boxtr.v=v+w;
	if (v-w<boxbl.v) boxbl.v=v-w;
	return;

}


static void fill(int filled) {

	// depending on state of transparent and filled, fills shapes

	if (transp && !filled) {
		psout("stroke\n");
		return;
	}
	if (filled) {
		psout("gsave\n");
		psout("fill\n");
		psout("grestore\n");
		psout("stroke\n");
		return;
	}

	// not transparent, not filled

	psout("gsave\n");
	set_grey_internal(1.0);
	psout("fill\n");
	psout("grestore\n");
	psout("stroke\n");
	return;

}

static void shape_circle(double size,location point,int filled) {

	// plots a circle of (PS point) width size at point

	psout("newpath\n");
	pc(point.h+size*0.5,point.v,MOV);
	pc(point.h,point.v,NONE);
	pc(size*0.5);
	psout(" 0 360 arc\n");
	fill(filled);
	bb(point.h-size*0.5,point.v-size*0.5);
	bb(point.h+size*0.5,point.v+size*0.5);
	return;

}


static void shape_thtriangle(double size,location point,int filled) {

	// plots a top-heavy equilateral triangle
	// each side = width_at_centroid*1.5

	double ho,vo;
	psout("newpath\n");
	ho=point.h-0.6*size;
	vo=point.v+0.3464*size;
	pc(ho,vo,MOV);
	pc(ho+1.2*size,vo,LIN);
	pc(point.h,point.v-0.6928*size,LIN);
	pc(ho,vo,LIN);
	pc(ho+1.2*size,vo,LIN);
	fill(filled);
	bb(ho,point.v-0.6928*size);
	bb(ho+1.2*size,vo);
	return;

}

static void shape_bhtriangle(double size,location point,int filled) {

	// plots a bottom-heavy equilateral triangle

	double ho,vo;
	psout("newpath\n");
	ho=point.h-0.6*size;
	vo=point.v-0.3464*size;
	pc(ho,vo,MOV);
	pc(ho+1.2*size,vo,LIN);
	pc(point.h,point.v+0.6928*size,LIN);
	pc(ho,vo,LIN);
	pc(ho+1.2*size,vo,LIN);
	fill(filled);
	bb(ho,vo);
	bb(ho+1.2*size,point.v+0.6928*size);
	return;

}


static void shape_square(double size,location point,int filled) {

	// plots a square

	double ho,vo;
	psout("newpath\n");
	ho=point.h-0.5*size;
	vo=point.v-0.5*size;
	pc(ho,vo,MOV);
	pc(ho+size,vo,LIN);
	pc(ho+size,vo+size,LIN);
	pc(ho,vo+size,LIN);
	psout("closepath\n");
	fill(filled);
	bb(ho,vo);
	bb(ho+size,vo+size);
	return;

}


static void shape_diamond(double size,location point,int filled) {

	// plots a diamond

	double ho,vo;
	psout("newpath\n");
	double os=0.70711*size;
	ho=point.h-os;
	vo=point.v;
	pc(ho,vo,MOV);
	pc(ho+os,vo+os,LIN);
	pc(ho+2*os,vo,LIN);
	pc(ho+os,vo-os,LIN);
	psout("closepath\n");
	fill(filled);
	bb(ho,vo-os);
	bb(ho+2*os,vo+os);
	return;

}


static void shape_vcross(double size,double thick,location point,int filled) {

	// plots a vertical/horizontal cross
	// uses lthick to determine diameter

	double xc,yc,ow,ot;
	psout("newpath\n");
	xc=point.h;
	yc=point.v;
	ow=size*0.5;
	ot=thick*0.5;
	pc(xc-ow,yc-ot,MOV);
	pc(xc-ow,yc+ot,LIN);
	pc(xc-ot,yc+ot,LIN);
	pc(xc-ot,yc+ow,LIN);
	pc(xc+ot,yc+ow,LIN);
	pc(xc+ot,yc+ot,LIN);
	pc(xc+ow,yc+ot,LIN);
	pc(xc+ow,yc-ot,LIN);
	pc(xc+ot,yc-ot,LIN);
	pc(xc+ot,yc-ow,LIN);
	pc(xc-ot,yc-ow,LIN);
	pc(xc-ot,yc-ot,LIN);
	psout("closepath\n");
	fill(filled);
	bb(xc-ow,yc-ow);
	bb(xc+ow,yc+ow);
	return;

}


static void shape_dcross(double size,double thick,location point,int filled) {

	// plots a diagonal cross

	double xc,yc,of,on,in;
	psout("newpath\n");
	xc=point.h;
	yc=point.v;
	of=0.35355*(thick+size);
	on=0.35355*(size-thick);
	in=0.70711*thick;
	pc(xc-on,yc-of,MOV);
	pc(xc-of,yc-on,LIN);
	pc(xc-in,yc,LIN);
	pc(xc-of,yc+on,LIN);
	pc(xc-on,yc+of,LIN);
	pc(xc,yc+in,LIN);
	pc(xc+on,yc+of,LIN);
	pc(xc+of,yc+on,LIN);
	pc(xc+in,yc,LIN);
	pc(xc+of,yc-on,LIN);
	pc(xc+on,yc-of,LIN);
	pc(xc,yc-in,LIN);
	psout("closepath\n");
	fill(filled);
	bb(xc-of,yc-of);
	bb(xc+of,yc+of);
	return;

}

void shape_point(shapetype shape,location point,
	double size,double thick,int filled) {

	// this is the home function for shapes plotted as parts of
	// lines.  The main switch statement selects
	// which particular
	// shape will be plotted.  size is a general parameter for shape
	// size; different shape functions may use this in their own way.

	switch (shape) {
		case CIRCLE: 		shape_circle(size,point,filled);break;
		case SQUARE: 		shape_square(size,point,filled);break;
		case THTRIANGLE:shape_thtriangle(size,point,filled);break;
		case BHTRIANGLE:shape_bhtriangle(size,point,filled);break;
		case DIAMOND:		shape_diamond(size,point,filled);break;
		case VCROSS:		shape_vcross(size,thick,point,filled);break;
		case DCROSS:		shape_dcross(size,thick,point,filled);break;
	}
	return;
}


void scatter_point(int npoint,location* point,markerdef markerstyle) {

	// plots a series of markers at a series of points
	// using the current marker style

	int i;
	psout(markerstyle.bwidth);
	psout(" setlinewidth\n");
	for (i=0;i<npoint;i++) {
		shape_point(markerstyle.shape,point[i],markerstyle.swidth,
			markerstyle.hwidth,markerstyle.sfilled);
	}
	psout(lwd);
	psout(" setlinewidth\n");
	return;

}


static void linedash(location* point,linedef& linestyle) {

	// plots a line between the two points given by the first
	// two entries in point, saving the current status of the
	// line parameters (which define how far along dashes etc
	// we are)

	// first work out the angle (radians) from point0 to point1
	// and how far away it is

	double dxag,dist,left;
	double curh,curv,hdisp,vdisp,proj,newh,newv;

	if (lwd) {
		dxag=linestyle.dxag*lwd/DEFAULT_LINEWIDTH;
	}
  else dxag=linestyle.dxag;

	bb(point[0].h,point[0].v);
  bb(point[1].h,point[1].v);

	curh=point[0].h;
	curv=point[0].v;
	hdisp=point[1].h-curh;
	vdisp=point[1].v-curv;

	if (!hdisp) {	// vertical, so can't handle with atan
		proj=PI*0.5;  //either +90 or -90 degrees
	}
	else {
		proj=fabs(atan(vdisp/hdisp));
	}
	// work out the quadrant (default 1)
	if (vdisp>=0.0 && hdisp<0.0) proj=PI-proj;     // quadrant 2
	else if (vdisp<0.0 && hdisp<=0.0) proj=PI+proj;  // quadrant 3
	else if (vdisp<0.0 && hdisp>0.0) proj=2*PI-proj; // quadrant 4

	dist=sqrt(hdisp*hdisp+vdisp*vdisp);

	// how much is left of current segment?

	left=(linestyle.dsl[linestyle.dsp]-linestyle.dpr)*dxag;

	while (left<dist) {

		// where is the point at the end of this segment?

		newv=curv+left*sin(proj);
		newh=curh+left*cos(proj);

		// is this a dash? then print to here (else blank)

		if (!(linestyle.dsp%2)) {
			pc(curh,curv,MOV);
			pc(newh,newv,LIN);
			psout("stroke\n");
		}

		// now go to next point, and next segment

		dist-=left;
		curh=newh;
		curv=newv;

		if (!linestyle.dsl[++linestyle.dsp]) linestyle.dsp=0;
						// go back to start if end of list

		left=(linestyle.dsl[linestyle.dsp])*dxag;

	}

	// here just print to final point

  left-=dist;
	if (!linestyle.dsp%2) {
		pc(curh,curv,MOV);
		pc(point[1].h,point[1].v,LIN);
		psout("stroke\n");
	}

	// update linestyle.dpr

	linestyle.dpr = linestyle.dsl[linestyle.dsp]-left/dxag;

	return;

}

static void lineshape(location* point,linedef& linestyle) {

	// plots a line of shapes between two points, saving progress

	// first work out the angle (radians) from point0 to point1
	// and how far away it is

	double dxag,dist,left;
	double curh,curv,hdisp,vdisp,proj,newh,newv,width;

	location trpt;

	curh=point[0].h;
	curv=point[0].v;
	hdisp=point[1].h-curh;
	vdisp=point[1].v-curv;
	if (!hdisp) {	// vertical, so can't handle with atan
		proj=PI*0.5;  //either +90 or -90 degrees
	}
	else {
		proj=fabs(atan(vdisp/hdisp));
	}
	// work out the quadrant (default 1)
	if (vdisp>=0.0 && hdisp<0.0) proj=PI-proj;     // quadrant 2
	else if (vdisp<0.0 && hdisp<=0.0) proj=PI+proj;  // quadrant 3
	else if (vdisp<0.0 && hdisp>0.0) proj=2*PI-proj; // quadrant 4

	dist=sqrt(hdisp*hdisp+vdisp*vdisp);
	width=linestyle.swidth;

	if (!linestyle.dpr) {		// print a shape at start
		shape_point(linestyle.shape,point[0],width,width*0.1,linestyle.sfilled);
	}

	// how much is left of current segment?

	left=linestyle.sspace-linestyle.dpr;

	while (left<dist) {

		// where is the point at the end of this segment?

		newv=curv+left*sin(proj);
		newh=curh+left*cos(proj);

		// print a new shape here

		trpt.h=newh;
		trpt.v=newv;

		shape_point(linestyle.shape,trpt,width,width*0.1,linestyle.sfilled);

		// now go to next point, and next segment

		dist-=left;
		curh=newh;
		curv=newv;

		left=linestyle.sspace;

	}

	// here just print to final point

	// update linestyle.dpr
	left-=dist;
	linestyle.dpr = linestyle.sspace-left;
	if (!linestyle.dpr) linestyle.dpr=1E-10;
		// so we don't print twice at the same place

	return;

}


void curve_point(int npoint,location* point,linedef linestyle) {

	// plots a curve using the current line type, with postscript points
  // as coordinates

	if (npoint<2) return;
	int i;
	location* pptr;

	// if current line type is solid, plot the whole curve

	if (linestyle.lclass==SOLID) {

		pc(lwd);
		psout(" setlinewidth\n");
		pc(point[0].h,point[0].v,MOV);
		bb(point[0].h,point[0].v);
		for (i=1;i<npoint;i++) {
			pc(point[i].h,point[i].v,LIN);
			bb(point[i].h,point[i].v);
		}
		psout("stroke\n");

	}

	// for dashed lines do it one segment at a time

	else if (linestyle.lclass==DASH) {

		pc(lwd);
		psout(" setlinewidth\n");
		linestyle.dsp=0;
		linestyle.dpr=0.0;
		pptr=point;
		for (i=0;i<npoint-1;i++) {
			linedash(pptr,linestyle);
			pptr++;
		}
	}

	// shape lines have their own routine too

	else {   // shape line

		pc(linestyle.swidth*0.1);
		psout(" setlinewidth\n");
		linestyle.dpr=0.0;
		pptr=point;
		for (i=0;i<npoint-1;i++) {
			lineshape(pptr,linestyle);
			pptr++;
		}
	}

	return;
}


static int bracktext(char*& text,fonttype& f) {

	// looks for a valid font name (in brackets) in text
	// blanks are ignored, any other text causes abortion
	// returns 1 if a font name is found

	char buffer[MAXBGSTR];
	char* bptr=buffer;
	char fontlist[NFONT][20];
	strcpy(fontlist[0],"HELVETICA");
	strcpy(fontlist[1],"TIMES");
	strcpy(fontlist[2],"COURIER");

	// make sure first non-blank is an open-bracket

	while (*text==' ') text++;
	if (*text!='(') return 0;
	text++;
	while (*text && *text!=')') {
		if (*text!=' ') *bptr++=*text;
		text++;
	}
	if (!*text) return 0;  // reached end of line
	*bptr=0;  // make buffer into a string
	text++;
	if (!strcmpi(fontlist[0],buffer)) {
		f=HELVETICA;
		return 1;
	}
	if (!strcmpi(fontlist[1],buffer)) {
		f=TIMES;
		return 1;
	}
	if (!strcmpi(fontlist[2],buffer)) {
		f=COURIER;
		return 1;
	}
	return 0;

}


static int bracknum(char*& text,double& siz,int symbol) {

	// looks for a valid text size or symbol number (in brackets) in text
	// blanks are ignored, any other text causes abortion
	// returns 1 if a valid value (range dependent on whether number
  // in brackets is a symbol number or text size) is found

	char buffer[MAXBGSTR];
	char* bptr=buffer;

	// make sure first non-blank is an open-bracket

	while (*text==' ') text++;
	if (*text!='(') return 0;
	text++;
	while (*text && *text!=')') {
		if (*text!=' ') *bptr++=*text;
		text++;
	}
	if (!*text) return 0;  // reached end of line
	*bptr=0;  // make buffer into a string
	text++;
	siz=atof(buffer);
	if (symbol && siz>=32.0 && siz<=254 ||
		!symbol && siz>=0.0 && siz<=100.0) return 1;
	return 0;

}



static void appendtext(double& voffset,char* text,textdef textstyle,
	int& textctr) {

	// adds ps spec for a text item in one style
	// voffset is the vertical offset for subscripts and superscripts
	// which must be corrected for in the next text call

  textctr++;
	psout("[ 0 ");
	if (textstyle.sub || textstyle.super) {
		pc(voffset,textstyle.size*SSMULT,NONE);
	}
	else {
		pc(voffset,textstyle.size,NONE);
	}
	switch (textstyle.font) {
		case HELVETICA:
			if (!textstyle.italic && !textstyle.bold) psout("/Helvetica");
			else if (textstyle.italic && !textstyle.bold) psout("/Helvetica-Oblique");
			else if (textstyle.bold && !textstyle.italic) psout("/Helvetica-Bold");
			else psout("/Helvetica-BoldOblique");
			break;
		case TIMES:
			if (!textstyle.italic && !textstyle.bold) psout("/Times-Roman");
			else if (textstyle.italic && !textstyle.bold) psout("/Times-Italic");
			else if (textstyle.bold && !textstyle.italic) psout("/Times-Bold");
			else psout("/Times-BoldItalic");
			break;
		case COURIER:
			if (!textstyle.italic && !textstyle.bold) psout("/Courier");
			else if (textstyle.italic && !textstyle.bold) psout("/Courier-Oblique");
			else if (textstyle.bold && !textstyle.italic) psout("/Courier-Bold");
			else psout("/Courier-BoldOblique");
			break;
		case SYMBOL:
			psout("/Symbol");
  }
	psout(" (");
	psout(text);
	psout(") ]\n");
	voffset=0.0;
	return;

}


static void dumptext(location point,justtype just,double rot,
	textdef textstyle,int& textctr) {

	// prints out a line of text (buffered by appendtext)

	double angle=rot/180.0*3.1415927;
	double dx=-0.35*sin(angle);
	double dy=0.35*cos(angle);
	double right=-1.0;

	psout(textctr);
	psout(" array astore ");
	if (just==TL || just==TC || just==TR) {
		point.h-=textstyle.size*dx*2.0;
		point.v-=textstyle.size*dy*2.0;
	}
	if (just==CL || just==CC || just==CR) {
		point.h-=textstyle.size*dx;
		point.v-=textstyle.size*dy;
	}
	pc(point.h,point.v,NONE);
	if (just==TR || just==CR || just==BR) {
		right=-1.0;
		pc(rot,right,NONE);
	}
	else if (just==TC || just==CC || just==BC) pc(rot,-0.5,NONE);
	else pc(rot,0.0,NONE);
	psout(" cent_text\n");
	textctr=0;
	return;

}



void text_point(char* text,location point,textdef textstyle,
	justtype just,double rot) {

	// plots text (which may contain font/style conversion codes)
	// justified according to just with respect to point
	// rot is the rotation anticlockwise from 0 in degrees
	// the anchor point (for a given just) is determined before rotation
	// NB: textstyle is local, not global, here

	char textbuf[MAXBGSTR],ch;
	char *bptr,*tptr;
	location curorig;
	fonttype newf;
	double rotrad=rot/180.0*PI;
	double voffset=0.0;
	double newsiz,symbnum;
  double maxtextsize=textstyle.size;
	int textctr;

	curorig.h=point.h;	// origin for current line
	curorig.v=point.v;
	tptr=text;					// current character in text
	bptr=textbuf;				// current position in buffer

	textctr=0;					// counts how many bits to text

	if (just==CENTRE) just=CC;
	if (just==RIGHT) just=CR;
	if (just==LEFT) just=CL;

	while (*tptr) {
		if (*tptr!='!') {  // printable text
			if (*tptr=='(' || *tptr==')') *bptr++='\\';	// to deal with ( and )
			*bptr++=*tptr++;
		}
		else { // possibly a command
			ch=*++tptr;
			tptr++;
			if (ch=='!') {	// a ! to print
				*bptr++='!';
			}
			else if (ch) {	// a command (ignored if not in allowed list)

				// first print off the buffer ...

				*bptr=0;
				bptr=textbuf;	// start again
				appendtext(voffset,textbuf,textstyle,textctr);

				// ... then process command

				if (ch>='a' && ch<='z') ch-=32;
				switch (ch) {
					case 'F':
						if (bracktext(tptr,newf)) textstyle.font=newf;
						break;
					case 'S':
						if (bracknum(tptr,newsiz,0)) {
							if (textstyle.sub) {	// if in subscripts, change offset
								voffset=(textstyle.size-newsiz)*SSMULT*0.5;
							}
							else if (textstyle.super) {
								voffset=(newsiz-textstyle.size)*SSMULT*0.5;
							}
							textstyle.size=newsiz;
              if (newsiz>maxtextsize) maxtextsize=newsiz;
						}
						break;
					case 'C':   // character from Symbols font
						if (bracknum(tptr,symbnum,1)) {
							newf=textstyle.font;
							textstyle.font=SYMBOL;
							textbuf[0]=(int)symbnum;
							textbuf[1]=0;
							if (*textbuf==')' || *textbuf=='(' || *textbuf=='\\') {
								textbuf[1]=*textbuf;
								textbuf[0]='\\';
								textbuf[2]=0;
              }
							appendtext(voffset,textbuf,textstyle,textctr);
							textstyle.font=newf;
						}
						break;
					case 'B':
						textstyle.bold=1;
						break;
					case 'I':
						textstyle.italic=1;
						break;
					case 'P':
						textstyle.bold=0;
						textstyle.italic=0;
						if (textstyle.sub) {
							textstyle.sub=0;
							voffset=textstyle.size*SSMULT*0.5;
						}
						else if (textstyle.super) {
							textstyle.super=0;
							voffset=-textstyle.size*SSMULT*0.5;
						}
						break;
					case '^':
						if (!textstyle.super) {	// ignore if already superscripts
							textstyle.super=1;
							if (textstyle.sub) {
								textstyle.sub=0;
								voffset=textstyle.size*SSMULT;	// to go from sub to sup
							}
							else {
								voffset=textstyle.size*SSMULT*0.5;
							}
						}
						break;
					case '_':
						if (!textstyle.sub) {
							textstyle.sub=1;
							if (textstyle.super) {
								textstyle.super=0;
								voffset=-textstyle.size*SSMULT;
							}
							else {
								voffset=-textstyle.size*SSMULT*0.5;
							}
						}
						break;
					case 'N':
						*bptr=0;
            bptr=textbuf;
						appendtext(voffset,textbuf,textstyle,textctr);
						dumptext(curorig,just,rot,textstyle,textctr);  // print this line
						curorig.h=curorig.h+(textstyle.size*LINESPACE)*sin(1.5708-rotrad);
						curorig.v=curorig.v-(textstyle.size*LINESPACE)*sin(1.5708-rotrad);
						break;
				}
      }
		}
	}

	*bptr=0;
	appendtext(voffset,textbuf,textstyle,textctr);
	dumptext(curorig,just,rot,textstyle,textctr); // print remaining text

	// too difficult to work out bounding box extension for text
	// just declare point +/- half of largest textsize encountered

	bb(point.h-maxtextsize/2.0,point.v-maxtextsize/2.0);
	bb(point.h+maxtextsize/2.0,point.v+maxtextsize/2.0);

	return;

}


// ********************************************************
// FUNCTIONS TO PLOT AREAS BETWEEN CURVES AND FILLED SHAPES
// ********************************************************


void filline(location s, location f) {

	// draws a line from s to f

	psout("newpath\n");
	pc(s.h,s.v,MOV);
	pc(f.h,f.v,LIN);
	psout("stroke\n");
	bb(s.h,s.v);
	bb(f.h,f.v);
	return;

}

static double mod(double a,double b) {

	return a-int(a/b)*b;
}

static void adjust_lightness(double& red,double& green,double& blue,double grey) {

	double r=red,g=green,b=blue,c=1.0-grey;

	red=r*c+grey;
	green=g*c+grey;
	blue=b*c+grey;
}

static void fillbox(double minx,double miny,double maxx,double maxy,
	patterndef patternstyle) {

	// fills a rectangle (at least) with current pattern
	
	double x,y,gap,mult,width,red,green,blue;
	long colour_style_bak;
	location s,f,point;

	// if not transparent, first wipe out all ink underneath

	if (!transp) {
		psout("gsave\n");
		psout("1 1 1 setrgbcolor\n");
		//set_grey_internal(1.0);
		psout("newpath\n");
		pc(minx,miny,MOV);
		pc(minx,maxy,LIN);
		pc(maxx,maxy,LIN);
		pc(maxx,miny,LIN);
		pc(minx,miny,LIN);
		psout("fill\n");
		psout("grestore\n");
	}

	pc(patternstyle.lwidth);
	psout(" setlinewidth\n");
	switch (patternstyle.pattern) {

		case VL:
			x=minx-mod(minx,patternstyle.spacing);
			s.v=miny;
			f.v=maxy;
			while (x<=maxx) {
				s.h=f.h=x;
				filline(s,f);
				x+=patternstyle.spacing;
			}
			break;
		case HL:
			y=miny-mod(miny,patternstyle.spacing);
			s.h=minx;
			f.h=maxx;
			while (y<maxy) {
				s.v=f.v=y;
				filline(s,f);
				y+=patternstyle.spacing;
			}
			break;
		case LDL:
			mult=1.414*patternstyle.spacing;
			gap=maxx-minx+mult;
			s.h=minx-mod(minx,mult);
			f.h=s.h+gap;
			s.v=miny-mod(miny,mult);
			f.v=s.v-gap;
			while (s.v<maxy+gap) {
				filline(s,f);
				s.v+=mult;
				f.v=s.v-gap;
			}
			break;
		case RDL:
			mult=1.414*patternstyle.spacing;
			gap=maxx-minx+mult;
			f.h=minx-mod(minx,mult);
			s.h=f.h+gap;
			s.v=miny-mod(miny,mult);
			f.v=s.v-gap;
			while (s.v<maxy+gap) {
				filline(s,f);
				s.v+=mult;
				f.v=s.v-gap;
			}
			break;
		case HV: // Horizontal-vertical hatching
			x=minx-mod(minx,patternstyle.spacing);
			s.v=miny;
			f.v=maxy;
			while (x<=maxx) {
				s.h=f.h=x;
				filline(s,f);
				x+=patternstyle.spacing;
			}
			y=miny-mod(miny,patternstyle.spacing);
			s.h=minx;
			f.h=maxx;
			while (y<maxy) {
				s.v=f.v=y;
				filline(s,f);
				y+=patternstyle.spacing;
			}
			break;
		case DH: // Diagonal hatching
			mult=1.414*patternstyle.spacing;
			gap=maxx-minx+mult;
			s.h=minx-mod(minx,mult);
			f.h=s.h+gap;
			s.v=miny-mod(miny,mult);
			f.v=s.v-gap;
			while (s.v<maxy+gap) {
				filline(s,f);
				s.v+=mult;
				f.v=s.v-gap;
			}
			f.h=minx-mod(minx,mult);
			s.h=f.h+gap;
			s.v=miny-mod(miny,mult);
			f.v=s.v-gap;
			while (s.v<maxy+gap) {
				filline(s,f);
				s.v+=mult;
				f.v=s.v-gap;
			}
			break;
		case GREY:
			psout("gsave\n");
			if (colour_style) {
				colour_style_bak=colour_style;
				red=(double)(colour_style>>16)/255.0;
				green=(double)((colour_style&0x00FF00)>>8)/255.0;
				blue=(double)(colour_style&0x0000FF)/255.0;
				adjust_lightness(red,green,blue,patternstyle.grey);
				colour(red,green,blue);
				colour_style=colour_style_bak;
			}
			else set_grey_internal(patternstyle.grey);
			psout("newpath\n");
			pc(minx,miny,MOV);
			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(maxx,miny,LIN);
			pc(minx,miny,LIN);
			psout("fill\n");
			psout("grestore\n");
			break;
		case SHAPEFILL:
			if (!patternstyle.lwidth) width=3.0;
			else width=5.0*patternstyle.lwidth;  // shape width
			y=miny-mod(miny,(patternstyle.spacing+width)*3.0);
			mult=0.0;
			while (y<=maxy) {
				x=minx-mod(minx,patternstyle.spacing+width);
				if (patternstyle.stagger) x+=patternstyle.spacing*mult;
				mult+=0.3334;
				if (mult>1.0) mult=0.0;
				while (x<=maxx) {
					point.h=x;
					point.v=y;
					shape_point(patternstyle.shape,point,width,0,patternstyle.fill);
					x+=patternstyle.spacing+width;
				}
				y+=patternstyle.spacing+width;
			}
			break;
	}
	pc(lwd);
	psout(" setlinewidth\n");
	bb(minx,miny);
	bb(maxx,maxy);
	return;
}


double density_logarithmic(double value,bool ifdual,bool ifzeroclass) {

	// computes a value in the range 0-1 for a logarithmic data scale
	
	int nclass,nclass_neg,nclass_sum,ct,pp,pn;
	bool found;
	double result;
	
	nclass=nclass_neg=0;
	
	if (!value && ifzeroclass) return 0.0;
	
	if (datascale_curr.bounds_neg) {
		ct=0;
		while (datascale_curr.bounds_neg[ct++]>0.0) nclass_neg++;
		pn=nclass_neg-1;
		ct=nclass_neg-2;
		found=false;
		while (ct>=0 && !found) {
			if (datascale_curr.bounds_neg[ct--]<=-value) found=true;
			else pn--;
		}
	}
	
	if (datascale_curr.bounds) {
		ct=0;
		while (datascale_curr.bounds[ct++]>0.0) nclass++;
		pp=nclass-1;
		ct=nclass-2;
		found=false;
		while (ct>=0 && !found) {
			if (datascale_curr.bounds[ct--]<=value) found=true;
			else pp--;
		}
	}
		
	if (ifdual) {
		if (value<0.0) {
			if (datascale_curr.bounds_neg) {
				if (nclass_neg>1 && pn)
					result=pn/(double)(nclass_neg-1);
				else
					result=0.00001;
				return -result;
			}
			else return 0.0;
		}
		else {
			if (datascale_curr.bounds) {
				if (nclass && pp)
					result=pp/(double)(nclass-1);
				else result=0.00001;
				return result;
			}
			else return 0.0;
		}
	}
	
	nclass_sum=nclass+nclass_neg;

	if (value>0.0) {
		if (datascale_curr.bounds) {
			pp+=nclass_neg;
			if (nclass_sum && pp)
				return pp/(double)(nclass_sum-1);
			else
				return 0.00001;
		}
		else return 0.0;
	}
	else {
		if (datascale_curr.bounds_neg) {
			pn=nclass_neg-pn-1;
			if (nclass_sum && pn)
				return pn/(double)(nclass_sum-1);
			else
				return 0.00001;
		}
		else return 0.0;
	}
	
	// Will never get here, but ...
	
	return 0;
}

void filldata_dither(double minx,double miny,double maxx,double maxy,double val,int format) {

	// Fills a box with a dither pattern based on the specified value and
	// current datascale

	// format= 0  =  box
	//         1  =  upper-left triangle
	//         2  =  lower-right triangle

	double density,maxval,width,x,y,mult,nclass;
	location point;

	// if not transparent, first wipe out all ink underneath

	if (!transp) {
		psout("gsave\n");
		set_grey_internal(1.0);
		psout("newpath\n");
		pc(minx,miny,MOV);

		if (format==1) {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else if (format==2) {

			pc(maxx,miny,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(maxx,miny,LIN);
			pc(minx,miny,LIN);

		}


		psout("fill\n");
		psout("grestore\n");
	}

	psout("gsave\n");
	psout("newpath\n");
	pc(minx,miny,MOV);
		if (format==1) {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else if (format==2) {

			pc(maxx,miny,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(maxx,miny,LIN);
			pc(minx,miny,LIN);

		}
	psout("clip\n");

	set_grey_internal(0.0);

	if (datascale_curr.zeroclass && !val) density=0.0;
	else if (datascale_curr.logarithmic)
		density=density_logarithmic(val,datascale_curr.dual,false);
	else {
		if (val<datascale_curr.minval) density=0.0;
		else if (datascale_curr.zeroclass) {
			maxval=datascale_curr.minval+(datascale_curr.nclass-1)*datascale_curr.interval;
			nclass=datascale_curr.nclass-1;
		}
		else {
			maxval=datascale_curr.minval+datascale_curr.nclass*datascale_curr.interval;
			nclass=datascale_curr.nclass;
		}

		if (val>maxval) density=1.0;
		else {
			density=(val-datascale_curr.minval)/(maxval-datascale_curr.minval);
		}

		// Density is now a continuous value -- round to nearest class

		if (double(int(density*(double)nclass))/(double)nclass!=density)
			density=double(int(density*(double)nclass)+1)/(double)nclass;

		//density=density*0.95+0.05;
	}

	// Plot dither pattern using filled circles

	width=pattern_style.spacing*density; //marker_style.swidth*density*0.25;
	psout("0 setlinewidth\n");

	y=miny-mod(miny,pattern_style.spacing*2.0);
	mult=0.0;
	while (y<=maxy+pattern_style.spacing) {
		x=minx-mod(minx,pattern_style.spacing);
		x+=pattern_style.spacing*mult;
		mult+=0.5;
		if (mult>=1.0) mult=0.0;
		while (x<=maxx+pattern_style.spacing) {
			point.h=x;
			point.v=y;
			if (width) shape_point(CIRCLE,point,width,0,1);
			x+=pattern_style.spacing;
		}
		y+=pattern_style.spacing;
	}

	psout("grestore\n");
}

void filldata_fast(double minx,double miny,double maxx,double maxy,double val) {

	// Fills a box with a colour pattern based on the specified value,
	// current datascale and current colourscale

	double density,red,green,blue,maxval,width,x,y,mult,nclass,sign;
	double above,below;
	colourscaledef colourscale;
	location point;
	int i,c,cabove,cbelow;

	psout("newpath\n");
	pc(minx,miny,MOV);
	pc(minx,maxy,LIN);
	pc(maxx,maxy,LIN);
	pc(maxx,miny,LIN);
	//pc(minx,miny,LIN);

	psout("clip\n");

	// If we have a current colour key table ...
	
	if (pcolourkey) {
		
		// First check if there is a class with this unique value
		
		for (i=0;i<ncolourkey;i++) {
			if (val==pcolourkey[i].minval && val==pcolourkey[i].maxval) {
				colour(pcolourkey[i].colour[0],pcolourkey[i].colour[1],
					pcolourkey[i].colour[2]); // assume RGB
				psout("fill\n");
				return;
			}
		}
	
		for (i=0;i<ncolourkey;i++) {
			if (val>=pcolourkey[i].minval && val<=pcolourkey[i].maxval) {
				colour(pcolourkey[i].colour[0],pcolourkey[i].colour[1],
					pcolourkey[i].colour[2]); // assume RGB
				//printf("colour(%g,%g,%g)\n",pcolourkey[i].colour[0],pcolourkey[i].colour[1],
				//	pcolourkey[i].colour[2]);
				psout("fill\n");
				return; // done it - escape this mess
			}
		}
		// Didn't find a class
		return;
	}
	else {
		
		// Datascale and colour scale ...
		
		if (!datascale_curr.dual) {
			if (datascale_curr.zeroclass && val==0.0) density=0.0;
			else if (datascale_curr.logarithmic)
				density=density_logarithmic(val,false,false);
			else {
				if (val<datascale_curr.minval)
					density=0.0;
				else {
					maxval=datascale_curr.minval+datascale_curr.nclass*datascale_curr.interval;
					nclass=datascale_curr.nclass;
				}
		
				if (val>maxval) density=1.0;
				else if (val>datascale_curr.minval) {
					density=(val-datascale_curr.minval)/(maxval-datascale_curr.minval);
				}
		
				// Density is now a continuous value -- round to nearest class
		
				//if (double(int(density*(double)nclass))/(double)nclass!=density)
				//	density=double(int(density*(double)nclass)+1)/(double)nclass;
				if (double(int(density*(double)nclass))/(double)nclass!=density)
					density=double(int(density*(double)nclass))/(double)(nclass-1);
		
				if (density==0.0) density=0.00001;
			}
		}
		else { // dual data scale (with different colour scales for above and below zero
			if (datascale_curr.zeroclass && val==0.0) density=0.0;
			else if (datascale_curr.logarithmic)
				density=density_logarithmic(val,true,false);
			else {
				if (val<datascale_curr.minval)
					density=0.0;
				else {
					maxval=datascale_curr.minval+datascale_curr.nclass*datascale_curr.interval;
				}
	
				if (val<0.0)
					nclass=int(-datascale_curr.minval/datascale_curr.interval+0.0001);
				else
					nclass=int(maxval/datascale_curr.interval+0.0001);
	
				if (val>maxval) density=1.0;
				else if (val<datascale_curr.minval) density=-1.0;
				else if (val==0.0) density=0.0001;
				else if (val>0.0) density=val/maxval;
				else // val<0.0
					density=-val/datascale_curr.minval;
				
				// density now in range -1 to 0 (for negative colour scale)
				// or 0 to 1 (for positive colour scale)
				
				// Density is now a continuous value -- round to nearest class
				
				if (density<0.0) {
					sign=-1.0;
					density=-density;
				}
				else sign=1.0;
				
				if (nclass==1) density=1.0;
				else if (double(int(density*(double)nclass))/(double)nclass!=density)
					density=double(int(density*(double)nclass))/(double)(nclass-1);
		
				if (density==0.0) density=0.00001;
				
				density*=sign;
			}
		}
	}

	if (density==0.0) {

		red=1.0;
		green=1.0;
		blue=1.0;
	}
	else {

		if (density<0.0) {
			colourscale=colourscale_curr_dual;
			density=-density;
		}
		else colourscale=colourscale_curr;
		
		if (colourscale.pflexcolours) {
		
			c=0;
			cabove=cbelow=-1;
			while (colourscale.pflexcolours[c].offset>=0.0) {
						
				if (colourscale.pflexcolours[c].offset<density) {
					if (cbelow>=0) {
						if (colourscale.pflexcolours[c].offset>below) {
							cbelow=c;
							below=colourscale.pflexcolours[c].offset;
						}
					}
					else {
						cbelow=c;
						below=colourscale.pflexcolours[c].offset;
					}
				}
				else if (colourscale.pflexcolours[c].offset>=density) {
					if (cabove>=0) {
						if (colourscale.pflexcolours[c].offset<above) {
							cabove=c;
							above=colourscale.pflexcolours[c].offset;
						}
					}
					else {
						cabove=c;
						above=colourscale.pflexcolours[c].offset;
					}
				}
				c++;
			}
						
			if (cbelow>=0) {
				if (cabove>=0) {
					mult=(density-below)/(above-below);
					red=colourscale.pflexcolours[cbelow].red+mult*
						(colourscale.pflexcolours[cabove].red-colourscale.pflexcolours[cbelow].red);
					green=colourscale.pflexcolours[cbelow].green+mult*
						(colourscale.pflexcolours[cabove].green-colourscale.pflexcolours[cbelow].green);
					blue=colourscale.pflexcolours[cbelow].blue+mult*
						(colourscale.pflexcolours[cabove].blue-colourscale.pflexcolours[cbelow].blue);
				}
				else {
					red=colourscale.pflexcolours[cbelow].red;
					green=colourscale.pflexcolours[cbelow].green;
					blue=colourscale.pflexcolours[cbelow].blue;
				}
			}
			else if (cabove>=0) {
				red=colourscale.pflexcolours[cabove].red;
				green=colourscale.pflexcolours[cabove].green;
				blue=colourscale.pflexcolours[cabove].blue;			
			}
			else {
				red=0.0;
				green=0.0;
				blue=0.0;
			}
		
		}
	}

	colour(red,green,blue);

	psout("fill\n");
}

void filldata_colour(double minx,double miny,double maxx,double maxy,double val,int format) {
	// Fills a box with a colour pattern based on the specified value,
	// current datascale and current colourscale
	// format= 0  =  box
	//         1  =  upper-left triangle
	//         2  =  lower-right triangle

	double density,red,green,blue,maxval,width,x,y,mult,nclass,sign;
	double above,below;
	colourscaledef colourscale;
	location point;
	int i,c,cabove,cbelow;

	// if not transparent, first wipe out all ink underneath

	if (!transp) {
		psout("gsave\n");
		set_grey_internal(1.0);
		psout("newpath\n");
		pc(minx,miny,MOV);
		if (format==1) {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else if (format==2) {

			pc(maxx,miny,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(maxx,miny,LIN);
			pc(minx,miny,LIN);

		}

		psout("fill\n");
		psout("grestore\n");
	}

	psout("gsave\n");
	psout("newpath\n");
	pc(minx,miny,MOV);
		if (format==1) {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else if (format==2) {

			pc(maxx,miny,LIN);
			pc(maxx,maxy,LIN);
			pc(minx,miny,LIN);

		}
		else {

			pc(minx,maxy,LIN);
			pc(maxx,maxy,LIN);
			pc(maxx,miny,LIN);
			pc(minx,miny,LIN);

		}
	psout("clip\n");

	// If we have a current colour key table ...
	
	if (pcolourkey) {
		
		// First check if there is a class with this unique value
		
		for (i=0;i<ncolourkey;i++) {
			if (val==pcolourkey[i].minval && val==pcolourkey[i].maxval) {
				colour(pcolourkey[i].colour[0],pcolourkey[i].colour[1],
					pcolourkey[i].colour[2]); // assume RGB
				fill(1);
				psout("grestore\n");
				colour();
				return;
			}
		}
	
		for (i=0;i<ncolourkey;i++) {
			if (val>=pcolourkey[i].minval && val<=pcolourkey[i].maxval) {
				colour(pcolourkey[i].colour[0],pcolourkey[i].colour[1],
					pcolourkey[i].colour[2]); // assume RGB
				//printf("colour(%g,%g,%g)\n",pcolourkey[i].colour[0],pcolourkey[i].colour[1],
				//	pcolourkey[i].colour[2]);
				fill(1);
				psout("grestore\n");
				colour();
				return; // done it - escape this mess
			}
		}
		// Didn't find a class
		psout("grestore\n");
		return;
	}
	else {
		
		// Datascale and colour scale ...
		
		if (!datascale_curr.dual) {
			if (datascale_curr.zeroclass && val==0.0) density=0.0;
			else if (datascale_curr.logarithmic)
				density=density_logarithmic(val,false,false);
			else {
				if (val<datascale_curr.minval)
					density=0.0;
				else {
					maxval=datascale_curr.minval+datascale_curr.nclass*datascale_curr.interval;
					nclass=datascale_curr.nclass;
				}
		
				if (val>maxval) density=1.0;
				else if (val>datascale_curr.minval) {
					density=(val-datascale_curr.minval)/(maxval-datascale_curr.minval);
				}
		
				// Density is now a continuous value -- round to nearest class
		
				//if (double(int(density*(double)nclass))/(double)nclass!=density)
				//	density=double(int(density*(double)nclass)+1)/(double)nclass;
				if (double(int(density*(double)nclass))/(double)nclass!=density)
					density=double(int(density*(double)nclass))/(double)(nclass-1);
		
				if (density==0.0) density=0.00001;
			}
		}
		else { // dual data scale (with different colour scales for above and below zero
			if (datascale_curr.zeroclass && val==0.0) density=0.0;
			else if (datascale_curr.logarithmic)
				density=density_logarithmic(val,true,false);
			else {
				if (val<datascale_curr.minval)
					density=0.0;
				else {
					maxval=datascale_curr.minval+datascale_curr.nclass*datascale_curr.interval;
				}
	
				if (val<0.0)
					nclass=int(-datascale_curr.minval/datascale_curr.interval+0.0001);
				else
					nclass=int(maxval/datascale_curr.interval+0.0001);
	
				if (val>maxval) density=1.0;
				else if (val<datascale_curr.minval) density=-1.0;
				else if (val==0.0) density=0.0001;
				else if (val>0.0) density=val/maxval;
				else // val<0.0
					density=-val/datascale_curr.minval;
				
				// density now in range -1 to 0 (for negative colour scale)
				// or 0 to 1 (for positive colour scale)
				
				// Density is now a continuous value -- round to nearest class
				
				if (density<0.0) {
					sign=-1.0;
					density=-density;
				}
				else sign=1.0;
				
				if (nclass==1) density=1.0;
				else if (double(int(density*(double)nclass))/(double)nclass!=density)
					density=double(int(density*(double)nclass))/(double)(nclass-1);
		
				if (density==0.0) density=0.00001;
				
				density*=sign;
			}
		}
	}

	if (density==0.0) {

		red=1.0;
		green=1.0;
		blue=1.0;
	}
	else {

		if (density<0.0) {
			colourscale=colourscale_curr_dual;
			density=-density;
		}
		else colourscale=colourscale_curr;
		
		if (colourscale.pflexcolours) {
		
			c=0;
			cabove=cbelow=-1;
			while (colourscale.pflexcolours[c].offset>=0.0) {
						
				if (colourscale.pflexcolours[c].offset<density) {
					if (cbelow>=0) {
						if (colourscale.pflexcolours[c].offset>below) {
							cbelow=c;
							below=colourscale.pflexcolours[c].offset;
						}
					}
					else {
						cbelow=c;
						below=colourscale.pflexcolours[c].offset;
					}
				}
				else if (colourscale.pflexcolours[c].offset>=density) {
					if (cabove>=0) {
						if (colourscale.pflexcolours[c].offset<above) {
							cabove=c;
							above=colourscale.pflexcolours[c].offset;
						}
					}
					else {
						cabove=c;
						above=colourscale.pflexcolours[c].offset;
					}
				}
				c++;
			}
						
			if (cbelow>=0) {
				if (cabove>=0) {
					mult=(density-below)/(above-below);
					red=colourscale.pflexcolours[cbelow].red+mult*
						(colourscale.pflexcolours[cabove].red-colourscale.pflexcolours[cbelow].red);
					green=colourscale.pflexcolours[cbelow].green+mult*
						(colourscale.pflexcolours[cabove].green-colourscale.pflexcolours[cbelow].green);
					blue=colourscale.pflexcolours[cbelow].blue+mult*
						(colourscale.pflexcolours[cabove].blue-colourscale.pflexcolours[cbelow].blue);
				}
				else {
					red=colourscale.pflexcolours[cbelow].red;
					green=colourscale.pflexcolours[cbelow].green;
					blue=colourscale.pflexcolours[cbelow].blue;
				}
			}
			else if (cabove>=0) {
				red=colourscale.pflexcolours[cabove].red;
				green=colourscale.pflexcolours[cabove].green;
				blue=colourscale.pflexcolours[cabove].blue;			
			}
			else {
				red=0.0;
				green=0.0;
				blue=0.0;
			}
		
		}
	}

	colour(red,green,blue);

	fill(1);

	psout("grestore\n");

	colour();
}

void filldata(double minx,double miny,double maxx,double maxy,double val) {

	if (!colourscale_curr.pflexcolours)
		filldata_dither(minx,miny,maxx,maxy,val,0);
	else
		filldata_colour(minx,miny,maxx,maxy,val,0);
}


void filldata(double minx,double miny,double maxx,double maxy,double val,int format) {

	if (!colourscale_curr.pflexcolours)
		filldata_dither(minx,miny,maxx,maxy,val,format);
	else
		filldata_colour(minx,miny,maxx,maxy,val,format);
}


void fillpoly(int n,location* clippath,int outline,patterndef patternstyle) {

	// uses current fill pattern settings to plot and fill a polygon
	// whose outline is given by clippath
	// line of current width used to trace outline if outline=1

	int i;
	double maxx,maxy,minx,miny;
	location border[MAXBGPOINT];
	linedef linestyle;

	linestyle.lclass=SOLID;

	psout("gsave\n");
	psout("newpath\n");
	pc(clippath[0].h,clippath[0].v,MOV);
	for (i=1;i<n;i++) {
		pc(clippath[i].h,clippath[i].v,LIN);
	}
	psout("clip\n");

	// now fill it

	minx=1E10;
	miny=1E10;
	maxx=0.0;
	maxy=0.0;
	for (i=0;i<n;i++) {
		if (clippath[i].h<minx) minx=clippath[i].h;
		if (clippath[i].v<miny) miny=clippath[i].v;
		if (clippath[i].h>maxx) maxx=clippath[i].h;
		if (clippath[i].v>maxy) maxy=clippath[i].v;
	}
	fillbox(minx-patternstyle.spacing,miny-patternstyle.spacing,
		maxx+patternstyle.spacing,maxy+patternstyle.spacing,patternstyle);
	psout("grestore\n");

	// outline it

	if (outline) {
		if (n>=MAXBGPOINT) n=MAXBGPOINT-1;
		for (i=0;i<n;i++) {
			border[i].h=clippath[i].h;
			border[i].v=clippath[i].v;
		}
		border[i].h=clippath[0].h;
		border[i].v=clippath[0].v;
		curve_point(i,border,linestyle);
	}

	return;
}


static void sortupx(int n,location* array) {

	// sorts an array of (x,y) coordinates ascending for the x's

	char flag;
	location mem;
	int i;

	do {
		flag=0;
		for (i=0;i<n-1;i++)
			if (array[i].h>array[i+1].h) {
				mem.h=array[i].h;
				mem.v=array[i].v;
				array[i].h=array[i+1].h;
				array[i].v=array[i+1].v;
				array[i+1].h=mem.h;
				array[i+1].v=mem.v;
				flag=1;
			}
	}
	while (flag);
	return;

}



static void sortupy(int n,location* array) {

	// sorts an array of (x,y) coordinates ascending for the y's

	char flag;
	location mem;
	int i;

	do {
		flag=0;
		for (i=0;i<n-1;i++)
			if (array[i].v>array[i+1].v) {
				mem.h=array[i].h;
				mem.v=array[i].v;
				array[i].h=array[i+1].h;
				array[i].v=array[i+1].v;
				array[i+1].h=mem.h;
				array[i+1].v=mem.v;
				flag=1;
			}
	}
	while (flag);
	return;

}


static double vintersection(double x,location* segment,int index) {

	// returns y value at intersection between a vertical line at x
	// and the segment between the first two points in segment

	double m;
	double c;

	if (segment[index].h==segment[index+1].h) return segment[index].v;
	m=(segment[index+1].v-segment[index].v)/(segment[index+1].h-segment[index].h);
	c=segment[index].v-m*segment[index].h;
	return m*x+c;

}


static double hintersection(double y,location* segment,int index) {

	// returns x value at intersection between a horizontal line at y
	// and the segment between the first two points in segment

	double m;
	double c;

	if (segment[index].h==segment[index+1].h) return segment[index].h;
	m=(segment[index+1].v-segment[index].v)/(segment[index+1].h-segment[index].h);
	c=segment[index].v-m*segment[index].h;
	return (y-c)/m;

}


void vareabetween(int n1,location* curve1,int n2,
	location* curve2,patterndef patternstyle) {

	// finds and fills the area between two curves sorted sequentially along
	// the horizontal axis

	// first sort to correct order (in case not already): lowest
	// to highest along horizontal axis

	location min1,min2,max1,max2;
	location clippath[MAXBGPOINT];							// to store clip path for shape
	int curvptr=0;
	int clipptr=0;

	sortupx(n1,curve1);

	sortupx(n2,curve2);

	if (curve1[0].h>=curve2[n2-1].h || curve2[0].h>=curve1[n1-1].h) return;

						// nothing to do if no overlap

	if (curve1[0].h>curve2[0].h) {		// start from left end of curve1

		clippath[clipptr].h=curve1[0].h;
		clippath[clipptr++].v=curve1[0].v;

		// now find intersecting segment on curve2

		while (curve2[curvptr].h<=curve1[0].h) curvptr++;
		clippath[clipptr].h=curve1[0].h;
		clippath[clipptr++].v=vintersection(curve1[0].h,curve2,curvptr-1);
		while (curve2[curvptr].h<=curve1[n1-1].h && curvptr<n2) {  // while they overlap
			clippath[clipptr].h=curve2[curvptr].h;
			clippath[clipptr++].v=curve2[curvptr++].v;
		}
		if (curvptr==n2) {	// end of curve 2 reached
			// find intersected segment on curve 1
			curvptr=n1-1;
			while(curve1[curvptr].h>=curve2[n2-1].h) curvptr--;
			// find intersection
			clippath[clipptr].h=curve2[n2-1].h;
			clippath[clipptr++].v=vintersection(curve2[n2-1].h,curve1,curvptr);
		}
		else {  // beyond end of curve1, so find intersection point
			clippath[clipptr].h=curve1[n1-1].h;
			clippath[clipptr++].v=vintersection(curve1[n1-1].h,curve2,curvptr-1);
			curvptr=n1-1;
		}
		while(curvptr>=0) {
			clippath[clipptr].h=curve1[curvptr].h;
			clippath[clipptr++].v=curve1[curvptr--].v;
		}
	}
	else {   // start from left end of curve2

		clippath[clipptr].h=curve2[0].h;
		clippath[clipptr++].v=curve2[0].v;

		// now find intersecting segment on curve2

		while (curve1[curvptr].h<=curve2[0].h) curvptr++;
		clippath[clipptr].h=curve2[0].h;
		clippath[clipptr++].v=vintersection(curve2[0].h,curve1,curvptr-1);
		while (curve1[curvptr].h<=curve2[n2-1].h && curvptr<n1) {  // while they overlap
			clippath[clipptr].h=curve1[curvptr].h;
			clippath[clipptr++].v=curve1[curvptr++].v;
		}
		if (curvptr==n1) {	// end of curve 2 reached
			// find intersected segment on curve 1
			curvptr=n2-1;
			while(curve2[curvptr].h>=curve1[n1-1].h) curvptr--;
			// find intersection
			clippath[clipptr].h=curve1[n1-1].h;
			clippath[clipptr++].v=vintersection(curve1[n1-1].h,curve2,curvptr);
		}
		else {  // beyond end of curve1, so find intersection point
			clippath[clipptr].h=curve2[n2-1].h;
			clippath[clipptr++].v=vintersection(curve2[n2-1].h,curve1,curvptr-1);
			curvptr=n2-1;
		}
		while(curvptr>=0) {
			clippath[clipptr].h=curve2[curvptr].h;
			clippath[clipptr++].v=curve2[curvptr--].v;
		}
	}

	// trace and fill shape

	fillpoly(clipptr,clippath,0,patternstyle);

	return;

}



void hareabetween(int n1,location* curve1,int n2,
	location* curve2,patterndef patternstyle) {

	// finds and fills the area between two curves sorted sequentially along
	// the vertical axis

	// first sort to correct order (in case not already): lowest
	// to highest along vertical axis
		
	location min1,min2,max1,max2;
	location clippath[MAXBGPOINT];							// to store clip path for shape
	int curvptr=0;
	int clipptr=0;

	sortupy(n1,curve1);
	sortupy(n2,curve2);

	if (curve1[0].v>=curve2[n2-1].v || curve2[0].v>=curve1[n1-1].v) return;

						// nothing to do if no overlap

	if (curve1[0].v>curve2[0].v) {		// start from left end of curve1

		clippath[clipptr].h=curve1[0].h;
		clippath[clipptr++].v=curve1[0].v;

		// now find intersecting segment on curve2

		while (curve2[curvptr].v<=curve1[0].v) curvptr++;
		clippath[clipptr].v=curve1[0].v;
		clippath[clipptr++].h=hintersection(curve1[0].v,curve2,curvptr-1);
		while (curve2[curvptr].v<=curve1[n1-1].v && curvptr<n2) {  // while they overlap
			clippath[clipptr].h=curve2[curvptr].h;
			clippath[clipptr++].v=curve2[curvptr++].v;
		}
		if (curvptr==n2) {	// end of curve 2 reached
			// find intersected segment on curve 1
			curvptr=n1-1;
			while(curve1[curvptr].v>=curve2[n2-1].v) curvptr--;
			// find intersection
			clippath[clipptr].v=curve2[n2-1].v;
			clippath[clipptr++].h=hintersection(curve2[n2-1].v,curve1,curvptr);
		}
		else {  // beyond end of curve1, so find intersection point
			clippath[clipptr].v=curve1[n1-1].v;
			clippath[clipptr++].h=hintersection(curve1[n1-1].v,curve2,curvptr-1);
			curvptr=n1-1;
		}
		while(curvptr>=0) {
			clippath[clipptr].h=curve1[curvptr].h;
			clippath[clipptr++].v=curve1[curvptr--].v;
		}

	}
	else {   // start from left end of curve2

		clippath[clipptr].h=curve2[0].h;
		clippath[clipptr++].v=curve2[0].v;

		// now find intersecting segment on curve2

		while (curve1[curvptr].v<=curve2[0].v) curvptr++;
		clippath[clipptr].v=curve2[0].v;
		clippath[clipptr++].h=hintersection(curve2[0].v,curve1,curvptr-1);
		while (curve1[curvptr].v<=curve2[n2-1].v && curvptr<n1) {  // while they overlap
			clippath[clipptr].h=curve1[curvptr].h;
			clippath[clipptr++].v=curve1[curvptr++].v;
		}
		if (curvptr==n1) {	// end of curve 2 reached
			// find intersected segment on curve 1
			curvptr=n2-1;
			while(curve2[curvptr].v>=curve1[n1-1].v) curvptr--;
			// find intersection
			clippath[clipptr].v=curve1[n1-1].v;
			clippath[clipptr++].h=hintersection(curve1[n1-1].v,curve2,curvptr);
		}
		else {  // beyond end of curve1, so find intersection point
			clippath[clipptr].v=curve2[n2-1].v;
			clippath[clipptr++].h=hintersection(curve2[n2-1].v,curve1,curvptr-1);
			curvptr=n2-1;
		}
		while(curvptr>=0) {
			clippath[clipptr].h=curve2[curvptr].h;
			clippath[clipptr++].v=curve2[curvptr--].v;
		}

	}

	// trace and fill shape

	fillpoly(clipptr,clippath,0,patternstyle);
	return;

}

void set_grey_internal(double val) {
	
	// internal version of set_grey that does not change the default value
	
	// convert to set colour if user has specified a colour

	long colour_style_bak;

	if (colour_style) {
		colour_style_bak=colour_style;
		double red=(double)(colour_style>>16)/255.0;
		double green=(double)((colour_style&0x00FF00)>>8)/255.0;
		double blue=(double)(colour_style&0x0000FF)/255.0;
		adjust_lightness(red,green,blue,val);
		colour(red,green,blue);
		colour_style=colour_style_bak;
	}
	else {
		pc(val);
		psout(" setgray\n");
	}
}


double set_grey(double val) {

	// sets grey, returning old value

	double old;
	old=grey;
	grey=val;

	set_grey_internal(val);
	return old;
}


double set_linewidth(double val) {

	double old;
	old=lwd;
	lwd=val;
	pc(val);
	psout(" setlinewidth\n");
	return old;

}


int set_transparent(int val) {

	int old;
	old=transp;
	transp=val;
	return old;

}

int bgsprint(int length,char* string,double var,int width,int places,int exp) {

	// always prints in E format

	char buf[50],buf2[15],*bufptr,*strptr;
	int flag;

	if (exp) sprintf(buf2,"%%%d.%dE",width,places);
	else if (width || places) sprintf(buf2,"%%%d.%df",width,places);
	else sprintf(buf2,"%%G");
	sprintf(buf,buf2,var);

	if (length-1<strlen(buf)) return 0;

  // check if "-0.000E+12" or something silly like that

	bufptr=buf;
	while (*bufptr==' ') bufptr++;
	if (*bufptr=='-') { // check if only zero digits to the E
		flag=0;
		while (*bufptr!='E' && *bufptr) {
			if (*bufptr>='1' && *bufptr<='9') flag=1;  //a non-zero digit
			bufptr++;
		}
		if (!flag) return bgsprint(length,string,0.0,width,places,exp);  //rerun with var=exactly 0
	}
	bufptr=buf;
	if (*bufptr=='-') {
		strcpy(string,"!C(45)");
		strptr=string+strlen(string);
		bufptr++;
	}
	else strptr=string;
	*strptr=0;
	while (*bufptr!='E' && *bufptr) *strptr++=*bufptr++;
	if (*bufptr=='E') exp=1;
	*strptr=0;
	if (exp) {
		strcat(string,"!C(180)10!^");
		strptr=string+strlen(string);
		bufptr++;
		if (*bufptr=='+') bufptr++;
		else {
			*strptr=0;
			strcat(string,"!C(45)");
			strptr=string+strlen(string);
			bufptr++;   // get the "-" sign
		}
		while (*bufptr=='0') bufptr++;
		flag=0;
		while (*bufptr) {
			*strptr++=*bufptr++;
			flag=1;
		}
		if (!flag) *strptr++='0';
		*strptr=0;
	}
	return 1;

}


int bgsprint(int length,char* string,double var,
	int width,int places) {

	// assumes EXP format wanted

	return bgsprint(length,string,var,width,places,1);

}


int setxls(int width,int places,int exp,labeldef& xlabel_style) {

	// sets x-axis label style parameters

	xlabel_style.width=width;
	xlabel_style.places=places;
	xlabel_style.exp=exp;
	return 1;

}


int setyls(int width,int places,int exp,labeldef& ylabel_style) {

	// sets y-axis label style parameters

	ylabel_style.width=width;
	ylabel_style.places=places;
	ylabel_style.exp=exp;
	return 1;

}


int autoscalex(double minx,double maxx,double round,int fixrange,
	axisdef& xaxispars,xypair& minortic,xypair& autotic,
	location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& xlabel_style) {

	// automatically scales graph area in x direction
	// if zerox non-zero (TRUE) and minx>=0, x origin set to 0
	// if round<10 used as a criterion for rounding (e.g., if
	// round=2, rounding is to nearest 0.002, 0.2, 2, 20 etc.
	// if round>=10, an internal rule is used instead

	if (xaxispars.zero && !xaxispars.log) {
		if (minx<=0.0 && maxx<=0.0) maxx=0.0;
		else if (minx>=0.0 && maxx>=0.0) minx=0.0;
	}

	graphscale.isgeographic=false;

	double unit,gmax,gmin,minorunit;
	double interval=maxx-minx;
	double mord;

	if (!interval) {
		if (maxx)
			interval=pow(10.0,floor(log10(fabs(maxx))));
		else
			interval=1.0;
		maxx+=interval*0.5;
		minx-=interval*0.5;
	}

	if (xaxispars.log) {
		autotic.x=1;
		if (interval>=MAXBGAUTOTIC) minortic.x=0.0;
		else minortic.x=1.0; // between log 1.0 and log 10.0
	}
	else {
		mord=floor(log10(interval));
		interval/=pow(10.0,mord);	// now in the range 1.0-9.9999

		if (round>=10.0) {
			unit=0.2;
			if (interval/unit>MAXBGAUTOTIC) unit=0.3;
			if (interval/unit>MAXBGAUTOTIC) unit=0.5;
			if (interval/unit>MAXBGAUTOTIC) unit=1.0;
			if (interval/unit>MAXBGAUTOTIC) unit=2.0;
		}
		else {
			unit=round;
			if (interval/unit<5.0) unit/=10.0;
		}

		if (unit==0.3 || unit==0.5) minorunit=0.1;
		else if (unit==3.0 || unit==5.0) minorunit=1.0;
		else minorunit=unit/2.0;

		// now unit*10^mord is the automatic tic interval

		autotic.x=unit*pow(10.0,mord);
		minortic.x=minorunit*pow(10.0,mord);
	}

	if (fixrange) {
		gmax=maxx;
		gmin=minx;
	}
	else {
		gmax=floor(maxx/autotic.x+0.999999)*autotic.x;
		gmin=floor(minx/autotic.x+0.000001)*autotic.x;
	}

	if (axinverse) {
		graphscale.mult.x=(graphtr.v-graphbl.v)/(gmax-gmin);
		graphscale.orig.x=graphbl.v-gmin*graphscale.mult.x;
	}
	else {
		graphscale.mult.x=(graphtr.h-graphbl.h)/(gmax-gmin);
		graphscale.orig.x=graphbl.h-gmin*graphscale.mult.x;
	}
	
	// For bengraph_geog
	graphscale.min=gmin;
	graphscale.max=gmax;
	
	if (autotic.x>999999.0) return setxls(0,1,1,xlabel_style);
	if (autotic.x>=1.0) return setxls(0,0,0,xlabel_style);
	if (autotic.x>=0.09999) return setxls(0,1,0,xlabel_style);
	if (autotic.x>=0.009999) return setxls(0,2,0,xlabel_style);
	if (autotic.x>=0.0009999) return setxls(0,3,0,xlabel_style);
	if (autotic.x>=0.00009999) return setxls(0,4,0,xlabel_style);
	return setxls(0,1,1,xlabel_style);

}


int autoscaley(double miny,double maxy,double round,int fixrange,
	axisdef& yaxispars,xypair& minortic,xypair& autotic,
	location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& ylabel_style) {

	graphscale.isgeographic=false;

	if (yaxispars.zero && !yaxispars.log) {
		if (miny<=0.0 && maxy<=0.0) maxy=0.0;
		else if (miny>=0.0 && maxy>=0.0) miny=0.0;
	}

	double unit,gmax,gmin,minorunit;
	double interval=maxy-miny;
	double mord;
	
	if (!interval) {
		if (maxy)
			interval=pow(10.0,floor(log10(fabs(maxy))));
		else
			interval=1.0;
		maxy+=interval*0.5;
		miny-=interval*0.5;
	}

  if (!interval) interval=1.0;

	if (yaxispars.log) {
		autotic.y=1;
		if (interval>=MAXBGAUTOTIC) minortic.y=0.0;
		else minortic.y=1.0; // between log 1.0 and log 10.0
	}
	else {
		mord=floor(log10(interval));
		interval/=pow(10.0,mord);	// now in the range 1.0-9.9999
		if (round>=10.0) {
			unit=0.2;
			if (interval/unit>MAXBGAUTOTIC) unit=0.3;
			if (interval/unit>MAXBGAUTOTIC) unit=0.5;
			if (interval/unit>MAXBGAUTOTIC) unit=1.0;
			if (interval/unit>MAXBGAUTOTIC) unit=2.0;
		}
		else {
			unit=round;
			if (interval/unit<5.0) unit/=10.0;
		}

		if (unit==0.3 || unit==0.5) minorunit=0.1;
		else if (unit==3.0 || unit==5.0) minorunit=1.0;
		else minorunit=unit/2.0;

		// now unit*10^mord is the automatic tic interval

		autotic.y=unit*pow(10.0,mord);
		minortic.y=minorunit*pow(10.0,mord);
	}

        if (fixrange) {
		gmax=maxy;
		gmin=miny;
	}
	else {
		gmax=floor(maxy/autotic.y+0.999999)*autotic.y;
		gmin=floor(miny/autotic.y+0.000001)*autotic.y;
	}


	if (axinverse) {
		graphscale.mult.y=(graphtr.h-graphbl.h)/(gmax-gmin);
		graphscale.orig.y=graphbl.h-gmin*graphscale.mult.y;
	}
	else {
		graphscale.mult.y=(graphtr.v-graphbl.v)/(gmax-gmin);
		graphscale.orig.y=graphbl.v-gmin*graphscale.mult.y;
	}

	if (autotic.y>999999.0) return setyls(0,1,1,ylabel_style);
	if (autotic.y>=1.0) return setyls(0,0,0,ylabel_style);
	if (autotic.y>=0.1) return setyls(0,1,0,ylabel_style);
	if (autotic.y>=0.01) return setyls(0,2,0,ylabel_style);
	if (autotic.y>=0.001) return setyls(0,3,0,ylabel_style);
	if (autotic.y>=0.0001) return setyls(0,4,0,ylabel_style);
	return setyls(0,1,1,ylabel_style);

}


int reload(int id,int& nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo,FILE*& tdf,int& dataopen) {

	// attempts to reload a data series with given id from
	// temporary file

	/* Error handling: 0 no error
										 1 temporary data file not open
										 2 specified data series not found */

	long length;
	int fid;
	double dumdub;

	if (!dataopen) return 1;

	fseek(tdf,0,SEEK_SET);  // to start of file
	fread(&length,sizeof(length),1,tdf);
	fread(&fid,sizeof(fid),1,tdf);
	while (id!=fid && !feof(tdf)) {
		fseek(tdf,length,SEEK_CUR);	// to next record
		fread(&length,sizeof(length),1,tdf);
		fread(&fid,sizeof(fid),1,tdf);
	}
	if (id!=fid) return 2;

	fread(&nitem,sizeof(nitem),1,tdf);
	length=(long)nitem*(long)sizeof(dumdub);
	fread(xdata,length,1,tdf);
	fread(ydata,length,1,tdf);
	fread(errorhi,length,1,tdf);
	fread(errorlo,length,1,tdf);

	return 0;

}

double mercator(double lat) {

	// Returns raw Mercator projection value corresponding to lat in degrees

	double latr=lat*PI/180.0;

	return 0.5*log((1.0+sin(latr))/(1.0-sin(latr)));
}


double gtopx(double g,scaledef& graphscale) {

	// given a graph x value, returns the current page unit value

	if (graphscale.isgeographic) {
		//if (g<graphscale.westbound) g+=360.0;
	}

	return g*graphscale.mult.x+graphscale.orig.x;

}


double gtopy(double g,scaledef& graphscale) {

	if (graphscale.isgeographic) g=mercator(g);

	return g*graphscale.mult.y+graphscale.orig.y;

}


double ptogx(double p,scaledef& graphscale) {

	if (graphscale.isgeographic) {
		double rval=(p-graphscale.orig.x)/graphscale.mult.x;
		if (rval>360.0) rval-=360.0;
		return rval;
	}

	return (p-graphscale.orig.x)/graphscale.mult.x;

}


double ptogy(double p,scaledef& graphscale) {

	if (graphscale.isgeographic) {
		double v=(p-graphscale.orig.y)/graphscale.mult.y;
		 // Inverse Mercator
		double vr=2*atan(exp(v))-0.5*PI;
		return vr*180.0/PI;
	}

	return (p-graphscale.orig.y)/graphscale.mult.y;

}


double gtopx(double p) {

	return gtopx(p,graphscale);
}

double gtopy(double p) {

	return gtopy(p,graphscale);
}

double ptogx(double p) {

	return ptogx(p,graphscale);
}

double ptogy(double p) {

	return ptogy(p,graphscale);
}


static double ptops(double p,double pagescale) {

	// given a page x value, returns the current ps point value

	return p*pagescale;

}

static double pstop(double ps,double pagescale) {

	return ps/pagescale;

}


int fillint(int id1,int id2,int toaxis,
	scaledef& graphscale,double pagescale,int& userpattern,FILE*& tdf,
	int& dataopen,int& axinverse,double& mindx,double& maxdx,
	legenddef& legend_style,patterndef& pattern_style) {

	// fills in area between specified curves
	// if toaxis non-zero, y=0 assumed

	/* Error handling: inherited from reload */

	location data1[MAXBGPOINT],data2[MAXBGPOINT];
	double xdata[MAXBGPOINT];
	double ydata[MAXBGPOINT];
	double error[MAXBGPOINT];
	int i,notok,nitem1,nitem2;
	notok=reload(id1,nitem1,xdata,ydata,error,error,tdf,dataopen);
	if (notok) return notok;
	if (axinverse)
		for (i=0;i<nitem1;i++) {
			data1[i].v=ptops(gtopx(xdata[i],graphscale),pagescale);
			data1[i].h=ptops(gtopy(ydata[i],graphscale),pagescale);
		}
	else
		for (i=0;i<nitem1;i++) {
			data1[i].h=ptops(gtopx(xdata[i],graphscale),pagescale);
			data1[i].v=ptops(gtopy(ydata[i],graphscale),pagescale);
		}
	if (toaxis) {
		if (axinverse) {
			data2[0].v=ptops(gtopx(mindx,graphscale),pagescale);
			data2[1].v=ptops(gtopx(maxdx,graphscale),pagescale);
			data2[0].h=data2[1].h=ptops(gtopy(0.0,graphscale),pagescale);
			nitem2=2;
		}
		else {
			data2[0].h=ptops(gtopx(mindx,graphscale),pagescale);
			data2[1].h=ptops(gtopx(maxdx,graphscale),pagescale);
			data2[0].v=data2[1].v=ptops(gtopy(0.0,graphscale),pagescale);
			nitem2=2;
		}
	}
	else {
		notok=reload(id2,nitem2,xdata,ydata,error,error,tdf,dataopen);
		if (notok) return notok;
		if (axinverse)
			for (i=0;i<nitem2;i++) {
				data2[i].v=ptops(gtopx(xdata[i],graphscale),pagescale);
				data2[i].h=ptops(gtopy(ydata[i],graphscale),pagescale);
			}
		else
			for (i=0;i<nitem2;i++) {
				data2[i].h=ptops(gtopx(xdata[i],graphscale),pagescale);
				data2[i].v=ptops(gtopy(ydata[i],graphscale),pagescale);
			}
	}

	if (axinverse) hareabetween(nitem1,data1,nitem2,data2,pattern_style);
	else {
		vareabetween(nitem1,data1,nitem2,data2,pattern_style);
	}
	legend_style.flags|=PATTERN;
	return 0;

}


int logprint(char* labtext,double ctic,int exp) {

	// special print formats for log-axis labels

	double label=pow(10.0,ctic);
	if (ctic>5.9999 || ctic<=-4.0001 || exp)
		return bgsprint(30,labtext,label,0,0,1);
	if (ctic>-0.00001) return bgsprint(30,labtext,label,0,0,0);
	if (ctic>-1.0001) return bgsprint(30,labtext,label,0,1,0);
	if (ctic>-2.0001) return bgsprint(30,labtext,label,0,2,0);
	if (ctic>-3.0001) return bgsprint(30,labtext,label,0,3,0);
	if (ctic>-4.0001) return bgsprint(30,labtext,label,0,4,0);
	return 0;

}


void dprint(char* buffer,double dval,int scale,int& usernumformat,
	numformat& num_style) {

	// internal function to print a double number to a string buffer

	double size;
	int places;
	if (!usernumformat && scale) {
		if (!dval) size=0;
		else size=floor(log10(fabs(dval)));
		if (size>-6.0 && size<2.0) {
			places=(int)fabs(size-2.0);
			bgsprint(50,buffer,dval,0,places,0);
		}
		else if (size>=2.0 && size<5.0) bgsprint(50,buffer,dval,0,0,0);
		else bgsprint(50,buffer,dval,0,2,1);
	}
	else if (scale) bgsprint(50,buffer,dval,num_style.width,num_style.places,
		num_style.exp);
  else bgsprint(50,buffer,dval,0,0,0);
	return;

}



double sign(double d) {

	// returns sign (+/- 1) of d

	if (!d) return 1.0;
  return d/fabs(d);

}


int bgstart(char* fil) {

	// starts with default pagewidth (A4)

	return bgstart(fil,DEFAULT_PAGEWIDTH);

}



void assign_linestyle(lclasstype lclass,double d0,double d1,
	double d2,double d3,double d4,double d5,double d6,
	shapetype shape,int sfilled,linedef& line_style) {

	line_style.lclass=lclass;
	line_style.dsl[0]=d0;
	line_style.dsl[1]=d1;
	line_style.dsl[2]=d2;
	line_style.dsl[3]=d3;
	line_style.dsl[4]=d4;
	line_style.dsl[5]=d5;
	line_style.dsl[6]=d6;
	line_style.dxag=DEFAULT_DASHXAG;
	line_style.shape=shape;
	line_style.sfilled=sfilled;
	return;

}


void assign_markerstyle(shapetype shape,int filled,double mult,
	markerdef& marker_style) {

	marker_style.shape=shape;
	marker_style.sfilled=filled;
	marker_style.hwidth=marker_style.swidth*mult;
	return;

}


double xlog(double d,axisdef& xaxispars,int& logerror) {

	// if xaxispars.log false, returns d, otherwise
	// log10(d).  If d<=0, returns 0 and erroneous logs flag set

	if (!xaxispars.log) return d;
	if (d>0.0) return log10(d);
	logerror=1;
	return 0.0;

}


double ylog(double d,axisdef& yaxispars,int& logerror) {

	// if xaxispars.log false, returns d, otherwise
	// log10(d).  If d<=0, returns 0 and erroneous logs flag set

	if (!yaxispars.log) return d;
	if (d>0.0) return log10(d);
	logerror=1;
	return 0.0;

}


int opendata(FILE*& tdf,int& dataopen) {

	// opens DATA.$BG which contains data for potential plotting

	/*  Error handling: 0 no error
											1 cannot open temporary data file */

	tdf=fopen(TEMPDATAFILE,"w+b");
	if (!tdf) return 1;  // error
	dataopen=1;
	return 0;
}



int data_(int id,int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo,axisdef& xaxispars,axisdef& yaxispars,
	int& logerror,FILE*& tdf,double& mindx,double& maxdx,double& mindy,
	double& maxdy,double& minde,double& maxde,int& dataopen,xypair& minortic,
	xypair& autotic,location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& xlabel_style,labeldef& ylabel_style,
	int& seriesid) {

	// adds a new data series to temporary file DATA.$BG

	/* Error handling: 0 no error
                     1 cannot open temporary data file
										 2 log transformation error (warning only)  */

	double dumdub,trdata[MAXBGPOINT];
	long length;
	int i;

  logerror=0;

	if (!dataopen) {
		if (opendata(tdf,dataopen)) return 1;
		mindx=xlog(xdata[0],xaxispars,logerror);
		maxdx=xlog(xdata[0],xaxispars,logerror);
		mindy=ylog(ydata[0],yaxispars,logerror);
		maxdy=ylog(ydata[0],yaxispars,logerror);
		minde=ylog(ydata[0]-errorlo[0],yaxispars,logerror);
		maxde=ylog(ydata[0]+errorhi[0],yaxispars,logerror);
	}

	// work out how long this data set will be in file (in bytes)
	// NB: length calculation does not include length and id,
	// which are always read by reload (below)

	length=(long)nitem*sizeof(dumdub)*4+sizeof(nitem);

	// write whole series

	fwrite(&length,sizeof(length),1,tdf);
	fwrite(&id,sizeof(id),1,tdf);
	fwrite(&nitem,sizeof(nitem),1,tdf);
	length=(long)nitem*sizeof(dumdub);
	for (i=0;i<nitem;i++) trdata[i]=xlog(xdata[i],xaxispars,logerror);
	fwrite(trdata,length,1,tdf);
	for (i=0;i<nitem;i++) trdata[i]=ylog(ydata[i],yaxispars,logerror);
	fwrite(trdata,length,1,tdf);
	for (i=0;i<nitem;i++) trdata[i]=ylog(ydata[i]+errorhi[i],yaxispars,logerror);
	fwrite(trdata,length,1,tdf);
	for (i=0;i<nitem;i++) trdata[i]=ylog(ydata[i]-errorlo[i],yaxispars,logerror);
	fwrite(trdata,length,1,tdf);

	// update max and min statistics

	for (i=0;i<nitem;i++) {
		if (xlog(xdata[i],xaxispars,logerror)<mindx)
			mindx=xlog(xdata[i],xaxispars,logerror);
		if (xlog(xdata[i],xaxispars,logerror)>maxdx)
			maxdx=xlog(xdata[i],xaxispars,logerror);
		if (ylog(ydata[i],yaxispars,logerror)<mindy)
			mindy=ylog(ydata[i],yaxispars,logerror);
		if (ylog(ydata[i],yaxispars,logerror)>maxdy)
			maxdy=ylog(ydata[i],yaxispars,logerror);
		if (ylog(ydata[i]+errorhi[i],yaxispars,logerror)>maxde)
			maxde=ylog(ydata[i]+errorhi[i],yaxispars,logerror);
		if (ylog(ydata[i]-errorlo[i],yaxispars,logerror)<minde)
			minde=ylog(ydata[i]-errorlo[i],yaxispars,logerror);
	}

	// update graphscale parameters according to new stats

	autoscalex(mindx,maxdx,100.0,0,xaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,xlabel_style);
	autoscaley(minde,maxde,100.0,0,yaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,ylabel_style);

	xaxispars.displ=0.0;
	yaxispars.displ=0.0;

	if (axinverse) {
		if (xaxispars.reverse) yaxispars.intercept=ptogx(graphtr.v,graphscale);
		else yaxispars.intercept=ptogx(graphbl.v,graphscale);
		if (yaxispars.reverse) xaxispars.intercept=ptogy(graphtr.h,graphscale);
		else xaxispars.intercept=ptogy(graphbl.h,graphscale);
	}
	else {
		if (xaxispars.reverse) yaxispars.intercept=ptogx(graphtr.h,graphscale);
		else yaxispars.intercept=ptogx(graphbl.h,graphscale);
		if (yaxispars.reverse) xaxispars.intercept=ptogy(graphtr.v,graphscale);
		else xaxispars.intercept=ptogy(graphbl.v,graphscale);
	}

	seriesid=id+1;
	if (logerror) return 2;
	return 0;

}


// FUNCTIONS

void newgraph(double blx,double bly,double trx,double tryy,int flags) {

	// deletes current data (by closing DATA.$BG) and sets parameters
	// for a new graph

  double buf;

	if (dataopen) fclose(tdf);
	dataopen=0;
	seriesid=0;

	graphbl.h=blx;
	graphbl.v=bly;
	graphtr.h=trx;
	graphtr.v=tryy;

	if (flags&ZEROX) xaxispars.zero=1;
	else xaxispars.zero=0;
	if (flags&ZEROY) yaxispars.zero=1;
	else yaxispars.zero=0;
	if (flags&LOGX) xaxispars.log=1;
	else xaxispars.log=0;
	if (flags&LOGY) yaxispars.log=1;
	else yaxispars.log=0;
	if (flags&INVERSEAXES) axinverse=1;
	else axinverse=0;
	if (flags&REVERSEX) {
  	if (axinverse) {
			buf=graphtr.v;
			graphtr.v=graphbl.v;
			graphbl.v=buf;
		}
		else {
			buf=graphtr.h;
			graphtr.h=graphbl.h;
			graphbl.h=buf;
		}
		xaxispars.reverse=1;
	}
	else xaxispars.reverse=0;
	if (flags&REVERSEY) {
		if (axinverse) {
			buf=graphtr.h;
			graphtr.h=graphbl.h;
			graphbl.h=buf;
		}
		else {
			buf=graphtr.v;
			graphtr.v=graphbl.v;
			graphbl.v=buf;
		}
		yaxispars.reverse=1;
  }
	else yaxispars.reverse=0;
	xaxispars.displ=0.0;
	yaxispars.displ=0.0;
	xaxispars.intercept=0;
	yaxispars.intercept=0;

	graphscale.mult.x=1.0;
	graphscale.mult.x=1.0;
	graphscale.orig.x=0;
	graphscale.orig.y=0;

	legend_style.flags=0;

	autotic.x=floor((graphtr.h-graphbl.h)/10.0);
	autotic.y=floor((graphtr.v-graphbl.v)/10.0);

	usermarker=userline=userpattern=usernumformat=0;

	autoscalex(0,0,10,1,xaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,xlabel_style);
	autoscaley(0,0,10,1,yaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,ylabel_style);

	
	return;

}


void newgraph(double blx,double bly,double trx,double tryy) {

	newgraph(blx,bly,trx,tryy,0);

}


void newgraph() {

	newgraph(DEFAULT_GRAPHBL_H,DEFAULT_GRAPHBL_V,DEFAULT_GRAPHTR_H,
		DEFAULT_GRAPHTR_V,0);

}


void newgraph(int flags) {

	newgraph(DEFAULT_GRAPHBL_H,DEFAULT_GRAPHBL_V,DEFAULT_GRAPHTR_H,
		DEFAULT_GRAPHTR_V,flags);

}

fonttype textfont(fonttype newfont) {

	// sets text font, returning old font

	fonttype oldfont;
	oldfont=text_style.font;
	text_style.font=newfont;
	return oldfont;

}

fonttype textfont() {

	// returns current font

	return text_style.font;
}

double textsize(double newsize) {

	// sets textsize, returning old size

	double oldsize=text_style.size;
	if (!newsize) text_style.size=DEFAULT_TEXTSIZE;
	else text_style.size=newsize;
	return oldsize;

}

double textsize() {

	// returns current size

	return text_style.size;

}

void textbold(int newstatus) {

	// sets text to bold, if newstatus 1

	text_style.bold=newstatus;
	return;
}

void textitalic(int newstatus) {

	// sets text to italics, if newstatus 1

	text_style.italic=newstatus;
	return;
}


void markerstyle(int n) {

	// sets up global variable markerstyle according to a menu of types
	// NB: width is set by a separate function (markersize)

	n=n%16;
	switch (n) {
		case 0:
			assign_markerstyle(CIRCLE,1,0,marker_style);
			break;
		case 1:
			assign_markerstyle(SQUARE,1,0,marker_style);
			break;
		case 2:
			assign_markerstyle(THTRIANGLE,1,0,marker_style);
			break;
		case 3:
			assign_markerstyle(BHTRIANGLE,1,0,marker_style);
			break;
		case 4:
			assign_markerstyle(DIAMOND,1,0,marker_style);
			break;
		case 5:
			assign_markerstyle(VCROSS,1,0,marker_style);
			break;
		case 6:
			assign_markerstyle(DCROSS,1,0,marker_style);
			break;
		case 7:
			assign_markerstyle(VCROSS,1,HPROP,marker_style);
			break;
		case 8:
			assign_markerstyle(DCROSS,1,HPROP,marker_style);
			break;
		case 9:
			assign_markerstyle(CIRCLE,0,0,marker_style);
			break;
		case 10:
			assign_markerstyle(SQUARE,0,0,marker_style);
			break;
		case 11:
			assign_markerstyle(THTRIANGLE,0,0,marker_style);
			break;
		case 12:
			assign_markerstyle(BHTRIANGLE,0,0,marker_style);
			break;
		case 13:
			assign_markerstyle(DIAMOND,0,0,marker_style);
			break;
		case 14:
			assign_markerstyle(VCROSS,0,HPROP,marker_style);
			break;
		case 15:
			assign_markerstyle(DCROSS,0,HPROP,marker_style);
			break;
	}
	usermarker=1;
	return;

}


void linestyle(int n) {

	// sets up global variable linestyle according to a menu of types
	// NB: width is set by a separate function (linewidth)
  //     currently dxag is always 1.0 (no exaggeration)

	n=n%19;

	switch (n) {
		case 0:
			assign_linestyle(SOLID,0,0,0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 1:
			assign_linestyle(DASH,3.0,3.0,0.0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 2:
			assign_linestyle(DASH,10.0,5.0,0.0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 3:
			assign_linestyle(DASH,20.0,5.0,0.0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 4:
			assign_linestyle(DASH,20.0,5.0,3.0,5.0,0.0,0,0,CIRCLE,1,line_style);
			break;
		case 5:
			assign_linestyle(DASH,20.0,5.0,3.0,5.0,3.0,5.0,0.0,CIRCLE,1,line_style);
			break;
		case 6:
			assign_linestyle(DASH,1.0,2.0,0.0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 7:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,CIRCLE,1,line_style);
			break;
		case 8:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,SQUARE,1,line_style);
			break;
		case 9:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,THTRIANGLE,1,line_style);
			break;
		case 10:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,BHTRIANGLE,1,line_style);
			break;
		case 11:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,DIAMOND,1,line_style);
			break;
		case 12:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,VCROSS,1,line_style);
			break;
		case 13:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,DCROSS,1,line_style);
			break;
		case 14:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,CIRCLE,0,line_style);
			break;
		case 15:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,SQUARE,0,line_style);
			break;
		case 16:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,THTRIANGLE,0,line_style);
			break;
		case 17:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,BHTRIANGLE,0,line_style);
			break;
		case 18:
			assign_linestyle(SHAPE,0,0,0,0,0,0,0,DIAMOND,0,line_style);
			break;

	}
	userline=1;
	return;

}


double linewidth(double w) {

	// sets linewidth and sizes of shapes in shape lines

	double old;
	if (w>100.0) w=100.0;
	if (w<0.0) w=0.0;
	old=set_linewidth(w);
	if (w>10) w=10;
	line_style.swidth=w*4.0;
	line_style.sspace=w*10.0;
	return old;

}


double linewidth() {

	double old=set_linewidth(1.0);
	set_linewidth(old);
	return old;

}


double markersize(double w) {

	double old;
	old=marker_style.swidth;
	if (w>100.0) w=100.0;
	if (w<0.0) w=0.0;
	marker_style.swidth=w;
	marker_style.bwidth=DEFAULT_LINEWIDTH/DEFAULT_MARKERWIDTH*w;
	if (marker_style.hwidth)
		marker_style.hwidth=marker_style.bwidth*HPROP;

	return old;

}


double markersize() {

	return marker_style.swidth;

}



double patternscale(double d) {

	// sets scale parameters for fill patterns

	double old=pattern_style.lwidth;
	if (d<=1.0) pattern_style.lwidth=0.0;
	else pattern_style.lwidth=d;
	pattern_style.spacing=d*4.0;
	return old;

}


double patternscale() {

	return pattern_style.lwidth;

}


void patternstyle(int n) {

	n=n%20;
	switch (n) {
		case 0:
			pattern_style.pattern=GREY;
			pattern_style.grey=1.0;
			break;
		case 1:
			pattern_style.pattern=GREY;
			pattern_style.grey=0.90;
			break;
		case 2:
			pattern_style.pattern=GREY;
			pattern_style.grey=0.66;
			break;
		case 3:
			pattern_style.pattern=GREY;
			pattern_style.grey=0.0;
			break;
		case 4:
			pattern_style.pattern=RDL;
			break;
		case 5:
			pattern_style.pattern=LDL;
			break;
		case 6:
			pattern_style.pattern=VL;
			break;
		case 7:
			pattern_style.pattern=HL;
			break;
		case 8:
			pattern_style.pattern=HV;
			break;
		case 9:
			pattern_style.pattern=DH;
			break;
		case 10:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=CIRCLE;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 11:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=SQUARE;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 12:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=THTRIANGLE;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 13:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=BHTRIANGLE;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 14:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=DIAMOND;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 15:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=VCROSS;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 16:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=DCROSS;
			pattern_style.stagger=1;
			pattern_style.fill=0;
			break;
		case 17:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=CIRCLE;
			pattern_style.stagger=1;
			pattern_style.fill=1;
			break;
		case 18:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=SQUARE;
			pattern_style.stagger=1;
			pattern_style.fill=1;
			break;
		case 19:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=THTRIANGLE;
			pattern_style.stagger=1;
			pattern_style.fill=1;
			break;
		case 20:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=BHTRIANGLE;
			pattern_style.stagger=1;
			pattern_style.fill=1;
			break;
		case 21:
			pattern_style.pattern=SHAPEFILL;
			pattern_style.shape=DIAMOND;
			pattern_style.stagger=1;
			pattern_style.fill=1;
			break;
	}
	userpattern=1;
	return;

}


void colour(double red_or_hue,double green_or_light,double blue_or_sat,int ifhls) {

	// Specifies colour as RGB or HLS
	// RGB: red, green, blue in range 0-1
	// HLS: hue in range 0 (red) to 360, lightness in range 0-1, saturation in range 0-1
	
	long hexred,hexgreen,hexblue;
	double red,green,blue,r,g,b;

	if (green_or_light<0.0) green_or_light=0.0;
	else if (green_or_light>1.0) green_or_light=1.0;
	if (blue_or_sat<0.0) blue_or_sat=0.0;
	else if (blue_or_sat>1.0) blue_or_sat=1.0;

	if (ifhls) {
		hlstorgb(red_or_hue,green_or_light,blue_or_sat,red,green,blue);
	}
	else {
		if (red_or_hue<0.0) red_or_hue=0.0;
		else if (red_or_hue>1.0) red_or_hue=1.0;
		red=red_or_hue;
		green=green_or_light;
		blue=blue_or_sat;
	}
	
	hexred=long(red*255.0)<<16;
	hexgreen=long(green*255.0)<<8;
	hexblue=blue*255.0;

	colour_style=hexred+hexgreen+hexblue;

	red=(hexred>>16)/255.0;
	green=(hexgreen>>8)/255.0;
	blue=hexblue/255.0;

	char line[100];
	
	if (red==floor(red)) sprintf(line,"%g ",red);
	else sprintf(line,"%0.2g ",red);
	psout(line);
	
	if (green==floor(green)) sprintf(line,"%g ",green);
	else sprintf(line,"%0.2g ",green);
	psout(line);
	
	if (blue==floor(blue)) sprintf(line,"%g ",blue);
	else sprintf(line,"%0.2g ",blue);
	psout(line);
	
	psout("setrgbcolor\n");
}

void colour(double red,double green,double blue) {

	colour(red,green,blue,false);
}

void colour() {

	// Restores the default ink colour (black)

	colour(0,0,0);
}


int startbengraph(char* filnam,int eps) {

	// initialises bengraph:
	//		sets default values for lines, text and plotting areas
	//    opens filnam as eps output file
	//		writes ps header to filnam

	// check if filnam has an extension and, if not, add one (.eps)

	/* Error handling: 0 no error
                     1 cannot open filnam for output */

	int notok;

	colour_style=0x000000; // default colour (black)
	if (eps) iseps=1;
	else iseps=0;

	notok=bgstart(filnam);

	// set defaults

	text_style.font  =DEFAULT_FONT;
	text_style.size  =DEFAULT_TEXTSIZE;
	text_style.bold  =0;
	text_style.italic=0;
	text_style.super =0;
	text_style.sub   =0;

	line_style.lclass=SOLID;
	line_style.dxag=DEFAULT_DASHXAG;
	line_style.swidth=DEFAULT_LINEWIDTH*4.0;
	line_style.sspace=DEFAULT_LINEWIDTH*10.0;

	marker_style.shape=CIRCLE;
	marker_style.sfilled=1;
	marker_style.swidth=DEFAULT_MARKERWIDTH;
	marker_style.bwidth=DEFAULT_LINEWIDTH;
	marker_style.hwidth=0.0;

	patternscale(DEFAULT_PATTERNSCALE);
	pattern_style.pattern=RDL;

	pagescale=DEFAULT_PAGESCALE;

	xlabel_style.width=0;
	xlabel_style.places=1;
	xlabel_style.exp=0;
	xlabel_style.just=AUTOJUST;
	xlabel_style.rot=0.0;

	ylabel_style.width=0;
	ylabel_style.places=1;
	ylabel_style.exp=0;
	ylabel_style.just=AUTOJUST;
	ylabel_style.rot=0.0;

	tic_style.centre=TIC_CENTRE;
	tic_style.width=DEFAULT_LINEWIDTH;
	tic_style.length=DEFAULT_TICLENGTH/DEFAULT_PAGESCALE;
	minortic_style.centre=TIC_CENTRE;
	minortic_style.width=DEFAULT_LINEWIDTH;
	minortic_style.length=tic_style.length*DEFAULT_MTIC;
	wantminortics=0;	// default=no minor tics

	error_style.hatwidth=DEFAULT_LINEWIDTH*DEFAULT_EHM;
	error_style.linewidth=DEFAULT_LINEWIDTH;
	error_style.flags=HIGH|LOW;

	legend_style.orig.h=legend_style.start.h=DEFAULT_LORIG_H;
	legend_style.orig.v=legend_style.start.v=DEFAULT_LORIG_V;
	legend_style.spacing=DEFAULT_LSPACING;
	legend_style.width=DEFAULT_LWIDTH;
	legend_style.height=DEFAULT_LHEIGHT;
	legend_style.border=1;
	legend_style.tdisp=DEFAULT_LTDISP;
	legend_style.just=CL;
	legend_style.bwidth=DEFAULT_LINEWIDTH;
	legend_style.direction=0; // down
	legend_style.flags=0;

	arrow_style.angle=DEFAULT_ARROW_ANGLE;
	arrow_style.lwidth=DEFAULT_LINEWIDTH;
	arrow_style.hwidth=arrow_style.lwidth*DEFAULT_ARROW_HW_MULT;
	arrow_style.filled=1;

	usermarker=0;
	userline=0;
	userpattern=0;
	usercolour=0;
	outlinepats=DEFAULT_LINEWIDTH;
  usernumformat=0;

	graphbl.h=DEFAULT_GRAPHBL_H;
	graphbl.v=DEFAULT_GRAPHBL_V;
	graphtr.h=DEFAULT_GRAPHTR_H;
	graphtr.v=DEFAULT_GRAPHTR_V;

  orientation(0); // portrait

	newgraph(DEFAULT_GRAPHBL_H,DEFAULT_GRAPHBL_V,DEFAULT_GRAPHTR_H,
		DEFAULT_GRAPHTR_V,0);

	return notok;

}

int startbengraph(char* filnam) {

	// (default is .ps)
	
	return startbengraph(filnam,false);
}


int endbengraph() {

	return bgend();

}



double ptops(double p) {

	// given a page x value, returns the current ps point value

	return p*pagescale;

}

double pstop(double ps) {

	return ps/pagescale;

}


double maxx() {return maxdx;}
double maxy() {return maxdy;}
double minx() {return mindx;}
double miny() {return mindy;}
double maxe() {return maxde;}
double mine() {return minde;}



int data(int id,int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo) {

	// adds a new data series to temporary file DATA.$BG

	/* Error handling: 0 no error
                     1 cannot open temporary data file
										 2 log transformation error (warning only)  */

	return data_(id,nitem,xdata,ydata,errorhi,errorlo,xaxispars,
		yaxispars,logerror,tdf,mindx,maxdx,mindy,maxdy,minde,maxde,
		dataopen,minortic,autotic,graphtr,graphbl,graphscale,axinverse,
		xlabel_style,ylabel_style,seriesid);
}


int data(int id,int nitem,double* xdata,double* ydata,
	double* error) {

	// version with only one set of errors

	return data(id,nitem,xdata,ydata,error,error);

}


int data(int id,int nitem,double* xdata,double* ydata) {

	// version with no errors

	double error[MAXBGPOINT];
	int i;
	for (i=0;i<nitem;error[i++]=0.0);
	return data(id,nitem,xdata,ydata,error,error);

}


int data(int id,int nitem,double* ydata) {

	// version with no xdata

	double xdata[MAXBGPOINT],error[MAXBGPOINT];
	int i;
	for (i=0;i<nitem;i++) {
		xdata[i]=(double)(i);
		error[i]=0.0;
	}
	return data(id,nitem,xdata,ydata,error,error);

}



int data(int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo) {

	// version with no id

	return data(seriesid,nitem,xdata,ydata,errorhi,errorlo);

}


int data(int nitem,double* xdata,double* ydata,
	double* error) {

	// version with only one set of errors (no id)

	return data(seriesid,nitem,xdata,ydata,error,error);

}


int data(int nitem,double* xdata,double* ydata) {

	// version with no errors (no id)

	double error[MAXBGPOINT];
	int i;
	for (i=0;i<nitem;error[i++]=0.0);
	return data(seriesid,nitem,xdata,ydata,error,error);

}


int data(int nitem,double* ydata) {

	// version with no xdata (no id)

	double xdata[MAXBGPOINT],error[MAXBGPOINT];
	int i;
	for (i=0;i<nitem;i++) {
		xdata[i]=(double)(i);
		error[i]=0.0;
	}
	return data(seriesid,nitem,xdata,ydata,error,error);

}


void scalex(double min,double max,double round) {

	// specifies min,max and tic choice parameter for x-axis

	yaxispars.intercept=min;
	autoscalex(xlog(min,xaxispars,logerror),xlog(max,xaxispars,logerror),
		round,1,xaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,xlabel_style);

}

void scalex(double min,double max) {

	yaxispars.intercept=min;
	autoscalex(xlog(min,xaxispars,logerror),xlog(max,xaxispars,logerror),
		100.0,1,xaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,xlabel_style);

}

void scalex(double round) {

	autoscalex(mindx,maxdx,round,0,xaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,xlabel_style);
}



void scaley(double min,double max,double round) {

	xaxispars.intercept=min;
	autoscaley(ylog(min,yaxispars,logerror),ylog(max,yaxispars,logerror),
		round,1,yaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,ylabel_style);

}

void scaley(double min,double max) {

	xaxispars.intercept=min;
	autoscaley(ylog(min,yaxispars,logerror),ylog(max,yaxispars,logerror),
		100.0,1,yaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,ylabel_style);

}


void scaley(double round) {

	autoscaley(minde,maxde,round,0,yaxispars,minortic,autotic,
		graphtr,graphbl,graphscale,axinverse,ylabel_style);
}



int curve(int id) {

	// plots curve (line) with given id on current graph area
	// if line symbols have not been chosen, uses
	// id to choose a symbol

	/* Error handling: inherited from reload */

	location data[MAXBGPOINT];
	double xdata[MAXBGPOINT];
	double ydata[MAXBGPOINT];
	double error[MAXBGPOINT];
	int nitem;
	int notok,i;
	notok=reload(id,nitem,xdata,ydata,error,error,tdf,dataopen);
	if (notok) return notok;
	for (i=0;i<nitem;i++) {
		if (!axinverse) {
			data[i].h=gtopx(xdata[i]); // convert to page units
			data[i].h=ptops(data[i].h);  // convert to ps points
			data[i].v=gtopy(ydata[i]);
			data[i].v=ptops(data[i].v);
		}
		else {
			data[i].v=gtopx(xdata[i]); // convert to page units
			data[i].v=ptops(data[i].v);  // convert to ps points
			data[i].h=gtopy(ydata[i]);
			data[i].h=ptops(data[i].h);
		}
	}

	if (!userline) linestyle(id);
	curve_point(nitem,data,line_style);
	userline=0;
	legend_style.flags|=LINE;
	return 0;

}


int curve() {

	return curve(0);

}


int scatter(int id) {

	// equivalent to curve: plots points

	/* Error handling: inherited from reload */

	location data[MAXBGPOINT];
	double xdata[MAXBGPOINT];
	double ydata[MAXBGPOINT];
	double error[MAXBGPOINT];
	int nitem;
	int notok,i;
	notok=reload(id,nitem,xdata,ydata,error,error,tdf,dataopen);
	if (notok) return notok;
	for (i=0;i<nitem;i++) {
		if (!axinverse) {
			data[i].h=gtopx(xdata[i]); // convert to page units
			data[i].h=ptops(data[i].h);  // convert to ps points
			data[i].v=gtopy(ydata[i]);
			data[i].v=ptops(data[i].v);
		}
		else {
			data[i].v=gtopx(xdata[i]); // convert to page units
			data[i].v=ptops(data[i].v);  // convert to ps points
			data[i].h=gtopy(ydata[i]);
			data[i].h=ptops(data[i].h);
		}
	}

	if (!usermarker) markerstyle(id);
	scatter_point(nitem,data,marker_style);
	usermarker=0;
	legend_style.flags|=MARKER;
	return 0;

}


int scatter() {

	return scatter(0);
}


double errorstyle(double linewidth,double hatwidth,int flags) {

	// sets error bar parameters, returning linewidth

	double old=error_style.linewidth;
	if (linewidth>100.0) linewidth=100.0;
	if (linewidth<0.0) linewidth=0.0;
	if (hatwidth>100.0) hatwidth=100.0;
	if (hatwidth<0.0) hatwidth=0.0;
	error_style.linewidth=linewidth;
	error_style.hatwidth=hatwidth;
	error_style.flags=flags;
	return old;

}


double errorstyle(double linewidth,int flags) {

	double hatwidth=linewidth*DEFAULT_EHM;
	return errorstyle(linewidth,hatwidth,flags);

}

double errorstyle(double linewidth) {

	double hatwidth=linewidth*DEFAULT_EHM;
	return errorstyle(linewidth,hatwidth,HIGH|LOW);

}


double errorstyle(int flags) {

	return errorstyle(DEFAULT_LINEWIDTH,flags);

}


double errorstyle() {

	return error_style.linewidth;
}


int errorbars(int id) {

	// plots error bars using current symbol
	// flags specify whether to include high, low or both

	/* Error handling: inherited from reload */

	double errorlo[MAXBGPOINT];
	double errorhi[MAXBGPOINT];
	double xdata[MAXBGPOINT];
	double ydata[MAXBGPOINT];
	location s,f;
	int i,notok,nitem;
	double linewidth;
	int flags=error_style.flags;

	linewidth=set_linewidth(1.0);
	set_linewidth(error_style.linewidth);

	notok=reload(id,nitem,xdata,ydata,errorhi,errorlo,tdf,dataopen);
	if (notok) return notok;
	for (i=0;i<nitem;i++) {
		if (axinverse) {
			if (flags&HIGH) {
				s.h=ptops(gtopy(ydata[i]));
				f.h=ptops(gtopy(errorhi[i]));
				s.v=f.v=ptops(gtopx(xdata[i]));
				filline(s,f);
				s.v=f.v-error_style.hatwidth*0.5;
				f.v=s.v+error_style.hatwidth;
				s.h=f.h;
				filline(s,f);
			}
			if (flags&LOW) {
				s.h=ptops(gtopy(ydata[i]));
				f.h=ptops(gtopy(errorlo[i]));
				s.v=f.v=ptops(gtopx(xdata[i]));
				filline(s,f);
				s.v=f.v-error_style.hatwidth*0.5;
				f.v=s.v+error_style.hatwidth;
				s.h=f.h;
				filline(s,f);
			}
		}
		else {
			if (flags&HIGH) {
				s.v=ptops(gtopy(ydata[i]));
				f.v=ptops(gtopy(errorhi[i]));
				s.h=f.h=ptops(gtopx(xdata[i]));
				filline(s,f);
				s.h=f.h-error_style.hatwidth*0.5;
				f.h=s.h+error_style.hatwidth;
				s.v=f.v;
				filline(s,f);
			}
			if (flags&LOW) {
				s.v=ptops(gtopy(ydata[i]));
				f.v=ptops(gtopy(errorlo[i]));
				s.h=f.h=ptops(gtopx(xdata[i]));
				filline(s,f);
				s.h=f.h-error_style.hatwidth*0.5;
				f.h=s.h+error_style.hatwidth;
				s.v=f.v;
				filline(s,f);
			}
		}
  }
	set_linewidth(linewidth);
	legend_style.flags|=ERROR;
	return 0;

}

int errorbars() {

	return errorbars(0);
}


int fillbetween(int id1,int id2) {

	return fillint(id1,id2,0,graphscale,pagescale,userpattern,tdf,dataopen,
		axinverse,mindx,maxdx,legend_style,pattern_style);

}


int fillunder(int id) {

	int dummy;
	return fillint(id,dummy,1,graphscale,pagescale,userpattern,tdf,dataopen,
		axinverse,mindx,maxdx,legend_style,pattern_style);

}


void expandx(double left,double right) {

	// expands x axis to the left and right without affecting
	// autotics (envisaged for histograms) [NB: left and right are
	// reversed if x axis is reversed]
  // y-axis intercept is adjusted to new left position

	double gmax,gmin;

	if (axinverse) {
		gmax=ptogx(graphtr.v)+right;
		gmin=ptogx(graphbl.v)-left;
		graphscale.mult.x=(graphtr.v-graphbl.v)/(gmax-gmin);
		graphscale.orig.x=graphbl.v-gmin*graphscale.mult.x;
	}
	else {
		gmax=ptogx(graphtr.h)+right;
		gmin=ptogx(graphbl.h)-left;
		graphscale.mult.x=(graphtr.h-graphbl.h)/(gmax-gmin);
		graphscale.orig.x=graphbl.h-gmin*graphscale.mult.x;
	}
	if (yaxispars.reverse) yaxispars.intercept=gmax;
	else yaxispars.intercept=gmin;
	return;

}


void expandx() {

	// expands x axis assuming defaults (width=1,displ=0)

	expandx(0.5,0.5);
	return;

}


int histogram(int id,double width,double displ,int startx) {

	// plots series id as a set of histogram bars of given width and
	// with centre displaced displ from x position (graph units)
	// if startx non-zero, histogram base is on x axis, regardless of
	// whether y axis begins at zero

  /* Error handling: inherited from reload */

	location data;
	location boxcorners[2],poly[5];
	double xdata[MAXBGPOINT];
	double ydata[MAXBGPOINT];
	double errorhi[MAXBGPOINT];
	double errorlo[MAXBGPOINT];
	double old;
	int nitem;
	int notok,i;
	notok=reload(id,nitem,xdata,ydata,errorhi,errorlo,tdf,dataopen);
	if (notok) return notok;
	if (axinverse) {

		if (startx) boxcorners[0].h=graphscale.orig.y;
		else boxcorners[0].h=0;
		boxcorners[0].h=gtopy(boxcorners[0].h);
		boxcorners[0].h=boxcorners[0].h*pagescale;
		if (!userpattern) {
			patternstyle(id);
			userpattern=0;
		}
		for (i=0;i<nitem;i++) {
			data.v=xdata[i]-width*0.5+displ;
			data.h=ydata[i];
			boxcorners[0].v=data.v;
			boxcorners[0].v=gtopx(boxcorners[0].v);
			boxcorners[0].v=ptops(boxcorners[0].v);
			boxcorners[1].v=data.v+width;
			boxcorners[1].v=gtopx(boxcorners[1].v);
			boxcorners[1].v=ptops(boxcorners[1].v);
			boxcorners[1].h=data.h;
			boxcorners[1].h=gtopy(boxcorners[1].h);
			boxcorners[1].h=ptops(boxcorners[1].h);
			poly[0].h=boxcorners[0].h;
			poly[0].v=boxcorners[0].v;
			poly[1].h=boxcorners[0].h;
			poly[1].v=boxcorners[1].v;
			poly[2].h=boxcorners[1].h;
			poly[2].v=boxcorners[1].v;
			poly[3].h=boxcorners[1].h;
			poly[3].v=boxcorners[0].v;
			poly[4].h=boxcorners[0].h;
			poly[4].v=boxcorners[0].v;
			if (outlinepats>0.0) {
				old=set_linewidth(outlinepats);
				fillpoly(5,poly,1,pattern_style);
				set_linewidth(old);
			}
			else fillpoly(5,poly,0,pattern_style);
		}

	}
	else {   //non-inverted axes

		if (startx) boxcorners[0].v=graphscale.orig.y;
		else boxcorners[0].v=0;
		boxcorners[0].v=gtopy(boxcorners[0].v);
		boxcorners[0].v=boxcorners[0].v*pagescale;
		if (!userpattern) {
			patternstyle(id);
			userpattern=0;
		}
		for (i=0;i<nitem;i++) {
			data.h=xdata[i]-width*0.5+displ;
			data.v=ydata[i];
			boxcorners[0].h=data.h;
			boxcorners[0].h=gtopx(boxcorners[0].h);
			boxcorners[0].h=ptops(boxcorners[0].h);
			boxcorners[1].h=data.h+width;
			boxcorners[1].h=gtopx(boxcorners[1].h);
			boxcorners[1].h=ptops(boxcorners[1].h);
			boxcorners[1].v=data.v;
			boxcorners[1].v=gtopy(boxcorners[1].v);
			boxcorners[1].v=ptops(boxcorners[1].v);
			poly[0].h=boxcorners[0].h;
			poly[0].v=boxcorners[0].v;
			poly[1].h=boxcorners[0].h;
			poly[1].v=boxcorners[1].v;
			poly[2].h=boxcorners[1].h;
			poly[2].v=boxcorners[1].v;
			poly[3].h=boxcorners[1].h;
			poly[3].v=boxcorners[0].v;
			poly[4].h=boxcorners[0].h;
			poly[4].v=boxcorners[0].v;
			if (outlinepats>0.0) {
				old=set_linewidth(outlinepats);
				fillpoly(5,poly,1,pattern_style);
				set_linewidth(old);
			}
			else fillpoly(5,poly,0,pattern_style);
		}

	}
  userpattern=0;
  	legend_style.flags|=PATTERN;
	return 0;

}


int histogram(int id,double width) {

	return histogram(id,width,0.0,0);
}



int histogram(int id) {

	return histogram(id,1.0,0.0,0);
}


int histogram() {

	return histogram(0,1.0,0.0,0);

}


void xaxis(double displ,double yintercept,int print) {

	// plots an x-axis (user) at given y intercept (graph units)
	// displaced by displ in page units
  // if print non-zero, axis is not actually plotted

	double y;
	location s,f;

	if (print) {
		y=ptops(gtopy(yintercept)+displ);
		if (axinverse) {
			s.v=ptops(graphbl.v);
			f.v=ptops(graphtr.v);
			s.h=f.h=y;
		}
		else {
			s.h=ptops(graphbl.h);
			f.h=ptops(graphtr.h);
			s.v=f.v=y;
		}
		filline(s,f);
	}
	xaxispars.intercept=yintercept;
	xaxispars.displ=displ;

	return;

}


void xaxis(double displ,double yintercept) {

	xaxis(displ,yintercept,1);

}


void xaxis(double displ) {

	// assumes axis is to go along bottom of y axis

	xaxis(displ,xaxispars.intercept,1);
	return;

}

void xaxis() {

	xaxis(xaxispars.displ,xaxispars.intercept,1);
	return;

}


void yaxis(double displ,double xintercept,int print) {

	// plots an x-axis at given y intercept (graph units)
	// displaced by displ in page units

	double x;
	location s,f;

	if (print) {
		x=ptops(gtopx(xintercept)+displ);

		if (axinverse) {
			s.h=ptops(graphbl.h);
			f.h=ptops(graphtr.h);
			s.v=f.v=x;
		}
		else {
			s.v=ptops(graphbl.v);
			f.v=ptops(graphtr.v);
			s.h=f.h=x;
		}
		filline(s,f);
	}
	yaxispars.intercept=xintercept;
	yaxispars.displ=displ;
	return;

}

void yaxis(double displ,double xintercept) {

	yaxis(displ,xintercept,1);

}

void yaxis(double displ) {

	// assumes axis is to go along bottom of y axis

	yaxis(displ,yaxispars.intercept);
	return;

}

void yaxis() {

	yaxis(yaxispars.displ);
	return;

}


void border() {

	// draws a border around graph area

	location s,f;
	if (axinverse) {
		s.h=ptops(graphbl.h);
		s.v=ptops(graphbl.v);
		f.h=ptops(graphtr.h);
		f.v=ptops(graphbl.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphtr.h);
		f.v=ptops(graphtr.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphbl.h);
		f.v=ptops(graphtr.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphbl.h);
		f.v=ptops(graphbl.v);
		filline(s,f);
	}
	else {
		s.h=ptops(graphbl.h);
		s.v=ptops(graphbl.v);
		f.h=ptops(graphtr.h);
		f.v=ptops(graphbl.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphtr.h);
		f.v=ptops(graphtr.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphbl.h);
		f.v=ptops(graphtr.v);
		filline(s,f);
		s.h=f.h;
		s.v=f.v;
		f.h=ptops(graphbl.h);
		f.v=ptops(graphbl.v);
		filline(s,f);
  }
	return;

}


double ticstyle(double length,double width,int centre) {

	// sets tic style parameters, returning old length

	double old=tic_style.length;
	if (length>100.0) length=100.0;
	if (length<-100.0) length=0.0;
	if (width>100.0) width=100.0;
	if (width<0.0) width=0.0;
	tic_style.length=length;
	tic_style.width=width;
	tic_style.centre=!!centre;
	return old;

}


double ticstyle(double length,int centre) {

	return ticstyle(length,DEFAULT_LINEWIDTH,centre);

}


double ticstyle(double length) {

	return ticstyle(length,DEFAULT_LINEWIDTH,0);
}


double ticstyle() {

	return tic_style.length;
}


double minorticstyle(double length,double width,int centre) {

	// sets tic style parameters, returning old length

	double old=minortic_style.length;
	if (length>100.0) length=100.0;
	if (length<-100.0) length=0.0;
	if (width>100.0) width=100.0;
	if (width<0.0) width=0.0;
	minortic_style.length=length;
	minortic_style.width=width;
	minortic_style.centre=!!centre;
	return old;

}


double minorticstyle(double length,int centre) {

	return minorticstyle(length,DEFAULT_LINEWIDTH,centre);

}


double minorticstyle(double length) {

	return minorticstyle(length,DEFAULT_LINEWIDTH,0);
}


double minorticstyle() {

	return minortic_style.length;
}


void minortics(int i) {

	wantminortics=!!i;
	return;

}


void minortics() {

	minortics(1);
}


void xtics(double xdispl) {

	// plots tics, with xdisplacement (graph units) if desired
	// (this is intended for histograms)
	// displaced tics are shown only if they fall within
	// the axis length

	double gmin,gmax,tol,ctic,axend,mult=1.0,hh,cmtic;
	location s,f,sm,fm;
	double linewidth,yintercept,ydispl;

	yintercept=xaxispars.intercept;
	ydispl=xaxispars.displ;

	linewidth=set_linewidth(tic_style.width);

	if (axinverse) {
		gmin=ptogx(graphbl.v);
		gmax=ptogx(graphtr.v);
		tol=(gmax-gmin)*0.001;
		if ((gtopy(yintercept)+ydispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.h=ptops(gtopy(yintercept)+tic_style.length*0.5+ydispl);
			f.h=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl);
			sm.h=ptops(gtopy(yintercept)+minortic_style.length*0.5+ydispl);
			fm.h=ptops(gtopy(yintercept)-minortic_style.length*0.5+ydispl);
		}
		else {
			s.h=ptops(gtopy(yintercept)+ydispl);
			f.h=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult);
			sm.h=s.h;
			fm.h=ptops(gtopy(yintercept)+ydispl-minortic_style.length*mult);
		}
	}
	else {
		gmin=ptogx(graphbl.h);
		gmax=ptogx(graphtr.h);
		tol=(gmax-gmin)*0.001;
		if ((gtopy(yintercept)+ydispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.v=ptops(gtopy(yintercept)+tic_style.length*0.5+ydispl);
			f.v=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl);
			sm.v=ptops(gtopy(yintercept)+minortic_style.length*0.5+ydispl);
			fm.v=ptops(gtopy(yintercept)-minortic_style.length*0.5+ydispl);
		}
		else {
			s.v=ptops(gtopy(yintercept)+ydispl);
			f.v=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult);
			sm.v=s.v;
			fm.v=ptops(gtopy(yintercept)+ydispl-minortic_style.length*mult);
		}
	}

	if (xdispl<=0.0) {  // start left
		axend=gmin+tol*sign(gmin);
		ctic=(double)long(axend/autotic.x)*autotic.x+xdispl;
		while (ctic<gmax+tol) {
			if (ctic>gmin-tol) {
				if (axinverse)
					s.v=f.v=ptops(gtopx(ctic));
				else
					s.h=f.h=ptops(gtopx(ctic));
				filline(s,f);
				if (wantminortics) {
					if (xaxispars.log) {
						hh=minortic.x;
						cmtic=ctic+log10(hh);
						while (cmtic<ctic+autotic.x-tol && cmtic<gmax) {
							if (axinverse)
								sm.v=fm.v=ptops(gtopx(cmtic));
							else
								sm.h=fm.h=ptops(gtopx(cmtic));
							filline(sm,fm);
							hh+=minortic.x;
							cmtic=ctic+log10(hh);
						}
					}
					else {
						cmtic=ctic+minortic.x;
						while (cmtic<ctic+autotic.x-tol && cmtic<gmax) {
							if (axinverse)
								sm.v=fm.v=ptops(gtopx(cmtic));
							else
								sm.h=fm.h=ptops(gtopx(cmtic));
							filline(sm,fm);
							cmtic+=minortic.x;
            }
          }
				}
			}
			ctic+=autotic.x;
		}
	}
	else {  // start right
		axend=gmax-tol*sign(gmax);
		ctic=(double)long(axend/autotic.x+1.0)*autotic.x+xdispl;
		while (ctic>gmin+tol) {
			if (ctic<gmax-tol) {
				if (axinverse)
					s.v=f.v=ptops(gtopx(ctic));
				else
					s.h=f.h=ptops(gtopx(ctic));
				filline(s,f);
				if (wantminortics) {
					if (xaxispars.log) {
						hh=minortic.x;
						cmtic=ctic-1.0+log10(10.0-hh);
						while (cmtic>ctic-autotic.x+tol && cmtic>gmin) {
							if (axinverse)
								sm.v=fm.v=ptops(gtopx(cmtic));
							else
								sm.h=fm.h=ptops(gtopx(cmtic));
							filline(sm,fm);
							hh+=minortic.x;
							cmtic=ctic-1.0+log10(10.0-hh);
						}
					}
					else {
						cmtic=ctic-minortic.x;
						while (cmtic>ctic-autotic.x+tol && cmtic>gmin) {
							if (axinverse)
								sm.v=fm.v=ptops(gtopx(cmtic));
							else
								sm.h=fm.h=ptops(gtopx(cmtic));
							filline(sm,fm);
							cmtic-=minortic.x;
						}
					}
        }
			}
			ctic-=autotic.x;
		}
	}
	set_linewidth(linewidth);
	return;
}


void xtics() {

	xtics(0.0);
	return;
}


int xtic(double xpos) {

	// plots a single tic at (xpos)

	/* Error handling: 0 no error
										 1 log transformation error */

	double linewidth=set_linewidth(tic_style.width);
	double gmin,gmax,mult=1.0,yintercept,ydispl;
	location s,f;

	yintercept=xaxispars.intercept;
	ydispl=xaxispars.displ;

	logerror=0;
	xpos=xlog(xpos,xaxispars,logerror);

	if (axinverse) {
		if ((gtopy(yintercept)+ydispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.h=ptops(gtopy(yintercept)+tic_style.length*0.5+ydispl);
			f.h=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl);
		}
		else {
			s.h=ptops(gtopy(yintercept)+ydispl);
			f.h=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult);
		}
		s.v=f.v=ptops(gtopx(xpos));
	}
	else {
		if ((gtopy(yintercept)+ydispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.v=ptops(gtopy(yintercept)+tic_style.length*0.5+ydispl);
			f.v=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl);
		}
		else {
			s.v=ptops(gtopy(yintercept)+ydispl);
			f.v=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult);
		}
		s.h=f.h=ptops(gtopx(xpos));
	}
	filline(s,f);
	set_linewidth(linewidth);
	if (logerror) return 1;
	return 0;

}


void ytics(double ydispl) {

	double gmin,gmax,tol,ctic,axend,mult=1.0,vv,cmtic;
	location s,f,sm,fm;
	double linewidth,xintercept,xdispl;

	linewidth=set_linewidth(tic_style.width);
	xintercept=yaxispars.intercept;
	xdispl=yaxispars.displ;

	if (axinverse) {
		gmin=ptogy(graphbl.h);
		gmax=ptogy(graphtr.h);
		tol=(gmax-gmin)*0.001;
		if ((gtopx(xintercept)+xdispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.v=ptops(gtopx(xintercept)+tic_style.length*0.5+xdispl);
			f.v=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl);
			sm.v=ptops(gtopx(xintercept)+minortic_style.length*0.5+xdispl);
			fm.v=ptops(gtopx(xintercept)-minortic_style.length*0.5+xdispl);
		}
		else {
			s.v=ptops(gtopx(xintercept)+xdispl);
			f.v=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult);
			sm.v=s.v;
			fm.v=ptops(gtopx(xintercept)+xdispl-minortic_style.length*mult);
		}
	}
	else {
		gmin=ptogy(graphbl.v);
		gmax=ptogy(graphtr.v);
		tol=(gmax-gmin)*0.001;
		if ((gtopx(xintercept)+xdispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.h=ptops(gtopx(xintercept)+tic_style.length*0.5+xdispl);
			f.h=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl);
			sm.h=ptops(gtopx(xintercept)+minortic_style.length*0.5+xdispl);
			fm.h=ptops(gtopx(xintercept)-minortic_style.length*0.5+xdispl);
		}
		else {
			s.h=ptops(gtopx(xintercept)+xdispl);
			f.h=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult);
			sm.h=s.h;
			fm.h=ptops(gtopx(xintercept)+xdispl-minortic_style.length*mult);
		}
	}

	if (ydispl<=0.0) {  // start bottom
		axend=gmin+tol*sign(gmin);
		ctic=(double)long(axend/autotic.y)*autotic.y+ydispl;
		while (ctic<gmax+tol) {
			if (ctic>gmin-tol) {
				if (axinverse)
					s.h=f.h=ptops(gtopy(ctic));
				else
					s.v=f.v=ptops(gtopy(ctic));
				filline(s,f);
				if (wantminortics) {
					if (yaxispars.log) {
						vv=minortic.y;
						cmtic=ctic+log10(vv);
						while (cmtic<ctic+autotic.y-tol && cmtic<gmax) {
							if (axinverse)
								sm.h=fm.h=ptops(gtopy(cmtic));
							else
								sm.v=fm.v=ptops(gtopy(cmtic));
							filline(sm,fm);
							vv+=minortic.y;
							cmtic=ctic+log10(vv);
						}
					}
					else {
						cmtic=ctic+minortic.y;
						while (cmtic<ctic+autotic.y-tol && cmtic<gmax) {
							if (axinverse)
								sm.h=fm.h=ptops(gtopy(cmtic));
							else
								sm.v=fm.v=ptops(gtopy(cmtic));
							filline(sm,fm);
							cmtic+=minortic.y;
						}
					}
				}
			}
			ctic+=autotic.y;
		}
	}
	else {  // start top
		axend=gmax-tol*sign(gmax);
		ctic=(double)long(axend/autotic.y+1.0)*autotic.y+ydispl;
		while (ctic>gmin+tol) {
			if (ctic<gmax-tol) {
				if (axinverse)
					s.h=f.h=ptops(gtopy(ctic));
				else
					s.v=f.v=ptops(gtopy(ctic));
				filline(s,f);
				if (wantminortics) {
					if (yaxispars.log) {
						vv=minortic.y;
						cmtic=ctic-1.0+log10(10.0-vv);
						while (cmtic>ctic-autotic.y+tol && cmtic>gmin) {
							if (axinverse)
								sm.h=fm.h=ptops(gtopy(cmtic));
							else
								sm.v=fm.v=ptops(gtopy(cmtic));
							filline(sm,fm);
							vv+=minortic.y;
							cmtic=ctic-1.0+log10(10.0-vv);
						}
					}
					else {
						cmtic=ctic+minortic.y;
						while (cmtic>ctic-autotic.y+tol && cmtic>gmin) {
							if (axinverse)
								sm.h=fm.h=ptops(gtopy(cmtic));
							else
								sm.v=fm.v=ptops(gtopy(cmtic));
							filline(sm,fm);
							cmtic+=minortic.y;
						}
					}
				}
			}
			ctic-=autotic.y;
		}
	}

	set_linewidth(linewidth);

	return;
}


void ytics() {

	ytics(0.0);
	return;
}


int ytic(double ypos) {

	// plots a single tic at (ypos)

	/* Error handling: 0 no error
										 1 log transformation error */

	double linewidth=set_linewidth(tic_style.width);
	double gmin,gmax,mult=1.0,xintercept,xdispl;
	location s,f;

	xintercept=yaxispars.intercept;
	xdispl=yaxispars.displ;

	logerror=0;
	ypos=xlog(ypos,yaxispars,logerror);

	if (!axinverse) {
		if ((gtopx(xintercept)+xdispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.h=ptops(gtopx(xintercept)+tic_style.length*0.5+xdispl);
			f.h=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl);
		}
		else {
			s.h=ptops(gtopx(xintercept)+xdispl);
			f.h=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult);
		}
		s.v=f.v=ptops(gtopy(ypos));
	}
	else {
		if ((gtopx(xintercept)+xdispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre) {
			s.v=ptops(gtopx(xintercept)+tic_style.length*0.5+xdispl);
			f.v=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl);
		}
		else {
			s.v=ptops(gtopx(xintercept)+xdispl);
			f.v=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult);
		}
		s.h=f.h=ptops(gtopy(ypos));
	}
	filline(s,f);
	set_linewidth(linewidth);
	if (logerror) return 1;
	return 0;

}


void xlabels(double xdispl) {

	// plots labels, with xdisplacement (graph units) if desired
	// (this is intended for histograms)
	// displaced tics are shown only if they fall within
	// the axis length

	double gmin,gmax,tol,ctic,axend,mult=1.0;
	char labtext[30];
	location point;
	double xlsr=xlabel_style.rot,yintercept,ydispl;
  int width,places,exp;

	yintercept=xaxispars.intercept;
	ydispl=xaxispars.displ;

	if (!usernumformat) {
		width=xlabel_style.width;
		places=xlabel_style.places;
		exp=xlabel_style.exp;
	}
	else {
		width=num_style.width;
		places=num_style.places;
		exp=num_style.exp;
	}
	usernumformat=0;

	if (axinverse) {
		gmin=ptogx(graphbl.v);
		gmax=ptogx(graphtr.v);
		tol=(gmax-gmin)*0.001;
		if ((gtopy(yintercept)+ydispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.h=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl)
				-text_style.size/2.0*mult;
		else
			point.h=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
	}
	else {
		gmin=ptogx(graphbl.h);
		gmax=ptogx(graphtr.h);
		tol=(gmax-gmin)*0.001;
		if ((gtopy(yintercept)+ydispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.v=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl)
				-text_style.size/2.0*mult;
		else
			point.v=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
	}

	if (xdispl<=0.0) {  // start left
		axend=gmin+tol*sign(gmin);
		ctic=(double)long(axend/autotic.x)*autotic.x+xdispl;
		while (ctic<gmax+tol) {
			if (ctic>gmin-tol) {
				if (xaxispars.log) logprint(labtext,ctic,exp);
				else bgsprint(30,labtext,ctic,width,places,exp);
				if (axinverse)
					point.v=ptops(gtopx(ctic));
				else
					point.h=ptops(gtopx(ctic));
				if (xlabel_style.just) text_point(labtext,point,text_style,
					xlabel_style.just,xlsr);
				else {
					if (axinverse) {
						if (mult>0.0)
							text_point(labtext,point,text_style,CR,xlsr);
						else
							text_point(labtext,point,text_style,CL,xlsr);
					}
					else {
						if (mult>0.0)
							text_point(labtext,point,text_style,TC,xlsr);
						else
							text_point(labtext,point,text_style,BC,xlsr);
					}
				}
			}
			ctic+=autotic.x;
			if (fabs(ctic/autotic.x)<0.001) ctic=0.0;
		}
	}

	else {  // start right
		axend=gmax-tol*sign(gmax);
		ctic=(double)long(axend/autotic.x+1.0)*autotic.x+xdispl;
		while (ctic>gmin+tol) {
			ctic-=autotic.x;
			if (ctic<gmax-tol) {
				if (xaxispars.log) logprint(labtext,ctic,exp);
				else bgsprint(30,labtext,ctic,width,places,
					exp);
				if (fabs(ctic/autotic.x)<0.001) ctic=0.0;
				if (axinverse)
					point.h=ptops(gtopx(ctic));
				else
					point.v=ptops(gtopx(ctic));

				if (xlabel_style.just) text_point(labtext,point,text_style,
					xlabel_style.just,xlsr);
				else {
					if (axinverse) {
						if (mult>0.0)
							text_point(labtext,point,text_style,CR,xlsr);
						else
							text_point(labtext,point,text_style,CL,xlsr);
					}
					else
						if (mult>0.0)
							text_point(labtext,point,text_style,TC,xlsr);
						else
							text_point(labtext,point,text_style,BC,xlsr);
				}
			}
		}
	}
	return;
}


void xlabels() {

	xlabels(0.0);
	return;
}


void xlabels(double xdispl,justtype just,double rotation) {

	xlabel_style.just=just;
	xlabel_style.rot=rotation;
	xlabels(xdispl);
	return;
}


void xlabels(justtype just,double rotation) {

	xlabel_style.just=just;
	xlabel_style.rot=rotation;
	xlabels(0.0);
  return;

}


int xlabel(double xpos,char* text,justtype just,double rotation) {

	// plots label at xpos on x-axis

	/* Error handling:  0 on success
											1 on log transformation error */

	double gmin,gmax,mult=1.0,yintercept,ydispl;
	location point;

	yintercept=xaxispars.intercept;
	ydispl=xaxispars.displ;

	logerror=0;

	xpos=xlog(xpos,xaxispars,logerror);

	if (axinverse) {
		if ((gtopy(yintercept)+ydispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.h=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl)
				-text_style.size/2.0*mult;
		else
			point.h=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
		point.v=ptops(gtopx(xpos));
		if (!just && mult>0.0) just=CR;
    else if (!just && mult<0.0) just=CL;
	}
	else {
		if ((gtopy(yintercept)+ydispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.v=ptops(gtopy(yintercept)-tic_style.length*0.5+ydispl)
				-text_style.size/2.0*mult;
		else
			point.v=ptops(gtopy(yintercept)+ydispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
		point.h=ptops(gtopx(xpos));
		if (!just && mult>0.0) just=TC;
    else if (!just && mult<0.0) just=BC;
	}
	text_point(text,point,text_style,just,rotation);
	if (logerror) return 1;
	return 0;

}


void xlabel(double xpos,double dval,justtype just,double rotation) {

	double size;
	int places;
  char buffer[50];
	dprint(buffer,dval,1,usernumformat,num_style);
	xlabel(xpos,buffer,just,rotation);
	return;
	
}


void xlabel(double xpos,long lval,justtype just,double rotation) {

	char buffer[50];
	dprint(buffer,(double)lval,0,usernumformat,num_style);
	xlabel(xpos,buffer,just,rotation);

}


void xlabel(double xpos,int ival,justtype just,double rotation) {

	xlabel(xpos,(long)ival,just,rotation);

}

void xlabel(double xpos,justtype just,double rotation) {

	xlabel(xpos,xpos,just,rotation);

}

void xlabel(double xpos,char* text) {

	xlabel(xpos,text,xlabel_style.just,xlabel_style.rot);

}

void xlabel(double xpos,double dval) {

	xlabel(xpos,dval,xlabel_style.just,xlabel_style.rot);

}

void xlabel(double xpos,long lval) {

	xlabel(xpos,lval,xlabel_style.just,xlabel_style.rot);

}

void xlabel(double xpos,int ival) {

	xlabel(xpos,ival,xlabel_style.just,xlabel_style.rot);

}

void xlabel(double xpos) {

	xlabel(xpos,xpos,xlabel_style.just,xlabel_style.rot);

}


void ylabels(double ydispl) {

	double gmin,gmax,tol,ctic,axend,mult=1.0;
	char labtext[30];
	location point;
	double ylsr=ylabel_style.rot;
	int width,places,exp;
	double xintercept,xdispl;

	xintercept=yaxispars.intercept;
	xdispl=yaxispars.displ;

	if (!usernumformat) {
		width=ylabel_style.width;
		places=ylabel_style.places;
		exp=ylabel_style.exp;
	}
	else {
		width=num_style.width;
		places=num_style.places;
		exp=num_style.exp;
	}
	usernumformat=0;

	if (axinverse) {
		gmin=ptogy(graphbl.h);
		gmax=ptogy(graphtr.h);
		tol=(gmax-gmin)*0.001;
		if ((gtopx(xintercept)+xdispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.v=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl)
				-text_style.size/2.0*mult;
		else
			point.v=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
	}
	else {
		gmin=ptogy(graphbl.v);
		gmax=ptogy(graphtr.v);
		tol=(gmax-gmin)*0.001;
		if ((gtopx(xintercept)+xdispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.h=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl)
				-text_style.size/2.0*mult;
		else
			point.h=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
	}

	if (ydispl<=0.0) {  // start bottom
		axend=gmin+tol*sign(gmin);
		ctic=(double)long(axend/autotic.y)*autotic.y+ydispl;
		while (ctic<gmax+tol) {
			if (ctic>gmin-tol) {
				if (yaxispars.log) logprint(labtext,ctic,exp);
				else bgsprint(30,labtext,ctic,width,places,exp);
				if (axinverse)
					point.h=ptops(gtopy(ctic));
				else
					point.v=ptops(gtopy(ctic));
				if (fabs(ctic/autotic.y)<0.001) ctic=0.0;

				if (ylabel_style.just) text_point(labtext,point,text_style,
					ylabel_style.just,ylsr);
				else {
					if (axinverse) {
						if (mult>0.0)
							text_point(labtext,point,text_style,TC,ylsr);
						else
							text_point(labtext,point,text_style,BC,ylsr);
					}
					else {
						if (mult>0.0)
							text_point(labtext,point,text_style,CR,ylsr);
						else
							text_point(labtext,point,text_style,CL,ylsr);
					}
				}
			}
			ctic+=autotic.y;
		}
	}
	else {  // start top
		axend=gmax+tol*sign(gmax);
		ctic=(double)long(axend/autotic.y+1.0)*autotic.y+ydispl;
		while (ctic>gmin+tol) {
			ctic-=autotic.y;
			if (ctic<gmax-tol) {
				if (yaxispars.log) logprint(labtext,ctic,exp);
				else bgsprint(30,labtext,ctic,width,places,exp);
				if (fabs(ctic/autotic.y)<0.001) ctic=0.0;
				if (axinverse)
					point.h=ptops(gtopy(ctic));
				else
					point.v=ptops(gtopy(ctic));

				if (ylabel_style.just) text_point(labtext,point,text_style,
					ylabel_style.just,ylsr);
				else {
					if (axinverse) {
						if (mult>0.0)
							text_point(labtext,point,text_style,TC,ylsr);
						else
							text_point(labtext,point,text_style,BC,ylsr);
					}
					else {
						if (mult>0.0)
							text_point(labtext,point,text_style,CR,ylsr);
						else
							text_point(labtext,point,text_style,CL,ylsr);
					}
				}
			}
		}
	}
	return;
}


void ylabels() {

	ylabels(0.0);
	return;
}


void ylabels(double ydispl,justtype just,double rotation) {

	ylabel_style.just=just;
	ylabel_style.rot=rotation;
	ylabels(ydispl);
	return;
}


void ylabels(justtype just,double rotation) {

	ylabel_style.just=just;
	ylabel_style.rot=rotation;
	ylabels(0.0);
	return;

}



int ylabel(double ypos,char* text,justtype just,double rotation) {

	// plots label at ypos on y-axis

	/* Error handling:  0 on success
											1 on log transformation error */

	double gmin,gmax,mult=1.0,xintercept,xdispl;
	location point;

	xintercept=yaxispars.intercept;
	xdispl=yaxispars.displ;

	logerror=0;

	ypos=ylog(ypos,yaxispars,logerror);

	if (!axinverse) {
		if ((gtopx(xintercept)+xdispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.h=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl)
				-text_style.size/2.0*mult;
		else
			point.h=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
		point.v=ptops(gtopy(ypos));
		if (!just && mult>0.0) just=CR;
    else if (!just && mult<0.0) just=CL;
	}
	else {
		if ((gtopx(xintercept)+xdispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		if (tic_style.centre)
			point.v=ptops(gtopx(xintercept)-tic_style.length*0.5+xdispl)
				-text_style.size/2.0*mult;
		else
			point.v=ptops(gtopx(xintercept)+xdispl-tic_style.length*mult)
				-text_style.size/2.0*mult;
		point.h=ptops(gtopy(ypos));
		if (!just && mult>0.0) just=TC;
    else if (!just && mult<0.0) just=BC;
	}
	text_point(text,point,text_style,just,rotation);
	if (logerror) return 1;
	return 0;

}


void ylabel(double ypos,double dval,justtype just,double rotation) {

	double size;
	int places;
  char buffer[50];
	dprint(buffer,dval,1,usernumformat,num_style);
	ylabel(ypos,buffer,just,rotation);
	return;
	
}


void ylabel(double ypos,long lval,justtype just,double rotation) {

	char buffer[50];
	dprint(buffer,(double)lval,0,usernumformat,num_style);
	ylabel(ypos,buffer,just,rotation);

}


void ylabel(double ypos,int ival,justtype just,double rotation) {

	ylabel(ypos,(long)ival,just,rotation);

}

void ylabel(double ypos,justtype just,double rotation) {

	ylabel(ypos,ypos,just,rotation);

}

void ylabel(double ypos,char* text) {

	ylabel(ypos,text,ylabel_style.just,ylabel_style.rot);

}

void ylabel(double ypos,double dval) {

	ylabel(ypos,dval,ylabel_style.just,ylabel_style.rot);

}

void ylabel(double ypos,long lval) {

	ylabel(ypos,lval,ylabel_style.just,ylabel_style.rot);

}

void ylabel(double ypos,int ival) {

	ylabel(ypos,ival,ylabel_style.just,ylabel_style.rot);

}

void ylabel(double ypos) {

	ylabel(ypos,ypos,ylabel_style.just,ylabel_style.rot);

}


void textformat(int w,int p,int exp) {

	// sets format for numbers printed as text
	// reset by xlabels,ylabels but not xlabel,ylabel or other text functions

	if (w<0) w=0;
	if (w>20) w=30;
	if (p<0) p=0;
	if (p>20) p=30;

	num_style.width=w;
	num_style.places=p;
	num_style.exp=!!exp;
  usernumformat=1;

	return;

}


void textformat(int w,int p) {

	textformat(w,p,0);
}


void text(double x,double y,char* text,justtype just,double rotation) {

	// plots text at given coordinates (page units)
	// axis inversion ignored

	location point;
	if (axinverse) {
		point.h=ptops(x);
		point.v=ptops(y);
	}
	else {
		point.h=ptops(x);
		point.v=ptops(y);
	}
	text_point(text,point,text_style,just,rotation);
	return;

}


void text(double x,double y,double dval,justtype just,double rotation) {

	double size;
	int places;
	char buffer[50];
	dprint(buffer,dval,1,usernumformat,num_style);
	text(x,y,buffer,just,rotation);
	return;

}


void text(double x,double y,long longval,justtype just,double rotation) {

	char buffer[50];
	dprint(buffer,(double)longval,0,usernumformat,num_style);
	text(x,y,buffer,just,rotation);
	return;

}


void text(double x,double y,int ival,justtype just,double rotation) {

	text(x,y,(long)ival,just,rotation);
	return;

}


void text(double x,double y,char* title,justtype just) {

	text(x,y,title,just,0);
	return;

}


void text(double x,double y,long lval,justtype just) {

	text(x,y,lval,just,0);
	return;

}


void text(double x,double y,double dval,justtype just) {

	text(x,y,dval,just,0);
	return;

}


void text(double x,double y,int ival,justtype just) {

	text(x,y,ival,just,0);
	return;

}


void text(double x,double y,char* title) {

	text(x,y,title,BL,0);
	return;

}


void text(double x,double y,long lval) {

	text(x,y,lval,BL,0);
	return;

}


void text(double x,double y,double dval) {

	text(x,y,dval,BL,0);
	return;

}


void text(double x,double y,int ival) {

	text(x,y,ival,BL,0);
	return;

}


void title(char* title,double displ,justtype just) {

	// prints a title above the current graph
	// displ allows adjustment of the automatic position up or down
	// just (LEFT,RIGHT,CENTRE) determines where title is printed
	// relative to the current graph area

	double xpos,ypos;
	double left,right;

	if (graphbl.h<graphtr.h) {
		left=graphbl.h;
		right=graphtr.h;
	}
	else {
		left=graphtr.h;
		right=graphbl.h;
	}

	if (just==TL || just==BL || just==CL) {
		just=LEFT;
		xpos=left;
	}
	else if (just==TR || just==CR || just==BR) {
		just=RIGHT;
		xpos=right;
	}
	else {
		just=CENTRE;
		xpos=(left+right)/2.0;
	}

	if (graphbl.v<graphtr.v) ypos=graphtr.v;
	else ypos=graphbl.v;

	ypos+=text_style.size/DEFAULT_PAGESCALE+displ;
	text(xpos,ypos,title,just,0.0);
	return;

}

void title(char* text) {

	title(text,0.0,AUTOJUST);
}

void title(char* text,double displ) {

	title(text,displ,AUTOJUST);
}

void title(char* text,justtype just) {

	title(text,0.0,just);
}

void xtitle(char* title,double ydispl,justtype just) {

	// labels xaxis

	double mult,rot,yo;
	double yintercept=xaxispars.intercept;

	if (just==CENTRE) just=CC;
	else if (just==LEFT) just=CL;
	else if (just==RIGHT) just=CR;
	mult=1.0;
	rot=90.0;
	if (axinverse) {
		if ((gtopy(yintercept)+ydispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0) {
			mult=-1.0;
			rot=-90.0;
		}
		yo=gtopy(yintercept)+ydispl-(tic_style.length*2.0+
			text_style.size/pagescale*3.0)*mult;
		if (just==CC || just==TC || just==BC)
			text(yo,(graphtr.v-graphbl.v)/2.0+graphbl.v,title,just,rot);
		else {
			if (just==TL || just==CL || just==BL) {
				if (graphtr.v>graphbl.v)
					text(yo,graphbl.v,title,just,rot);
				else
					text(yo,graphtr.v,title,just,rot);
			}
			else { // right justified
				if (graphtr.v>graphbl.v)
					text(yo,graphtr.v,title,just,rot);
				else
					text(yo,graphbl.v,title,just,rot);
			}
		}
	}
	else {
		if ((gtopy(yintercept)+ydispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		yo=gtopy(yintercept)+ydispl-(tic_style.length*2.0+
			text_style.size/pagescale*3.0)*mult;
		if (just==CC || just==TC || just==BC)
			text((graphtr.h-graphbl.h)/2.0+graphbl.h,yo,title,just,0.0);
		else {
			if (just==TL || just==CL || just==BL) {
				if (graphtr.h>graphbl.h)
					text(graphbl.h,yo,title,just,0.0);
				else
					text(graphtr.h,yo,title,just,0.0);
			}
			else { // right justified
				if (graphtr.h>graphbl.h)
					text(graphtr.h,yo,title,just,0.0);
				else
					text(graphbl.h,yo,title,just,0.0);
			}
		}
	}
	xaxispars.displ=ydispl;
	return;
}



void xtitle(char* title,justtype just) {

	xtitle(title,xaxispars.displ,just);
	return;
}

void xtitle(char* title,double ydispl) {

	xtitle(title,ydispl,CENTRE);
}


void xtitle(char* title) {

	xtitle(title,xaxispars.displ,CENTRE);
	return;
}




void ytitle(char* title,double xdispl,justtype just) {

	// labels yaxis

	double mult,rot,xo;
	double xintercept=yaxispars.intercept;

	if (just==CENTRE) just=CC;
	else if (just==LEFT) just=CL;
	else if (just==RIGHT) just=CR;
	mult=1.0;
	rot=90.0;
	if (!axinverse) {
		if ((gtopx(xintercept)+xdispl-graphbl.h)>(graphtr.h-graphbl.h)/2.0) {
			mult=-1.0;
			rot=-90.0;
		}
		xo=gtopx(xintercept)+xdispl-(tic_style.length*2.0+text_style.size/
			pagescale*3.0)*mult;
		if (just==CC || just==TC || just==BC)
			text(xo,(graphtr.v-graphbl.v)/2.0+graphbl.v,title,just,rot);
		else {
			if (just==TL || just==CL || just==BL) {
				if (graphtr.v>graphbl.v)
					text(xo,graphbl.v,title,just,rot);
				else
					text(xo,graphtr.v,title,just,rot);
			}
			else { // right justified
				if (graphtr.v>graphbl.v)
					text(xo,graphtr.v,title,just,rot);
				else
					text(xo,graphbl.v,title,just,rot);
			}
		}
	}
	else {
		if ((gtopx(xintercept)+xdispl-graphbl.v)>(graphtr.v-graphbl.v)/2.0)
			mult=-1.0;
		xo=gtopx(xintercept)+xdispl-(tic_style.length*2.0+text_style.size/
			pagescale*3.0)*mult;
		if (just==CC || just==TC || just==BC)
			text((graphtr.h-graphbl.h)/2.0+graphbl.h,xo,title,just,0.0);
		else {
			if (just==TL || just==CL || just==BL) {
				if (graphtr.h>graphbl.h)
					text(graphbl.h,xo,title,just,0.0);
				else
					text(graphtr.h,xo,title,just,0.0);
			}
			else { // right justified
				if (graphtr.h>graphbl.h)
					text(graphtr.h,xo,title,just,0.0);
				else
					text(graphbl.h,xo,title,just,0.0);
			}
		}
	}
	yaxispars.displ=xdispl;
	return;
}


void ytitle(char* title,justtype just) {

	ytitle(title,yaxispars.displ,just);
	return;
}

void ytitle(char* title,double xdispl) {

	ytitle(title,xdispl,CENTRE);
}


void ytitle(char* title) {

	ytitle(title,yaxispars.displ,CENTRE);
	return;
}


void newlegend(double posh,double posv,int flags) {

	legend_style.orig.h=legend_style.start.h=posh;
	legend_style.orig.v=legend_style.start.v=posv;
	legend_style.flags=flags;
	return;
}


void newlegend(double posh,double posv) {

	newlegend(posh,posv,legend_style.flags);
}


void legendstyle(double width,double height,double spacing,
	int direction,double border,double textoffset,justtype just) {

	// sets legend style parameters.  If border<0, no border to legend boxes

	if (width<0.0) width=0.0;
	if (width>100.0) width=100.0;
	legend_style.width=width;
	if (height<0.0) height=0.0;
	if (height>100.0) height=100.0;
	legend_style.height=height;
	if (spacing<0.0) spacing=0.0;
	if (spacing>100.0) spacing=100.0;
	legend_style.spacing=spacing;
	legend_style.direction=!!direction;
	if (border<0.0) legend_style.border=0;
	else {
		if (border>10.0) border=10.0;
		legend_style.border=1;
		legend_style.bwidth=border;
	}
	if (fabs(textoffset)>100.0) textoffset=100.0*sign(textoffset);
	legend_style.tdisp=textoffset;
	legend_style.just=just;
	return;

}

void legendstyle(double width,double height,double spacing,
	int direction,double border,double textoffset) {

	legendstyle(width,height,spacing,direction,border,textoffset,CL);

}


void legend(int flags,char* text) {

	// plots next entry into legend

	location s,f,outline[5];
	int i;
	double linewidth,elinewidth,bw=legend_style.bwidth/2.0;
	double red,green,blue;
	long colour_style_bak;

	legend_style.flags=flags;

	if (axinverse) {
		outline[0].h=ptops(legend_style.orig.h);
		outline[0].v=ptops(legend_style.orig.v);
	}
	else {
		outline[0].h=ptops(legend_style.orig.h);
		outline[0].v=ptops(legend_style.orig.v);
	}
	outline[1].h=outline[0].h+legend_style.width*pagescale;
	outline[1].v=outline[0].v;
	outline[2].h=outline[1].h;
	outline[2].v=outline[0].v-legend_style.height*pagescale;
	outline[3].h=outline[0].h;
	outline[3].v=outline[2].v;
	outline[4].h=outline[0].h;
	outline[4].v=outline[0].v;
	if (flags&PATTERN) fillpoly(5,outline,0,pattern_style);
	if (legend_style.border) {
		psout("gsave\n");
		psout("0 setgray\n");
		linewidth=set_linewidth(legend_style.bwidth);
		for (i=0;i<4;i++) {
			s.h=outline[i].h; //-bw*(double)(!i)+bw*(double)(i==2);
			s.v=outline[i].v;
			f.h=outline[i+1].h; //+bw*(double)(!i)-bw*(double)(i==2);
			f.v=outline[i+1].v;
			filline(s,f);
		}
		set_linewidth(linewidth);
		psout("grestore\n");
	}
	//outline[0].v-=legend_style.height*pagescale*0.5;
	//outline[1].v=outline[0].v;
	if (flags&LINE) {
		psout("gsave\n");
		psout("1 setgray\n");
		psout("newpath\n");
		pc(outline[0].h,outline[0].v,MOV);
		pc(outline[1].h,outline[1].v,LIN);
		pc(outline[2].h,outline[2].v,LIN);
		pc(outline[3].h,outline[3].v,LIN);
		psout("clip\n");

		if (colour_style) {
			colour_style_bak=colour_style;
			red=(double)(colour_style>>16)/255.0;
			green=(double)((colour_style&0x00FF00)>>8)/255.0;
			blue=(double)(colour_style&0x0000FF)/255.0;
			colour(red,green,blue);
		}
		else psout("0 setgray\n");
		outline[0].v-=legend_style.height*pagescale*0.5;
		//outline[0].h-=legend_style.width;
		outline[1].v=outline[0].v;
		outline[1].h+=legend_style.width;
		psout("newpath\n");
		curve_point(2,outline,line_style);
		//outline[0].h+=legend_style.width;
		psout("grestore\n");
	}
	else {
		outline[0].v-=legend_style.height*pagescale*0.5;
		outline[1].v=outline[0].v;
	}
	if (flags&ERROR) {
		elinewidth=set_linewidth(error_style.linewidth);
		if (!axinverse && ((error_style.flags&HIGH) &&
			graphtr.v>graphbl.v || (error_style.flags&LOW) &&
			graphtr.v<=graphbl.v) || axinverse && ((error_style.flags&HIGH)
			&& graphtr.h>graphbl.h || (error_style.flags&LOW) &&
			graphtr.h<=graphbl.h)) {
			s.h=ptops(legend_style.orig.h+legend_style.width*0.5);
			s.v=outline[0].v;
			f.h=s.h;
			f.v=outline[0].v+legend_style.height*pagescale*0.33;
			filline(s,f);
			s.v=f.v;
			s.h=s.h-error_style.hatwidth*0.5;
			f.h=s.h+error_style.hatwidth;
			filline(s,f);
		}
		if (!axinverse && ((error_style.flags&LOW) && graphtr.v>graphbl.v ||
			(error_style.flags&HIGH) && graphtr.v<=graphbl.v) || axinverse &&
			((error_style.flags&LOW) && graphtr.h>graphbl.h ||
			(error_style.flags&HIGH) && graphtr.h<=graphbl.h)) {
			s.h=ptops(legend_style.orig.h+legend_style.width*0.5);
			s.v=outline[0].v;
			f.h=s.h;
			f.v=outline[0].v-legend_style.height*pagescale*0.33;
			filline(s,f);
			s.v=f.v;
			s.h=s.h-error_style.hatwidth*0.5;
			f.h=s.h+error_style.hatwidth;
			filline(s,f);
		}
		set_linewidth(elinewidth);
	}
	outline[0].h+=legend_style.width*pagescale*0.5;
	if (flags&MARKER) {
		scatter_point(1,outline,marker_style);
	}
	outline[0].h+=(legend_style.width*0.5+legend_style.tdisp)*pagescale;
	psout("gsave\n");
	psout("0.0 setgray\n");
	text_point(text,outline[0],text_style,legend_style.just,0.0);
	psout("grestore\n");
	if (legend_style.direction)
		legend_style.orig.h+=legend_style.width+legend_style.spacing;
	else
		legend_style.orig.v-=legend_style.height+legend_style.spacing;
	legend_style.flags=0;
	return;

}


void legend(char* text) {

	legend(legend_style.flags,text);
}


void legendtext(char* text,double sbefore,double safter) {

	// adds a line of text to legend, with extra spacing given by
	// sbefore and safter
	// NB: always vertical, regardless of legend_style.direction

	location origin;
	if (legend_style.direction) {
		legend_style.orig.h=legend_style.start.h;
		legend_style.orig.v-=legend_style.height+sbefore+
			text_style.size/pagescale*0.5;
	}
	else
		legend_style.orig.v-=sbefore+text_style.size/pagescale*0.5-
			legend_style.spacing;
		if (axinverse) {
			origin.h=ptops(legend_style.orig.h);
			origin.v=ptops(legend_style.orig.v);
		}
		else {
			origin.h=ptops(legend_style.orig.h);
			origin.v=ptops(legend_style.orig.v);
		}
	text_point(text,origin,text_style,CL,0.0);
	legend_style.orig.v-=safter+text_style.size/pagescale*0.5;
	return;

}


void legendtext(char* text) {

	if (legend_style.direction)
		legendtext(text,legend_style.height*0.5,legend_style.height*0.5);
	else
		legendtext(text,legend_style.spacing,legend_style.spacing);
	return;

}


void arrow(double sx,double sy,double fx,double fy) {

	// an arrow from sx,sy to fx,fy using current _style

	location corner[9];
	double proj,hangle,hwidth,hlength,z,sidelength,xmid,ymid,lwidth;
	double wdx,wdy,mult;
	int npoint,i;
	patterndef pat;

	if (axinverse) {
		sy=ptops(sy);
		sx=ptops(sx);
		fy=ptops(fy);
		fx=ptops(fx);
	}
	else {
		sx=ptops(sx);
		sy=ptops(sy);
		fx=ptops(fx);
		fy=ptops(fy);
	}
	if (sx==fx) {
		proj=PI*0.5;
	}
	else proj=fabs(atan(fabs(fy-sy)/fabs(fx-sx)));
	mult=1.0;  // quadrant 1
	if (sy<fy && sx>=fx) {  //quadrant 2
		proj=PI-proj;
		mult=1.0;
	}
	else {
		if (sy>=fy && sx>=fx) proj=PI+proj;  //quadrant 3
		else {
			if (sy>=fy && sx<fx) {
				proj=2*PI-proj;
				mult=1.0;
			}
		}
	}
	hangle=arrow_style.angle/114.5915573; // half head angle in radians
	hlength=sqrt((fx-sx)*(fx-sx)+(fy-sy)*(fy-sy)); // length of arrow
	if (arrow_style.angle<1.0) {  // a straight line
		wdx=-arrow_style.lwidth*0.5*cos(PI*0.5-proj)*mult;
		wdy=arrow_style.lwidth*0.5*sin(PI*0.5-proj)*mult;
		corner[0].h=sx-wdx;
		corner[0].v=sy-wdy;
		corner[1].h=fx-wdx;
		corner[1].v=fy-wdy;
		corner[2].h=fx+wdx;
		corner[2].v=fy+wdy;
		corner[3].h=sx+wdx;
		corner[3].v=sy+wdy;
		corner[4].h=corner[0].h;
		corner[4].v=corner[0].v;
		npoint=5;
	}
	else {	// an arrow
		// first find theoretical length of arrow head
		z=arrow_style.hwidth/2.0/tan(hangle);
		if (z>hlength) z=hlength;
		// now find known length of each side of arrow
		sidelength=z/cos(hangle);
		corner[0].h=fx;
		corner[0].v=fy;
		corner[1].h=corner[0].h-sidelength*cos(proj-hangle);
		corner[1].v=corner[0].v-sidelength*sin(proj-hangle);
		// find midpoint of line linking side corners of arrow head
		hwidth=sidelength*sin(hangle); // actual half-arrow head width
		if (hwidth*2.0<arrow_style.lwidth) lwidth=hwidth;
		else lwidth=arrow_style.lwidth*0.5;	// get half linewidth
		xmid=corner[1].h+hwidth*cos(PI*0.5-proj)*mult;
		ymid=corner[1].v-hwidth*sin(PI*0.5-proj)*mult;
		// from midpoint, out to edge of line
		wdx=-lwidth*cos(PI*0.5-proj)*mult;
		wdy=lwidth*sin(PI*0.5-proj)*mult;
		corner[2].h=xmid+wdx;
		corner[2].v=ymid+wdy;
		corner[3].h=sx+wdx;
		corner[3].v=sy+wdy;
		corner[4].h=sx-wdx;
		corner[4].v=sy-wdy;
		corner[5].h=xmid-wdx;
		corner[5].v=ymid-wdy;
		corner[6].h=corner[1].h+hwidth*2.0*cos(PI*0.5-proj)*mult;
		corner[6].v=corner[1].v-hwidth*2.0*sin(PI*0.5-proj)*mult;
		corner[7].h=corner[0].h;
		corner[7].v=corner[0].v;
		corner[8]=corner[1];
		npoint=9;
	}

	// now plot and fill it

	pat.pattern=GREY;
  pat.lwidth=1.0;
	if (arrow_style.filled) {
		pat.grey=set_grey(1.0);
		set_grey(pat.grey);
		fillpoly(npoint,corner,0,pat);
	}
	else {
		pat.grey=1.0;
		fillpoly(npoint,corner,1,pat);
	}
	return;
}


double arrowstyle(double width,double headwidth,double angle,int fill) {

	// sets arrow style, returning old width

	double old=arrow_style.lwidth;
	if (width<0.0) width=0.0;
	if (width>100.0) width=100.0;
	if (headwidth<0.0) headwidth=0.0;
	if (headwidth>100.0) headwidth=100.0;
	if (angle<0.0) angle=0.0;
	if (angle>179.0) angle=179.0;
	fill=!!fill;
	arrow_style.lwidth=width;
	arrow_style.hwidth=headwidth;
	arrow_style.angle=angle;
	arrow_style.filled=fill;
	return old;

}


double arrowstyle(double width,int fill) {

	double hwidth,angle;
	if (width<0.5) {
		hwidth=2.0;
		angle=40.0;
	}
	else {
		hwidth=width*(1.3+2.7*(100.0-width)/100.0);
		angle=40.0+70.0*width/100.0;
	}
	return arrowstyle(width,hwidth,angle,fill);

}

double arrowstyle(double width) {

	return arrowstyle(width,1);

}


double arrowstyle() {

	return arrow_style.lwidth;
}


void marker(double h,double v) {

	// plots current marker symbol at (h,v) (page units)

	location point[1];
	point[0].h=ptops(h);
	point[0].v=ptops(v);
	scatter_point(1,point,marker_style);
	return;

}


void line(double h1,double v1,double h2,double v2) {

	// plots a line from (h1,v1) to (h2,v2) using current line symbol
	// coordinates are in page units

	location point[2];
	point[0].h=ptops(h1);
	point[0].v=ptops(v1);
	point[1].h=ptops(h2);
	point[1].v=ptops(v2);
	curve_point(2,point,line_style);
	return;

}

void pattern(double blh,double blv,double trh,double trv) {

	// plots current pattern as a block with bottom left corner
	// (blh,blv) and top right corner (trh,trv) (page units)

	location point[5];
	point[0].h=point[3].h=point[4].h=ptops(blh);
	point[0].v=point[1].v=point[4].v=ptops(blv);
	point[1].h=point[2].h=ptops(trh);
	point[2].v=point[3].v=ptops(trv);
	fillpoly(5,point,0,pattern_style);
	return;

}

void rectangle(double blh,double blv,double trh,double trv) {

	// Equivalent to pattern, but assumes rectangle is to be filled
	// with ink at full density in the current colour. Results in smaller
	// PostScript files than pattern

	psout("gsave\n");
	set_grey_internal(1.0);
	psout("newpath\n");
	pc(ptops(blh),ptops(blv),MOV);
	pc(ptops(blh),ptops(trv),LIN);
	pc(ptops(trh),ptops(trv),LIN);
	pc(ptops(trh),ptops(blv),LIN);
	pc(ptops(blh),ptops(blv),LIN);
	psout("fill\n");
	psout("grestore\n");
}


void transparent(int i) {

	set_transparent(i);

}


void transparent() {

	set_transparent(1);
}


double outline(double d) {

	// sets outlining of histogram boxes
	// if d<0, no outline, otherwise d is linewidth for outline

	double old=outlinepats;
	if (fabs(d)<100.0) outlinepats=d;
	else outlinepats=100.0*sign(outlinepats);
	return old;

}

double outline() {

	return outlinepats;
}


void newpage() {

	if (iseps) next_page();
	return;

}


void testpage() {

	// produces a test page of bengraph marker symbols, line styles,
	// pattern styles, fonts and character symbols
	// to be called without an initial call to startbengraph
	// output file is called TESTPAGE.EPS
	// A4 page is assumed (units mm)

	int i,z;
	double c,r;
  char buf[10],buf2[10];
	pagescale=2.829;
	startbengraph("TESTPAGE");
	textfont(HELVETICA);
	textsize(10);
	textbold(0);
	textitalic(0);
	patternstyle(1);
	pattern(10,280,200,290);
	text(105.0,285.0,"!B!S(14)Key to symbols and styles",CC);
	r=265.0;
	text(10.0,r,"!BMARKER SYMBOLS");
	c=0.0;
	r-=11.0;
	for (i=0;i<N_MARKER;i++) {
		markerstyle(i);
		marker(12.0+c*19.0,r);
		text(20.0+c*19.0,r,i,CR);
		if (++c>9.0) {
			c=0.0;
			r-=10.0;
		}
	}
	if (!c) r+=10.0;
	r-=17.0;
	c=0.0;
	text(10.0,r,"!BLINE STYLES");
	r-=10.0;
	for (i=0;i<N_LINE;i++) {
		linestyle(i);
		line(11.0+c*38.0,r,33.0+c*38.0,r);
		text(40.0+c*38.0,r,i,CR);
		if (++c>4.0) {
			c=0.0;
			r-=10.0;
		}
	}
	if (!c) r+=10.0;
	r-=16.0;
	c=0.0;void colour(double red,double green,double blue);

	text(10.0,r,"!BPATTERN STYLES");
	r-=13.0;
	for (i=0;i<N_LINE;i++) {
		patternstyle(i);
		pattern(11.0+c*19.0,r-5.0,21.0+c*19.0,r+5.0);
		linestyle(0);
		linewidth(0.5);
		line(11.0+c*19.0,r-5.0,11.0+c*19.0,r+5.0);
		line(11.0+c*19.0,r-5.0,21.0+c*19.0,r-5.0);
		line(11.0+c*19.0,r+5.0,21.0+c*19.0,r+5.0);
		line(21.0+c*19.0,r+5.0,21.0+c*19.0,r-5.0);
		text(26.0+c*19.0,r,i,CR);
		if (++c>9.0) {
			c=0.0;
			r-=15.0;
		}
	}
	if (!c) r+=15.0;
	r-=20.0;
	text(10.0,r,"!BTEXT FONTS");
	r-=10.0;
  text(10.0,r,"!S(12)!F(Helvetica)Helvetica !BBold !P!IItalic!P     !F(Times)Times !BBold !P!IItalic!P     !F(Courier)Courier !BBold !P!IItalic");
	r-=15.0;
	text(10.0,r,"!BSYMBOLS CHARACTER SET");
	r-=10.0;
	text(22,r,0);
	text(52,r,5);
	text(81,r,10);
	text(111,r,15);
	text(141,r,20);
	text(171,r,25);
	r-=3;
	linewidth(0);
	for (i=0;i<9;i++) {
		line(20,r-i*6,200,r-i*6);
	}
	for (i=0;i<31;i++) {
		line(20+i*6,r,20+i*6,r-48);
	}
	r-=3.0;
	c=35.0;
	z=30;
	text(17,r,z,CR);
	for (i=32;i<255;i++) {
		sprintf(buf,"%d",i);
		strcpy(buf2,"!C(");
		strcat(buf2,buf);
		strcat(buf2,")");
		text(c,r,buf2,CC);
		c+=6.0;
		if (c>200.0) {
			r-=6.0;
			c=23.0;
			z+=30;
			text(17,r,z,CR);
		}
	}
	bgend();
  return;

}


static void set_colourscale(colourscaledef& cs,colourscaletype type) {

	switch (type) {
		
		case DITHER:
			cs.pflexcolours=NULL;
			break;
		case RED:
			cs.pflexcolours=fcd_red;
			break;
		case BLUE:
			cs.pflexcolours=fcd_blue;
			break;
		case GREEN:
			cs.pflexcolours=fcd_green;
			break;
		case SHADE:
			cs.pflexcolours=fcd_grey;
			break;
		case REDVIOLET:
			cs.pflexcolours=fcd_redviolet;
			break;
		case VIOLETRED:
			cs.pflexcolours=fcd_violetred;
			break;
		case REDBLUE:
			cs.pflexcolours=fcd_redblue;
			break;
		case BLUERED:
			cs.pflexcolours=fcd_bluered;
			break;
		case REDCYAN:
			cs.pflexcolours=fcd_redcyan;
			break;
		case CYANRED:
			cs.pflexcolours=fcd_cyanred;
			break;
		case GREYCYAN:
			cs.pflexcolours=fcd_greycyan;
			break;
		case GREYRED:
			cs.pflexcolours=fcd_greyred;
			break;
		case COOL:
			cs.pflexcolours=fcd_cool;
			break;
		case WARM:
			cs.pflexcolours=fcd_warm;
			break;
		case NEEGREEN:
			cs.pflexcolours=fcd_neegreen;
			break;
		case NEERED:
			cs.pflexcolours=fcd_neered;
			break;
		case ALTITUDE:
			cs.pflexcolours=fcd_altitude;
			break;
		case DEPTH:
			cs.pflexcolours=fcd_depth;
			break;
		case SOIL:
			cs.pflexcolours=fcd_soil;
			break;
		case VEGETATION:
			cs.pflexcolours=fcd_vegetation;
			break;
		case FIRE:
			cs.pflexcolours=fcd_fire;
			break;
		case WET:
			cs.pflexcolours=fcd_wet;
			break;
		case DRY:
			cs.pflexcolours=fcd_dry;
			break;
		default:
			// Should never reach here
			printf("set_colourscale: invalid colourscaletype %d\n",type);
	};
}

void colourscale(colourscaletype type) {
	
	set_colourscale(colourscale_curr,type);
	datascale_curr.dual=false;
}

void colourscale(colourscaletype tneg,colourscaletype tpos) {
	
	set_colourscale(colourscale_curr,tpos);
	set_colourscale(colourscale_curr_dual,tneg);
	datascale_curr.dual=true;
}


void colourscale(flexcolourdef* c) {

	colourscale_curr.pflexcolours=c;
	datascale_curr.dual=false;
}

void colourscale(flexcolourdef* cneg,flexcolourdef* cpos) {

	colourscale_curr.pflexcolours=cpos;
	colourscale_curr_dual.pflexcolours=cneg;
	datascale_curr.dual=true;
}

void setcolourkey(colourkeydef* c,int nclass,bool ifrgb) {

	int i;
	double red,green,blue;
	
	pcolourkey=c;
	ncolourkey=nclass;
	
	if (!ifrgb) { // Convert colour values from HSV to RGB
		for (i=0;i<nclass;i++) {
			hlstorgb(c[i].colour[0],c[i].colour[1],c[i].colour[2],red,green,blue);
			pcolourkey[i].colour[0]=red;
			pcolourkey[i].colour[1]=green;
			pcolourkey[i].colour[2]=blue;
		}
	}
}

void forgetcolourkey() {

	pcolourkey=NULL;
	ncolourkey=0;
}

void frame() {

	// Plots frame around current graph (or map)

	line(graphbl.h,graphbl.v,graphbl.h,graphtr.v);
	line(graphbl.h,graphtr.v,graphtr.h,graphtr.v);
	line(graphtr.h,graphtr.v,graphtr.h,graphbl.v);
	line(graphtr.h,graphbl.v,graphbl.h,graphbl.v);
}
