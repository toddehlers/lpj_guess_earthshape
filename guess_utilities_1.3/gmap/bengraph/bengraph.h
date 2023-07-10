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

#include <stdio.h>
#include <string>

enum commandtype{NONE,MOV,LIN};
enum shapetype{CIRCLE,SQUARE,THTRIANGLE,BHTRIANGLE,DIAMOND,VCROSS,DCROSS};
enum lclasstype{SOLID,DASH,SHAPE};
enum justtype{AUTOJUST,TL,TC,TR,CL,CC,CR,BL,BC,BR,LEFT,RIGHT,CENTRE};
enum fonttype{HELVETICA,TIMES,COURIER,SYMBOL};
enum patterntype{VL,HL,LDL,RDL,DH,HV,GREY,SHAPEFILL};
enum vectype{COAST,BOUND,SWEDEN,TEST};
enum projtype{GOODE,MERCATOR,WINKEL,MOLLWEIDE,ROBINSON,FAHEY,LAMBERT,CYLEQD,LAGRANGE,NPOLAR,SPOLAR};
enum colourscaletype{DITHER,RED,BLUE,GREEN,SHADE,REDVIOLET,VIOLETRED,REDBLUE,BLUERED,
	REDCYAN,CYANRED,GREYRED,GREYCYAN,COOL,WARM,NEEGREEN,NEERED,ALTITUDE,DEPTH,
	SOIL,VEGETATION,FIRE,WET,DRY};

// CONSTANTS AND DEFAULTS

const int MAXBGSTR=1024;
const int MAXBGPOINT=100000;  // in an enclosed shape
const double DEFAULT_LINEWIDTH=1.0;
const int MAXBGDASHSEG=7;
const int NFONT=3; // number of available fonts
const double PI=3.1415927;

const int MAXBGPATH=1024;
const int MAXBGDRIVE=1024;
const int MAXBGDIR=1024;
const int MAXBGFILE=1024;
const int MAXBGEXT=1024;
const int DRIVEBG=1;
const int DIRBG=2;
const int EXTBG=4;
const int WILDCARDSBG=8;

struct location {
	double h;
	double v;
}; //location in postscript point or page unit space

struct linedef {
	lclasstype lclass;
	double dsl[MAXBGDASHSEG]; // dash segment lengths: solid,dash,solid,dash, ..., 0
	double dxag; // multiplier for dash segment lengths
	int dsp; // where we are in dsl
	double dpr; // progress along this segment or between shapes
	shapetype shape;
	int sfilled; // fill shape?
	double swidth; // width of the shape
	double sspace; // spacing between shapes along line 
};

struct textdef {
	fonttype font;
	double size;
	int bold;
	int italic;
	int super;
	int sub;
};

struct markerdef {
	shapetype shape;
	int sfilled;
	double swidth;
	double bwidth; // border width in points
	double hwidth; // width of cross shapes
};

struct patterndef {
	patterntype pattern; // type of fill pattern
	shapetype shape; // shape
	double grey; // if fill=GREY, grey setting (0-->1)
	double lwidth; // of lines or shape outlines
	double spacing; // of lines or shapes (of horiz rows for shapes)
	int stagger; // whether or not for shapes
	int fill; // for shapes
};

struct numformat { // for printed numbers
	int width;
	int places;
	int exp;
};

struct datascaledef {
	bool zeroclass;
	bool dual;
		// if true this as a dual scale with separate colour scales for
		// negative and positive values
	bool logarithmic;
		// if true this is a logarithmic legend scale
		// class upper (in magnitude) boundaries are given in arrays bounds
		// and bounds_neg terminated by -1
	double* bounds;
	double* bounds_neg;
	
	// For normal (arithmetic) data scales
	int nclass;
	double minval;
	double interval;
	numformat format;
};

struct flexcolourdef {

	double red;
	double green;
	double blue;
	double offset;
};

struct colourscaledef {
	
	// Flexible colour scale (extension)
	
	flexcolourdef* pflexcolours;
	
	colourscaledef() {
		pflexcolours=NULL; // Signifies "not a flex colour scale"
	}
};

struct colourkeydef {
	std::string name; // Name of class
	double minval;
	double maxval;
	double colour[3]; // RGB assumed
};

