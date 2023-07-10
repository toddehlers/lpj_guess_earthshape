//////////////////////////////////////////////////////////////////////////////
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
extern "C" {
#include <lib_proj.h>
}
#include <bengraph_path.h>

const int MAXCLASS=200;
const double MINLAT=-89.9,MAXLAT=89.9;
const double MAXGRIDPOLAR=85;
double bounds[MAXCLASS],bounds_neg[MAXCLASS];

int setstyle(int width,int places,int exp,labeldef& xlabel_style) {

	// sets x-axis label style parameters

	xlabel_style.width=width;
	xlabel_style.places=places;
	xlabel_style.exp=exp;
	return 1;

}

int autoscale_legend(double minx,double maxx,double round,int fixrange,
	axisdef& xaxispars,xypair& minortic,xypair& autotic,
	location& graphtr,location& graphbl,scaledef& graphscale,
	int& axinverse,labeldef& xlabel_style) {

	const double MAXCLASSES=11; // one > maximum number of classes in legend 
	
	if (xaxispars.zero && !xaxispars.log) {
		if (minx<=0.0 && maxx<=0.0) maxx=0.0;
		else if (minx>=0.0 && maxx>=0.0) minx=0.0;
	}

	graphscale.isgeographic=false;

	double unit,gmax,gmin,minorunit;
	double interval=maxx-minx;
	double mord;

	if (!interval) interval=1.0;

	if (xaxispars.log) {
		autotic.x=1;
		if (interval>=MAXCLASSES) minortic.x=0.0;
		else minortic.x=1.0; // between log 1.0 and log 10.0
	}
	else {
		mord=floor(log10(interval));
		interval/=pow(10.0,mord);	// now in the range 1.0-9.9999

		if (round>=10.0) {
			unit=0.1;
			if (interval/unit>MAXCLASSES) unit=0.2;
			if (interval/unit>MAXCLASSES) unit=0.3;
			if (interval/unit>MAXCLASSES) unit=0.5;
			if (interval/unit>MAXCLASSES) unit=1.0;
			if (interval/unit>MAXCLASSES) unit=2.0;
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
	
	if (autotic.x>999999.0) return setstyle(0,1,1,xlabel_style);
	if (autotic.x>=1.0) return setstyle(0,0,0,xlabel_style);
	if (autotic.x>=0.09999) return setstyle(0,1,0,xlabel_style);
	if (autotic.x>=0.009999) return setstyle(0,2,0,xlabel_style);
	if (autotic.x>=0.0009999) return setstyle(0,3,0,xlabel_style);
	if (autotic.x>=0.00009999) return setstyle(0,4,0,xlabel_style);
	return setstyle(0,1,1,xlabel_style);
}

bool compute_bounds(int nclass,double maxval,double* bounds) {

	// Computes class boundaries for logarithmic data scales
	
	double base,fac;
	double xmin,xmax,t,v;
	double rmin,rmax,mag;
	bool found;
	int c,sig;
	
	const double FACTOR=1.0e3;
	
	if (maxval<=0.0 || nclass<1) return false;

	fac=pow(10.0,3.0-floor(log10(maxval)));
		
	maxval*=fac;
	
	base=exp(log(maxval)/(double)nclass);
		
	xmin=0;
	t=1;
	c=0;
	
	while (xmin<maxval) {
	
		t*=base;
		
		rmin=t;
		rmax=(t*base-t)*0.5+t;
		
		// Now look for suitable number between rmin and rmax
		
		mag=pow(10.0,floor(log10(rmin)));
				
		sig=0;
		found=false;
		
		while (!found) {
		
			v=pow(10.0,sig);
			if (fabs(mag*v-rmin)<mag*0.0001) {
				xmax=mag*v;
				found=true;
			}
			else if (mag*v*10.0>=rmin && mag*v*10.0<=rmax) {
				xmax=mag*v*10.0;
				found=true;
			}
			else if (mag*v*5.0>=rmin && mag*v*5.0<=rmax) {
				xmax=mag*v*5.0;
				found=true;
			}
			else {
				while (mag*v<rmin) v++;
			
				if (mag*v<=rmax) {
					xmax=mag*v;
					found=true;
				}
			}
						
			if (!found) {
				mag*=0.1;
			}
			
			sig++;
		}
		
		//printf("bounds[%d]=%g/%g=%g\n",c,xmax,fac,xmax/fac);
		
		bounds[c++]=xmax/fac;
		xmin=xmax;
	}
	
	bounds[c]=-1.0;
}


void scaledata(double minval,double maxval,double round,bool nozeroclass,
	bool logarithmic,datascaledef& datascale) {

	// For logarithmic legends, round is desired number of classes
	
	axisdef axdummy;
	axdummy.displ=0.0;
	axdummy.intercept=0.0;
	axdummy.reverse=false;
	axdummy.log=false;
	axdummy.zero=false;
	
	xypair minortic,autotic;
	location graphbl={0,0}, graphtr={0,0};
	
	scaledef scale;
	
	int axinverse=false;
	int nclass,ct;
	double biggest;
	
	labeldef style;
	
	if (logarithmic && (minval || maxval)) {
		
		datascale.logarithmic=true;	

		if (maxval>0.0 && minval>=0.0) {
			nclass=round;
			if (nclass<1) nclass=1;
			compute_bounds(nclass,maxval,datascale.bounds);
			datascale.bounds_neg=NULL;
		}
		else if (minval<0.0 && maxval<=0.0) {
			nclass=round;
			if (nclass<1) nclass=1;
			compute_bounds(nclass,-minval,datascale.bounds_neg);
			datascale.bounds=NULL;
		}
		else if (minval<0.0 && maxval>0.0) {
			nclass=round/2.0+1;
			if (maxval>-minval) {
				do {
					compute_bounds(nclass,maxval,datascale.bounds);
					ct=0;
					while (datascale.bounds[ct]<-minval) {
						datascale.bounds_neg[ct]=datascale.bounds[ct];
						ct++;
					}
					datascale.bounds_neg[ct]=datascale.bounds[ct];
					ct++;
				} while (ct+nclass++<round);
				datascale.bounds_neg[ct]=-1;
			}
			else {
				do {
					compute_bounds(nclass,-minval,datascale.bounds_neg);
					ct=0;
					while (datascale.bounds_neg[ct]<maxval) {
						datascale.bounds[ct]=datascale.bounds_neg[ct];
						ct++;
					}
					datascale.bounds[ct]=datascale.bounds_neg[ct];
					ct++;
				} while (ct+nclass++<round);
				datascale.bounds[ct]=-1;
			}
		}
		
		if (nozeroclass)
			datascale.zeroclass=false;
		else
			datascale.zeroclass=true;
	}
	else {
	
		datascale.logarithmic=false;
	
		autoscale_legend(minval,maxval,round,0,axdummy,minortic,autotic,
			graphtr,graphbl,scale,axinverse,style);
			
		nclass=int((scale.max-scale.min)/autotic.x+0.1);
		
		datascale.minval=scale.min;
		if (nozeroclass)
			datascale.zeroclass=false;	
		else if (minval<=0.0 && maxval>=0.0)
			datascale.zeroclass=true; // BLARP!!
		datascale.interval=autotic.x;
		datascale.nclass=int((scale.max-scale.min)/autotic.x+0.1);
		datascale.format.width=style.width;
		datascale.format.places=style.places;
		datascale.format.exp=style.exp;
		
		//printf("datascale.minval=%g datascale.interval=%g\n",datascale.minval,datascale.interval);
	}
}


int places(double dval) {

	// Returns number of decimal places required to represent dval
	// up to MAXPLACES
	
	const int MAXPLACES=6;
	int p=0;
	
	if (dval<0) dval=-dval;
	
	while (fabs(dval-int(dval+0.0000001))>0.000001 && p<MAXPLACES) {
		dval*=10;
		p++;
	}
		
	return p;
}


void datascale(int nclass,double minval,double interval,bool zeroclass) {	
	
	if (!interval || nclass<(int)zeroclass+1) return;
	
	if (interval<0.0) {
		minval+=interval*nclass;
		interval=-interval;
	}

	//if (zeroclass) minval=0.0;
	
	datascale_curr.interval=interval;
	datascale_curr.minval=minval;
	datascale_curr.nclass=nclass;
	datascale_curr.zeroclass=zeroclass;
	
	int maxp=0,p,i;
	double d;
	
	for (i=0;i<nclass-(int)zeroclass;i++) {
		d=minval+i*interval;
		p=places(d);
		if (p>maxp) maxp=p;
	}
		
	datascale_curr.format.width=0;
	datascale_curr.format.places=maxp;
	datascale_curr.format.exp=false;
}

void datascale(double minval,double maxval) {

	scaledata(minval,maxval,10,false,false,datascale_curr);
}

void datascale(double minval,double maxval,double round) {

	scaledata(minval,maxval,round,false,false,datascale_curr);
}

void datascale_nozeroclass(double minval,double maxval,double round) {

	scaledata(minval,maxval,round,true,false,datascale_curr);
}

void datascale_nozeroclass(double minval,double maxval) {

	scaledata(minval,maxval,10,true,false,datascale_curr);
}

void datascale_logarithmic(double minval,double maxval) {

	datascale_curr.bounds=bounds;
	datascale_curr.bounds_neg=bounds_neg;
	scaledata(minval,maxval,10,false,true,datascale_curr);
}

void datascale_logarithmic(double minval,double maxval,int nclass) {
	
	datascale_curr.bounds=bounds;
	datascale_curr.bounds_neg=bounds_neg;
	if (nclass>MAXCLASS-10) nclass=MAXCLASS-10;	
	scaledata(minval,maxval,nclass,false,true,datascale_curr);
}

void datascale_logarithmic_nozeroclass(double minval,double maxval) {

	scaledata(minval,maxval,10,true,true,datascale_curr);
}

void datascale_logarithmic_nozeroclass(double minval,double maxval,int nclass) {
	
	datascale_curr.bounds=bounds;
	datascale_curr.bounds_neg=bounds_neg;
	if (nclass>MAXCLASS-10) nclass=MAXCLASS-10;	
	scaledata(minval,maxval,nclass,true,true,datascale_curr);
}


PJ* ref; 
	
void init_proj(projtype proj,double lon0) {
	
	char string1[32],string2[32];
	
	static char *param_goode[]={
		"proj=goode",
		"ellps=clrk66"};

	static char *param_mercator[]={
		"proj=merc",
		"ellps=clrk66"};

	static char *param_winkel[]={
		"proj=wink2",
		"ellps=clrk66",
		"lat_1=0N"};

	static char *param_mollweide[]={
		"proj=moll",
		"ellps=clrk66"};

	static char *param_robinson[]={
		"proj=robin",
		"ellps=clrk66"};

	static char *param_fahey[]={
		"proj=fahey",
		"ellps=clrk66"};

	static char *param_lambert[]={
		"proj=cea",
		"ellps=clrk66"};

	static char *param_cyleqd[]={
		"proj=eqc",
		"ellps=clrk66"};

	static char *param_lagrange[]={
		"proj=lagrng",
		"ellps=clrk66"};
	
	static char *param_polar[]={
		"proj=stere",
		"ellps=clrk66",
		"lat_0=XX",
		"lon_0=XX"};
	
	switch (proj) {
		case GOODE:
			ref=pj_init(sizeof(param_goode)/sizeof(char*),param_goode);
			break;
		case MERCATOR:
			ref=pj_init(sizeof(param_mercator)/sizeof(char*),param_mercator);
			break;
		case WINKEL:
			ref=pj_init(sizeof(param_winkel)/sizeof(char*),param_winkel);
			break;
		case MOLLWEIDE:
			ref=pj_init(sizeof(param_mollweide)/sizeof(char*),param_mollweide);
			break;
		case ROBINSON:
			ref=pj_init(sizeof(param_robinson)/sizeof(char*),param_robinson);
			break;
		case FAHEY:
			ref=pj_init(sizeof(param_fahey)/sizeof(char*),param_fahey);
			break;
		case LAMBERT:
			ref=pj_init(sizeof(param_lambert)/sizeof(char*),param_lambert);
			break;
		case CYLEQD:
			ref=pj_init(sizeof(param_cyleqd)/sizeof(char*),param_cyleqd);
			break;
		case LAGRANGE:
			ref=pj_init(sizeof(param_lagrange)/sizeof(char*),param_lagrange);
			break;
		case NPOLAR:
			strcpy(string1,"lat_0=90.0");
			sprintf(string2,"lon_0=%g",lon0);
			param_polar[2]=string1;
			param_polar[3]=string2;
			ref=pj_init(sizeof(param_polar)/sizeof(char*),param_polar);
			break;
		case SPOLAR:
			strcpy(string1,"lat_0=-90.0");
			sprintf(string2,"lon_0=%g",lon0);
			param_polar[2]=string1;
			param_polar[3]=string2;
			ref=pj_init(sizeof(param_polar)/sizeof(char*),param_polar);
			break;
		default:
			printf("Unknown projection %d\n",proj);
			exit(99);
	}

	if (!ref) {
		char* emess;
		emess=pj_strerrno(pj_errno);
		printf("Could not initialise projection: %s\n",emess);
		exit(99);
	}	
}

void init_proj(projtype proj) {

	init_proj(proj,0);
}


void scalegeog(double west,double south,double east,double north,projtype proj,
	double lon0,double& aspect) {

	// Scales graph area as a map stretching from west (degrees) to east (degrees)
	// and from south (degrees) to north (degrees)
	// lon0 = origin longitude for polar projections 

	const double RES=0.01;
	
	double northy,southy,westx,eastx;
	double minx,maxx,miny,maxy,delx,dely,x,y;
	double graspect;
	bool first;
	XY cartesian;
	LP lp;

	init_proj(proj,lon0);
	
	west=fmod(west+180.0,360.0)-180.0;
	east=fmod(east+180.0,360.0)-180.0;
	
	if (west>=east) east+=360.0;

	if (north>MAXLAT) north=MAXLAT;
	if (south<MINLAT) south=MINLAT;

	yaxispars.intercept=west;

	graphscale.isgeographic=false;
	graphscale.westbound=west;
	graphscale.eastbound=east;
	graphscale.northbound=north;
	graphscale.southbound=south;

	graphscale.mult.x=(graphtr.h-graphbl.h)/(east-west);
	graphscale.orig.x=graphbl.h-west*graphscale.mult.x;

	xaxispars.intercept=south;

	// Mercator projection value for north:

	lp.lam=west*DEG_TO_RAD;
	lp.phi=0.0;
	cartesian=pj_fwd(lp,ref);
	westx=cartesian.x;
	
	lp.lam=east*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	eastx=cartesian.x;
	
	lp.lam=0.0;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	northy=cartesian.y;
	
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	southy=cartesian.y;
	
	// Scan plot area to find coordinate bounds
	
	delx=(east-west)*RES;
	dely=(north-south)*RES;
	
	x=west;
	first=true;
	while (x<=east) {
	
		y=south;
		while (y<=north) {
		
			lp.lam=x*DEG_TO_RAD;
			lp.phi=y*DEG_TO_RAD;
			cartesian=pj_fwd(lp,ref);

			if (first) {
				minx=maxx=cartesian.x;
				miny=maxy=cartesian.y;
				first=false;
			}
			else {
				if (cartesian.x<minx) minx=cartesian.x;
				else if (cartesian.x>maxx) maxx=cartesian.x;
				if (cartesian.y<miny) miny=cartesian.y;
				else if (cartesian.y>maxy) maxy=cartesian.y;
			}
			
			y+=dely;
		}
		
		x+=delx;
	}
		
	graspect=(graphtr.h-graphbl.h)/(graphtr.v-graphbl.v);
	aspect=(maxx-minx)/(maxy-miny);
	
	if (aspect<graspect) {
	
		// Fit height in window

		graphscale.mult.y=(graphtr.v-graphbl.v)/(maxy-miny);
		graphscale.orig.y=graphbl.v-miny*graphscale.mult.y;
		
		graphscale.mult.x=(graphtr.h-graphbl.h)/(maxx-minx)*aspect/graspect;
		graphscale.orig.x=graphbl.h-minx*graphscale.mult.x+(1.0-aspect/graspect)*0.5*(graphtr.h-graphbl.h);
	}
	else {

		graphscale.mult.x=(graphtr.h-graphbl.h)/(maxx-minx);
		graphscale.orig.x=graphbl.h-minx*graphscale.mult.x;
		
		graphscale.mult.y=(graphtr.v-graphbl.v)/(maxy-miny)*graspect/aspect;
		graphscale.orig.y=graphbl.v-miny*graphscale.mult.y+(1.0-graspect/aspect)*0.5*(graphtr.v-graphbl.v);
	
	}
}

void scalegeog(double west,double south,double east,double north,projtype proj,double& aspect) {

	scalegeog(west,south,east,north,proj,0,aspect);
}

void scalegeog(double west,double south,double east,double north,projtype proj) {

	double aspect;
	scalegeog(west,south,east,north,proj,0,aspect);
}

void plotbound(vectype vec) {

	// Plots boundaries (coasts or political boundaries) on current graph

	const int MAXSEG=10000;
	
	XY cartesian;
	LP lp;

	int nseg,n,ct;
	double lon,lat;
	float flon,flat;
	location vertex[MAXSEG];
	FILE* in;

	// temporary

	if (vec==COAST)
		in=fopen(file_veccoast,"rb");
	else if (vec==BOUND) {
		in=fopen(file_vecbound,"rb");
		//printf("Reading from %s in=%d\n",file_vecbound,in);
	}
	else if (vec==SWEDEN)
		in=fopen(file_vecsweden,"rb");

	if (!in) return;
	
	psout("gsave\n");
	set_grey_internal(1.0);
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
	set_grey_internal(grey);
	psout("newpath\n");

	while (!feof(in)) {
		if (vec==TEST) {
			fscanf(in,"%d",&nseg);
		}
		else fread(&nseg,sizeof(int),1,in);
		//printf("nseg=%d\n",nseg);
		if (!feof(in)) {

			ct=0;
			for (n=0;n<nseg;n++) {
				if (vec==TEST) {
					fscanf(in,"%g %g",&flon,&flat);
					lon=flon;
					lat=flat;
				}
				else {
					fread(&lon,sizeof(double),1,in);
					fread(&lat,sizeof(double),1,in);
				}
				
				lp.lam=lon*DEG_TO_RAD;
				lp.phi=lat*DEG_TO_RAD;
				cartesian=pj_fwd(lp,ref);

				if (lat<=MAXLAT && lat>=MINLAT) {
			
					if (ct<MAXSEG) {
						vertex[ct].h=cartesian.x;
						vertex[ct].v=cartesian.y;
					}
					ct++;
				}
			}

			nseg=ct;

			if (nseg>MAXSEG) nseg=MAXSEG;
			for (n=0;n<nseg-1;n++) {
				if (vertex[n].h>=ptogx(graphbl.h) && vertex[n].h<=ptogx(graphtr.h) &&
					vertex[n].v>=ptogy(graphbl.v) && vertex[n].v<=ptogy(graphtr.v) ||
					vertex[n+1].h>=ptogx(graphbl.h) && vertex[n+1].h<=ptogx(graphtr.h) &&
					vertex[n+1].v>=ptogy(graphbl.v) && vertex[n+1].v<=ptogy(graphtr.v)) { 
					line(gtopx(vertex[n].h),gtopy(vertex[n].v),
						gtopx(vertex[n+1].h),gtopy(vertex[n+1].v));
							
				}
			}
		}
	}

	psout("grestore\n");

	fclose(in);
}


void plotpoly(char* filename) {
	
	// Fills polygons with white (for ocean mask)
	
	const int MAXSEG=10000;
	
	XY cartesian;
	LP lp;

	int narc,n,m,ncoord,ct;
	double lon,lat,initialx,initialy,x,y;
	float minx,maxx,miny,maxy;
	float flon,flat;
	location vertex[MAXSEG];
	bool first,inwindow,warned=false;
	FILE* in;

	double east=ptogx(graphbl.h);
	double west=ptogx(graphtr.h);
	double south=ptogy(graphbl.v);
	double north=ptogy(graphtr.v);
	
	// temporary

	in=fopen(filename,"rt");
	if (!in) {
		printf("Could not open mask file %s for input\n",filename);
		return;
	}

	psout("gsave\n");
	set_grey_internal(1.0);
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
	set_grey_internal(grey);
	
	while (!feof(in)) {
		
		fscanf(in,"%d %g %g %g %g",&narc,&minx,&miny,&maxx,&maxy);

		if (!feof(in)) {
			
			psout("gsave\n");
			psout("% New polygon\n");
			psout("newpath\n");
			first=true;
			ct=0;
			warned=false;

			for (n=0;n<narc;n++) {
			
				fscanf(in,"%d",&ncoord);
				if (ncoord==0) {
				
					if (ct) {
						
						inwindow=false;
						for (m=0;m<ct && !inwindow;m++) {
							if (vertex[m].h>=east && vertex[m].h<=west &&
								vertex[m].v>=south && vertex[m].v<=north)
									inwindow=true;
						}
						
						if (inwindow) {
							for (m=0;m<ct;m++) {
								
								if (first) {
									initialx=ptops(gtopx(vertex[m].h));
									initialy=ptops(gtopy(vertex[m].v));
									pc(initialx,initialy,MOV);
									first=false;
								}
								else {
									x=ptops(gtopx(vertex[m].h));
									y=ptops(gtopy(vertex[m].v));
									pc(x,y,LIN);
								}
							}
						}
					}
				
					psout("clip\n");
					set_grey_internal(1.0);
					psout("fill\n");
					first=true;
					ct=0;
					warned=false;
				}
								
				for (m=0;m<ncoord;m++) {
				
					fscanf(in,"%g %g",&flon,&flat);
			
					lon=flon;
					lat=flat;
					
					if (lon<-180.0) lon=-180.0;
					else if (lon>180.0) lon=180.0;
					
					if (lat<MINLAT) lat=MINLAT;
					else if (lat>MAXLAT) lat=MAXLAT;
				
					lp.lam=lon*DEG_TO_RAD;
					lp.phi=lat*DEG_TO_RAD;
					cartesian=pj_fwd(lp,ref);
			
					if (ct<MAXSEG) {
						vertex[ct].h=cartesian.x;
						vertex[ct].v=cartesian.y;
						ct++;
					}
					else if (!warned) {
						printf("Warning from plotpoly: MAXSEG (%d) exceeded\n",MAXSEG);
						warned=true;
					}
				}
			}

			if (ct) {
				
				inwindow=false;
				for (m=0;m<ct && !inwindow;m++) {
					if (vertex[m].h>=east && vertex[m].h<=west &&
						vertex[m].v>=south && vertex[m].v<=north)
							inwindow=true;
				}
				
				if (inwindow) {
					for (m=0;m<ct;m++) {
						
						if (first) {
							initialx=ptops(gtopx(vertex[m].h));
							initialy=ptops(gtopy(vertex[m].v));
							pc(initialx,initialy,MOV);
							first=false;
						}
						else {
							x=ptops(gtopx(vertex[m].h));
							y=ptops(gtopy(vertex[m].v));
							pc(x,y,LIN);
						}
					}
				}
			}
			
			psout("clip\n");
			set_grey_internal(1.0);
			psout("fill\n");
			psout("grestore\n");
		}
	}
	
	set_grey_internal(grey);

	psout("grestore\n");
}

void fillgrid(double west,double south,double east,double north) {

	// Fills a grid cell with current pattern

	double gleft,gright,gtop,gbottom;
	location vertex[4];
	XY cartesian;
	LP lp;
	
	if (north>=90.0 || south<=-90.0) return;

	psout("gsave\n");
	set_grey_internal(1.0);
		
	// clip to graph window
	
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
	
	// clip to grid cell boundary

	lp.lam=west*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[0].h=cartesian.x;
	vertex[0].v=cartesian.y;
	gleft=gright=gtopx(vertex[0].h);
	gtop=gbottom=gtopy(vertex[0].v);

	lp.lam=west*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[1].h=cartesian.x;
	vertex[1].v=cartesian.y;
	if (gtopx(vertex[1].h)>gright) gright=gtopx(vertex[1].h);
	else if (gtopx(vertex[1].h)<gleft) gleft=gtopx(vertex[1].h);
	if (gtopy(vertex[1].v)>gtop) gtop=gtopy(vertex[1].v);
	else if (gtopy(vertex[1].v)<gbottom) gbottom=gtopy(vertex[1].v);
	
	lp.lam=east*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[2].h=cartesian.x;
	vertex[2].v=cartesian.y;
	if (gtopx(vertex[2].h)>gright) gright=gtopx(vertex[2].h);
	else if (gtopx(vertex[2].h)<gleft) gleft=gtopx(vertex[2].h);
	if (gtopy(vertex[2].v)>gtop) gtop=gtopy(vertex[2].v);
	else if (gtopy(vertex[2].v)<gbottom) gbottom=gtopy(vertex[2].v);

	lp.lam=east*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[3].h=cartesian.x;
	vertex[3].v=cartesian.y;
	if (gtopx(vertex[3].h)>gright) gright=gtopx(vertex[3].h);
	else if (gtopx(vertex[3].h)<gleft) gleft=gtopx(vertex[3].h);
	if (gtopy(vertex[3].v)>gtop) gtop=gtopy(vertex[3].v);
	else if (gtopy(vertex[3].v)<gbottom) gbottom=gtopy(vertex[3].v);

	psout("newpath\n");
	pc(ptops(gtopx(vertex[0].h)),ptops(gtopy(vertex[0].v)),MOV);
	pc(ptops(gtopx(vertex[1].h)),ptops(gtopy(vertex[1].v)),LIN);
	pc(ptops(gtopx(vertex[2].h)),ptops(gtopy(vertex[2].v)),LIN);
	pc(ptops(gtopx(vertex[3].h)),ptops(gtopy(vertex[3].v)),LIN);
	pc(ptops(gtopx(vertex[0].h)),ptops(gtopy(vertex[0].v)),LIN);
	psout("clip\n");

	set_grey_internal(grey);
	
	int i;

	pattern(gleft,gbottom,gright,gtop);

	psout("grestore\n");
}

void fillgrid(double west,double south,double east,double north,double val,int format) {

	// Fills a grid cell with pattern corresponding to specified value
	// using current data scale
	
	double gleft,gright,gtop,gbottom;
	int n;
	location vertex[4];
	bool inwindow;
	XY cartesian;
	LP lp;

	double eastb=ptogx(graphbl.h);
	double westb=ptogx(graphtr.h);
	double southb=ptogy(graphbl.v);
	double northb=ptogy(graphtr.v);
	
	if (north>=90.0 || south<=-90.0) return;
	
	lp.lam=west*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[0].h=cartesian.x;
	vertex[0].v=cartesian.y;
	lp.lam=west*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[1].h=cartesian.x;
	vertex[1].v=cartesian.y;
	lp.lam=east*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[2].h=cartesian.x;
	vertex[2].v=cartesian.y;
	lp.lam=east*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[3].h=cartesian.x;
	vertex[3].v=cartesian.y;
	
	inwindow=false;
	for (n=0;n<4 && !inwindow;n++)
		if (vertex[n].h>=eastb && vertex[n].h<=westb &&
			vertex[n].v>=southb && vertex[n].v<=northb)
				inwindow=true;

	if (!inwindow) return;
	
	psout("gsave\n");
	set_grey_internal(1.0);
	
	// clip to graph window
	
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
	
	// clip to grid cell boundary

	gleft=gright=gtopx(vertex[0].h);
	gtop=gbottom=gtopy(vertex[0].v);

	if (gtopx(vertex[1].h)>gright) gright=gtopx(vertex[1].h);
	else if (gtopx(vertex[1].h)<gleft) gleft=gtopx(vertex[1].h);
	if (gtopy(vertex[1].v)>gtop) gtop=gtopy(vertex[1].v);
	else if (gtopy(vertex[1].v)<gbottom) gbottom=gtopy(vertex[1].v);
	
	if (gtopx(vertex[2].h)>gright) gright=gtopx(vertex[2].h);
	else if (gtopx(vertex[2].h)<gleft) gleft=gtopx(vertex[2].h);
	if (gtopy(vertex[2].v)>gtop) gtop=gtopy(vertex[2].v);
	else if (gtopy(vertex[2].v)<gbottom) gbottom=gtopy(vertex[2].v);

	if (gtopx(vertex[3].h)>gright) gright=gtopx(vertex[3].h);
	else if (gtopx(vertex[3].h)<gleft) gleft=gtopx(vertex[3].h);
	if (gtopy(vertex[3].v)>gtop) gtop=gtopy(vertex[3].v);
	else if (gtopy(vertex[3].v)<gbottom) gbottom=gtopy(vertex[3].v);

	psout("newpath\n");
	pc(ptops(gtopx(vertex[0].h)),ptops(gtopy(vertex[0].v)),MOV);
	pc(ptops(gtopx(vertex[1].h)),ptops(gtopy(vertex[1].v)),LIN);
	pc(ptops(gtopx(vertex[2].h)),ptops(gtopy(vertex[2].v)),LIN);
	pc(ptops(gtopx(vertex[3].h)),ptops(gtopy(vertex[3].v)),LIN);
	pc(ptops(gtopx(vertex[0].h)),ptops(gtopy(vertex[0].v)),LIN);
	psout("clip\n");

	set_grey_internal(grey);
	
	filldata(ptops(gleft),ptops(gbottom),ptops(gright),ptops(gtop),val,format);

	psout("grestore\n");
}

void clip_window() {
	
	// clip to graph window
	
	psout("gsave\n");
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
}

void restore() {

	colour();
	psout("grestore\n");
}

void fillgrid_fast(double west,double south,double east,double north,double val) {

	// Fills a grid cell with pattern corresponding to specified value
	// using current data scale
	
	double gleft,gright,gtop,gbottom;
	int n;
	location vertex[4];
	bool inwindow;
	XY cartesian;
	LP lp;

	double eastb=ptogx(graphbl.h);
	double westb=ptogx(graphtr.h);
	double southb=ptogy(graphbl.v);
	double northb=ptogy(graphtr.v);
	
	if (north>=90.0 || south<=-90.0) return;
		
	lp.lam=west*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[0].h=cartesian.x;
	vertex[0].v=cartesian.y;
	lp.lam=west*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[1].h=cartesian.x;
	vertex[1].v=cartesian.y;
	lp.lam=east*DEG_TO_RAD;
	lp.phi=north*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[2].h=cartesian.x;
	vertex[2].v=cartesian.y;
	lp.lam=east*DEG_TO_RAD;
	lp.phi=south*DEG_TO_RAD;
	cartesian=pj_fwd(lp,ref);
	vertex[3].h=cartesian.x;
	vertex[3].v=cartesian.y;
	
	inwindow=false;
	for (n=0;n<4 && !inwindow;n++)
		if (vertex[n].h>=eastb && vertex[n].h<=westb &&
			vertex[n].v>=southb && vertex[n].v<=northb)
				inwindow=true;

	if (!inwindow) return;
	
	gleft=gright=gtopx(vertex[0].h);
	gtop=gbottom=gtopy(vertex[0].v);

	if (gtopx(vertex[1].h)>gright) gright=gtopx(vertex[1].h);
	else if (gtopx(vertex[1].h)<gleft) gleft=gtopx(vertex[1].h);
	if (gtopy(vertex[1].v)>gtop) gtop=gtopy(vertex[1].v);
	else if (gtopy(vertex[1].v)<gbottom) gbottom=gtopy(vertex[1].v);
	
	if (gtopx(vertex[2].h)>gright) gright=gtopx(vertex[2].h);
	else if (gtopx(vertex[2].h)<gleft) gleft=gtopx(vertex[2].h);
	if (gtopy(vertex[2].v)>gtop) gtop=gtopy(vertex[2].v);
	else if (gtopy(vertex[2].v)<gbottom) gbottom=gtopy(vertex[2].v);

	if (gtopx(vertex[3].h)>gright) gright=gtopx(vertex[3].h);
	else if (gtopx(vertex[3].h)<gleft) gleft=gtopx(vertex[3].h);
	if (gtopy(vertex[3].v)>gtop) gtop=gtopy(vertex[3].v);
	else if (gtopy(vertex[3].v)<gbottom) gbottom=gtopy(vertex[3].v);
	
	psout("gsave\n");
	filldata_fast(ptops(gleft),ptops(gbottom),ptops(gright),ptops(gtop),val);
	psout("grestore\n");
}

void fillgrid(double west,double south,double east,double north,double val) {

	fillgrid(west,south,east,north,val,0);
}

void plotgrid(double lon,double lat) {

	// Plots the current marker symbol at the specified geographic location

	double px,py;

	lonlattop(lon,lat,px,py);
	marker(px,py);	
}


void scalelegend(double left,double bottom,double right,double top,
	bool vertical,char* units,bool closemax,bool skipodd) {

	// Plots a scale legend based on current data scale

	const double GAPFRAC=0.2;

	double width,pos,val,textpos,pval,upper,lower,minht,maxht,diff;
	double minwd,maxwd;
	int i,nclass,nclass_pos,nclass_neg,ct;
	double gap;
	char buf[100];
	bool plotit=true,skip;
	bool zeroclass=false,forcezeroclass=false;
	bool ifcat;

	linedef oldlinestyle=line_style;
	linestyle(0);
	linewidth(0);
	
	if (pcolourkey) {
		nclass=ncolourkey;
		datascale_curr.logarithmic=false;
	}
	else if (datascale_curr.logarithmic) {
		nclass_pos=nclass_neg=0;
		if (datascale_curr.bounds_neg) {
			ct=0;
			while (datascale_curr.bounds_neg[ct++]>0) nclass_neg++;
		}
		if (datascale_curr.bounds) {
			ct=0;
			while (datascale_curr.bounds[ct++]>0) nclass_pos++;
		}
		nclass=nclass_neg+nclass_pos;
	}
	else
		nclass=datascale_curr.nclass;

	if (vertical) {
	
		// Shrink / expand
		minht=text_style.size*0.35;
		maxht=minht*2;
		diff=nclass*minht-(top-bottom);
		if (diff>0.0) {
			top+=diff/2.0;
			bottom-=diff/2.0;
		}
		
		diff=top-bottom-nclass*maxht;
		if (diff>0.0) {
			top-=diff*0.5;
			bottom+=diff*0.5;
		}
	
		if (pcolourkey || !datascale_curr.zeroclass)
			width=(top-bottom)/(double)nclass;
		else {
			if (datascale_curr.zeroclass && !datascale_curr.logarithmic && datascale_curr.minval<0.0
			   && datascale_curr.minval+nclass*datascale_curr.interval>0.0) {
								
				if (fabs(int(-datascale_curr.minval/datascale_curr.interval+0.000001)+
					datascale_curr.minval/datascale_curr.interval)<datascale_curr.interval*0.000001)
					
						width=(top-bottom)/(double)(nclass+1);
				else
						width=(top-bottom)/(double)(nclass+2);
				
			}
			else 
				width=(top-bottom)/(double)(nclass+1);
		}
		
		pos=bottom;
		gap=0.0;
		if (pcolourkey) {
			ifcat=true;
			for (i=0;i<nclass && ifcat;i++)
				if (pcolourkey[i].minval!=pcolourkey[i].maxval) ifcat=false;
			if (ifcat) gap=width*GAPFRAC*0.5;
		}
		textpos=right+text_style.size*0.2;
		val=datascale_curr.minval+datascale_curr.interval*0.5;
		skip=false;
		for (i=0;i<nclass;i++) {
			
			if (datascale_curr.logarithmic) {
				if (i<nclass_neg) {
					lower=-datascale_curr.bounds_neg[nclass_neg-i-1];
					if (i<nclass_neg-1)
						upper=-datascale_curr.bounds_neg[nclass_neg-i-2];
					else
						upper=0.0;
				}
				else if (i==nclass_neg) {
					lower=0.0;
					upper=datascale_curr.bounds[0];
				}
				else {
					lower=datascale_curr.bounds[i-nclass_neg-1];
					upper=datascale_curr.bounds[i-nclass_neg];
				}
				val=(upper+lower)/2.0;
				//printf("%d: %g %g %g\n",i,lower,upper,val);
			}
			
			if (datascale_curr.zeroclass && !pcolourkey && !datascale_curr.logarithmic &&
				datascale_curr.minval+i*datascale_curr.interval<0.0 && 
				datascale_curr.minval+(i+1)*datascale_curr.interval>datascale_curr.interval*0.000001) {
				
				if (!val) val=0.0000001;
				
				filldata(ptops(left),ptops(pos+gap),ptops(right),ptops(pos+width-gap),val);
			
				line(left,pos+gap,left,pos+width-gap);
				line(left,pos+width-gap,right,pos+width-gap);
				line(right,pos+width-gap,right,pos+gap);
				line(right,pos+gap,left,pos+gap);
				
				pval=datascale_curr.minval+i*datascale_curr.interval;
				if (fabs(pval)<1.0e-15) pval=0.0;
				
				bgsprint(99,buf,pval,datascale_curr.format.width,datascale_curr.format.places,
					    datascale_curr.format.exp);
				
				text(textpos,pos,buf,CL);
				
				pos+=width;
				
				forcezeroclass=true;
			}
			
			if (pcolourkey) {
				val=(pcolourkey[nclass-1-i].minval+pcolourkey[nclass-1-i].maxval)*0.5;
			}
			else if (datascale_curr.zeroclass) {
				
				if (zeroclass) zeroclass=false;
				else if (datascale_curr.logarithmic && i==nclass_neg ||
					!datascale_curr.logarithmic && 
					fabs(val-datascale_curr.interval*0.5)<datascale_curr.interval*0.1 ||
					forcezeroclass) {
									
					// zeroclass
					
					filldata(ptops(left),ptops(pos+gap),ptops(right),ptops(pos+width-gap),0);
					
					line(left,pos+gap,left,pos+width-gap);
					line(left,pos+width-gap,right,pos+width-gap);
					line(right,pos+width-gap,right,pos+gap);
					line(right,pos+gap,left,pos+gap);
					
					text(textpos,pos+width*0.5,"0",CL);
					
					pos+=width;
					
					zeroclass=true;
					forcezeroclass=false;
				}
			}
			
			filldata(ptops(left),ptops(pos+gap),ptops(right),ptops(pos+width-gap),val);
			
			line(left,pos+gap,left,pos+width-gap);
			line(left,pos+width-gap,right,pos+width-gap);
			line(right,pos+width-gap,right,pos+gap);
			line(right,pos+gap,left,pos+gap);

			if (pcolourkey) {
				if (i<nclass-1) {
					if (pcolourkey[nclass-1-i].minval==pcolourkey[nclass-2-i].maxval) {
						upper=pcolourkey[nclass-1-i].minval;
						lower=pcolourkey[nclass-1-i].maxval;
					}
					else {
						lower=pcolourkey[nclass-1-i].minval;
						upper=pcolourkey[nclass-1-i].maxval;
					}
				}
				else {
					if (nclass>1) {
						if (pcolourkey[0].maxval==pcolourkey[1].minval) {
							upper=pcolourkey[0].minval;
							lower=pcolourkey[0].maxval;
						}
						else {
							lower=pcolourkey[0].minval;
							upper=pcolourkey[0].maxval;
						}
					}
					else {
						lower=pcolourkey[0].minval;
						upper=pcolourkey[0].maxval;
					}
				}
				
				bgsprint(99,buf,lower,0,0,0);
				//sprintf(buf,"%g",lower);
				if (upper==lower) {
					if (ifcat && pcolourkey[nclass-1-i].name != "") {
						text(textpos,pos+width*0.5,(char*)pcolourkey[nclass-1-i].name.c_str(),CL);
					}
					else {
						text(textpos,pos+width*0.5,buf,CL);
					}
					skip=true; // skip the next label
				}
				else {
					if (!skip) text(textpos,pos,buf,CL);
					skip=false;
				}
			}
			else {
				if (datascale_curr.logarithmic) {
					bgsprint(99,buf,lower,0,0,0);
				}
				else {
					pval=datascale_curr.minval+i*datascale_curr.interval;
					if (fabs(pval)<1.0e-15) pval=0.0;
				
					bgsprint(99,buf,pval,datascale_curr.format.width,datascale_curr.format.places,
						datascale_curr.format.exp);
				}
				
/*				if (i!=nclass-1 || closemax)
					bgsprint(99,buf,pval,datascale_curr.format.width,datascale_curr.format.places,
						datascale_curr.format.exp);
				else {
					bgsprint(99,buf,pval,datascale_curr.format.width,datascale_curr.format.places,
						datascale_curr.format.exp);
*/					
				if (i==nclass-1 && !closemax) {
					sprintf(buf,"%s %s",buf,units);
				}
				
				if (!zeroclass) {
					if (plotit || !skipodd) text(textpos,pos,buf,CL);
				}
			}
			

			val+=datascale_curr.interval;
			pos+=width;

			if (i==nclass-1) {
			 
				if (pcolourkey) {
					if (!skip) {
						bgsprint(99,buf,upper,0,0,0);
						//sprintf(buf,"%g",upper);
						if (upper!=lower) text(textpos,pos,buf,CL);
					}
				}
				else {
					if (datascale_curr.zeroclass &&
						(datascale_curr.logarithmic && nclass==nclass_neg ||
						!datascale_curr.logarithmic &&
						fabs(val-datascale_curr.interval*0.5)<datascale_curr.interval*0.1)) {
							
							filldata(ptops(left),ptops(pos+gap),ptops(right),ptops(pos+width-gap),0);
							
							line(left,pos+gap,left,pos+width-gap);
							line(left,pos+width-gap,right,pos+width-gap);
							line(right,pos+width-gap,right,pos+gap);
							line(right,pos+gap,left,pos+gap);
							
							text(textpos,pos+width*0.5,"0",CL);
					}
					else if (closemax) {
						if (datascale_curr.logarithmic)
							bgsprint(99,buf,upper,0,0,0);
						else
							bgsprint(99,buf,datascale_curr.minval+(i+1)*datascale_curr.interval,
								datascale_curr.format.width,datascale_curr.format.places,
								datascale_curr.format.exp);
						sprintf(buf,"%s %s",buf,units);
						text(textpos,pos,buf,CL);
					}
				}
			}
			plotit=!plotit;
		}
	}
	else { // horizontal scale
		
		// Shrink / expand
		minwd=text_style.size*0.35*2;
		maxwd=(top-bottom)*4;
		diff=nclass*minwd-(right-left);
		if (diff>0.0) {
			right+=diff/2.0;
			left-=diff/2.0;
		}
		
		diff=right-left-nclass*maxwd;
		if (diff>0.0) {
			right-=diff*0.5;
			left+=diff*0.5;
		}		
		
		if (pcolourkey || !datascale_curr.zeroclass)
			width=(right-left)/(double)(nclass);
		else {
			
			if (datascale_curr.zeroclass && !datascale_curr.logarithmic && datascale_curr.minval<0.0
				&& datascale_curr.minval+nclass*datascale_curr.interval>0.0) {
									
				if (fabs(int(-datascale_curr.minval/datascale_curr.interval+0.000001)+
					datascale_curr.minval/datascale_curr.interval)<datascale_curr.interval*0.000001)
			
					width=(right-left)/(double)(nclass+1);
				else
					width=(right-left)/(double)(nclass+2);
			}
			else
				width=(right-left)/(double)(nclass+1);
		}
		
		pos=left;
		gap=0.0;
		if (pcolourkey) {
			ifcat=true;
			for (i=0;i<nclass && ifcat;i++)
				if (pcolourkey[i].minval!=pcolourkey[i].maxval) ifcat=false;
			if (ifcat) gap=width*GAPFRAC*0.5;
		}
		textpos=bottom-text_style.size*0.2;
		val=datascale_curr.minval+datascale_curr.interval*0.5;
		skip=false;
		for (i=0;i<nclass;i++) {
			
			if (datascale_curr.logarithmic) {
				if (i<nclass_neg) {
					lower=-datascale_curr.bounds_neg[nclass_neg-i-1];
					if (i<nclass_neg-1)
						upper=-datascale_curr.bounds_neg[nclass_neg-i-2];
					else
						upper=0.0;
				}
				else if (i==nclass_neg) {
					lower=0.0;
					upper=datascale_curr.bounds[0];
				}
				else {
					lower=datascale_curr.bounds[i-nclass_neg-1];
					upper=datascale_curr.bounds[i-nclass_neg];
				}
				val=(upper+lower)/2.0;
			}
			
			if (datascale_curr.zeroclass && !pcolourkey && !datascale_curr.logarithmic &&
				datascale_curr.minval+i*datascale_curr.interval<0.0 && 
				datascale_curr.minval+(i+1)*datascale_curr.interval>datascale_curr.interval*0.000001) {
								
				if (!val) val=0.0000001;
								
				filldata(ptops(pos+gap),ptops(bottom),ptops(pos+width-gap),ptops(top),val);

				line(pos+gap,bottom,pos+gap,top);
				line(pos+gap,top,pos+width-gap,top);
				line(pos+width-gap,top,pos+width-gap,bottom);
				line(pos+width-gap,bottom,pos+gap,bottom);
				
				pval=datascale_curr.minval+i*datascale_curr.interval;
				if (fabs(pval)<1.0e-15) pval=0.0;
				
				bgsprint(99,buf,pval,datascale_curr.format.width,datascale_curr.format.places,
					    datascale_curr.format.exp);
				
				text(pos,textpos,buf,TC);
				
				pos+=width;
				
				forcezeroclass=true;
			}
			
		
			if (pcolourkey) {
				val=(pcolourkey[i].minval+pcolourkey[i].maxval)*0.5;
			}
			else if (datascale_curr.zeroclass) {
				if (zeroclass) zeroclass=false;
				else if (datascale_curr.logarithmic && i==nclass_neg ||
					!datascale_curr.logarithmic && 
					fabs(val-datascale_curr.interval*0.5)<datascale_curr.interval*0.1 ||
					forcezeroclass) {

					// zeroclass
					
					filldata(ptops(pos+gap),ptops(bottom),ptops(pos+width-gap),ptops(top),0);
					
					line(pos+gap,bottom,pos+gap,top);
					line(pos+gap,top,pos+width-gap,top);
					line(pos+width-gap,top,pos+width-gap,bottom);
					line(pos+width-gap,bottom,pos+gap,bottom);
					
					text(pos+width*0.5,textpos,"0",TC);
					
					pos+=width;
					
					zeroclass=true;
					forcezeroclass=false;
					
					//printf("Zero\n");
				}
			}

			filldata(ptops(pos+gap),ptops(bottom),ptops(pos+width-gap),ptops(top),val);
			//printf("val=%g\n",val);

			line(pos+gap,bottom,pos+gap,top);
			line(pos+gap,top,pos+width-gap,top);
			line(pos+width-gap,top,pos+width-gap,bottom);
			line(pos+width-gap,bottom,pos+gap,bottom);

			if (pcolourkey) {
				if (i<nclass-1) {
					if (pcolourkey[i].maxval==pcolourkey[i+1].minval) {
						upper=pcolourkey[i].maxval;
						lower=pcolourkey[i].minval;
					}
					else {
						lower=pcolourkey[i].maxval;
						upper=pcolourkey[i].minval;
					}
				}
				else {
					if (nclass>1) {
						if (pcolourkey[i].minval==pcolourkey[i-1].maxval) {
							upper=pcolourkey[i].maxval;
							lower=pcolourkey[i].minval;
						}
						else {
							lower=pcolourkey[i].maxval;
							upper=pcolourkey[i].minval;
						}
					}
					else {
						lower=pcolourkey[i].minval;
						upper=pcolourkey[i].maxval;
					}
				}
				
				bgsprint(99,buf,lower,0,0,0);
				//sprintf(buf,"%g",lower);
				if (upper==lower) {
					if (ifcat && pcolourkey[i].name != "") {
						text(pos+width*0.5,textpos,(char*)pcolourkey[i].name.c_str(),TC);
					}
					else {
						text(pos+width*0.5,textpos,buf,TC);
					}
					skip=true; // skip the next label
				}
				else {
					if (!skip) text(pos,textpos,buf,TC);
					skip=false;
				}
				
			}
			else {
				if (!zeroclass) {
					if (datascale_curr.logarithmic) {
						bgsprint(99,buf,lower,0,0,0);
					}	
					else {
						pval=datascale_curr.minval+i*datascale_curr.interval;
						if (fabs(pval)<1.0e-15) pval=0.0;
						bgsprint(99,buf,pval,datascale_curr.format.width,
							    datascale_curr.format.places,datascale_curr.format.exp);
					}
		
					if (plotit || !skipodd) text(pos,textpos,buf,TC);
				}
				
				val+=datascale_curr.interval;
			}
			
			pos+=width;

			if (i==nclass-1) {
				if (pcolourkey) {
					if (!skip) {
						bgsprint(99,buf,upper,0,0,0);
						//sprintf(buf,"%g",upper);
						if (upper!=lower) text(pos,textpos,buf,TC);
					}
				}
				else {
					if (datascale_curr.zeroclass &&
						(datascale_curr.logarithmic && nclass==nclass_neg ||
						!datascale_curr.logarithmic &&
						fabs(val-datascale_curr.interval*0.5)<datascale_curr.interval*0.1)) {
					//	if (datascale_curr.zeroclass &&
					//	 fabs(val-datascale_curr.interval*0.5)<datascale_curr.interval*0.1) {
					
						filldata(ptops(pos+gap),ptops(bottom),ptops(pos+width-gap),ptops(top),0);
						
						line(pos+gap,bottom,pos+gap,top);
						line(pos+gap,top,pos+width-gap,top);
						line(pos+width-gap,top,pos+width-gap,bottom);
						line(pos+width-gap,bottom,pos+gap,bottom);
						
						text(pos+width*0.5,textpos,"0",TC);
					}
					else if (closemax) {
						if (datascale_curr.logarithmic)
							bgsprint(99,buf,upper,0,0,0);
						else
							bgsprint(99,buf,datascale_curr.minval+(i+1)*datascale_curr.interval,
								datascale_curr.format.width,datascale_curr.format.places,
								datascale_curr.format.exp);
						sprintf(buf,"%s %s",buf,units);
						text(pos,textpos,buf,TC);
						pos+=width;
					}
				}
			}
			plotit=!plotit;
		}

		text(pos,textpos,units,TL);
	}

	line_style=oldlinestyle;
}

void scalelegend(double left,double bottom,double right,double top,
				 bool vertical,char* units,bool closemax) {

	scalelegend(left,bottom,right,top,vertical,units,closemax,false);
}

void scalelegend(double left,double bottom,double right,double top) {

	scalelegend(left,bottom,right,top,false,"",false);
}

void scalelegend(double left,double bottom,double right,double top,bool closemax) {

	scalelegend(left,bottom,right,top,false,"",closemax);
}

void scalelegend(double left,double bottom,double right,double top,char* units) {

	scalelegend(left,bottom,right,top,false,units,false);
}

void frame() {

	// Plots frame around current graph (or map)

	line(graphbl.h,graphbl.v,graphbl.h,graphtr.v);
	line(graphbl.h,graphtr.v,graphtr.h,graphtr.v);
	line(graphtr.h,graphtr.v,graphtr.h,graphbl.v);
	line(graphtr.h,graphbl.v,graphbl.h,graphbl.v);
}

void lonlattop(double lon,double lat,double& px,double& py) {
	
	XY cartesian;
	LP lp;
	
	lp.lam=lon*DEG_TO_RAD;
	lp.phi=lat*DEG_TO_RAD;

	cartesian=pj_fwd(lp,ref);

	px=gtopx(cartesian.x);
	py=gtopy(cartesian.y);
}

void grid(double lonint,double latint,bool ifframe) {

	// Plots a grid using current line symbol with specified longitude/latitude spacing
	
	const double EPSF=0.005;
	const int MAXPT=1000;
	location vertex[MAXPT];

	double epslon=(graphscale.eastbound-graphscale.westbound)*EPSF;
	double epslat=(graphscale.northbound-graphscale.southbound)*EPSF;

	if (!epslon || !epslat) return;

	psout("gsave\n");
	set_grey_internal(1.0);
	psout("newpath\n");
	pc(ptops(graphbl.h),ptops(graphbl.v),MOV);
	pc(ptops(graphbl.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphtr.v),LIN);
	pc(ptops(graphtr.h),ptops(graphbl.v),LIN);
	pc(ptops(graphbl.h),ptops(graphbl.v),LIN);
	psout("clip\n");
	psout("newpath\n");
	set_grey_internal(grey);

	double lon,lat,x,y;
	double minlat=-MAXGRIDPOLAR,maxlat=MAXGRIDPOLAR;
	int npt,el;

	for (lon=-180.0+lonint;lon<=180.0-lonint;lon+=lonint) {
		npt=0;

		for (lat=minlat; lat<=maxlat;lat+=epslat) {

			lonlattop(lon,lat,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
				vertex[npt].v=ptops(y);
				if (npt==MAXPT-1) {
					curve_point(npt,vertex,line_style);
					vertex[0].h=ptops(x);
					vertex[0].v=ptops(y);
					npt=0;
				}	
				npt++;
			}
			else {
				if (npt>1) curve_point(npt,vertex,line_style);
				npt=0;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	}
	
	for (lat=-90+latint;lat<=90.0-latint;lat+=latint) {
		npt=0;
		for (lon=-180.0; lon<=180.0; lon+=epslon) {
			lonlattop(lon,lat,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
				vertex[npt].v=ptops(y);
				if (npt==MAXPT-1) {
                                        curve_point(npt,vertex,line_style);
                                        vertex[0].h=ptops(x);
                                        vertex[0].v=ptops(y);
                                        npt=0;
                                }
				npt++;
			}
			else {
				if (npt>1) curve_point(npt,vertex,line_style);
				npt=0;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	}

	if (ifframe) {

		linestyle(0);

		npt=0;
		for (lon=-180.0; lon<=180.0; lon+=epslon) {
			lonlattop(lon,MINLAT+0.01,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
		 		vertex[npt].v=ptops(y);
				if (npt==MAXPT-1) {
					curve_point(npt,vertex,line_style);
					vertex[0].h=ptops(x);
					vertex[0].v=ptops(y);
					npt=0;
				}
		 	 	npt++;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	
		npt=0;
		for (lon=-180.0; lon<=180.0; lon+=epslon) {
			lonlattop(lon,MAXLAT-0.01,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
				vertex[npt].v=ptops(y);
				if (npt==MAXPT-1) {
                                        curve_point(npt,vertex,line_style);
                                        vertex[0].h=ptops(x);
                                        vertex[0].v=ptops(y);
                                        npt=0;
                                }
				npt++;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	
		npt=0;
		for (lat=MINLAT; lat<=MAXLAT;lat+=epslat) {
			lonlattop(-180.0,lat,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
				vertex[npt].v=ptops(y);
				if (npt==MAXPT-1) {
                                        curve_point(npt,vertex,line_style);
                                        vertex[0].h=ptops(x);
                                        vertex[0].v=ptops(y);
                                        npt=0;
                                }
				npt++;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	
		npt=0;
		for (lat=MINLAT; lat<=MAXLAT;lat+=epslat) {
			lonlattop(180.0,lat,x,y);
			if (x>=graphbl.h && x<=graphtr.h && y>=graphbl.v && y<=graphtr.v) {
				vertex[npt].h=ptops(x);
				vertex[npt].v=ptops(y);
			 	if (npt==MAXPT-1) {
                                        curve_point(npt,vertex,line_style);
                                        vertex[0].h=ptops(x);
                                        vertex[0].v=ptops(y);
                                        npt=0;
                                }
				npt++;
			}
		}
		if (npt>1) curve_point(npt,vertex,line_style);
	}
	
	psout("grestore\n");
}

void grid(double lonint,double latint) {

	grid(lonint,latint,true);
}
