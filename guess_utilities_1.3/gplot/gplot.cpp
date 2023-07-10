////////////////////////////////////////////////////////////////////////////////////////
// GPLOT
// Time series visualisation utility for LPJ-GUESS
//
// Written by Ben Smith
// This version dated 2008-06-19
//
// gplot -help for documentation 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <gutil.h>
#include <bengraph.h>
#include <math.h>
#include "gplot_path.h"

// Graph position on page in mm from left hand corner (portrait mode, no secondary axis)
const double PLEFT=40;
const double PBOTTOM=110;
const double PRIGHT=150;
const double PTOP=200;

// Graph position on page in mm from left hand corner (portrait mode, secondary axis)
const double PSLEFT=40;
const double PSBOTTOM=110;
const double PSRIGHT=170;
const double PSTOP=200;

// Graph position with inverse axes, portrait mode
const double PILEFT=40;
const double PIBOTTOM=90;
const double PIRIGHT=150;
const double PITOP=220;

// Map position on page in mm from left hand corner (landscape mode, no secondary axis)
const double LLEFT=40;
const double LBOTTOM=50;
const double LRIGHT=210;
const double LTOP=170;

// Map position on page in mm from left hand corner (landscape mode, secondary axis)
const double LSLEFT=40;
const double LSBOTTOM=50;
const double LSRIGHT=190;
const double LSTOP=170;

// Graph position with inverse axes, landscape
const double LILEFT=60;
const double LIBOTTOM=50;
const double LIRIGHT=190;
const double LITOP=170;

const double OTHRESH=1.51; // threshold aspect ratio for landscape mode

// Legend offset
const double LEGDX=10;
const double LEGDY=35;
const double LEGWD=50;
const double LEGHT=50;

const int MAXREC=100000; // maximum number of records permitted
const int MAXITEM=1024; // maximum number of items in input file
const int MAXYITEM=18; // maximum number of Y items to plot on a single axis
const int MAXLEGEND=1000; // maximum number of legend items

// Bengraph options
const double MARKERSIZE=6;
const double TICSIZE=2;
const double TEXTSIZE_LABELS=12;
const double TEXTSIZE_TITLES=14;
const double LINEWIDTH=1;
const double LEGSPACING=2;
const double AXOFFSET=5;

const struct {
		double red;
		double green;
		double blue;
	} colours[MAXYITEM]= {
		0,0,0,       // 1 = black
		1,0,0,       // 2 = red
		0,0,1,       // 3 = blue
		0,1,0,       // 4 = green
		1,1,0,       // 5 = yellow
		0,1,1,       // 6 = cyan
		1,0,1,       // 7 = magenta
		0.5,0.5,0.5, // 8 = grey
		1,0.5,0,     // 9 = orange
		0,0.6,0,     // 10 = dk green
		0.6,0,1,     // 11 = lilac
		1,0.5,0.5,   // 12 = pink
		0.5,0.8,0.8, // 13 = turquoise
		0.8,0.8,0.8, // 14 = lt grey
		0.1,0.5,0.7, // 15 = prussian
		0.8,0.8,0.5, // 16 = gold
		1,0,0.5,     // 17 = carmine
		0.7,0.4,0.3  // 18 = brown
	};
	
struct Legpars {

	int id;
	xtring label;
	double red,green,blue;
	double hue,lightness,saturation;
	int marker,line,pattern;
	double markersize,linewidth,patternscale;
};
	
		
const int MAXPATH=1024; // Maximum string length for pathname
const char TEMPFILE[]="$GPTEMP00.ps";

// Global data

double ydata[MAXYITEM][MAXREC];
double xdata[MAXREC];


void regress(double* x,double* y,int n,double& a,double& b,double& r) {

	// Performs a linear regression of array y on array x (n values)
	// returning parameters a and b in the fitted model: y=a+bx
	// (Used by function soiltemp)
	// Source: Press et al 1986, Sect 14.2
	
	// Now also returns Pearson r

	int i;
	double sx,sy,sxx,sxy,delta,meanx,meany,sdx,sdy;

	sx=0.0;
	sy=0.0;
	sxx=0.0;
	sxy=0.0;
	for (i=0;i<n;i++) {
		sx+=x[i];
		sy+=y[i];
		sxx+=x[i]*x[i];
		sxy+=x[i]*y[i];
	}
	delta=(double)n*sxx-sx*sx;
	a=(sxx*sy-sx*sxy)/delta;
	b=((double)n*sxy-sx*sy)/delta;
	
	meanx=sx/(double)n;
	meany=sy/(double)n;
	sdx=sdy=0.0;
	for (i=0;i<n;i++) {
		sdx+=(x[i]-meanx)*(x[i]-meanx);
		sdy+=(y[i]-meany)*(y[i]-meany);
	}
	sdx=sqrt(sdx/double(n-1));
	sdy=sqrt(sdy/double(n-1));
	if (!sdx || !sdy) r=0.0;
	else r=b*sdx/sdy;
}

void getpwr(double v,double& pwr,double& base) {

	double sign,mag;
	
	if (!v) {
		pwr=1;
		base=0;
	}
	
	else {
	
		mag=fabs(v);
		sign=v/mag;
		pwr=floor(log10(mag));
		base=floor(mag/pow(10,pwr))*pow(10,pwr)*sign;
	}	
}

void clip(double left,double right,double top,double bottom,double offset) {
	
	psout("gsave\n");
	//psout("1.0 setgray\n");
	psout("newpath\n");
	pc(ptops(left)-offset,ptops(bottom)-offset,MOV);
	pc(ptops(right)+offset,ptops(bottom)-offset,LIN);
	pc(ptops(right)+offset,ptops(top)+offset,LIN);
	pc(ptops(left)-offset,ptops(top)+offset,LIN);
	pc(ptops(left)-offset,ptops(bottom)-offset,LIN);
	psout("clip\n");
	psout("newpath\n");
}

void grestore() {

	psout("grestore\n");
}


int find_legend_item(xtring label,int id,Legpars legpars[]) {
	
	int i,ct;
	bool abort=false;
	
	for (i=0;i<MAXLEGEND && !abort;i++) {
		
		if (legpars[i].id==-1) abort=true;
		else if (legpars[i].label==label) return i;
	}
	
	abort=false;
	for (i=0;i<MAXLEGEND && !abort;i++) {
		
		if (legpars[i].id==-1) abort=true;
		else if (legpars[i].label.lower()==label.lower()) return i;
	}
	
	ct=0;
	for (i=0;i<MAXLEGEND;i++) {
		
		if (legpars[i].id==-1) return -1; // not found
		else if (legpars[i].label=="" && ct++==id) return i;
	}
	
	// Not found
	
	return -1;
}


void plotyitem(double ydata[MAXYITEM][MAXREC],xtring label[MAXITEM],
	int ydataitemno[MAXYITEM],int i,bool ifsinglex,bool ifstack,bool ifscatter,bool ifmono,
	double left,double bottom,double right,double top,double msize,int legbox,int legbd,
	double textsiz,Legpars legpars[],bool havelegend,bool dolegend) {

	int legid;
	
	// printf("Plotting item %d %s\n",i,(char*)label[ydataitemno[i]]);
	if (havelegend) {
			
		legid=find_legend_item(label[ydataitemno[i]],i,legpars);
		// printf("legid=%d pattern=%d\n",legid,legpars[legid].pattern);
	}
	else legid=-1;
		
	if (ifmono) {
		if (ifsinglex || ifscatter) {
			if (legid>=0) {
				if (legpars[legid].marker!=-1)
					markerstyle(legpars[legid].marker);
				if (legpars[legid].markersize!=-1) {
					msize=legpars[legid].markersize;
					markersize(msize);
				}
			}
			clip(left,right,top,bottom,msize*0.5);
			scatter(i);
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(MARKER,label[ydataitemno[i]]);
		}
		else if (ifstack) {
			//printf("legid=%d i=%d\n",legid,i);
			if (legid>=0) {
				if (legpars[legid].pattern!=-1) {
					//printf("patternstyle=%d\n",legpars[legid].pattern);
					patternstyle(legpars[legid].pattern);
				}
				else
					patternstyle((i+1)%18);
				if (legpars[legid].patternscale!=-1)
					patternscale(legpars[legid].patternscale);
			}
			else patternstyle((i+1)%18);
			clip(left,right,top,bottom,0);
			if (i)
				fillbetween(i,i-1);
			else
				fillunder(i);
			grestore();
			linestyle(0);
			clip(left,right,top,bottom,0);
			curve(i);
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(PATTERN,label[ydataitemno[i]]);
		}
		else {
			if (legid>=0) {
				if (legpars[legid].line!=-1)
					linestyle(legpars[legid].line);
				if (legpars[legid].linewidth!=-1)
					linewidth(legpars[legid].linewidth);
			}
			clip(left,right,top,bottom,0);
			curve(i);
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(LINE,label[ydataitemno[i]]);
		}
	}
	else {
		if (legid>=0) {
			if (legpars[legid].red!=-1 || legpars[legid].green!=-1 || legpars[legid].blue!=-1) {
				if (legpars[legid].red==-1) legpars[legid].red=0;
				if (legpars[legid].green==-1) legpars[legid].green=0;
				if (legpars[legid].blue==-1) legpars[legid].blue=0;
				colour(legpars[legid].red,legpars[legid].green,legpars[legid].blue);
			}
			else if (legpars[legid].hue!=-1) {
				colour(legpars[legid].hue,legpars[legid].lightness,legpars[legid].saturation,1);
			}
			else colour(colours[i].red,colours[i].green,colours[i].blue);
		}
		else colour(colours[i].red,colours[i].green,colours[i].blue);
		linestyle (0);
		if (ifsinglex || ifscatter) {
			if (legid>=0) {
				if (legpars[legid].marker!=-1)
					markerstyle(legpars[legid].marker);
				if (legpars[legid].markersize!=-1) {
					msize=legpars[legid].markersize;
					markersize(msize);
				}
			}
			clip(left,right,top,bottom,msize*0.5);
			scatter(i);
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(MARKER,label[ydataitemno[i]]);
		}
		else if (ifstack) {
			if (legid>=0) {
				if (legpars[legid].pattern!=-1) {
					//printf("patternstyle=%d\n",legpars[legid].pattern);

					patternstyle(legpars[legid].pattern);
				}
				else
					patternstyle(3);
				if (legpars[legid].patternscale!=-1)
					patternscale(legpars[legid].patternscale);
			}
			else patternstyle(3);
			linestyle(0);
			//colour(colours[i].red,colours[i].green,colours[i].blue);
			clip(left,right,top,bottom,0);
			//printf("calling fill for %d\n",i);
			if (i)
				fillbetween(i,i-1);
			else
				fillunder(i);
			//printf("Done!\n");
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(PATTERN,label[ydataitemno[i]]);
			clip(left,right,top,bottom,0);
			curve(i);
			grestore();
		}
		else {
			if (legid>=0) {
				if (legpars[legid].line!=-1)
					linestyle(legpars[legid].line);
				if (legpars[legid].linewidth!=-1)
					linewidth(legpars[legid].linewidth);
			}
			clip(left,right,top,bottom,0);
			curve(i);
			grestore();
			legendstyle(legbox,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,legbd,2);
			if (dolegend) legend(LINE,label[ydataitemno[i]]);
		}
		colour(0,0,0);
	}
}