const fonttype DEFAULT_FONT=HELVETICA;
const double DEFAULT_TEXTSIZE=12.0;
const double DEFAULT_DASHXAG=1.0;
const double DEFAULT_MARKERWIDTH=7.0;
const double DEFAULT_PATTERNSCALE=1.0;
const double HPROP=0.33;
	// width of the limb of a cross as a proportion of total width
const double MAXBGAUTOTIC=6.0; // maximum number of tics on automatic scaling

const int N_MARKER=16; // number of default marker symbols
const int N_LINE=19; // number of default linestyles
const int N_PATTERN=22; // number of default pattern styles

const double DEFAULT_PAGESCALE=2.829; // default page unit=mm
const double DEFAULT_PAGEWIDTH=594.1; // in ps points

const double DEFAULT_GRAPHBL_H=40.0;
const double DEFAULT_GRAPHBL_V=130.0;
const double DEFAULT_GRAPHTR_H=170.0;
const double DEFAULT_GRAPHTR_V=230.0; // default graph pos in mm

const int TIC_CENTRE=0;
const double DEFAULT_TICLENGTH=6.0; // tic length in points
const double DEFAULT_EHM=10.0; // error bar hat as a multiple of line width
const double DEFAULT_MTIC=0.5; // minor tic length as a proportion of major tic length

const int ZEROX=1;
const int ZEROY=2;
const int LOGX=4;
const int LOGY=8;
const int REVERSEX=16;
const int REVERSEY=32;
const int INVERSEAXES=64; // flag bits for newgraph

const int HIGH=1;
const int LOW=2; // flag bits for error bars

const int MARKER=1;
const int LINE=2;
const int PATTERN=4;
const int ERROR=8;
	// flag bits for legends (NB: also uses LINE from commandtype)

const double DEFAULT_LORIG_H=40.0; //legend parameters
const double DEFAULT_LORIG_V=90.0;
const double DEFAULT_LSPACING=3.0;
const double DEFAULT_LWIDTH=12.0;
const double DEFAULT_LHEIGHT=6.0;
const double DEFAULT_LTDISP=4.0;

const double DEFAULT_ARROW_ANGLE=40.0;
const double DEFAULT_ARROW_HW_MULT=4.0; // width of head as a propn of line

struct xypair {
	double x;
	double y;
}; // for certain graph attributes

struct scaledef {
	bool isgeographic; // true if geographic scaling
	double westbound;
		// for geographic scaling, latitude that corresponds to western edge of graph
	double eastbound;
	double northbound;
	double southbound;
	xypair mult; // multiplier
	xypair orig; // origin
	
	double min,max; // used by bengraph_geog
};

struct axisdef {
	double displ;
	double intercept;
	int reverse; // if true, axis goes from top to bottom instead
	int log; // if true, log transformed
	int zero; // if true, axis starts at zero if possible
};


struct ticdef {
	int centre; // 1=centred on axis
	double length; // page units
	double width; // points
};

struct labeldef {
	int width;
	int places;
	int exp;
	justtype just; // rotation
	double rot; // justification
};

struct errordef { // error bar design
	double hatwidth;
	double linewidth;
	int flags; // HIGH or LOW or both
};

struct legenddef { // legend layout parameters
	location orig; // current origin (moved as items are added)
	double spacing; // between legend boxes (page units)
	double width; // of legend box
	double height; // of legend box
	int border; // 1=border; 0=none
	double bwidth; // of border in points
	double tdisp; // text displacement
	justtype just; // text justification
	int direction; // 0=down; 1=across
	location start; // original origin (used by legendtext)
	int flags;
};

struct arrowdef {
	double angle; // of head
	double hwidth; // of head (ps points)
	double lwidth; // of line (ps points)
	int filled;
};

const double SSMULT=0.67; // multiplier for point size of super/subscripts
const double LINESPACE=1.25; // spacing between lines of text
const char TEMPDATAFILE[50]="data.xxbg";

// GLOBAL VARIABLE DECLARATIONS