void plotgraph(xtring outfile,int nydataitem,int ny2dataitem,
	int nrec,double ydata[MAXYITEM][MAXREC],double xdata[MAXREC],bool ifmono,
	xtring xtit,xtring ytit,xtring y2tit,xtring tit,bool ifsinglex,
	bool ifsingley,bool ifsingley2,xtring label[MAXITEM],int ydataitemno[MAXYITEM],
	int y2dataitemno[MAXYITEM],int xitemno,bool ifstack,bool ifscatter,
	double textsiz,double linewid,double width,double height,int orient,
	bool ifoffset,double sxlo,double sxhi,double sylo,double syhi,
	double sy2lo,double sy2hi,bool ifinverse,bool iflogx,bool iflogy,bool iflogy2,
	bool ifframe,bool ifmtic,bool iftrend,bool ifeq,
	double& sxr,double& syr,double& sy2r,Legpars legpars[],bool havelegend) {

	int i,j;
	double pwr,base,from,to;
	double left,bottom,right,top;
	double legx,legy,legwd,leght;
	double msize,x,y,r,s,a,b,rr,x1,x2,y1,y2;
	double xd[MAXREC],yd[MAXREC];
	int flags;
	int legbox,legbord,legid;
	
//	startbengraph(outfile);
	
	if (iftrend && ifsinglex)
		printf("Unable to compute trend(s)\n");
	
	if (ifstack) {
		if (!ny2dataitem)
			legbox=11;
			//legendstyle(11,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,1,2);
		else
			legbox=14;
			//legendstyle(14,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,1,2);
		legbord=1;
	}
	else if (ifscatter) {
		//legendstyle(8,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,-1,2);
		legbox=8;
		legbord=-1;
	}
	else {
		//legendstyle(14,5,LEGSPACING/TEXTSIZE_LABELS*textsiz,0,-1,2);
		legbox=14;
		legbord=-1;
	}
		
	msize=MARKERSIZE/TEXTSIZE_LABELS*textsiz;
	markersize(msize);
	ticstyle(TICSIZE/TEXTSIZE_LABELS*textsiz,linewid,0);
	minorticstyle(TICSIZE/TEXTSIZE_LABELS*textsiz*0.66);
	textsize(textsiz);
	linewidth(linewid);

	if (orient==0) {  // automatic page orientation
		if (width/height>OTHRESH && width>160 || width>150 && height<=150) orient=2;
		else orient=1;
	}
	
	if (orient==2) { // landscape mode
		orientation(1);
		if (ifinverse) {
			left=LILEFT;
			right=LIRIGHT;
			top=LITOP;
			bottom=LIBOTTOM;
		}
		else {
			if (ny2dataitem) {
				left=LSLEFT;
				right=LSRIGHT;
				top=LSTOP;
				bottom=LSBOTTOM;
			}
			else {
				left=LLEFT;
				right=LRIGHT;
				top=LTOP;
				bottom=LBOTTOM;
			}
		}
		if (width>0 && height>0) {
			
			if (width>LRIGHT-LLEFT) left=145-width*0.5;
			else left+=(right-left)*0.5-width*0.5;
			right=left+width;
			bottom+=((top-bottom)-height)*0.5;
			top=bottom+height;
		}
	}
	else { // portrait mode
		if (ifinverse) {
			left=PILEFT;
			right=PIRIGHT;
			top=PITOP;
			bottom=PIBOTTOM;
		}
		else {
			if (ny2dataitem) {
				left=PSLEFT;
				right=PSRIGHT;
				top=PSTOP;
				bottom=PSBOTTOM;
			}
			else {
				left=PLEFT;
				right=PRIGHT;
				top=PTOP;
				bottom=PBOTTOM;
			}
		}
		if (width>0 && height>0) {
			
			if (width>PRIGHT-PLEFT) left=110-width*0.5;
			else left+=(right-left)*0.5-width*0.5;
			right=left+width;
			bottom+=((top-bottom)-height)*0.5;
			top=bottom+height;
		}
	}
	
	if (width>0 && height>0) {
		
		left+=((right-left)-width)*0.5;
		right=left+width;
		bottom+=((top-bottom)-height)*0.5;
		top=bottom+height;
	}
	
	// Legend position
	
	legwd=LEGWD/TEXTSIZE_LABELS*textsiz;
	leght=LEGHT/TEXTSIZE_LABELS*textsiz;
	
	if (orient==1 && right<210-legwd || orient==2 && right<295-legwd) { // legend to right of graph
		legx=right+LEGDX/TEXTSIZE_LABELS*textsiz;
		legy=bottom+(top-bottom)/2.0+leght/2.0;
		if (legy>top) legy=top;
		if (ny2dataitem && !ifinverse) legx+=AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset+textsiz*2;
	}
	else { // legend underneath graph
		legx=left+(right-left)/2.0-legwd/2.0;
		if (legx>295-legwd) legx=295-legwd;
		legy=bottom-LEGDY/TEXTSIZE_LABELS*textsiz;
		if (legy<leght) legy=leght;
	}
	
	flags=0;
	if (ifinverse) flags=INVERSEAXES;
	if (iflogx && !ifsinglex) flags|=LOGX;
	if (iflogy && !ifsingley) flags|=LOGY;
		
	newgraph(left,bottom,right,top,flags);
	newlegend(legx,legy);
	
	// Data for primary y-axis
	
	for (i=0;i<nydataitem;i++) {
		data(nrec,xdata,ydata[i]);
	}
	
	if (ifsinglex) {
	
		getpwr(xdata[0],pwr,base);
		from=base-pow(10,pwr);
		to=base+pow(10,pwr);
		scalex(from,to);
	}
	if (ifsingley) {
	
		getpwr(ydata[0][0],pwr,base);
		from=base-pow(10,pwr);
		to=base+pow(10,pwr);
		scaley(from,to);
	}
	
	if (sxlo!=sxhi && !iflogx) scalex(sxlo,sxhi,sxr);
	if (sylo!=syhi && !iflogy) scaley(sylo,syhi,syr);
	
	if (ifeq && !(iflogx && !iflogy || iflogy && !iflogx)) {
	
		linewidth(linewid*2);
		clip(left,right,top,bottom,0);
		if (ifinverse) {
			x=ptogx(top);
			y=ptogy(left);
			r=x<y? x:y;
			x=ptogy(right);
			y=ptogx(bottom);
			s=x>y? x:y;
			line(gtopy(r),gtopx(r),gtopy(s),gtopx(s));
		}
		else {
			x=ptogx(left);
			y=ptogy(bottom);
			r=x<y? x:y;
			x=ptogx(right);
			y=ptogy(top);
			s=x>y? x:y;
			line(gtopx(r),gtopy(r),gtopx(s),gtopy(s));
		}
		linewidth(linewid);
		
		grestore();
	}
		
	for (i=0;i<nydataitem;i++) {
	
		plotyitem(ydata,label,ydataitemno,i,ifsinglex,
			ifstack,ifscatter,ifmono,left,bottom,right,top,msize,
			legbox,legbord,textsiz,legpars,havelegend,true);
			
		linewidth(linewid);
		markersize(msize);
		patternscale(1);
	}
	
	if (ifstack) {
		
		for (i=0;i<nydataitem;i++) {
		
			linestyle(0);
			
			plotyitem(ydata,label,ydataitemno,i,ifsinglex,
				false,false,true,left,bottom,right,top,msize,
				legbox,legbord,textsiz,legpars,false,false);
				
			linewidth(linewid);
			markersize(msize);
			patternscale(1);
		}
	}
	
	// trend line
	
	if (!ifsinglex && iftrend) {
		
		for (i=0;i<nydataitem;i++) {
		
			if (havelegend) {
				
				legid=find_legend_item(label[ydataitemno[i]],i,legpars);
				//printf("legid=%d pattern=%d\n",legid,legpars[legid].pattern);
			}
			else legid=-1;
		
			for (j=0;j<nrec;j++) {
				if (iflogx) xd[j]=log10(xdata[j]);
				else xd[j]=xdata[j];
				if (iflogy) yd[j]=log10(ydata[i][j]);
				else yd[j]=ydata[i][j];
			}
		
			regress(xd,yd,nrec,a,b,r);
			
			printf("\nTrend model for %s (R-squared = %g):\n",
				(char*)label[ydataitemno[i]],r*r);
			
			if (iflogy)
				printf("log10(%s) =",(char*)label[ydataitemno[i]]);
			else
				printf("%s =",(char*)label[ydataitemno[i]]);
				
			if (a) printf(" %g",a);
			
			if (b==1) {
				if (a) printf(" +");
			}
			else if (b>0) {
				if (a) printf(" + %g *",fabs(b));
				else printf(" %g *",fabs(b));
			}
			else if (b==-1) printf(" -");
			else if (b<0) printf(" - %g *",fabs(b));
			
			if (iflogx && b)
				printf(" log10(%s)",(char*)label[xitemno]);
			else if (b)
				printf(" %s",(char*)label[xitemno]);
				
			if (!a && !b) printf(" 0");
				
			printf("\n");

			linewidth(linewid*2);
			linestyle(0);
			if (!ifmono) {
				if (legid>=0) {
					if (legpars[legid].red!=-1 || legpars[legid].green!=-1 || legpars[legid].blue!=-1) {
						if (legpars[legid].red==-1) legpars[legid].red=0;
						if (legpars[legid].green==-1) legpars[legid].green=0;
						if (legpars[legid].blue==-1) legpars[legid].blue=0;
						colour(legpars[legid].red,legpars[legid].green,legpars[legid].blue);
					}
					else if (legpars[legid].hue!=-1) {
						colour(legpars[legid].hue,legpars[legid].lightness,legpars[legid].saturation,1);
					}
					else colour(colours[i].red,colours[i].green,colours[i].blue);
				}
				else colour(colours[i].red,colours[i].green,colours[i].blue);
			}
			clip(left,right,top,bottom,0);
			
			if (ifinverse) {
			
				x1=ptogx(bottom);
				x2=ptogx(top);
				y1=a+b*x1;
				y2=a+b*x2;
				line(gtopy(y1),gtopx(x1),gtopy(y2),gtopx(x2));
			}
			else {
			
				x1=ptogx(left);
				x2=ptogx(right);
				y1=a+b*x1;
				y2=a+b*x2;
				line(gtopx(x1),gtopy(y1),gtopx(x2),gtopy(y2));
			}

			grestore();
			linewidth(linewid);
			linestyle(0);
			colour(0,0,0);
		}
		
	}
		
	yaxis(-AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset);
	if (iflogy) minortics(true);
	minortics(ifmtic);
	ytics();
	ylabels();
	if (ytit!="") {
		textsize(TEXTSIZE_TITLES/TEXTSIZE_LABELS*textsiz);
		if (ifinverse) ytitle(ytit);
		else ytitle(ytit,-AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset-textsiz*0.3);
		textsize(textsiz);
	}
	
	// Data for secondary y-axis
	
	if (ny2dataitem) {
	
		flags=0;
		if (ifinverse) flags=INVERSEAXES;
		if (iflogx && !ifsinglex) flags|=LOGX;
		if (iflogy2 && !ifsingley2) flags|=LOGY;
		
		newgraph(left,bottom,right,top,flags);
		for (i=0;i<ny2dataitem;i++)
			data(i+nydataitem,nrec,xdata,ydata[i+nydataitem]);
	
		if (ifsinglex) {
	
			getpwr(xdata[0],pwr,base);
			from=base-pow(10,pwr);
			to=base+pow(10,pwr);
			scalex(from,to);
		}
		if (ifsingley2) {
			
			getpwr(ydata[nydataitem][0],pwr,base);
			from=base-pow(10,pwr);
			to=base+pow(10,pwr);
			scaley(from,to);
		}
		
		if (sxlo!=sxhi && !iflogx) scalex(sxlo,sxhi,sxr);
		if (sy2lo!=sy2hi && !iflogy2) scaley(sy2lo,sy2hi,sy2r);
		
		for (i=0;i<ny2dataitem;i++) {
			
			plotyitem(ydata,label,ydataitemno,i+nydataitem,ifsinglex,
				false,ifscatter,ifmono,left,bottom,right,top,msize,
				legbox,-1,textsiz,legpars,havelegend,true);
			
			linewidth(linewid);
			markersize(msize);
			patternscale(1);
		}

		if (ifinverse)
			yaxis(AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset,ptogx(top));
		else
			yaxis(AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset,ptogx(right));
		if (iflogy2) minortics(true);
		minortics(ifmtic);
		ytics();
		ylabels();
		if (y2tit!="") {
			textsize(TEXTSIZE_TITLES/TEXTSIZE_LABELS*textsiz);
			if (ifinverse) ytitle(y2tit);
			else ytitle(y2tit,AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset+textsiz*0.3);
			textsize(textsiz);
		}
	}

	xaxis(-AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset);
	if (iflogx) minortics(true);
	minortics(ifmtic);
	xtics();
	xlabels();
	if (xtit!="") {
		textsize(TEXTSIZE_TITLES/TEXTSIZE_LABELS*textsiz);
		if (ifinverse) xtitle(xtit,-AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset-textsiz*0.3);
		else xtitle(xtit);
		textsize(textsiz);
	}
	if (tit!="") {
		textsize(TEXTSIZE_TITLES/TEXTSIZE_LABELS*textsiz);
		if (ifinverse && ny2dataitem)
			title(tit,AXOFFSET/TEXTSIZE_LABELS*textsiz*ifoffset+textsiz*2);
		else
			title(tit,textsiz/2);
	}
	
	linestyle(0);
	if (ifframe) frame();
	
//	endbengraph();
}

bool readline(FILE*& in,xtring& line) {
	
	char ch;
	
	line="";
	
	ch=getc(in);
	while (!feof(in)) {
		
		if (ch=='\n') return true;
		else line+=ch;
		ch=getc(in);
	}
	
	return false;
}

xtring& remove_trailing_blanks(xtring& s) {
	
	int p=s.len();

	while (s[p-1]==' ') p--;
	
	s[p]=0;
	
	return s;
}

bool readheader(FILE*& in,xtring label[MAXITEM],int& ncol,
	xtring filename,double values[MAXITEM],bool& ifvalues,int& lineno,
	bool force_space_is_sep,bool force_space_not_sep,bool& space_is_sep) {

	// Reads header row of an RCA-GUESS output file
	// label = array of header labels
	// ncol  = number of columns (labels)
	// Returns false if too many items in file
	
	xtring line,item;
	bool alphabetics=false;
	int pos,i;
	
	ncol=0;
	
	if (force_space_not_sep) space_is_sep=false;
	else space_is_sep=true;
	
	while (!ncol && !feof(in)) {

		readline(in,line);
		if (line.find("\t")>=0 && !force_space_is_sep) space_is_sep=false;
		lineno++;

		pos=line.findnotoneof(" \r");
		
		while (pos!=-1 && line.findnotoneof(" \t\r")!=-1) {
			if (ncol==MAXITEM) {
				printf("Too many items in %s (maximum allowed is %d)\n",
					(char*)filename,MAXITEM);
				return false;
			}
			if (line[pos]=='\t') {
				item="";
				label[ncol]=item;
				pos++;
				line=line.mid(pos);
				pos=line.findnotoneof(" \r");
			}
			else {
				line=line.mid(pos);
				if (space_is_sep) pos=line.findoneof(" \t\r");
				else pos=line.findoneof("\t\r");
				if (pos>0) {
					item=line.left(pos);
					item=remove_trailing_blanks(item);
					label[ncol]=item;
					while (line[pos]==' ' || line[pos]=='\r') pos++;
					if (line[pos]=='\t') pos++;
					line=line.mid(pos);
					pos=line.findnotoneof(" \r");
				}
				else {
					item=remove_trailing_blanks(line);
					label[ncol]=item;
				}
			}
			
			if (item.isnum())
				values[ncol]=item.num();
			else
				alphabetics=true;
			
			ncol++;
		}
	}
	
	if (!ncol) {
		printf("%s contains no data\n",(char*)filename);
		return false;
	}
	else if (ncol<2) {
		printf("At least two columns expected in %s\n",(char*)filename);
		return false;
	}
	
	if (!alphabetics) {
		ifvalues=true;
		for (i=0;i<ncol;i++) label[i].printf("Item %d",i+1);
	}
	
	return true;
}

void listitems(xtring label[MAXITEM],int nitem) {

	int ct=21;
	int i;
	
	printf("Available items are: ");
	for (i=0;i<nitem;i++) {
		if (ct+label[i].len()>64) {
			printf("\n");
			ct=0;
		}
		printf("%s",(char*)label[i]);
		ct+=label[i].len();
		if (i<nitem-1) {
			printf(", ");
			ct+=2;
		}
	}
	printf("\n");
}

int finditem(xtring item,int itemnox,xtring infile,xtring label[MAXITEM],int nitem) {

	int i;
	int itemno=itemnox;
	
	if (itemno) {
		if (itemno>nitem) {
			printf("Item number %d not found in %s\n",itemno,(char*)infile);
			listitems(label,nitem);
			return -1;
		}
		
		return itemno-1; // 0-based item number
	}
	else if (item!="") {
		itemno=-1;
		for (i=0;i<nitem;i++) {
			if (label[i]==item) {
				itemno=i;
				i=nitem;
			}
		}
		if (itemno==-1) {
		
			// Not found - try case insensitive comparison
			
			itemno=-1;
			for (i=0;i<nitem;i++) {
				if (label[i].lower()==item.lower()) {
					itemno=i;
					i=nitem;
				}
			}
			
			if (itemno==-1) {
				printf("Item %s not found in %s\n",(char*)item,(char*)infile);
				listitems(label,nitem);
				return -1;
			}
			else {
				printf("Item %s not found in %s\n",(char*)item,(char*)infile);
				printf("Choosing %s instead\n",(char*)label[itemno]);
			}
		}
	}
	
	return itemno;
}