extern FILE* out;
extern double grey;
extern double lwd;
extern int transp;
extern int pageorientation;
extern double pagewidth; // in ps points = 594.1 for A4
extern location boxbl,boxtr; // bounding box
extern char filename[MAXBGPATH];
extern char pstitle[MAXBGFILE];
extern char tempfil[]; // temporary postscript file
extern linedef line_style;
extern textdef text_style;
extern markerdef marker_style;
extern patterndef pattern_style;
extern long colour_style; // default colour in hexadecimal RGB format 0xRRGGBB
extern FILE *tdf;
extern int dataopen; // whether temporary data file open
extern double mindx,mindy,minde,maxdx,maxdy,maxde;
extern int seriesid;
extern double pagescale; // scaler from page units to ps points
extern scaledef graphscale; // scaler from graph units to page units
extern location graphbl;
extern location graphtr; // corners of the graph area corresponding to graphscale
extern xypair autotic; // not a location, but the automatic tic interval
extern xypair minortic; // interval between minor tics
extern int usermarker; // flag to say user has explicitly chosen a marker symbol
extern int userline; // flag
extern int userpattern; // flag
extern int usernumformat;
extern int usercolour; // flag
extern double outlinepats; // flag for outlining of histogram bars (-ve = no outline, +ve = linewidth)
extern int logerror; // flag
extern axisdef xaxispars;
extern axisdef yaxispars;
extern ticdef tic_style;
extern ticdef minortic_style;
extern labeldef xlabel_style;
extern labeldef ylabel_style;
extern errordef error_style;
extern legenddef legend_style;
extern arrowdef arrow_style;
extern numformat num_style;
extern int axinverse; // if true, user x-axis is vertical, user y-axis horizontal
extern int wantminortics; // whether wanted or not
extern int iseps;
extern datascaledef datascale_curr;
extern colourkeydef* pcolourkey;
extern int ncolourkey; // number of classes in current colour key

// PUBLIC FUNCTIONS

void psout(char* str);
void psout(double d);
void psout(int i);
void pc(double x);
void pc(double h,double v,commandtype c);
void psheader();

int bgsprint(int length,char* string,double var,int width,int places);
int bgsprint(int length,char* string,double var,
	int width,int places,int exp);
int autoscalex(double minx,double maxx,double round,int fixrange,
	axisdef& xaxispars,xypair& minortic,xypair& autotic,
	location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& xlabel_style);
int autoscaley(double miny,double maxy,double round,int fixrange,
	axisdef& yaxispars,xypair& minortic,xypair& autotic,
	location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& ylabel_style);
int reload(int id,int& nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo,FILE*& tdf,int& dataopen);
int fillint(int id1,int id2,int toaxis,
	scaledef& graphscale,double pagescale,int& userpattern,FILE*& tdf,
	int& dataopen,int& axinverse,double& mindx,double& maxdx,
	legenddef& legend_style,patterndef& pattern_style);
int logprint(char* labtext,double ctic,int exp);
void dprint(char* buffer,double dval,int scale,int& usernumformat,
	numformat& numstyle);
double sign(double d);
int bgstart(char* fil);
void assign_linestyle(lclasstype lclass,double d0,double d1,
	double d2,double d3,double d4,double d5,double d6,
	shapetype shape,int sfilled,linedef& line_style);
void assign_markerstyle(shapetype shape,int filled,double mult,
	markerdef& marker_style);
double xlog(double d,axisdef& xaxispars,int& logerror);
double ylog(double d,axisdef& yaxispars,int& logerror);
int opendata(FILE*& tdf,int& dataopen);
void next_page();
int data_(int id,int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo,axisdef& xaxispars,axisdef& yaxispars,
	int& logerror,FILE*& tdf,double& mindx,double& maxdx,double& mindy,
	double& maxdy,double& minde,double& maxde,int& dataopen,xypair& minortic,
	xypair& autotic,location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& xlabel_style,labeldef& ylabel_style,
	int& seriesid);
int fnsplit(char* filename,char* drive,char* dir,char* file,char* ext);
int strcmpi(char* w1,char* w2);
void scrout(char* str);
void scrout(double d);
void scrout(int i);
void next_page();
int bgend();
int bgstart(char* filnam,double user_pagewidth);
void shape_point(shapetype shape,location point,
	double size,double thick,int filled);
void scatter_point(int npoint,location* point,markerdef markerstyle);
void curve_point(int npoint,location* point,linedef linestyle);
void text_point(char* text,location point,textdef textstyle,
	justtype just,double rot);
void filline(location s, location f);
void fillpoly(int n,location* clippath,int outline,patterndef patternstyle);
void vareabetween(int n1,location* curve1,int n2,
	location* curve2,patterndef patternstyle);
void hareabetween(int n1,location* curve1,int n2,
	location* curve2,patterndef patternstyle);