bool readline(FILE*& in,int nitem,xtring sval[],bool space_is_sep) {
	
	xtring line,item;
	int i=0,j,pos;
	
	if (feof(in)) return false;
	
	readline(in,line);

	pos=line.findnotoneof(" \r");

	while (pos!=-1 && i<nitem) {
		if (line[pos]=='\t') {
			item="";
			sval[i]=item;
			pos++;
			line=line.mid(pos);
			pos=line.findnotoneof(" \r");
		}
		else {
			line=line.mid(pos);
			if (space_is_sep) pos=line.findoneof(" \t\r");
			else pos=line.findoneof("\t\r");

			if (pos>0) {
				item=line.left(pos);
				sval[i]=remove_trailing_blanks(item);
				while (line[pos]==' ' || line[pos]=='\r') pos++;
				if (line[pos]=='\t') pos++;
				line=line.mid(pos);
				pos=line.findnotoneof(" \r");
			}
			else {
				item=remove_trailing_blanks(line);
				sval[i]=item;
			}
		}
		
		i++;
	}
	
	if (!i) return false;
	
	for (j=i;j<nitem;j++) sval[j]="";
	
	return true;
}

	
bool readdata(xtring infile,xtring label[MAXITEM],
	int& nydataitem,xtring ydataitem[MAXYITEM],
	int nydataitemno,int ydataitemno[MAXYITEM],
	int& ny2dataitem,xtring y2dataitem[MAXYITEM],
	int ny2dataitemno,int y2dataitemno[MAXYITEM],
	xtring& xitem,int& xitemno,
	double ydata[MAXYITEM][MAXREC],double xdata[MAXREC],int& nrec,
	bool& ifsinglex,bool& ifsingley,bool& ifsingley2,
	xtring& xtitle,xtring& ytitle,xtring& y2title,bool ifstack,
	bool& iflogx,bool& iflogy,bool& iflogy2,
	bool& force_space_is_sep,bool& force_space_not_sep) {

	int nitem,i,j,pt,pt2;
	double dval[MAXITEM],firstx,firsty,firsty2;
	xtring sval[MAXITEM];
	bool found,blank,isnum,ifvalues,havex,taken;
	bool space_is_sep;
	int lineno=0,itemno;
	int nchar;
	
	FILE* in=fopen(infile,"rt");
	if (!in) {
		printf("Could not open %s for input\n",(char*)infile);
		return false;
	}
	
	if (!readheader(in,label,nitem,infile,dval,ifvalues,lineno,
		force_space_is_sep,force_space_not_sep,space_is_sep)) {
		fclose(in);
		return false;
	}
	
	havex=false;
	if (xitem!="" || xitemno) {

		itemno=finditem(xitem,xitemno,infile,label,nitem);
		if (itemno==-1) {
			fclose(in);
			return false;
		}
		xitemno=itemno;
		if (xtitle=="") xtitle=label[itemno];
		havex=true;
	}
		
	pt=0;
	while (ydataitemno[pt] && pt<MAXYITEM) {
		
		itemno=finditem("",ydataitemno[pt],infile,label,nitem);
		if (itemno==-1) {
			fclose(in);
			return false;
		}
		ydataitemno[pt++]=itemno;
	}
			
	i=0;
	while (ydataitem[i]!="" && i<MAXYITEM) {

		itemno=finditem(ydataitem[i],0,infile,label,nitem);
		if (itemno==-1) {
			fclose(in);
			return false;
		}
		ydataitemno[pt++]=itemno;
		i++;
	}
	
	// Secondary Y-axis
	
	pt2=0;
	while (y2dataitemno[pt2] && pt2<MAXYITEM) {
		
		itemno=finditem("",y2dataitemno[pt2],infile,label,nitem);
		if (itemno==-1) {
			fclose(in);
			return false;
		}
		y2dataitemno[pt2++]=itemno;
	}
			
	i=0;
	while (y2dataitem[i]!="" && i<MAXYITEM) {

		itemno=finditem(y2dataitem[i],0,infile,label,nitem);
		if (itemno==-1) {
			fclose(in);
			return false;
		}
		y2dataitemno[pt2++]=itemno;
		i++;
	}
	
	if (!havex) {
	
		xitemno=-1;
		for (i=0;i<nitem && xitemno<0;i++) {
			taken=false;
			for (j=0;j<pt;j++)
				if (ydataitemno[j]==i) taken=true;
			for (j=0;j<pt2;j++)
				if (y2dataitemno[j]==i) taken=true;
			if (!taken) xitemno=i;
		}

		if (xitemno<0) {
			printf("Please specify X-data item (with -x)\n");
			return false;
		}
		if (xtitle=="") xtitle=label[xitemno];
	} 

	if (!pt) { // no y-data items selected
	
		i=0;
		while (pt+pt2<MAXYITEM && i<nitem) {
			
			found=false;
			for (j=0;j<pt2;j++)
				if (y2dataitemno[j]==i) found=true;
			if (i!=xitemno && !found) {
				itemno=finditem("",i+1,infile,label,nitem);
				if (itemno>=0) {
					ydataitemno[pt++]=itemno;
				}
			}
			i++;
		}
	}
	
	// Move y2 data item numbers past y data item numbers in index array
	for (i=0;i<pt2;i++) ydataitemno[pt+i]=y2dataitemno[i];

	if (!pt && !pt2) {
		printf("%s must contain at least one X and one Y-data item\n",(char*)infile);
		fclose(in);
		return false;
	}
	
	if (pt2 && !pt) {
		printf("No data for primary Y-axis\n");
		fclose(in);
		return false;
	}
	
	if (ytitle=="") ytitle=label[ydataitemno[0]];
	if (y2title=="" && pt2) y2title=label[y2dataitemno[0]];
	
	// not ydataitemno contains a list of (pt) 0-based indices to y-data items
		
	nrec=0;
	
	ifsinglex=ifsingley=ifsingley2=true;
	while (!feof(in) || ifvalues) {
	
		if (ifvalues) {
			found=true;
			ifvalues=false;
			for (i=0;i<pt;i++) {
				if (ifstack && i)
					ydata[i][nrec]=ydata[i-1][nrec]+dval[ydataitemno[i]];
				else
					ydata[i][nrec]=dval[ydataitemno[i]];
				if (!i) firsty=ydata[0][0];
				else if (ydata[i][nrec]!=firsty) ifsingley=false;
				if (ydata[i][nrec]<=0 && iflogy) {
					printf("Zero or negative value(s) for %s - cannot log transform Y-axis\n",
						(char*)label[ydataitemno[i]]);
					iflogy=false;
				}
			}
			for (i=0;i<pt2;i++) {
				ydata[i+pt][nrec]=dval[y2dataitemno[i]];
				if (!i) firsty2=ydata[pt][0];
				else if (ydata[i+pt][0]!=firsty2) ifsingley2=false;
				if (ydata[i+pt][nrec]<=0 && iflogy2) {
					printf("Zero or negative value(s) for %s - cannot log transform secondary Y-axis\n",
						(char*)label[y2dataitemno[i]]);
					iflogy2=false;
				}
			}
			xdata[nrec]=dval[xitemno];
			firstx=xdata[0];
			if (xdata[0]<=0 && iflogx) {
				printf("Zero or negative value(s) for %s - cannot log transform X-axis\n",
					(char*)label[xitemno]);
				iflogx=false;
			}
		}
		else {
			found=readline(in,nitem,sval,space_is_sep);
			if (found) {
				lineno++;
				
				blank=true;
				isnum=true;
				found=false;
				for (i=0;i<nitem;i++) {
					nchar=sval[i].len();
					if (nchar) {
						if (sval[i][nchar-1]=='\r') sval[i]=sval[i].left(nchar-1);
					}
					if (sval[i]!="") {
						blank=false;
						for (j=0;j<pt;j++) {
							if (i==ydataitemno[j] || i==xitemno) {
								if (!sval[i].isnum()) isnum=false;
							}
						}
					}
				}
				
				if (blank)
					printf("Line %d of %s is blank - ignoring\n",lineno,(char*)infile);
				else if (!isnum) {
					printf("Line %d of %s contains non-numeric data - ignoring\n",
						lineno,(char*)infile);
				}
				else {
					found=true;
					for (i=0;i<pt;i++) {
						if (ifstack && i)
							ydata[i][nrec]=ydata[i-1][nrec]+sval[ydataitemno[i]].num();
						else
							ydata[i][nrec]=sval[ydataitemno[i]].num();
						if (!nrec && !i) firsty=ydata[i][nrec];
						else if (ydata[i][nrec]!=firsty) ifsingley=false;
						if (ydata[i][nrec]<=0 && iflogy) {
							printf("Zero or negative value(s) for %s - cannot log transform Y-axis\n",
								(char*)label[ydataitemno[i]]);
							iflogy=false;
						}
					}
					for (i=0;i<pt2;i++) {
						ydata[i+pt][nrec]=sval[y2dataitemno[i]].num();
						if (!nrec && !i) firsty2=ydata[pt][0];
						else if (ydata[i+pt][nrec]!=firsty2) ifsingley2=false;
						if (ydata[i+pt][nrec]<=0 && iflogy2) {
							printf("Zero or negative value(s) for %s - cannot log transform secondary Y-axis\n",
								(char*)label[y2dataitemno[i]]);
							iflogy2=false;
						}
					}
					xdata[nrec]=sval[xitemno].num();
					if (!nrec) firstx=xdata[0];
					else if (xdata[nrec]!=firstx) ifsinglex=false;
					if (xdata[nrec]<=0 && iflogx) {
						printf("Zero or negative value(s) for %s - cannot log transform X-axis\n",
							(char*)label[xitemno]);
						iflogx=false;
					}
				}
			}
			
		}
		
		if (found) {
					
			nrec++;
			if (nrec==MAXREC) {
				printf("Too many records in %s (maximum allowed is %d)\n",(char*)infile,MAXREC);
				fclose(in);
				return false;
			}
		}
	}
	
	if (ifsinglex) {
		printf("WARNING: Only one value (%g) for X-data item %s\n",
			firstx,(char*)label[xitemno]);
	}
	if (ifsingley) {
		printf("WARNING: Only one value (%g) for Y-data\n",firsty);
	}

	if (!nrec) {
		printf("No data found in %s\n",(char*)infile);
		fclose(in);
		return false;
	}
		
	nydataitem=pt;
	ny2dataitem=pt2;
	
	fclose(in);
	return true;
}


void helptext(FILE* out,xtring exe) {

	fprintf(out,"GPLOT\n");
	fprintf(out,"Two-dimensional visualisation utility for plain text output files from LPJ-GUESS.\n\n");
	fprintf(out,"Usage: %s <input-file> <options>\n",(char*)exe);
	fprintf(out,"Options:\n\n");
	fprintf(out,"-o <output-file>\n");
	fprintf(out,"    Pathname for output PostScript file\n");
	fprintf(out,"-i <item-name> | <column-number> { <item-name> | <column-number> }\n");
	fprintf(out,"    Item names or 1-based column numbers for output data\n");
	fprintf(out,"    X-data item then Y-data item(s)\n");
	fprintf(out,"-x <item-name> | <column-number>\n");
	fprintf(out,"    Item name or 1-based column number for X-axis\n");
	fprintf(out,"-y <item-name> | <column-number> { <item-name> | <column-number> }\n");
	fprintf(out,"    Item name(s) or 1-based column number(s) for primary Y-axis\n");
	fprintf(out,"-y2 <item-name> | <column-number> { <item-name> | <column-number> }\n");
	fprintf(out,"    Item name(s) or 1-based column number(s) for secondary Y-axis\n");
	fprintf(out,"-t <title>\n");
	fprintf(out,"    Title text for display above plot. Multi-word titles should be given in\n");
	fprintf(out,"    single quotes (e.g. 'Plot of NPP'). The following format specifiers may be\n");
	fprintf(out,"    embedded:\n");
	fprintf(out,"    !b    change to bold font\n");
	fprintf(out,"    !i    change to italic font\n");
	fprintf(out,"    !^    change to superscript font\n");
	fprintf(out,"    !_    change to subscript font\n");
	fprintf(out,"    !p    change to plain font\n");
	fprintf(out,"    !s(x) change text size to x points\n");
	fprintf(out,"    !c(x) insert character with code x from symbols character set\n");
	fprintf(out,"    !!    insert exclamation mark\n");
	fprintf(out,"-xt <axis-title>\n");
	fprintf(out,"    Title text for X-axis. See -t\n");
	fprintf(out,"-yt <axis-title>\n");
	fprintf(out,"    Title text for primary Y-axis. See -t\n");
	fprintf(out,"-y2t <axis-title>\n");
	fprintf(out,"    Title text for secondary Y-axis. See -t\n");
	fprintf(out,"-sx <min> <max> [<round>]\n");
	fprintf(out,"    X-axis range with optional tic interval rounding parameter\n");
	fprintf(out,"-sy <min> <max> [<round>]\n");
	fprintf(out,"    Primary Y-axis range with optional tic interval rounding parameter\n");
	fprintf(out,"-sy2 <min> <max> [<round>]\n");
	fprintf(out,"    Secondary Y-axis range with optional tic interval rounding parameter\n");
	fprintf(out,"-inverse\n");
	fprintf(out,"    Vertical X-axis, horizontal Y-axes\n");
	fprintf(out,"-logx\n");
	fprintf(out,"    Log-transform X-axis\n");
	fprintf(out,"-logy\n");
	fprintf(out,"    Log-transform primary Y-axis\n");
	fprintf(out,"-logy2\n");
	fprintf(out,"    Log-transform secondary Y-axis\n");
	fprintf(out,"-stack\n");
	fprintf(out,"    Stacked area plot (primary Y-axis data only)\n");
	fprintf(out,"-trend\n");
	fprintf(out,"    Show trend lines (simple linear regression, primary Y-axis data only)\n");
	fprintf(out,"-eq\n");
	fprintf(out,"    Show line of equality between primary axes\n");
	fprintf(out,"-scatter\n");
	fprintf(out,"    Scatterplot\n");
	fprintf(out,"-size <w> <h>\n");
	fprintf(out,"    Data area dimensions on page in mm\n");
	fprintf(out,"-offset\n");
	fprintf(out,"    Offset axes from data area\n");
	fprintf(out,"-mono\n");
	fprintf(out,"    Monochrome plot\n");
	fprintf(out,"-frame\n");
	fprintf(out,"    Frame around data area\n");
	fprintf(out,"-mtic\n");
	fprintf(out,"    Show minor tics\n");
	fprintf(out,"-portrait\n");
	fprintf(out,"    Forces 'portrait'-mode page orientation\n");
	fprintf(out,"-landscape\n");
	fprintf(out,"    Forces 'landscape'-mode page orientation\n");
	fprintf(out,"-textsize <points>\n");
	fprintf(out,"   Text size for label text in points\n");
	fprintf(out,"-linewidth <points>\n");
	fprintf(out,"   Width of axes, curves and map frame in points\n");
	fprintf(out,"-help\n");
	fprintf(out,"   Displays this help message\n");
}

void printhelp(xtring exe) {
	
	int result;
	xtring command;
		
	command=(xtring)"man "+(xtring)FILE_MANPAGE;
	result=system(command);
	if (result)
		printf("Could not execute command %s\n",(char*)command);

	exit(99);
}


bool getext(xtring filename,xtring& dir,xtring& file,xtring& ext) {

	char bdrive[1],bdir[MAXPATH],bfile[MAXPATH],bext[MAXPATH];
	
	fnsplit((char*)filename,bdrive,bdir,bfile,bext);
	
	ext=bext;
	file=bfile;
	dir=bdir;
	
	if (bext!="") return true;
	
	return false;
}