void set_grey_internal(double val);
double set_grey(double val);
double set_linewidth(double val);
int set_transparent(int val);
int closedpoly(int npoint,location* point,int filled);
int orientation(int i);
void newgraph(double blx,double bly,double trx,double tryy,int flags);
void newgraph(double blx,double bly,double trx,double tryy);
void newgraph();
void newgraph(int flags);
fonttype textfont(fonttype newfont);
fonttype textfont();
double textsize(double newsize);
double textsize();
void textbold(int newstatus);
void textitalic(int newstatus);
void markerstyle(int n);
void linestyle(int n);
double linewidth(double w);
double linewidth();
double markersize(double w);
double markersize();
double patternscale(double d);
double patternscale();
void patternstyle(int n);
int startbengraph(char* filnam);
int startbengraph(char* filnam,int eps);
int endbengraph();
double gtopx(double g);
double gtopy(double g);
double ptogx(double p);
double ptogy(double p);
double ptops(double p);
double pstop(double ps);
double maxx();
double maxy();
double minx();
double miny();
double maxe();
double mine();
int data(int id,int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo);
int data(int id,int nitem,double* xdata,double* ydata,
	double* error);
int data(int id,int nitem,double* xdata,double* ydata);
int data(int id,int nitem,double* ydata);
int data(int nitem,double* xdata,double* ydata,
	double* errorhi,double* errorlo);
int data(int nitem,double* xdata,double* ydata,
	double* error);
int data(int nitem,double* xdata,double* ydata);
int data(int nitem,double* ydata);
void scalex(double min,double max,double round);
void scalex(double min,double max);
void scalex(double round);
void scaley(double min,double max,double round);
void scaley(double min,double max);
void scaley(double round);
int curve(int id);
int curve();
int scatter(int id);
int scatter();
double errorstyle(double linewidth,double hatwidth,int flags);
double errorstyle(double linewidth,int flags);
double errorstyle(double linewidth);
double errorstyle(int flags);
double errorstyle();
int errorbars(int id);
int errorbars();
int fillbetween(int id1,int id2);
int fillunder(int id);
void expandx(double left,double right);
void expandx();
int histogram(int id,double width,double displ,int startx);
int histogram(int id,double width);
int histogram(int id);
int histogram();
void xaxis(double displ,double yintercept,int print);
void xaxis(double displ,double yintercept);
void xaxis(double displ);
void xaxis();
void yaxis(double displ,double xintercept,int print);
void yaxis(double displ,double xintercept);
void yaxis(double displ);
void yaxis();
void border();
double ticstyle(double length,double width,int centre);
double ticstyle(double length,int centre);
double ticstyle(double length);
double ticstyle();
double minorticstyle(double length,double width,int centre);
double minorticstyle(double length,int centre);
double minorticstyle(double length);
double minorticstyle();
void minortics(int i);
void minortics();
void xtics(double xdispl);
void xtics();
int xtic(double xpos);
void ytics(double ydispl);
void ytics();
int ytic(double ypos);
void xlabels(double xdispl);
void xlabels();
void xlabels(double xdispl,justtype just,double rotation);
void xlabels(justtype just,double rotation);
int xlabel(double xpos,char* text,justtype just,double rotation);
void xlabel(double xpos,double dval,justtype just,double rotation);
void xlabel(double xpos,long lval,justtype just,double rotation);
void xlabel(double xpos,int ival,justtype just,double rotation);
void xlabel(double xpos,justtype just,double rotation);
void xlabel(double xpos,char* text);
void xlabel(double xpos,double dval);
void xlabel(double xpos,long lval);
void xlabel(double xpos,int ival);
void xlabel(double xpos);
void ylabels(double ydispl);
void ylabels();
void ylabels(double ydispl,justtype just,double rotation);
void ylabels(justtype just,double rotation);
int ylabel(double ypos,char* text,justtype just,double rotation);
void ylabel(double ypos,double dval,justtype just,double rotation);
void ylabel(double ypos,long lval,justtype just,double rotation);
void ylabel(double ypos,int ival,justtype just,double rotation);
void ylabel(double ypos,justtype just,double rotation);
void ylabel(double ypos,char* text);
void ylabel(double ypos,double dval);
void ylabel(double ypos,long lval);
void ylabel(double ypos,int ival);
void ylabel(double ypos);
void textformat(int w,int p,int exp);
void textformat(int w,int p);
void text(double x,double y,char* text,justtype just,double rotation);
void text(double x,double y,double dval,justtype just,double rotation);
void text(double x,double y,long longval,justtype just,double rotation);
void text(double x,double y,int ival,justtype just,double rotation);
void text(double x,double y,char* title,justtype just);
void text(double x,double y,long lval,justtype just);
void text(double x,double y,double dval,justtype just);
void text(double x,double y,int ival,justtype just);
void text(double x,double y,char* title);
void text(double x,double y,long lval);
void text(double x,double y,double dval);
void text(double x,double y,int ival);
void title(char* title,double displ,justtype just);
void title(char* text);
void title(char* text,double displ);
void title(char* text,justtype just);
void xtitle(char* title,double ydispl,justtype just);
void xtitle(char* title,justtype just);
void xtitle(char* title,double ydispl);
void xtitle(char* title);
void ytitle(char* title,double xdispl,justtype just);
void ytitle(char* title,justtype just);
void ytitle(char* title,double xdispl);
void ytitle(char* title);
void newlegend(double posh,double posv,int flags);
void newlegend(double posh,double posv);
void legendstyle(double width,double height,double spacing,
	int direction,double border,double textoffset,justtype just);