bool processargs(int argc,char* argv[],xtring& infile,xtring& outfile,xtring& title,
	xtring& xtitle,xtring& ytitle,xtring& y2title,
	int& nydataitem,xtring ydataitem[MAXYITEM],int& nydataitemno,
	int ydataitemno[MAXYITEM],int& ny2dataitem,xtring y2dataitem[MAXYITEM],
	int& ny2dataitemno,int y2dataitemno[MAXYITEM],int& orient,bool& ifmono,
	xtring& xitem,int &xitemno,bool& ifstack,bool& ifscatter,
	double& textsize,double& linewidth,double& width,double& height,
	bool& ifoffset,double& sxlo,double& sxhi,double& sylo,double& syhi,
	double& sy2lo,double& sy2hi,bool& ifinverse,
	bool& iflogx,bool& iflogy,bool& iflogy2,bool& ifframe,bool& ifmtic,
	bool& iftrend,bool& ifeq,
	double& sxr,double& syr,double& sy2r,
	bool& havegs,xtring& ext,bool& ifxview,
 	bool& force_space_is_sep,bool& force_space_not_sep,
	xtring& filelegend) {

	int i,itemno;
	xtring arg,item,filepart,dir,file;
	bool slut,havexdata,haveydata,havey2data,haveinfile,havewindowsize;
	double dval;
	
	// Defaults
	
	nydataitem=0;
	nydataitemno=0;
	ny2dataitem=0;
	ny2dataitemno=0;
	havexdata=false;
	haveydata=false;
	havey2data=false;
	haveinfile=false;
	havewindowsize=false;
	iflogx=iflogy=iflogy2=false;
	ifframe=ifmtic=false;	
	outfile="figure.ps";
	title="";
	xtitle="";
	ytitle="";
	y2title="";
	orient=0; // automatic page orientation
	for (i=0;i<MAXYITEM;i++) {
		ydataitem[i]="";
		ydataitemno[i]=0;
		y2dataitemno[i]=0;
	}
	xitem="";
	xitemno=0;
	ifmono=false;
	ifstack=false;
	ifscatter=false;
	ifoffset=false;
	textsize=TEXTSIZE_LABELS;
	linewidth=LINEWIDTH;
	sxlo=sxhi=sylo=syhi=0.0;
	sy2lo=sy2hi=0.0;
	sxr=syr=sy2r=10.0;
	ifinverse=false;
	iftrend=false;
	ifeq=false;
	force_space_is_sep=force_space_not_sep=false;
	havegs=false;
	ifxview=false;
	filelegend="";

	if (argc<2) {
		printf("Input file name or path must be specified\n");
		return false;
	}
	
	for (i=1;i<argc;i++) {
		arg=argv[i];
		if (arg[0]=='-') {
			arg=arg.lower();
			if (arg=="-t") { // title
				if (argc>=i+2) {
					title=argv[i+1];
					i+=1;
				}
				else {
					printf("Option -t must be followed by title string\n");
					return false;
				}
			}
			else if (arg=="-xt") { // x-axis title
				if (argc>=i+2) {
					xtitle=argv[i+1];
					i+=1;
				}
				else {
					printf("Option -xt must be followed by X-axis title string\n");
					return false;
				}
			}
			else if (arg=="-yt") { // y-axis title
				if (argc>=i+2) {
					ytitle=argv[i+1];
					i+=1;
				}
				else {
					printf("Option -yt must be followed by Y-axis title string\n");
					return false;
				}
			}
			else if (arg=="-y2t") { // secondary y-axis title
				if (argc>=i+2) {
					y2title=argv[i+1];
					i+=1;
				}
				else {
					printf("Option -y2t must be followed by secondary Y-axis title string\n");
					return false;
				}
			}
			else if (arg=="-o") { // output file
				if (argc>=i+2) {
					outfile=argv[i+1];
					if (getext(outfile,dir,file,ext)) {
						
						ext=ext.lower();
						if (ext=="tif" || ext=="jpg" || ext=="png" || ext=="eps" || ext=="pdf") {
							
							// See if we have GhostScript available
							
							if (system("gs -dNOPAUSE -dBATCH -dQUIET -dNODISPLAY")) {
								havegs=false;
								outfile=dir+"/"+file+".ps";
								printf("GhostScript not available - output will be a PostScript file \n");
							}
							else {
								havegs=true;
								
								//printf("Output file is '%s' in directory '%s' of type '%s'\n",
								//	  (char*)file,(char*)dir,(char*)ext);
							}
						}
						else if (ext!="ps") {
							
							outfile+=".ps";
						}
					}
					else outfile+=".ps";
				}
				else {
					printf("Option -o must be followed by output file name or path\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-xv") {
				if (havegs)
					ifxview=true;
				else if (!system("gs -dNOPAUSE -dBATCH -dQUIET -dNODISPLAY"))
					ifxview=true;
				else
					printf("GhostScript not available - map will not be displayed on screen\n");
			}
			else if (arg=="-i") { // item list column label or number (first item is x-data, rest are y-data)
				if (havexdata) {
					printf("Options -x and -i cannot be combined\n");
					return false;
				}
				else if (haveydata) {
					printf("Options -y and -i cannot be combined\n");
					return false;
				}
				else if (havey2data) {
					printf("Options -y2 and -i cannot be combined\n");
					return false;
				}
				slut=false;
				while (argc>=i+2 && !slut) {
					item=argv[i+1];
					if (item[0]=='-') slut=true;
					else {
						if (item.isnum()) {
							dval=item.num();
							if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
								itemno=dval;
								if (itemno>MAXITEM) {
									printf("Option -i: too many items (maximum allowed is %d)\n",MAXITEM);
									return false;
								}
								if (havexdata) {
									if (nydataitemno+nydataitem==MAXYITEM) {
										printf("Option -i: too many Y-data items specified (maximum allowed is %d)\n",MAXYITEM);
										return false;
									}
									ydataitemno[nydataitemno++]=itemno;
								}
								else {
									xitemno=itemno;
									havexdata=true;
								}
							}
						}
						else {
							if (havexdata) {
								if (nydataitemno+nydataitem==MAXYITEM) {
									printf("Option -i: too many Y-data items specified (maximum allowed is %d)\n",MAXYITEM);
									return false;
								}
								ydataitem[nydataitem++]=item;
							}
							else {
								xitem=item;
								havexdata=true;
							}
						}
						i++;
					}
				}
				if (!(nydataitem+nydataitemno) || !havexdata) {
					printf("Option -i must be followed by label or item number for at least one X-data and one Y-data item\n");
					return false;
				}
				haveydata=true;
				havey2data=true;
			}
			else if (arg=="-y") { // y data items
				if (haveydata) {
					printf("Options -i and -y cannot be combined\n");
					return false;
				}
				slut=false;
				while (argc>=i+2 && !slut) {
					item=argv[i+1];
					if (item[0]=='-') slut=true;
					else {
						if (item.isnum()) {
							dval=item.num();
							if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
								itemno=dval;
								if (itemno>MAXITEM) {
									printf("Option -y: too many items (maximum allowed is %d)\n",MAXITEM);
									return false;
								}
								if (nydataitemno+nydataitem==MAXYITEM) {
									printf("Option -y: too many items specified (maximum allowed is %d)\n",MAXYITEM);
									return false;
								}
								ydataitemno[nydataitemno++]=itemno;
							}
						}
						else {
							if (nydataitemno+nydataitem==MAXYITEM) {
								printf("Option -y: too many items specified (maximum allowed is %d)\n",MAXYITEM);
								return false;
							}
							ydataitem[nydataitem++]=item;
						}
						i++;
					}
				}
				if (!nydataitem && !nydataitemno) {
					printf("Option -y must be followed by label or item number for at least one item\n");
					return false;
				}
				haveydata=true;
			}
			else if (arg=="-y2") { // secondary y data items
				if (havey2data) {
					printf("Options -i and -y2 cannot be combined\n");
					return false;
				}
				slut=false;
				while (argc>=i+2 && !slut) {
					item=argv[i+1];
					if (item[0]=='-') slut=true;
					else {
						if (item.isnum()) {
							dval=item.num();
							if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
								itemno=dval;
								if (itemno>MAXITEM) {
									printf("Option -y2: too many items (maximum allowed is %d)\n",MAXITEM);
									return false;
								}
								if (ny2dataitemno+ny2dataitem==MAXYITEM) {
									printf("Option -y2: too many items specified (maximum allowed is %d)\n",MAXYITEM);
									return false;
								}
								y2dataitemno[ny2dataitemno++]=itemno;
							}
						}
						else {
							if (ny2dataitemno+ny2dataitem==MAXYITEM) {
								printf("Option -y: too many items specified (maximum allowed is %d)\n",MAXYITEM);
								return false;
							}
							y2dataitem[ny2dataitem++]=item;
						}
						i++;
					}
				}
				if (!ny2dataitem && !ny2dataitemno) {
					printf("Option -y2 must be followed by label or item number for at least one item\n");
					return false;
				}
				havey2data=true;
			}
			else if (arg=="-x") { // x data item
				if (havexdata) {
					printf("Options -i and -x cannot be combined\n");
					return false;
				}
				if (argc>=i+2) {
					xitem=argv[i+1];
					if (xitem.isnum()) {
						dval=xitem.num();
						if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
							xitemno=dval;
							if (xitemno>MAXITEM) {
								printf("Option -x: too many items (maximum allowed is %d)\n",MAXITEM);
								return false;
							}
							xitem="";
						}
						else xitemno=0;
					}
					else xitemno=0;
				}
				else {
					printf("Option -x must be followed by item label or column number\n");
					return false;
				}
				i+=1;
				havexdata=true;
			}
			else if (arg=="-textsize") { // base text size (points)
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -textsize must be followed by text size in points\n");
						return false;
					}
					textsize=arg.num();
				}
				else {
					printf("Option -textsize must be followed by text size in points\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-linewidth") { // line width (points)
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -linewidth must be followed by line width in points\n");
						return false;
					}
					linewidth=arg.num();
				}
				else {
					printf("Option -linewidth must be followed by line width in points\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-size") { // Window size in mm
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -size <w> <h>: <w> must be a number\n");
						return false;
					}
					width=arg.num();
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -size <w> <h>: <h> must be a number\n");
						return false;
					}
					height=arg.num();
					if (width<=0 || height<=0) {
						printf("Option -size <w> <h>: <w> and <h> must be positive values\n");
						return false;
					}
				}
				else {
					printf("Option -size must be followed by <w> <h>\n");
					return false;
				}
				i+=2;
				havewindowsize=true;
			}
			else if (arg=="-sx") { // Scale x axis
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -sx <min> <max> [<round>]: <min> must be a number\n");
						return false;
					}
					sxlo=arg.num();
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -sx <min> <max> [<round>]: <max> must be a number\n");
						return false;
					}
					sxhi=arg.num();
					if (sxlo>=sxhi) {
						printf("Option -sx <min> <max> [<round>]: <max> must be greater than <min>\n");
						return false;
					}
					if (argc>=i+4) { // includes <round> parameter
						arg=argv[i+3];
						if (arg.isnum()) {
							sxr=arg.num();
							if (sxr<=0) sxr=10.0;
							i++;
						}
					}
				}
				else {
					printf("Option -sx must be followed by <min> <max> [<round>]\n");
					return false;
				}
				i+=2;
			}
			else if (arg=="-sy") { // Scale y axis
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -sy <min> <max> [<round>]: <min> must be a number\n");
						return false;
					}
					sylo=arg.num();
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -sy <min> <max> [<round>]: <max> must be a number\n");
						return false;
					}
					syhi=arg.num();
					if (sylo>=syhi) {
						printf("Option -sy <min> <max> [<round>]: <max> must be greater than <min>\n");
						return false;
					}
					if (argc>=i+4) { // includes <round> parameter
						arg=argv[i+3];
						if (arg.isnum()) {
							syr=arg.num();
							if (syr<=0) syr=10.0;
							i++;
						}
					}
				}
				else {
					printf("Option -sy must be followed by <min> <max> [<round>]\n");
					return false;
				}
				i+=2;
			}
			else if (arg=="-sy2") { // Scale secondary y axis
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -sy2 <min> <max> [<round>]: <min> must be a number\n");
						return false;
					}
					sy2lo=arg.num();
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -sy2 <min> <max> [<round>]: <max> must be a number\n");
						return false;
					}
					sy2hi=arg.num();
					if (sy2lo>=sy2hi) {
						printf("Option -sy2 <min> <max> [<round>]: <max> must be greater than <min>\n");
						return false;
					}
					if (argc>=i+4) { // includes <round> parameter
						arg=argv[i+3];
						if (arg.isnum()) {
							sy2r=arg.num();
							if (sy2r<=0) sy2r=10.0;
							i++;
						}
					}
				}
				else {
					printf("Option -sy2 must be followed by <min> <max> [<round>]\n");
					return false;
				}
				i+=2;
			}
			else if (arg=="-landscape") { // force landscape mode
				orient=2;
			}
			else if (arg=="-portrait") { // force portrait mode
				orient=1;
			}
			else if (arg=="-frame") {
				ifframe=true;
			}
			else if (arg=="-mtic") { // include minor tics
				ifmtic=true;
			}
			else if (arg=="-mono") { // monochrome
				ifmono=true;
			}
			else if (arg=="-stack") { // stacked area curves
				ifstack=true;
			}
			else if (arg=="-scatter") { // scatterplot
				ifscatter=true;
			}
			else if (arg=="-offset") { // axis offset
				ifoffset=true;
			}
			else if (arg=="-inverse") { // inverse axes
				ifinverse=true;
			}
			else if (arg=="-logx") { // log-transform X-axis
				iflogx=true;
			}
			else if (arg=="-logy") { // log-transform Y-axis
				iflogy=true;
			}
			else if (arg=="-logy2") { // log-transform secondary Y-axis
				iflogy2=true;
			}
			else if (arg=="-trend") { // show trend line
				iftrend=true;
			}
			else if (arg=="-eq") { // show line of equality
				ifeq=true;
			}
			else if (arg=="-seps" || arg=="-sepst" || arg=="-septs") {
				force_space_is_sep=true;
			}
			else if (arg=="-sep" || arg=="-sept") {
				force_space_not_sep=true;
			}
			else if (arg=="-legend" || arg=="-leg") { 
				if (argc>=i+2) {
					filelegend=argv[i+1];
				}
				else {
					printf("Option -legend must be followed by legend file name or path\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-h" || arg=="-help") printhelp(argv[0]);
			else {
				printf("Invalid option %s\n",(char*)arg);
				return false;
			}
		}
		else {
			if (haveinfile) {
				printf("Only one input file may be specified\n");
				return false;
			}
			else {
				infile=arg;
				haveinfile=true;
			}
		}
	}

	if (!haveinfile) {
		printf("No input file specified\n");
		return false;
	}
	
	if (ifscatter && ifstack) {
		printf("Cannot combine -scatter with -stack\n");
		return false;
	}
	
	if (nydataitem+nydataitemno+ny2dataitem+ny2dataitemno>MAXYITEM) {
		printf("Too many Y-data items (maximum allowed is %d)\n",MAXYITEM);
		return false;
	}
	
/*	if (haveydata && !havexdata) {
		printf("Please specify X-data item (with option -x)\n");
		return false;
	}
	
	if (ny2dataitem+ny2dataitemno && !(nydataitem+nydataitemno)) {
		printf("Please specify primary Y-data items (with option -y)\n");
		return false;
	}*/
	
	if (!havewindowsize) width=height=-1;
	
	if (title=="") {
		filepart=infile;
		i=infile.len()-1;
		while (i>=0) {
			if (infile[i]=='/' || infile[i]=='\\') {
				filepart=infile.mid(i+1);
				i=0;
			}
			i--;
		}
		
		title.printf("%s",(char*)filepart);
	}

	return true;
}


void abort(xtring exe) {

	printf("Usage: %s <input-file> [ <options> ]\n",(char*)exe);
	printf("Options: -o <output-file>  -xv\n");
	printf("         -i <item-name> | <column-number> { <item-name> | <column-number> }\n");
	printf("         -x <item-name> | <column-number>\n");
	printf("         -y <item-name> | <column-number> { <item-name> | <column-number> }\n");
	printf("         -y2 <item-name> | <column-number> { <item-name> | <column-number> }\n");
	printf("         -seps  -sept\n");
	printf("         -t <title>  -xt <axis-title>  -yt <axis-title>  -y2t <axis-title>\n");
	printf("         -sx <min> <max> [<round>]\n");
	printf("         -sy <min> <max> [<round>]  -sy2 <min> <max> [<round>]\n");
	printf("         -inverse  -logx  -logy  -logy2  -mtic\n");
	printf("         -legend <legend-file>  -mono\n");
	printf("         -stack  -scatter  -trend  -eq  -size <w> <h>  -offset  -frame\n");
	printf("         -portrait  -landscape  -textsize <points>  -linewidth <points>\n");
	printf("         -help\n");

	exit(99);
}

bool callgs(xtring infile,xtring& outfile,xtring type,bool ifview) {
	
	int result;
	xtring command;
	type=type.lower();
	
	
	if (ifview) {
		
		command=(xtring)"gs -dBATCH -dQUIET -sDEVICE=x11 '"+infile+"'";
		result=system(command);
		if (result)
			printf("GhostScript error %d - could not open X11 viewer\n",result);
	}
	else {
	
		printf("\nConverting PostScript file ...\n");
		
		command="gs -dBATCH -dNOPAUSE -dQUIET -sDEVICE=";
		
		if (type=="tif")
			command+=(xtring)"tiff24nc -r300 -sOutputFile='"+outfile+"' '"+infile+"'";
		else if (type=="jpg")
			command+=(xtring)"jpeg -r300 -sOutputFile='"+outfile+"' '"+infile+"'";
		else if (type=="png")
			command+=(xtring)"png16m -r300 -sOutputFile='"+outfile+"' '"+infile+"'";
		else if (type=="pdf")
			command+=(xtring)"pdfwrite -sOutputFile='"+outfile+"' '"+infile+"'";
		else if (type=="eps")
			command+=(xtring)"epswrite -sOutputFile='"+outfile+"' '"+infile+"'";
		else {
			printf("Unknown output file format\n");
			return false;
		}
	
		result=system(command);
		if (result) {
			printf("GhostScript error %d - PostScript file could not be converted\n",result);
			outfile=infile;
			return false;
		}
	
		command="rm '";
		command+=infile+"'";
		system(command);
	}
	
	return true;
}