void legendstyle(double width,double height,double spacing,
	int direction,double border,double textoffset);
void legend(int flags,char* text);
void legend(char* text);
void legendtext(char* text,double sbefore,double safter);
void legendtext(char* text);
void arrow(double sx,double sy,double fx,double fy);
double arrowstyle(double width,double headwidth,double angle,int fill);
double arrowstyle(double width,int fill);
double arrowstyle(double width);
double arrowstyle();
void marker(double h,double v);
void line(double h1,double v1,double h2,double v2);
void pattern(double blh,double blv,double trh,double trv);
void rectangle(double blh,double blv,double trh,double trv);
void colour(double red,double green,double blue,int hls);
void colour(double red,double green,double blue);
void colour();
void transparent(int i);
void transparent();
double outline(double d);
double outline();
void testpage();
void newpage();
void plotbound(vectype vec);
void plotpoly(char* filename);
void frame();
double mercator(double lat);
void fillgrid(double west,double south,double east,double north);
void fillgrid(double west,double south,double east,double north,double val);
void fillgrid(double west,double south,double east,double north,double val,int format);
void fillgrid_fast(double west,double south,double east,double north,double val);
void clip_window();
void restore();
void plotgrid(double lon,double lat);
void scalegeog(double west,double south,double east,double north,projtype proj);
void scalegeog(double west,double south,double east,double north,projtype proj,double& aspect);
void scalegeog(double west,double south,double east,double north,projtype proj,
	double lon0,double& aspect);
void filldata(double minx,double miny,double maxx,double maxy,double val);
void filldata(double minx,double miny,double maxx,double maxy,double val,int format);
void filldata_fast(double minx,double miny,double maxx,double maxy,double val);
void datascale(int nclass,double minval,double interval,bool zeroclass);
void datascale(double minval,double maxval,double round);
void datascale(double minval,double maxval);
void datascale_nozeroclass(double minval,double maxval,double round);
void datascale_nozeroclass(double minval,double maxval);
void datascale_logarithmic(double minval,double maxval,int nclass);
void datascale_logarithmic(double minval,double maxval);
void datascale_logarithmic_nozeroclass(double minval,double maxval,int nclass);
void datascale_logarithmic_nozeroclass(double minval,double maxval);
void scalelegend(double left,double bottom,double right,double top,
	bool vertical,char* units,bool closemax);
void scalelegend(double left,double bottom,double right,double top);
void scalelegend(double left,double bottom,double right,double top,bool closemax);
void scalelegend(double left,double bottom,double right,double top,char* units);
void scalelegend(double left,double bottom,double right,double top,
	bool vertical,char* units,bool closemax);
void scalelegend(double left,double bottom,double right,double top,
	bool vertical,char* units,bool closemax,bool skipodd);
void colourscale(colourscaletype type);
void colourscale(colourscaletype tneg,colourscaletype tpos);
void colourscale(flexcolourdef* c);
void colourscale(flexcolourdef* cneg,flexcolourdef* cpos);
void lonlattop(double lon,double lat,double& px,double& py);
void grid(double lonint,double latint,bool ifframe);
void grid(double lonint,double latint);
void setcolourkey(colourkeydef* c,int nclass,bool ifrgb);
void forgetcolourkey();