bool parse_legend_record(FILE*& in,xtring infile,int line,Legpars& spec,bool allow) {
	
	// Possible items: red, green, blue, hue, light, sat
	// markerstyle, linestyle, patternstyle, linewidth, markersize, patternscale
	
	const int MAXITEM=13;
	
	xtring ident[12]={"red","green","blue","hue","light","sat",
		"marker","line","pattern","mscale","lscale","pscale"};
		
	xtring sval[MAXITEM],name,param;
	int i,j,k,ct,pos,id,len;
	double dparam;
	
	readfor(in,"13a",sval);
	
	if (!feof(in)) {
		
		spec.label="";
		spec.red=spec.green=spec.blue=-1;
		spec.hue=spec.lightness=spec.saturation=-1;
		spec.marker=spec.line=spec.pattern=-1;
		spec.markersize=spec.linewidth=spec.patternscale=-1;
		
		for (i=0;i<MAXITEM;i++) {
	
			//printf("sval[%d]='%s'\n",i,(char*)sval[i]);
			
			if (sval[i]=="") return true;
			
			pos=sval[i].find('=');
			if (pos==-1) {
				if (spec.label=="") {
					
					if (!allow) {
						printf("In %s, line %d - too many legend items - maximum allowed is %d\n",
							(char*)infile,line,MAXLEGEND);
						return false;
					}
					
					spec.label=sval[i];
				}
				else {
					printf("In %s, line %d - could not parse '%s'\n",(char*)infile,line,(char*)sval[i]);
					return false;
				}
			}
			else if (!pos || sval[i].len()<pos+2) {
				printf("In %s, line %d - could not parse '%s'\n",(char*)infile,line,(char*)sval[i]);
					return false;
			}
			else {
				name=sval[i].left(pos);
				param=sval[i].mid(pos+1);
				if (!param.isnum()) {
					printf("In %s, line %d - number expected after '%s'=\n",(char*)infile,line,(char*)name);
					return false;
				}
				dparam=param.num();
				name=name.lower();
				len=name.len();
			
				ct=0;
				for (j=0;j<12;j++) {
					if (ident[j].len()>=len) {
						if (ident[j].left(len)==name) {
							ct++;
							id=j;
						}
					}
				}
				
				if (ct!=1) {
					printf("In %s, line %d - could not parse '%s'\n",(char*)infile,line,(char*)sval[i]);
					return false;
				}
				
				if (!allow) {
					printf("In %s, line %d - too many legend items - maximum allowed is %d\n",
						(char*)infile,line,MAXLEGEND);
					return false;
				}
				
				switch (id) {
					
					case 0: // red
						if (spec.red!=-1) {
							printf("In %s, line %d - duplicate value for RED\n",(char*)infile,line);
							return false;
						}
						else if (spec.hue!=-1 || spec.lightness!=-1 || spec.saturation!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>1.0) {
							printf("In %s, line %d - RED channel value must be in range 0-1\n",(char*)infile,line);
							return false;
						}
						spec.red=dparam;
						break;
						
					case 1: // green
						if (spec.green!=-1) {
							printf("In %s, line %d - duplicate value for GREEN\n",(char*)infile,line);
							return false;
						}
						else if (spec.hue!=-1 || spec.lightness!=-1 || spec.saturation!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>1.0) {
							printf("In %s, line %d - GREEN channel value must be in range 0-1\n",(char*)infile,line);
							return false;
						}
						spec.green=dparam;
						break;
						
					case 2: // blue
						if (spec.blue!=-1) {
							printf("In %s, line %d - duplicate value for BLUE\n",(char*)infile,line);
							return false;
						}
						else if (spec.hue!=-1 || spec.lightness!=-1 || spec.saturation!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>1.0) {
							printf("In %s, line %d - BLUE channel value must be in range 0-1\n",(char*)infile,line);
							return false;
						}
						spec.blue=dparam;
						break;
						
					case 3: // hue
						if (spec.hue!=-1) {
							printf("In %s, line %d - duplicate value for HUE\n",(char*)infile,line);
							return false;
						}
						else if (spec.red!=-1 || spec.green!=-1 || spec.blue!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>360.0) {
							printf("In %s, line %d - HUE must be in range 0-360\n",(char*)infile,line);
							return false;
						}
						spec.hue=dparam;
						break;
						
					case 4: // lightness
						if (spec.lightness!=-1) {
							printf("In %s, line %d - duplicate value for LIGHT\n",(char*)infile,line);
							return false;
						}
						else if (spec.red!=-1 || spec.green!=-1 || spec.blue!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>1.0) {
							printf("In %s, line %d - LIGHT must be in range 0-1\n",(char*)infile,line);
							return false;
						}
						spec.lightness=dparam;
						break;
						
					case 5: // saturation
						if (spec.saturation!=-1) {
							printf("In %s, line %d - duplicate value for SAT\n",(char*)infile,line);
							return false;
						}
						else if (spec.red!=-1 || spec.green!=-1 || spec.blue!=-1) {
							printf("In %s, line %d - cannot combine RGB and HLS colours for same legend item\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.0 || dparam>1.0) {
							printf("In %s, line %d - SAT must be in range 0-1\n",(char*)infile,line);
							return false;
						}
						spec.saturation=dparam;
						break;
						
					case 6: // marker
						if (spec.marker!=-1) {
							printf("In %s, line %d - duplicate value for MARKER\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0 || dparam>15) {
							printf("In %s, line %d - MARKER must be in range 0-15\n",(char*)infile,line);
							return false;
						}
						spec.marker=dparam;
						break;
						
					case 7: // line
						if (spec.line!=-1) {
							printf("In %s, line %d - duplicate value for LINE\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0 || dparam>18) {
							printf("In %s, line %d - LINE must be in range 0-18\n",(char*)infile,line);
							return false;
						}
						spec.line=dparam;
						break;

					case 8: // pattern
						if (spec.pattern!=-1) {
							printf("In %s, line %d - duplicate value for PATTERN\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0 || dparam>18) {
							printf("In %s, line %d - PATTERN must be in range 0-18\n",(char*)infile,line);
							return false;
						}
						spec.pattern=dparam;
						break;
					
					case 9: // mscale
						if (spec.markersize!=-1) {
							printf("In %s, line %d - duplicate value for MSCALE\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.1 || dparam>10.0) {
							printf("In %s, line %d - MSCALE must be in range 0.1-10\n",(char*)infile,line);
							return false;
						}
						spec.markersize=dparam*MARKERSIZE;
						break;
					
					case 10: // lscale
						if (spec.linewidth!=-1) {
							printf("In %s, line %d - duplicate value for LSCALE\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.1 || dparam>10.0) {
							printf("In %s, line %d - LSCALE must be in range 0.1-10\n",(char*)infile,line);
							return false;
						}
						spec.linewidth=dparam*LINEWIDTH;
						break;
					
					case 11: // pscale
						if (spec.patternscale!=-1) {
							printf("In %s, line %d - duplicate value for PSCALE\n",(char*)infile,line);
							return false;
						}
						else if (dparam<0.1 || dparam>10.0) {
							printf("In %s, line %d - PSCALE must be in range 0.1-10\n",(char*)infile,line);
							return false;
						}
						spec.patternscale=dparam;
						break;
				};
			}
		}
		
		return true;
	}
	
	else return false;
}

bool parse_legend_file(xtring filename,Legpars legpars[MAXLEGEND]) {
	
	FILE* in=fopen(filename,"rt");
	if (!in) {
		printf("Could not open legend file %s for input\n",(char*)filename);
		return false;
	}
	
	int i;
	for (i=0;i<MAXLEGEND;i++) legpars[i].id=-1;
	
	int ct=0;
	int line=1;
	bool done=false;
	
	while (!done) {
		
		if (ct<MAXLEGEND) {
			if (parse_legend_record(in,filename,line,legpars[ct],true)) {
				if (!(legpars[ct].label=="" &&
					legpars[ct].red==-1 && legpars[ct].green==-1 && legpars[ct].blue==-1 &&
					legpars[ct].hue==-1 && legpars[ct].lightness==-1 && legpars[ct].saturation==-1 &&
					legpars[ct].marker==-1 && legpars[ct].line==-1 && legpars[ct].pattern==-1 &&
					legpars[ct].markersize==-1 && legpars[ct].linewidth==-1 && legpars[ct].patternscale==-1)) {
					
					if (legpars[ct].hue!=-1 && legpars[ct].lightness==-1) legpars[ct].lightness=0.5;
					if (legpars[ct].hue!=-1 && legpars[ct].saturation==-1) legpars[ct].saturation=1.0;
					if (legpars[ct].hue==-1 && (legpars[ct].lightness!=-1 || legpars[ct].saturation!=-1)) {
						
						printf("In %s, line %d - HUE must be specified for HLS colour\n",(char*)filename,line);
						return false;
					}
					
					/*printf("%d '%s' :",ct,(char*)legpars[ct].label);
					if (legpars[ct].red!=-1) printf(" red=%g",legpars[ct].red);
					if (legpars[ct].green!=-1) printf(" green=%g",legpars[ct].green);
					if (legpars[ct].blue!=-1) printf(" blue=%g",legpars[ct].blue);
					if (legpars[ct].hue!=-1) printf(" hue=%g",legpars[ct].hue);
					if (legpars[ct].lightness!=-1) printf(" light=%g",legpars[ct].lightness);
					if (legpars[ct].saturation!=-1) printf(" sat=%g",legpars[ct].saturation);
					if (legpars[ct].marker!=-1) printf(" marker=%d",legpars[ct].marker);
					if (legpars[ct].line!=-1) printf(" line=%d",legpars[ct].line);
					if (legpars[ct].pattern!=-1) printf(" pattern=%d",legpars[ct].pattern);
					if (legpars[ct].markersize!=-1) printf(" markersize=%g",legpars[ct].markersize);
					if (legpars[ct].linewidth!=-1) printf(" linewidth=%g",legpars[ct].linewidth);
					if (legpars[ct].patternscale!=-1) printf(" patternscale=%g",legpars[ct].patternscale);
					printf("\n");*/
					
					legpars[ct].id=ct;
					ct++;
				}
				line++;
			}
			else if (feof(in)) {
				fclose(in);
				return true;
			}
			else {
				fclose(in);
				return false;
			}
		}
		else {
			if (parse_legend_record(in,filename,line,legpars[0],false))
				done=true;
			else {
				fclose(in);
				return false;
			}
		}
	}
	
	fclose(in);
	
	return true;
}


int main(int argc,char* argv[]) {

	xtring title,outfile,ext,infile,xtitle,ytitle,y2title,file;
	xtring ydataitem[MAXYITEM],xitem,label[MAXITEM];
	xtring y2dataitem[MAXYITEM];
	int ydataitemno[MAXYITEM],y2dataitemno[MAXYITEM],nrec;
	int nydataitem,nydataitemno,orient,xitemno;
	int ny2dataitem,ny2dataitemno;
	xtring header;
	bool ifmono,ifsinglex,ifsingley,ifsingley2;
	bool ifstack,ifscatter,ifoffset,ifinverse;
	bool iflogx,iflogy,iflogy2,ifframe,ifmtic;
	bool iftrend,ifeq;
	double textsize,linewidth;
	double width,height;
	double sxlo,sxhi,sylo,syhi,sy2lo,sy2hi;
	double sxr,syr,sy2r; 
	bool force_space_is_sep,force_space_not_sep;
	bool havegs,ifxview;
	xtring filelegend;
	Legpars legpars[MAXLEGEND];
	
	if (!processargs(argc,argv,infile,outfile,title,xtitle,ytitle,y2title,
		nydataitem,ydataitem,nydataitemno,ydataitemno,
		ny2dataitem,y2dataitem,ny2dataitemno,y2dataitemno,orient,
		ifmono,xitem,xitemno,
		ifstack,ifscatter,textsize,linewidth,width,height,ifoffset,
		sxlo,sxhi,sylo,syhi,sy2lo,sy2hi,ifinverse,iflogx,iflogy,iflogy2,
		ifframe,ifmtic,iftrend,ifeq,sxr,syr,sy2r,
  		havegs,ext,ifxview,
		force_space_is_sep,force_space_not_sep,filelegend))
			abort(argv[0]);

	// else ...

	unixtime(header);
	header=(xtring)"[GPLOT  "+header+"]\n\n";
	printf("%s",(char*)header);
	
	
	if (filelegend!="") {
		
		if (!parse_legend_file(filelegend,legpars))
			abort(argv[0]);
	}
	
	if (!readdata(infile,label,nydataitem,ydataitem,nydataitemno,ydataitemno,
		ny2dataitem,y2dataitem,ny2dataitemno,y2dataitemno,
		xitem,xitemno,ydata,xdata,nrec,ifsinglex,ifsingley,
		ifsingley2,xtitle,ytitle,y2title,ifstack,iflogx,iflogy,iflogy2,
		force_space_is_sep,force_space_not_sep))
			abort(argv[0]);
	
	if (havegs) file=(xtring)TEMPFILE;
	else file=outfile;
	
	if (startbengraph(file)==1) {
		printf("Could not open temporary file for output\n");
		abort(argv[0]);
	}

	plotgraph(outfile,nydataitem,ny2dataitem,nrec,ydata,xdata,ifmono,
		xtitle,ytitle,y2title,title,ifsinglex,ifsingley,ifsingley2,
		label,ydataitemno,y2dataitemno,xitemno,
		ifstack,ifscatter,textsize,linewidth,width,height,orient,ifoffset,
		sxlo,sxhi,sylo,syhi,sy2lo,sy2hi,ifinverse,iflogx,iflogy,iflogy2,
		ifframe,ifmtic,iftrend,ifeq,sxr,syr,sy2r,legpars,filelegend!="");
	
	if (endbengraph()) {
		if (havegs)
			printf("Could not write PostScript output to temporary file\n");
		else
			printf("Could not open %s for output\n",(char*)file);
		abort(argv[0]);
	};
	
	if (ifxview) {
		if (!callgs(file,outfile,"",true))
			abort(argv[0]);
	}
	
	if (havegs) {
		if (!callgs(file,outfile,ext,false))
			abort(argv[0]);
	}
	
	printf("\nOutput is in file %s\n\n",(char*)outfile);
	
	return 0;
};
