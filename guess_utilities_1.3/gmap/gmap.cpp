//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


////////////////////////////////////////////////////////////////////////////////////////
// GMAP
// Geographic visualisation utility for plain text longitude/latitude referenced data
//
// Written by Ben Smith
// This version dated 2008-05-10
//
// gmap -help for documentation 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <gutil.h>
#include <bengraph.h>
#include <lib_proj.h>
#include <bicubic.h>
#include <gmap_path.h>
#include <ctype.h>
#include <string>

// Map position on page in mm from left hand corner (portrait mode)
const double PLEFT=20;
const double PBOTTOM=40;
const double PRIGHT=190;
const double PTOP=255;

// Map position on page in mm from left hand corner (landscape mode)
const double LLEFT=20;
const double LBOTTOM=40;
const double LRIGHT=276;
const double LTOP=175;

// Dimensions for horizontal legend
const double HLEGX=120; // maximum width
const double HLEGH=5; // height
const double HLEGOFF=15; // displacement from bottom of map

// Dimensions for vertical legend
const double VLEGX=30; // Maximum width (including text)
const double VLEGY=140; // Maximum height
const double VLEGTHRESH=0.6; // if map aspect is lower than this, legend will be vertical
const double VLEGSNEAK=10; // number of mm extra at bottom if legend is vertical
const double VLEGW=6; // width of legend box

// Default lon/lat window for map
const int WEST=-15;
const int EAST=60;
const int SOUTH=40;
const int NORTH=70;

// Default projection
const projtype PROJ=FAHEY;

// Default pixel size and offset
const double PIXX=0.5;
const double PIXY=0.5;
const double PIXDX=0.25;
const double PIXDY=0.25;

const double LSTHRESH=1.2; // Threshold aspect for landscape mode

const int MAXCAT=30; // maximum number of unique values to interpret as categorical data
const int MAXCLASS=100; // maximum number of classes in file
colourkeydef c[MAXCLASS];

const int MAXGRID=1000000; // maximum number of grid cells permitted
const int MAXPIXEL=1000000; // maximum number of pixels in smoothed rasters
const int MAXITEM=1024; // maximum number of items in input file
const int MAXDENS=64; // maximum number of interpolation points in each dimension
const int DEFAULTDENS=16; // default density between interpolation points

const int MAXPATH=1024; // Maximum string length for pathname

const char TEMPFILE[]="$GMTEMP00.ps";

struct Point {
	double x;
	double y;
	double val;
};

Point* pixdata;
int npixel;
double categories[MAXCLASS];
int ncat;

double gmod(double dval,double base) {

	return dval-floor(dval/base)*base;
}

inline double distance(double x1,double y1,double x2,double y2) {
	
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

bool regular_grid_av(float*& pgrid,double pixx,double pixy,
	double resx,double resy,double& ox,double& oy,int& nx,int& ny) {

	// Fits a regular lat/lon grid to current pixel data by interpolation
	// resx,resy=desired resolution (degrees longitude and latitude) of output grid
	// (minx,miny)=output grid origin
	// ox,oy = number of grid points in output grid
	// -9999 = novalue
	
	const double UNDEF=-9999;
	
	int i,j,r,c,cor;
	double dval,dist,excess;
	double minx,maxx,miny,maxy;
	double x,y;
	const double EPS=0.0001;
	float* pgrid_sum;
	
	// Find boundaries of pixel data
	
	for (i=0;i<npixel;i++) {

		dval=pixdata[i].x;
		if (!i) minx=maxx=dval;
		else if (dval<minx) minx=dval;
		else if (dval>maxx) maxx=dval;

		dval=pixdata[i].y;
		if (!i) miny=maxy=dval;
		else if (dval<miny) miny=dval;
		else if (dval>maxy) maxy=dval;
	}
	
	excess=gmod((maxx-minx),resx);
	ox=minx-resx*2+excess*0.5;
	if (excess) {
		ox-=resx;
		nx=floor((maxx-minx)/resx)+7;
	}
	else nx=floor((maxx-minx)/resx)+5;
	
	//printf("minx=%g maxx=%g width=%g resx=%g excess=%g ox=%g nx=%d ox+nx*resx=%g\n",
	//	minx,maxx,maxx-minx,resx,excess,ox,nx,ox+nx*resx);
		
	excess=gmod((maxy-miny),resy);
	oy=miny-resy*2+excess*0.5;
	if (excess) {
		oy-=resy;
		ny=floor((maxy-miny)/resy)+7;
	}
	else ny=floor((maxy-miny)/resy)+5;
	
	printf("Fitting data to regular grid at resolution %g x %g (long/lat) degrees\n",
		pixx,pixy);
	printf("-pixsize to change\n");
	if (npixel>10000) printf("This could take some time ...\n");
	
	pgrid=new float[nx*ny];
	pgrid_sum=new float[nx*ny];
	if (!pgrid || !pgrid_sum) {
		printf("Out of memory creating interpolation lattice\n");
		exit(99);
	}
	
	// initialise to -9999
	
	for (i=0;i<nx*ny;i++) {
		pgrid[i]=UNDEF;
		pgrid_sum[i]=0.0;
	}
	
	// Accumulate inverse distance-weighted averages for each point on grid
	
	for (i=0;i<npixel;i++) {
	
	
		c=(pixdata[i].x-ox)/resx+EPS-1;
		r=(pixdata[i].y-oy)/resy+EPS-1;
		cor=c;

		for (j=0;j<16;j++) {
		
			if (c>=0 && c<nx && r>=0 && r<ny) {
				x=ox+c*resx;
				y=oy+r*resy;
				dist=distance(x,y,pixdata[i].x,pixdata[i].y);
				if (dist<EPS) { 
					pgrid[c*ny+r]=pixdata[i].val;
					pgrid_sum[c*ny+r]=UNDEF;
				}
				else if (pgrid_sum[c*ny+r]!=UNDEF) {
					if (pgrid[c*ny+r]==UNDEF) pgrid[c*ny+r]=pixdata[i].val*1.0/dist;
					else pgrid[c*ny+r]+=pixdata[i].val*1.0/dist;
					pgrid_sum[c*ny+r]+=1.0/dist;
				}
			}
			
			c++;
			if (j==3 || j==7 || j==11) {
				r++;
				c=cor;
			}
		}
	}

	// Build averages
	
	for (c=0;c<nx;c++) {
	
		x=ox+c*resx;
		for (r=0;r<ny;r++) {
		
			y=oy+r*resy;
			
			if (pgrid[c*ny+r]!=UNDEF && pgrid_sum[c*ny+r]!=UNDEF)
				if (pgrid_sum[c*ny+r]) pgrid[c*ny+r]/=pgrid_sum[c*ny+r];
		}
	}
	
	// Clean up
	
	delete[] pgrid_sum;
	
	return true;
}



bool regular_grid(float*& pgrid,double pixx,double pixy,
	double resx,double resy,double& ox,double& oy,int& nx,int& ny) {

	// Fits a regular lat/lon grid to current pixel data by interpolation
	// resx,resy=desired resolution (degrees longitude and latitude) of output grid
	// (minx,miny)=output grid origin
	// ox,oy = number of grid points in output grid
	// -9999 = novalue
	
	const double UNDEF=-9999;
	
	int i,r,c,bl,br,tl,tr;
	double dist_bl,dist_br,dist_tl,dist_tr;
	double dval,excess,dist,facsum;
	double minx,maxx,miny,maxy;
	double x,y;
	const double EPS=0.0001;
	bool ifinterp,closer;
	
	// Find boundaries of pixel data
	
	for (i=0;i<npixel;i++) {

		dval=pixdata[i].x;
		if (!i) minx=maxx=dval;
		else if (dval<minx) minx=dval;
		else if (dval>maxx) maxx=dval;

		dval=pixdata[i].y;
		if (!i) miny=maxy=dval;
		else if (dval<miny) miny=dval;
		else if (dval>maxy) maxy=dval;
	}
	
	excess=gmod((maxx-minx),resx);
	ox=minx-resx*2+excess*0.5;
	if (excess) {
		ox-=resx;
		nx=floor((maxx-minx)/resx)+7;
	}
	else nx=floor((maxx-minx)/resx)+5;
	
	//printf("minx=%g maxx=%g width=%g resx=%g excess=%g ox=%g nx=%d ox+nx*resx=%g\n",
	//	minx,maxx,maxx-minx,resx,excess,ox,nx,ox+nx*resx);
		
	excess=gmod((maxy-miny),resy);
	oy=miny-resy*2+excess*0.5;
	if (excess) {
		oy-=resy;
		ny=floor((maxy-miny)/resy)+7;
	}
	else ny=floor((maxy-miny)/resy)+5;
	
	printf("Fitting data to regular grid at resolution %g x %g (long/lat) degrees\n",
		pixx,pixy);
	printf("-pixsize to change\n");
	if (npixel>10000) printf("This could take some time ...\n");
	
	pgrid=new float[nx*ny];
	if (!pgrid) {
		printf("Out of memory creating interpolation lattice\n");
		exit(99);
	}
	
	// initialise to -9999
	
	for (i=0;i<nx*ny;i++)
		pgrid[i]=UNDEF;
	
	// Try and find four corner points for bilinear interpolation
	
	for (c=0;c<nx;c++) {
	
		x=ox+c*resx;
		for (r=0;r<ny;r++) {
		
			y=oy+r*resy;
			dist_bl=dist_br=dist_tl=dist_tr=-1.0; // negative=not found
			ifinterp=true;
			closer=false;
			
			for (i=0;i<npixel && ifinterp;i++) {
				
				if (fabs(x-pixdata[i].x)<EPS && fabs(y-pixdata[i].y)<EPS) {
				
					// same point - no interpolation needed
					
					pgrid[c*ny+r]=pixdata[i].val;
					ifinterp=false;
				}
				else {
					dist=distance(x,y,pixdata[i].x,pixdata[i].y);
					//if (dist<=(resx+resy)*0.5+EPS) {
					if (dist<=(resx+resy)*0.5*2.0+EPS) {
						if (pixdata[i].x<=x && pixdata[i].y<=y) { // BL candidate
							if (dist_bl<0 || dist<dist_bl) {
								bl=i;
								dist_bl=dist;
							}
						}
						else if (pixdata[i].x<=x && pixdata[i].y>y) { // TL candidate
							if (dist_tl<0 || dist<dist_tl) {
								tl=i;
								dist_tl=dist;
							}
						}
						else if (pixdata[i].x>x && pixdata[i].y>y) { // TR candidate
							if (dist_tr<0 || dist<dist_tr) {
								tr=i;
								dist_tr=dist;
							}
						}
						else if (pixdata[i].x>x && pixdata[i].y<=y) { // BR candidate
							if (dist_br<0 || dist<dist_br) {
								br=i;
								dist_br=dist;
							}
						}
						closer=true;
					}
				}
			}
			
			if (ifinterp && closer) {
				
				// Perform interpolation by
				// weighted nearest neighbour average
				
//				printf("------------------------------------------------------\n");
//				printf("x=%g y=%g\n",x,y);
				
				facsum=0.0;
				dval=0.0;
				if (dist_bl>0) {
					facsum+=1.0/dist_bl;
					dval+=pixdata[bl].val/dist_bl;
//					printf("bl=(%g,%g)=%g dist=%g\n",
//						pixdata[bl].x,pixdata[bl].y,pixdata[bl].val,dist_bl);
				}
				if (dist_tl>0) {
					facsum+=1.0/dist_tl;
					dval+=pixdata[tl].val/dist_tl;
//					printf("tl=(%g,%g)=%g dist=%g\n",
//						pixdata[tl].x,pixdata[tl].y,pixdata[tl].val,dist_tl);
				}
				if (dist_tr>0) {
					facsum+=1.0/dist_tr;
					dval+=pixdata[tr].val/dist_tr;
//					printf("tr=(%g,%g)=%g dist=%g\n",
//						pixdata[tr].x,pixdata[tr].y,pixdata[tr].val,dist_tr);
				}
				if (dist_br>0) {
					facsum+=1.0/dist_br;
					dval+=pixdata[br].val/dist_br;
//					printf("br=(%g,%g)=%g dist=%g\n",
//						pixdata[br].x,pixdata[br].y,pixdata[br].val,dist_br);
				}
				pgrid[c*ny+r]=dval/facsum;
//				printf(" --> %g\n",pgrid[c*ny+r]);
				
			}
		}
	}

//	fillgrid(ox+.25,oy+.25,ox+(nx-1)*pixx+.25,oy+(ny-1)*pixy+.25,100);
//	fillgrid(minx,miny,maxx+.5,maxy+.5,4);

}


bool dobicubic(double pixx,double pixy,double pixdx,double pixdy,int dens) {
		
	int i,j,nx,ny,x,y;
	double minx,miny;
	double lf,rt,tp,bt;
	double dval;
	float* grid;
	double z[4][4],zd[MAXDENS*MAXDENS];
	bool known[4][4];
	double ppx,ppy;
		
	ppx=pixx/(double)dens;
	ppy=pixy/(double)dens;
	
	if (!regular_grid_av(grid,pixx,pixy,pixx,pixy,minx,miny,nx,ny)) {
		return false;
	}
	
/*	FILE* out=fopen("grid.txt","wt");
	fprintf(out,"lon\tlat\tval\n");
	for (x=0;x<nx;x++)
		for (y=0;y<ny;y++)
			fprintf(out,"%g\t%g\t%g\n",minx+x*pixx,miny+y*pixy,grid[x*ny+y]);
	fclose(out);
*/	

	printf("Performing bicubic interpolation to resolution %g x %g degrees\n",
		ppx,ppy);
		
	clip_window();
	
	//fillgrid(minx+pixx+pixdx,miny+pixy+pixdy,
	//	minx+double(nx-3)*pixx+pixdx+pixx,miny+double(ny-3)*pixy+pixdy+pixy,2);
	
	for (x=1;x<nx-2;x++) {
		for (y=1;y<ny-2;y++) {
			
			z[0][0]=grid[(x-1)*ny+y-1];
			z[0][1]=grid[x*ny+y-1];
			z[0][2]=grid[(x+1)*ny+y-1];
			z[0][3]=grid[(x+2)*ny+y-1];
			
			z[1][0]=grid[(x-1)*ny+y];
			z[1][1]=grid[x*ny+y];
			z[1][2]=grid[(x+1)*ny+y];
			z[1][3]=grid[(x+2)*ny+y];
			
			z[2][0]=grid[(x-1)*ny+y+1];
			z[2][1]=grid[x*ny+y+1];
			z[2][2]=grid[(x+1)*ny+y+1];
			z[2][3]=grid[(x+2)*ny+y+1];
			
			z[3][0]=grid[(x-1)*ny+y+2];
			z[3][1]=grid[x*ny+y+2];
			z[3][2]=grid[(x+1)*ny+y+2];
			z[3][3]=grid[(x+2)*ny+y+2];
			
			for (i=0;i<4;i++)
				for (j=0;j<4;j++) {
					if (z[i][j]==-9999) known[i][j]=false;
					else known[i][j]=true;
				}
			
			if (bicubic(pixx,pixy,z,known,dens,zd)) {
					
				for (i=0;i<dens;i++) {
					
					lf=gmod(minx+x*pixx+pixdx+((double)i-0.5)*ppx+180.0,360.0)-180.0;
					rt=gmod(minx+x*pixx+pixdx+((double)i+0.5)*ppx+180.0,360.0)-180.0;
		
					for (j=0;j<dens;j++) {
					
						tp=miny+y*pixy+pixdy+(j-0.5)*ppy;
						if (tp>89.9999) tp=89.9999;
						else if (tp<-89.9998) tp=-89.9998;
						
						bt=miny+y*pixy+pixdy+(j+0.5)*ppy;
						if (bt>89.9998) bt=89.9998;
						else if (bt<-89.9999) bt=-89.9999;
								
						if (lf<=rt)
							fillgrid_fast(lf,bt,rt,tp,zd[j*dens+i]);
						else {
							fillgrid_fast(lf,bt,180.0,tp,zd[j*dens+i]);
							fillgrid_fast(-180.0,bt,rt,tp,zd[j*dens+i]);
						}
					}
				}
			}
		}
	}
	
	restore();
	
	delete grid;
	
	//printf("Interpolation was successful!\n");
	
	return true;
}


colourscaletype colourscale_from_xtring(xtring cs) {
	
	if (cs=="dither") return DITHER;
	else if (cs=="red") return RED;
	else if (cs=="blue") return BLUE;
	else if (cs=="green") return GREEN;
	else if (cs=="shade") return SHADE;
	else if (cs=="redviolet") return REDVIOLET;
	else if (cs=="violetred") return VIOLETRED;
	else if (cs=="redblue") return REDBLUE;
	else if (cs=="bluered") return BLUERED;
	else if (cs=="redcyan") return REDCYAN;
	else if (cs=="cyanred") return CYANRED;
	else if (cs=="greyred") return GREYRED;
	else if (cs=="greycyan") return GREYCYAN;
	else if (cs=="cool") return COOL;
	else if (cs=="warm") return WARM;
	else if (cs=="neegreen") return NEEGREEN;
	else if (cs=="neered") return NEERED;
	else if (cs=="altitude") return ALTITUDE;
	else if (cs=="depth") return DEPTH;
	else if (cs=="soil") return SOIL;
	else if (cs=="vegetation") return VEGETATION;
	else if (cs=="fire") return FIRE;
	else if (cs=="wet") return WET;
	else if (cs=="dry") return DRY;
	
	return REDVIOLET;
}


bool mapmodel(xtring title,xtring cscale1,xtring cscale2,
	double minval,double maxval,double interval,int nclass,
 	bool ifscale,bool iflogscale,double round,
	double north,double south,double east,double west,projtype proj,
	double pixx,double pixy,double pixdx,double pixdy,
	double left,double right,double top,double bottom,bool ifgrid,
	bool ifframe,bool ifplotcoast,bool ifplotbound,bool ifplotsweden,
	double textsize_title,double textsize_legend,double linewd,
	bool ifhorizlegend,bool ifvertlegend,int orient,bool nozeroclass,
	double lon0,int dens,bool ifmaskocean,bool ifmasksweden,bool ifcat) {
	
	// Plots a single-page colour map	

	double dlon,dlat,dval,titlepos;
	float flon,flat,fval;
	int cla,i;
	double gridx,gridy,diag;
	double aspect,graspect,width,height,gap;
	bool iflandscape;
	double lf,rt,tp,bt;
	bool named_classes = false;
	
	if (orient==1 || orient==0) iflandscape=false;
	else iflandscape=true;
		
	// Determine grid line spacing
	diag=sqrt((north-south)*(north-south)+(east-west)*(east-west));
	if (diag>365) {
		gridx=30.0;
		gridy=30.0;
	}
	else if (diag>90) {
		gridx=15.0;
		gridy=15.0;
	}
	else if (diag>45) {
		gridx=10.0;
		gridy=5.0;
	}
	else {
		gridx=5.0;
		gridy=5.0;
	}
	
	double t=textsize(textsize_legend);
	textbold(0);
	
	// Fit map in window
	newgraph(left,bottom,right,top);
	scalegeog(west,south,east,north,proj,lon0,aspect);
	
	if (aspect>LSTHRESH && orient==0) iflandscape=true;
	
	if (iflandscape) {
		left=LLEFT;
		right=LRIGHT;
		top=LTOP;
		bottom=LBOTTOM;
		orientation(1);
	}
	else {
		left=PLEFT;
		right=PRIGHT;
		top=PTOP;
		bottom=PBOTTOM;
		orientation(0);
	}

	// Do we have categorical data and at least one class with a name
	// given in the legend? If so, prefer vertical legend (unless the
	// user has specifically asked for a horizontal legend).
	if (ifcat) {
		for (i = 0; i < ncolourkey; i++) {
			if (pcolourkey[i].name != "") {
				 named_classes = true;
			}
		}
	}
	
	// Vertical legend?
	if (((aspect<VLEGTHRESH || nclass>12 || named_classes) && !ifhorizlegend) || ifvertlegend) {
		right-=VLEGX;
		bottom-=VLEGSNEAK;
	}
	
	width=right-left;
	height=top-bottom;
	graspect=width/height;
	if (aspect>graspect) {
		gap=height-width/aspect;
		bottom=bottom+gap*0.5;
		top=top-gap*0.5;
		newgraph(left,bottom,right,top);
	}
	else {
		gap=width-height*aspect;
		left=left+gap*0.5;
		right=right-gap*0.5;
		newgraph(left,bottom,right,top);
	}
	
	titlepos=top+10;
		
	scalegeog(west,south,east,north,proj,lon0,aspect);
		
	patternstyle(0);
	
	// Determine data scaling

	//printf("ifcat=%d ifscale=%d nclass=%d minval=%g maxval=%g\n",ifcat,ifscale,nclass,minval,maxval);
	
	if (!ifcat) {
		if (ifscale) {
			if (nclass<0) { // no parameters specified by user
				if (nozeroclass)
					datascale_nozeroclass(minval,maxval,round); // = min,max
				else {
					datascale(minval,maxval,round);
				}
			}
			else if (!nclass) { // max and min specified by user
				if (nozeroclass)
					datascale_nozeroclass(minval,maxval,round);
				else
					datascale(minval,maxval,round);
			}
			else {
				if (nozeroclass || (minval>0.0 || minval+interval*nclass<0.0))
					datascale(nclass,minval,interval,false);
				else
					datascale(nclass,minval,interval,true);	
			}
		}
		else if (iflogscale) {
			//printf("minval=%g maxval=%g nclass=%d\n",minval,maxval,nclass);
			if (nozeroclass) 
				datascale_logarithmic_nozeroclass(minval,maxval,nclass);
			else
				datascale_logarithmic(minval,maxval,nclass);
		}
		else {
			if (nozeroclass)
				datascale_nozeroclass(minval,maxval,round); // = min,max
			else {
				//printf("Calling datascale: minval=%g maxval=%g round=%g\n",minval,maxval,round);
				datascale(minval,maxval,round);
			}
		}
	}

	if (cscale2=="")
		colourscale(colourscale_from_xtring(cscale1));
	else
		colourscale(colourscale_from_xtring(cscale1),
			colourscale_from_xtring(cscale2));
	
	if (aspect>=VLEGTHRESH && nclass<=12 && !named_classes && !ifvertlegend || ifhorizlegend) { // horizontal legend
		width=right-left;
		if (width>HLEGX) {
			gap=width-HLEGX;
			left+=gap*0.5;
			right-=gap*0.5;
		}
		scalelegend(left,bottom-HLEGOFF,right,bottom-HLEGOFF+HLEGH,false,"",true,false);
	}
	else { // vertical legend
		height=top-bottom;
		if (height<VLEGY) {
			top+=(VLEGY-height)*0.5;
			bottom-=(VLEGY-height)*0.5;
			height=VLEGY;
		}
		gap=height-VLEGY;
		scalelegend(right+10,bottom+gap*0.5,right+10+VLEGW,top-gap*0.5,true,"",true,false);
	}
	
	if (dens>0) {
		
		if (npixel*dens*dens>MAXPIXEL) {
			dens--;
			while (npixel*dens*dens>MAXPIXEL) dens--;
			printf("Smoothing density reduced to %d due to large grid\n",dens);
		}
		
		if (!dobicubic(pixx,pixy,pixdx,pixdy,dens)) {
			printf("Could not perform bicubic interpolation\n");
			return false;
		}
		
	}
	else {
	
		for (i=0;i<npixel;i++) {
			lf=gmod(pixdata[i].x-pixx*0.5+pixdx+180.0,360.0)-180.0;
			rt=gmod(pixdata[i].x+pixx*0.5+pixdx+180.0,360.0)-180.0;
			tp=pixdata[i].y+pixy*0.5+pixdy;
			if (tp>89.9999) tp=89.9999;
			else if (tp<-89.9998) tp=-89.9998;
			bt=pixdata[i].y-pixy*0.5+pixdy;
			if (bt>89.9998) bt=89.9998;
			else if (bt<-89.9999) bt=-89.9999;
			
			if (lf<=rt)
				fillgrid(lf,bt,rt,tp,pixdata[i].val);
			else {
				fillgrid(lf,bt,180.0,tp,pixdata[i].val);
				fillgrid(-180.0,bt,rt,tp,pixdata[i].val);
			}
		}
	}
		

	if (ifmaskocean) plotpoly((char*)FILE_OCEANMASK);
	else if (ifmasksweden) plotpoly((char*)FILE_SWEDENMASK);
	
	linestyle(1);
	linewidth(0.5);
	
	if (ifgrid) grid(gridx,gridy,proj!=NPOLAR && proj!=SPOLAR);
	
	linestyle(0);
	linewidth(linewd);
	if (ifplotcoast) plotbound(COAST);
	if (ifplotbound) plotbound(BOUND);
	if (ifplotsweden) plotbound(SWEDEN);
	if (ifframe) frame();
	
	textsize(textsize_title);
	textbold(1);
	
	text(left+(right-left)*0.5,titlepos,title,BC);
	
	textbold(0);
	textsize(t);
	
	return true;
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

bool readheader(FILE*& in,xtring* label,int& ncol,int& lonitemno,int& latitemno,
	xtring filename,double* values,bool force_space_is_sep,bool force_space_not_sep,
	bool& ifvalues,int& lineno,bool& space_is_sep,bool ifwrite) {

	// Reads header row of an RCA-GUESS output file
	// label = array of header labels
	// ncol  = number of columns (labels)
	// lonitemno = guess at column number (1-based) containing longitude
	// latitemno = guess at column number (1-based) containing latitude
	// Returns false if too many items in file
	
	xtring line,item;
	bool alphabetics=false;
	int pos,p,i;
	
	ncol=0;
	
	if (force_space_not_sep) space_is_sep=false;
	else space_is_sep=true;
	
	while (!ncol && !feof(in)) {

		readline(in,line);
		//printf("line='%s'\n",(char*)line);
		if (line.find("\t")>=0 && !force_space_is_sep) space_is_sep=false;
		lineno++;
		
		lonitemno=-1;
		latitemno=-1;
		
		//printf("line='%s'\n",(char*)line);	
		pos=line.findnotoneof(" \r");
		//printf("pos=%d\n",pos);
	
		while (pos!=-1 && line.findnotoneof(" \t\r")!=-1) {
		/*	if (ncol==MAXITEM) {
				printf("Too many items in %s (maximum allowed is %d)\n",
					(char*)filename,MAXITEM);
				return false;
			}*/
			if (line[pos]=='\t') {
				item="";
				if (ifwrite) label[ncol]=item;
				//printf("1. Item %d = '%s'\n",ncol,(char*)item);
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
					if (ifwrite) label[ncol]=item;
					//printf("2. Item %d = '%s'\n",ncol,(char*)item);
					while (line[pos]==' ' || line[pos]=='\r') pos++;
					if (line[pos]=='\t') pos++;
					line=line.mid(pos);
					pos=line.findnotoneof(" \r");
				}
				else {
					item=remove_trailing_blanks(line);
					if (ifwrite) label[ncol]=item;
					//printf("3. Item %d = '%s'\n",ncol,(char*)item);
				}
			}
			
			//printf("item %d '%s' isnum=%d item.num()=%g\n",ncol,(char*)item,item.isnum(),item.num());
			
			if (item.isnum()) {
				if (ifwrite) values[ncol]=item.num();
			}
			else
				alphabetics=true;
			
			ncol++;
			
			if (item.len()>2) {
				if (lonitemno<0)
					if (item.left(3).lower()=="lon") lonitemno=ncol;
				if (latitemno<0)
					if (item.left(3).lower()=="lat") latitemno=ncol;
			}
		}
	}
	
	if (!ncol) {
		printf("%s contains no data\n",(char*)filename);
		return false;
	}
	else if (ncol<3) {
		printf("At least three columns expected in %s\n",(char*)filename);
		return false;
	}
		
	if (lonitemno<0 || latitemno<0) {
		lonitemno=1;
		latitemno=2;
	}
	
	ifvalues=!alphabetics;
		
	return true;
}


bool finditem(xtring item,int& itemno,xtring infile,xtring* label,int nitem) {

	int i;
	
	if (itemno) {
		// taking data from specified column number - no header assumed

		if (itemno>nitem) {
			printf("Item number %d not found in %s\n",itemno,(char*)infile);
			return false;
		}
		
		itemno--; // convert to 0-based
	}
	else {
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
				return false;
			}
			else {
				printf("Item %s not found in %s\n",(char*)item,(char*)infile);
				printf("Choosing %s instead\n",(char*)label[itemno]);
			}
		}
	}
	
	return true;
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
	
	/*FILE* out=fopen("temp.txt","a");
	if (out) {
		for (j=0;j<nitem;j++)
			fprintf(out,"'%s'\t",(char*)sval[j]);
		fprintf(out,"\n");
	}
	fclose(out);*/
	
	return true;
}

bool readdata(xtring infile,xtring item,xtring lonitem,xtring latitem,
	int itemno,int lonitemno,int latitemno,double& minval,double& maxval,
	double pixx,double pixy,double pixdx,double pixdy,
	bool force_space_is_sep,bool force_space_not_sep,
	double& north,double& south,double& east,double& west,
	bool& iscat,bool force_cat) {

	int nitem,i,autolonitem,autolatitem;
	xtring* label;
	double dlon,dlat,ditem;
	double* dval;
	xtring* sval;
	bool found,blank,isnum;
	bool ifvalues;
	bool space_is_sep;
	bool warned=false;
	int lineno,nchar;
	
	FILE* in=fopen(infile,"rt");
	if (!in) {
		printf("Could not open %s for input\n",(char*)infile);
		return false;
	}
	
	// First read file to find out how many records and items it contains
	
	lineno=0;
	
	if (!readheader(in,label,nitem,autolonitem,autolatitem,infile,dval,
		force_space_is_sep,force_space_not_sep,
		ifvalues,lineno,space_is_sep,false)) {
			
		return false;
	}
	
	if (nitem>MAXITEM) {
		printf("Too many items in %s (maximum allowed is %d)\n",
			(char*)filename,MAXITEM);
		return false;
	}
	
	//printf("nitem=%d\n",nitem);
	sval=new xtring[nitem];
	if (!sval) {
		printf("Out of memory\n");
		return false;
	}
	
	npixel=ifvalues;
	
	while (!feof(in)) {
		found=readline(in,nitem,sval,space_is_sep);
		lineno++;
		if (found) {
			if (npixel==MAXGRID) {
				printf("Too many records in %s\n",(char*)infile);
				delete[] sval;
				return false;
			}

			npixel++;
		}
	}
	
	if (!npixel) {
		printf("No data found in %s\n",(char*)infile);
		delete[] sval;
		return false;
	}
	
	// Rewind file and read again with actual data
	
	label=new xtring[nitem];
	if (!label) {
		printf("Out of memory\n");
		delete[] sval;
		return false;
	}
	
	dval=new double[nitem];
	if (!dval) {
		printf("Out of memory\n");
		delete[] sval;
		delete[] label;
		return false;
	}
		
	//printf("npixel=%d\n",npixel);
	pixdata=new Point[npixel];
	if (!pixdata) {
		printf("Out of memory reserving space for %d data points\n",npixel);
		delete[] sval;
		delete[] label;
		delete[] dval;
		return false;
	}
	
	rewind(in);
	lineno=0;
	
	if (!readheader(in,label,nitem,autolonitem,autolatitem,infile,dval,
		force_space_is_sep,force_space_not_sep,
		ifvalues,lineno,space_is_sep,true)) {
		
		delete[] sval;
		delete[] label;
		delete[] dval;
		delete[] pixdata;	
		return false;
	}

	
	if (lonitem=="" && lonitemno==0) lonitemno=autolonitem;
	if (latitem=="" && latitemno==0) latitemno=autolatitem;
	
	//printf("lonitem='%s' latitem='%s' ifvalues=%d\n",(char*)label[lonitemno-1],(char*)label[latitemno-1],ifvalues);
	
	if (!finditem(item,itemno,infile,label,nitem)) {
		delete[] sval;
		delete[] label;
		delete[] dval;
		delete[] pixdata;
		return false;
	}
	if (!finditem(lonitem,lonitemno,infile,label,nitem)) {
		delete[] sval;
		delete[] label;
		delete[] dval;
		delete[] pixdata;
		return false;
	}
	if (!finditem(latitem,latitemno,infile,label,nitem)) {
		delete[] sval;
		delete[] label;
		delete[] dval;
		delete[] pixdata;
		return false;
	}
	
	//fmt.printf("%da",nitem);
	
	npixel=0;
	ncat=0;
	iscat=true;
	
	while (!feof(in) || ifvalues) {
	
		if (ifvalues) {
			found=true;
			ifvalues=false;
			dlon=dval[lonitemno];
			//dlon=fmod(dlon+180.0,360.0)-180.0;
			dlat=dval[latitemno];
			if (dlat<-90.0) dlat=-90.0;
			else if (dlat>90.0) dlat=90.0;
			ditem=dval[itemno];
		}
		else {
			//found=readfor(in,fmt,sval);
			found=readline(in,nitem,sval,space_is_sep);
			lineno++;
			if (found) {	
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
						if (i==lonitemno || i==latitemno || i==itemno) {
							if (!sval[i].isnum()) {
								isnum=false;
								//i=nitem;
							}
						}
					}
				}
				
				if (blank)
					printf("Line %d is blank - ignoring\n",lineno);
				/*else if (!isnum) {
					printf("Line %d of %s contains non-numeric data - ignoring\n",
						lineno,(char*)infile);
				}*/
				else if (sval[lonitemno]=="" || sval[latitemno]=="" || sval[itemno]=="") {
					printf("Missing data on line %d - ignoring\n",lineno);
				}
				else if (!sval[lonitemno].isnum() || !sval[latitemno].isnum() ||
					!sval[itemno].isnum() || sval[lonitemno]=="" || sval[latitemno]=="" ||
					sval[itemno]=="") {
					
					printf("Line %d contains non-numeric data - ignoring\n",lineno);
				}
				else {
					found=true;
					//dlon=fmod(sval[lonitemno].num()+180.0,360.0)-180.0;
					dlon=sval[lonitemno].num();
					dlat=sval[latitemno].num();
					if (dlat<-90.0) dlat=-90.0;
					else if (dlat>90.0) dlat=90.0;
					ditem=sval[itemno].num();
					
					//printf("%g\t%g\t'%s' %g\n",dlon,dlat,(char*)sval[itemno],ditem);
				}
			}
			
		}
		
		if (found) {
				
			// Try to interpret as categorical data
			if (iscat) {
				found=false;
				for (i=0;i<ncat;i++) {
					if (ditem==categories[i]) {
						i=ncat;
						found=true;
					}
				}
				if (!found) {
					if (force_cat && ncat==MAXCLASS && !warned) {
						printf("Too many unique values to plot as categorical data\n");
						return false;
					}
					if ((!force_cat && ncat==MAXCAT) || (force_cat && ncat==MAXCLASS)) iscat=false;
					else categories[ncat++]=ditem;
				}
			} 
		
			pixdata[npixel].x=dlon;
			pixdata[npixel].y=dlat;
			pixdata[npixel].val=ditem;
			
			if (!npixel) {
				minval=maxval=ditem;
				north=dlat+pixy*0.5+pixdy;
				south=dlat-pixy*0.5+pixdy;
				east=dlon+pixx*0.5+pixdx;
				west=dlon-pixx*0.5+pixdx;
			}
			else {
				if (ditem<minval) minval=ditem;
				if (ditem>maxval) maxval=ditem;
				if (dlat+pixy*0.5+pixdy>north) north=dlat+pixy*0.5+pixdy;
				if (dlat-pixy*0.5+pixdy<south) south=dlat-pixy*0.5+pixdy;
				if (dlon+pixx*0.5+pixdx>east) east=dlon+pixx*0.5+pixdx;
				if (dlon-pixx*0.5+pixdx<west) west=dlon-pixx*0.5+pixdx;
			}
			
			npixel++;
		/*	if (npixel==MAXGRID) {
				printf("Too many records in %s\n",(char*)infile);
				return false;
			}*/
		}
	}
	
	delete[] sval;
	delete[] dval;
	delete[] label;
		
	return true;
}

bool estimate_pixsize(double& pixx,double& pixy,double& pixdx,double& pixdy,
	bool setpix,bool setoffset) {

	const int NSAMPLE=10; // Number of places in data matrix to search for nearest neighbours
	int i,j,targ;
	double minx,miny,diff;
	bool firstx=true,firsty=true;
	
	for (i=0;i<NSAMPLE;i++) {
		targ=(double)npixel/(double)NSAMPLE*(double)i;
		for (j=0;j<npixel;j++) {
			if (firstx) {
				if (pixdata[targ].x!=pixdata[j].x) {
					minx=fabs(pixdata[targ].x-pixdata[j].x);
					firstx=false;
				}
			}
			else {
				diff=fabs(pixdata[targ].x-pixdata[j].x);
				if (diff && diff<minx) minx=diff;
			}
			if (firsty) {
				if (pixdata[targ].y!=pixdata[j].y) {
					miny=fabs(pixdata[targ].y-pixdata[j].y);
					firsty=false;
				}
			}
			else {
				diff=fabs(pixdata[targ].y-pixdata[j].y);
				if (diff && diff<miny) miny=diff;
			}
		}
	}
	
	if (firstx || firsty) return false;
	
	if (setpix) {
		pixx=minx;
		pixy=miny;
	}
	
	if (setoffset) {
		pixdx=minx*0.5;
		pixdy=miny*0.5;
	}
	
	return true;
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

bool processargs(int argc,char* argv[],xtring& title,double& minval,double& maxval,
	double& interval,int& nclass,double& round,bool& ifscale,bool& iflogscale,
	xtring& cscale_request1,xtring& cscale_request2,xtring& outfile,xtring& infile,
	xtring& item,int& itemno,double& north,double& south,double& east,double& west,
	projtype& proj,double& pixx,double& pixy,double& pixdx,double& pixdy,
	bool& havepixsize,bool& havepixoffset,
	bool& ifgrid,bool& ifframe,bool& ifn,bool& ifs,bool& ife,bool& ifw,
	xtring& filelegend,bool& ifhlslegend,
	bool& ifplotcoast,bool& ifplotbound,bool& ifplotsweden,
	double& textsize,double& textsize_legend,double& linewidth,bool& ifhorizlegend,
	bool& ifvertlegend,int& orient,
	xtring& lonitem,int& lonitemno,xtring& latitem,int& latitemno,
	bool& nozeroclass,double& lon0,int& dens,bool& ifmaskocean,bool& ifmasksweden,
	bool& havegs,xtring& ext,bool& ifxview,
	bool& force_space_is_sep,bool& force_space_not_sep,bool& force_cat) {

	int i;
	xtring arg,arg2,filepart,dir,file;
	bool haveinfile=false;
	bool haven,haves,havee,havew;
	double dval;
	
	// Defaults
	title="";
	cscale_request1="redviolet";
	cscale_request2="";
	outfile="figure.ps";
	ifgrid=true;
	ifframe=true;
	itemno=3;
	textsize=14;
	textsize_legend=12;
	linewidth=0.5;
	ifhorizlegend=ifvertlegend=false;
	orient=0; // automatic page orientation
	lonitem=latitem="";
	lonitemno=latitemno=0;
	nozeroclass=false;
	lon0=0;
	ifn=ifs=ife=ifw=false;
	north=90.0;
	south=-90.0;
	east=180.0;
	west=-180.0;
	dens=-1;
	ifmaskocean=ifmasksweden=false;
	haven=haves=havee=havew=false;
	havegs=false;
	ifxview=false;
	force_space_is_sep=force_space_not_sep=false;
	force_cat=false;
	
	// Defaults for data scaling
	ifscale=false;
	iflogscale=false;
	round=10;

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
			else if (arg=="-s") { // data scale
				if (iflogscale) {
					printf("Cannot combine option -s with -slog\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Cannot combine option -s with -legend, -rgblegend or -hlslegend\n");
					return false;
				}
				else if (force_cat) {
					printf("Cannot combine option -s with -cat\n");
					return false;
				}
				else {
					if (argc>=i+3) {
						arg=argv[i+1];
						arg2=argv[i+2];
						if (!arg.isnum() || !arg2.isnum()) {
							ifscale=true;
							nclass=-1;
							//printf("Option -s must be followed by [<min> <range> <nclass>] | [<min> <max>]\n");
							//return false;
						}
						else {
							minval=arg.num();
							maxval=arg2.num();
							i+=2;
							if (argc>=i+2) {
								arg=argv[i+1];
								if (arg.isnum()) { // -s <min> <range> <nclass>
									interval=maxval;
									if (!interval) {
										printf("Option -s <min> <range> <nclass>: <range> must be non-zero\n");
										return false;
									}
									nclass=arg.num();
									if (arg.num()!=nclass || nclass<1) {
										printf("Option -s <min> <range> <nclass>: <nclass> must be a positive integer\n");
										return false;
									}
									i++;
								}
								else { // -s <min> <max>
									if (maxval<=minval) {
										printf("Option -s <min> <max>: <min> must be < <max>\n");
										return false;
									}
									nclass=0;
								}
							}
							else {
								if (maxval<=minval) {
									printf("Option -s <min> <max>: <min> must be < <max>\n");
									return false;
								}
								nclass=0;
							}
							ifscale=true;
						}
					}
					else {
						ifscale=true;
						nclass=-1; // signifies scalar data
						//printf("Option -s must be followed by [<min> <range> <nclass>] | [<min> <max>]\n");
						//return false;
					}
				}
			}
			else if (arg=="-slog") { // logarithmic data scale
				if (ifscale) {
					printf("Cannot combine option -slog with -s\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Cannot combine option -slog with -legend, -rgblegend or -hlslegend\n");
					return false;
				}
				else if (force_cat) {
					printf("Cannot combine option -s with -cat\n");
					return false;
				}
				else {
					iflogscale=true;
					minval=maxval=0.0;
					nclass=10;
					if (argc>=i+3) {
						arg=argv[i+1];
						if (arg.isnum()) {
							minval=arg.num();
							arg=argv[i+2];
							if (!arg.isnum()) {
								printf("Option -slog [<min> <max> [<nclass>]]: <max> must be a number\n");
								return false;
							}
							maxval=arg.num();
							if (minval>=maxval) {
								printf("Option -slog [<min> <max> [<nclass>]]: <min> must be < <max>\n");
								return false;
							}
							i+=2;
							if (argc>=i+2) {
								arg=argv[i+1];
								if (arg.isnum()) {
									nclass=arg.num();
									if (nclass<1) nclass=1;
									else if (nclass>100) nclass=100;
									i++;
								}
							}
						}
					}
				}
			}
			else if (arg=="-r") { // rounding parameter for data scale
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -r <round>: <round> must be a number\n");
						return false;
					}
					round=arg.num();
					if (round<1 || int(round)!=round) {
						printf("Option -r <round>: <round> must be a positive integer\n");
						return false;
					}
					i+=1;
				}
				else {
					printf("Option -r must be followed by <round>\n");
					return false;
				}
			}
			else if (arg=="-c") { // colour scale
				if (argc>=i+2) {
					arg=argv[i+1];
					arg=arg.lower();
					if (arg=="dither" || arg=="red" || arg=="blue" ||
						arg=="green" || arg=="shade" || arg=="redviolet" ||
						arg=="violetred" || arg=="redblue" || arg=="bluered" ||
						arg=="redcyan" || arg=="cyanred" || arg=="greyred" ||
						arg=="greycyan" || arg=="cool" || arg=="warm" ||
						arg=="neegreen" || arg=="neered" || arg=="altitude" ||
						arg=="depth" || arg=="soil" || arg=="vegetation" || arg=="fire" ||
						arg=="wet" || arg=="dry" ) {
							cscale_request1=arg;
					}
					else {
						printf("Option -c must be followed by colour scale specifier\n");
						printf("Possible specifiers are:\n");
						printf("  DITHER,RED,BLUE,GREEN,SHADE,REDVIOLET,VIOLETRED,\n");
						printf("  REDBLUE,BLUERED,REDCYAN,CYANRED,GREYRED,GREYCYAN,\n");
						printf("  COOL,WARM,WET,DRY,DEPTH,ALTITUDE,NEEGREEN,NEERED,\n");
						printf("  SOIL,VEGETATION,FIRE\n");
						return false;
					}
					i+=1;
					if (argc>=i+2) { // second argument?
						arg=argv[i+1];
						arg=arg.lower();
						if (arg[0]!='-') {
							if (arg=="dither" || arg=="red" || arg=="blue" ||
								arg=="green" || arg=="shade" || arg=="redviolet" ||
								arg=="violetred" || arg=="redblue" || arg=="bluered" ||
								arg=="redcyan" || arg=="cyanred" || arg=="greyred" ||
								arg=="greycyan" || arg=="cool" || arg=="warm" ||
								arg=="neegreen" || arg=="neered" || arg=="altitude" ||
								arg=="depth" || arg=="soil" || arg=="vegetation" || arg=="fire" ||
								arg=="wet" || arg=="dry" ) {
									cscale_request2=arg;
									i+=1;
							}
						}
					}
				}
				else {
					printf("Option -c must be followed by colour scale specifier\n");
					printf("Possible specifiers are:\n");
					printf("  DITHER,RED,BLUE,GREEN,SHADE,REDVIOLET,VIOLETRED,\n");
					printf("  REDBLUE,BLUERED,REDCYAN,CYANRED,GREYRED,GREYCYAN,\n");
					printf("  COOL,WARM,WET,DRY,DEPTH,ALTITUDE,NEEGREEN,NEERED,\n");
					printf("  SOIL,VEGETATION,FIRE\n");
					return false;
				}
			}
			else if (arg=="-w") { // window
				if (argc>=i+2) {
					arg=argv[i+1];
					arg=arg.lower();
					if (arg=="sweden") {
						if (!havew) west=10.5;
						if (!haves) south=55;
						if (!havee) east=25;
						if (!haven) north=69.5;
					}
					else if (arg=="baltic") {
						if (!havew) west=7.5;
						if (!haves) south=48.5;
						if (!havee) east=40;
						if (!haven) north=69.5;
					}
					else if (arg=="nordic") {
						if (!havew) west=-25;
						if (!haves) south=54;
						if (!havee) east=32;
						if (!haven) north=72;
					}
					else if (arg=="scandinavia") {
						if (!havew) west=4;
						if (!haves) south=54;
						if (!havee) east=32;
						if (!haven) north=72;
					}
					else if (arg=="eu15") {
						if (!havew) west=-10.5;
						if (!haves) south=34;
						if (!havee) east=32;
						if (!haven) north=72;
					}
					else if (arg=="europe") {
						if (!havew) west=-26;
						if (!haves) south=34;
						if (!havee) east=69.5;
						if (!haven) north=82;
					}
					else if (arg=="us48") {
						if (!havew) west=-125;
						if (!haves) south=24.5;
						if (!havee) east=-66;
						if (!haven) north=49.5;
					}
					else if (arg=="usa") {
						if (!havew) west=-178.5;
						if (!haves) south=24.5;
						if (!havee) east=-66;
						if (!haven) north=72;
					}
					else if (arg=="namerica") {
						if (!havew) west=-180;
						if (!haves) south=6;
						if (!havee) east=-51;
						if (!haven) north=84;
					}
					else if (arg=="samerica") {
						if (!havew) west=-92;
						if (!haves) south=-56;
						if (!havee) east=-34;
						if (!haven) north=13;
					}
					else if (arg=="africa") {
						if (!havew) west=-26;
						if (!haves) south=-36;
						if (!havee) east=58.5;
						if (!haven) north=38;
					}
					else if (arg=="asia") {
						if (!havew) west=25;
						if (!haves) south=-12;
						if (!havee) east=180;
						if (!haven) north=82;
					}
					else if (arg=="eurasia") {
						if (!havew) west=-11;
						if (!haves) south=-12;
						if (!havee) east=180;
						if (!haven) north=82;
					}
					else if (arg=="australia") {
						if (!havew) west=112.5;
						if (!haves) south=-44;
						if (!havee) east=154;
						if (!haven) north=-10;
					}
					else if (arg=="japan") {
						if (!havew) west=128.5;
						if (!haves) south=30;
						if (!havee) east=146.5;
						if (!haven) north=46;
					}
					else if (arg=="arctic") {
						if (!havew) west=-180;
						if (!haves) south=59.5;
						if (!havee) east=180;
						if (!haven) north=90;
					}
					else if (arg=="boreal") {
						if (!havew) west=-180;
						if (!haves) south=40;
						if (!havee) east=180;
						if (!haven) north=71;
					}
					else if (arg=="globalland") {
						if (!havew) west=-180;
						if (!haves) south=-56;
						if (!havee) east=180;
						if (!haven) north=84;
					}
					else if (arg=="global") {
						if (!havew) west=-180;
						if (!haves) south=-90;
						if (!havee) east=180;
						if (!haven) north=90;
					}
					else {
						printf("Option -w must be followed by window specifier\n");
						printf("Possible specifiers are:\n");
						printf("  AFRICA,ARCTIC,ASIA,AUSTRALIA,BALTIC,BOREAL,EU15,EURASIA,\n");
						printf("  EUROPE,GLOBAL,GLOBALLAND,JAPAN,NAMERICA,NORDIC,SAMERICA,\n");
						printf("  SCANDINAVIA,SWEDEN,US48,USA\n");
						return false;
					}
					i+=1;
					ifn=ifs=ife=ifw=true;
				}
				else {
					printf("Option -w must be followed by window specifier\n");
					printf("Possible specifiers are:\n");
					printf("  AFRICA,ARCTIC,ASIA,AUSTRALIA,BALTIC,BOREAL,EU15,EURASIA,\n");
					printf("  EUROPE,GLOBAL,GLOBALLAND,JAPAN,NAMERICA,NORDIC,SAMERICA,\n");
					printf("  SCANDINAVIA,SWEDEN,US48,USA\n");
					return false;
				}
			}
			else if (arg=="-p") { // projection
				if (argc>=i+2) {
					arg=argv[i+1];
					arg=arg.lower();
					if (arg=="goode") proj=GOODE;
					else if (arg=="mercator") proj=MERCATOR;
					else if (arg=="lambert") proj=LAMBERT;
					else if (arg=="winkel") proj=WINKEL;
					else if (arg=="mollweide") proj=MOLLWEIDE;
					else if (arg=="robinson") proj=ROBINSON;
					else if (arg=="fahey") proj=FAHEY;
					else if (arg=="cyleqd") proj=CYLEQD;
					//else if (arg=="lagrange") proj=LAGRANGE;
					else if (arg=="npolar") proj=NPOLAR;
					else if (arg=="spolar") proj=SPOLAR;
					else {
						printf("Option -p must be followed by projection specifier\n");
						printf("Possible specifiers are:\n");
						printf("  FAHEY,WINKEL,MERCATOR,LAMBERT,CYLEQD,MOLLWEIDE,GOODE,\n");
						printf("  ROBINSON,NPOLAR,SPOLAR\n");
						return false;
					}
					i+=1;
				}
				else {
					printf("Option -p must be followed by projection specifier\n");
					printf("Possible specifiers are:\n");
					printf("  FAHEY,WINKEL,MERCATOR,LAMBERT,CYLEQD,MOLLWEIDE,GOODE,\n");
					printf("  ROBINSON,NPOLAR,SPOLAR\n");
					return false;
				}
			}
			else if (arg=="-bound") { // boundaries
				if (argc>=i+2) {
					arg=argv[i+1];
					arg=arg.lower();
					if (arg=="coast") {
						ifplotcoast=true;
						ifplotbound=false;
						ifplotsweden=false;
					}
					else if (arg=="polit") {
						ifplotcoast=false;
						ifplotbound=true;
						ifplotsweden=false;
					}
					else if (arg=="sweden") {
						ifplotcoast=false;
						ifplotbound=false;
						ifplotsweden=true;
					}
					else if (arg=="none") {
						ifplotcoast=false;
						ifplotbound=false;
						ifplotsweden=false;
					}
					else {
						printf("Option -bound must be followed by boundary specifier\n");
						printf("Possible specifiers are:\n");
						printf("  COAST,POLIT,SWEDEN,NONE\n");
						return false;
					}
					i+=1;
				}
				else {
					printf("Option -bound must be followed by boundary specifier\n");
					printf("Possible specifiers are:\n");
					printf("  COAST,BOUND,SWEDEN,NONE\n");
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
			else if (arg=="-i") { // item 
				if (argc>=i+2) {
					item=argv[i+1];
					if (item.isnum()) {
						dval=item.num();
						if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
							itemno=dval;
							if (itemno>MAXITEM) {
								printf("Option -i: too many items (maximum allowed is %d)\n",MAXITEM);
								return false;
							}
							item="";
						}
						else itemno=0;
					}
					else itemno=0;
				}
				else {
					printf("Option -i must be followed by item label or column number\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-lon") { // longitude column label or item number 
				if (argc>=i+2) {
					lonitem=argv[i+1];
					if (lonitem.isnum()) {
						dval=lonitem.num();
						if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
							lonitemno=dval;
							if (lonitemno>MAXITEM) {
								printf("Option -lon: too many items (maximum allowed is %d)\n",MAXITEM);
								return false;
							}
							lonitem="";
						}
					}
				}
				else {
					printf("Option -lon must be followed by label or column number\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-lat") { // latitude column label or item number 
				if (argc>=i+2) {
					latitem=argv[i+1];
					if (latitem.isnum()) {
						dval=latitem.num();
						if (dval>=1.0 && int(dval)==dval) { // seems to be an item number
							latitemno=dval;
							if (latitemno>MAXITEM) {
								printf("Option -lat: too many items (maximum allowed is %d)\n",MAXITEM);
								return false;
							}
							latitem="";
						}
					}
				}
				else {
					printf("Option -lat must be followed by label or column number\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-north") { // northern boundary
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -north must be followed by a number in degrees latitude\n");
						return false;
					}
					north=arg.num();
					haven=ifn=true;
				}
				else {
					printf("Option -north must be followed by a number in degrees latitude\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-south") { // southern boundary
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -south must be followed by a number in degrees latitude\n");
						return false;
					}
					south=arg.num();
					haves=ifs=true;
				}
				else {
					printf("Option -south must be followed by a number in degrees latitude\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-east") { // eastern boundary
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -east must be followed by a number in degrees latitude\n");
						return false;
					}
					east=arg.num();
					havee=ife=true;
				}
				else {
					printf("Option -east must be followed by a number in degrees longitude\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-west") { // western boundary
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -west must be followed by a number in degrees latitude\n");
						return false;
					}
					west=arg.num();
					havew=ifw=true;
				}
				else {
					printf("Option -west must be followed by a number in degrees longitude\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-rot") { // rotation in degrees for polar projections
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -rot must be followed by a number in degrees\n");
						return false;
					}
					lon0=arg.num();
				}
				else {
					printf("Option -rot must be followed by a number in degrees\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-textsize_title") { // text size (points)
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -textsize_title must be followed by text size in points\n");
						return false;
					}
					textsize=arg.num();
				}
				else {
					printf("Option -textsize_title must be followed by text size in points\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-textsize_legend") { // text size (points)
				if (argc>=i+2) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -textsize_legend must be followed by text size in points\n");
						return false;
					}
					textsize_legend=arg.num();
				}
				else {
					printf("Option -textsize_legend must be followed by text size in points\n");
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
			else if (arg=="-pixsize") { // pixel dimensions
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -pixsize <x> <y>: <x> must be a number\n");
						return false;
					}
					pixx=arg.num();
					if (pixx<=0.0 || pixx>180.0) {
						printf("Option -pixsize <x> <y>: expecting a number in positive degrees\n");
						return false;
					}
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -pixsize <x> <y>: <y> must be a number\n");
						return false;
					}
					pixy=arg.num();
					if (pixy<=0.0 || pixy>90.0) {
						printf("Option -pixsize <x> <y>: expecting a number in positive degrees\n");
						return false;
					}
				}
				else {
					printf("Option -pixsize must be followed by <x> <y>\n");
					return false;
				}
				i+=2;
				havepixsize=true;
			}
			else if (arg=="-pixoffset") { // pixel offset from reference point
				if (argc>=i+3) {
					arg=argv[i+1];
					if (!arg.isnum()) {
						printf("Option -pixoffset <dx> <dy>: <dx> must be a number\n");
						return false;
					}
					pixdx=arg.num();
					arg=argv[i+2];
					if (!arg.isnum()) {
						printf("Option -pixoffset <dx> <dy>: <dy> must be a number\n");
						return false;
					}
					pixdy=arg.num();
				}
				else {
					printf("Option -pixoffset must be followed by <dx> <dy>\n");
					return false;
				}
				i+=2;
				havepixoffset=true;
			}
			else if (arg=="-smooth") { // specify smoothing and subpixel density
				if (argc>=i+2) {
					arg=argv[i+1];
					if (arg.isnum()) {
						dens=arg.num();
						if (dens<1 || dens>MAXDENS) {
							printf("Option -smooth <dens>: pixel density must be in range 1-%d\n",MAXDENS);
							return false;
						}
						i+=1;
					}
					else { // does not seem to be a smoothing value
						dens=DEFAULTDENS;
					}
				}
				else {
					dens=DEFAULTDENS;
				}
			}
			else if (arg=="-mask") { // mask
				if (argc>=i+2) {
					arg=argv[i+1];
					arg=arg.lower();
					if (arg=="ocean") {
						ifmaskocean=true;
						i+=1;
					}
					else if (arg=="sweden") {
						ifmasksweden=true;
						i+=1;
					}
					else {
						// No argument - assume default = ocean
						ifmaskocean=true;
					}
				}
				else {
					ifmaskocean=true;
				}
			}
			else if (arg=="-nogrid") {
				ifgrid=false;
			}
			else if (arg=="-noframe") {
				ifframe=false;
			}
			else if (arg=="-horiz") { // force horizontal legend
				ifhorizlegend=true;
			}
			else if (arg=="-vert") { // force vertical legend
				ifvertlegend=true;
			}
			else if (arg=="-landscape") { // force landscape mode
				orient=2;
			}
			else if (arg=="-portrait") { // force portrait mode
				orient=1;
			}
			else if (arg=="-nozero") { // suppress "exactly zero" data class
				nozeroclass=true;
			}
			else if (arg=="-cat") { // enforce categorical interpretation of data
				if (iflogscale) {
					printf("Cannot combine option -cat with -slog\n");
					return false;
				}
				else if (ifscale) {
					printf("Cannot combine option -cat with -s\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Cannot combine option -cat with -legend, -rgblegend or -hlslegend\n");
					return false;
				}
				else {
					force_cat=true;
				}
			}
			else if (arg=="-seps" || arg=="-sepst" || arg=="-septs") {
				force_space_is_sep=true;
			}
			else if (arg=="-sep" || arg=="-sept") {
				force_space_not_sep=true;
			}
			else if (arg=="-hlslegend") { // HLS legend file
				if (ifscale || iflogscale || force_cat) {
					printf("Cannot combine option -hlslegend with -s, -slog or -cat\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Only one legend file may be specified\n");
					return false;
				}
				else if (argc>=i+2) {
					filelegend=argv[i+1];
					ifhlslegend=true;
				}
				else {
					printf("Option -hlslegend must be followed by legend file name or path\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-rgblegend") { // RGB legend file
				if (ifscale || iflogscale || force_cat) {
					printf("Cannot combine option -rgblegend with -s, -slog or -cat\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Only one legend file may be specified\n");
					return false;
				}
				else if (argc>=i+2) {
					filelegend=argv[i+1];
					ifhlslegend=false;
				}
				else {
					printf("Option -rgblegend must be followed by legend file name or path\n");
					return false;
				}
				i+=1;
			}
			else if (arg=="-legend") { // Same as -rgblegend
				if (ifscale || iflogscale || force_cat) {
					printf("Cannot combine option -legend with -s, -slog or -cat\n");
					return false;
				}
				else if (filelegend!="") {
					printf("Only one legend file may be specified\n");
					return false;
				}
				else if (argc>=i+2) {
					filelegend=argv[i+1];
					ifhlslegend=false;
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
		printf("Input file name or path must be specified\n");
		return false;
	}

	if (itemno && item=="") item.printf("Item %d",itemno);

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
		
		title.printf("%s !c(190) %s",(char*)filepart,(char*)item);
	}
	
	if (north<=-89.0) north=-89.0;
	else if (north>89.0) north=89.0;
	if (south<-89.0) south=-89.0;
	else if (south>=89.0) south=89.0;
	if (west<-180.0) west=-180.0;
	else if (west>=180.0) west=180.0;
	if (east<=-180.0) east=-180.0;
	else if (east>180.0) east=180.0;
	
	if (north<=south) {
		printf("Northern boundary of window must be north of southern boundary\n");
		return false;
	}
	
	if (east<=west) {
		printf("Eastern boundary of window must be east of western boundary\n");
		return false;
	}
	
	return true;
}

void abort(xtring exe) {

	printf("Usage: %s <input-file> [ <options> ]\n",(char*)exe);
	printf("Options: -o <output-file>  -xv  -i <item-name> | <column-number>\n");
	printf("         -lon <item-name> | <column-number>  -lat <item-name> | <column-number>\n");
	printf("         -seps  -sept  -pixsize <x> <y>  -pixoffset <dx> <dy>\n");
	printf("         -c <colour-scale> [<colour-scale>]\n");
	printf("         -s [<min> <range> <nclass>] | [<min> <max>]  -cat\n");
	printf("         -slog [<min> <max> [<nclass>]]  -r <round>  -nozero\n");
	printf("         -legend <legend-file>  -hlslegend <legend-file>  -horiz  -vert\n");
	printf("         -w <window>  -west <lon>  -east <lon>  -south <lat>  -north <lat>\n");
	printf("         -t <title>  -p <projection>  -rot <lon>\n");
	printf("         -bound NONE | COAST | POLIT | SWEDEN\n");
	printf("         -nogrid  -noframe  -portrait  -landscape\n");
	printf("         -textsize_title <points>  -textsize_legend <points>\n");
	printf("         -linewidth <points>\n");
	printf("         -mask [OCEAN | SWEDEN]  -smooth [<density>]\n");
	printf("         -help\n");

	exit(99);
}

// Returns a new string, with leading and trailing whitespace removed
xtring trim_whitespace(const xtring& str) {
	xtring copy = str;

	int first = -1, last = -1;

	for (unsigned int i = 0; i < copy.len(); i++) {

		if (!isspace(copy[i])) {
				
			if (first == -1) {
				first = i;
			}

			last = i;
		}

	}

	if (first == -1) {
		return "";
	}

	return copy.mid(first, (last-first)+1);
}

bool readcolourkey(xtring filename,bool ifrgb) {

	// Reads colour and data scale information from a file in format:
	// <limit-val>
	// <1> <2> <3> 
	// <limit-val>
	// <1> <2> <3> 
	// <limit-val>
	// etc.
	// where <1> <2> <3> is either an RGB or HLS triplet, depending on 
	// the value of ifrgb
	// If the final <limit-val> is missing, it is interpreted as a categorical legend
	
	int nclass=0,count=0;
	bool waitlim=true,first=true,ifcat;
	double triplet[3],lim,lastlim;
	xtring line;
	
	FILE* in=fopen(filename,"rt");
	if (!in) {
		printf("Could not open %s for input\n",(char*)filename);
		return false;
	}
	
	// First check if it is a categorical or scalar legend
	while (!feof(in)) {
		readfor(in,"A#",&line);
		if (line!="") count++;
	}
	
	ifcat=!(count%2);
	//printf("count=%d ifcat=%d ifrgb=%d\n",count,ifcat,ifrgb);
	
	rewind(in);
	
	while (!feof(in)) {
		if (waitlim) {
			// Use fscanf here instead of readfor so we can continue reading
			// the rest of the line into a string if necessary (for category
			// name). using readfor(in, "f,a#", ...) doesn't work since that
			// returns false if the last line ends in EOF rather than \n.
			if (fscanf(in, "%lf", &lim) == 1) {
				xtring name;
				readfor(in, "a#", &name);
				name = trim_whitespace(name);

				if (ifcat) {
					c[nclass].name = name;
					c[nclass].minval=c[nclass].maxval=lim;
				}
				else {
					if (first) first=false;
					else {
						if (lim<lastlim) {
							c[nclass].minval=lim;
							c[nclass].maxval=lastlim;
						}
						else {
							c[nclass].minval=lastlim;
							c[nclass].maxval=lim;
						}
						nclass++;
					}
					lastlim=lim;
				}
				waitlim=false;
			}
		}
		else {
			if (nclass==MAXCLASS) {
				printf("Too many classes in %s: maximum allowed is %d\n",(char*)filename,MAXCLASS);
				return false;
			}
			if (readfor(in,"3f",triplet)) {
				if (ifrgb) {
					c[nclass].colour[0]=triplet[0];
					c[nclass].colour[1]=triplet[1];
					c[nclass].colour[2]=triplet[2];				
				}
				else {
					c[nclass].colour[0]=triplet[0]-120.0;
					c[nclass].colour[1]=triplet[1]*0.01;
					c[nclass].colour[2]=triplet[2]*0.01;
				}
				if (ifcat) {
					nclass++;
				}
				waitlim=true;
			}
		}
	}
	
	if (waitlim && !ifcat) {
		printf("Missing closing limit for class %d in %s\n",nclass+1,(char*)filename);
		return false;
	}
	
	//printf("Setting colour key ...\n");
	
	setcolourkey(c,nclass,ifrgb);
	
	fclose(in);
	
	return true;
}

void makecatcolourkey(int& nclass) {

	// Makes a colour key for categorical data
	
	const double HSTEP=60;
	const double SSTEP=-0.25;
	double LSTEP=0.25;
	
	double hue=0;
	double lum=0.5;
	double sat=1.0;
	int count;
	
	int i;
	count=0;
	
	for (i=0;i<ncat;i++) {
		c[i].minval=c[i].maxval=categories[i];
		c[i].colour[0]=hue;
		c[i].colour[1]=lum;
		c[i].colour[2]=sat;
		
		hue+=HSTEP;
		if (hue>=360.0) {
			hue-=360.0-HSTEP/3;
			sat+=SSTEP;
			if (sat<0.25) {
				sat=0.67;
				lum+=LSTEP;
			}
		}
	}
	
	setcolourkey(c,ncat,false);
	
	nclass=ncat;
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

int main(int argc,char* argv[]) {

	xtring title,outfile,infile,item,filelegend,file;
	double minval,maxval,interval,dmin,dmax,round;
	double north,south,east,west,n,s,e,w;
	double pixx,pixy,pixdx,pixdy;
	double left,right,top,bottom;
	int nclass,itemno;
	bool ifscale,iflogscale,ifgrid,ifframe,ifcat,ifhlslegend;
	bool havepixsize=false,havepixoffset=false;
	bool ifplotcoast,ifplotbound,ifplotsweden,ifmaskocean,ifmasksweden;
	xtring cscale_request1,cscale_request2;
	double textsize_title,textsize_legend,linewidth;
	int orientation;
	bool ifhorizlegend,ifvertlegend,nozeroclass;
	xtring lonitem,latitem,header,ext;
	int lonitemno,latitemno,dens;
	double lon0;
	bool ifn,ifs,ife,ifw;
	bool havegs,ifxview;
	bool force_space_is_sep,force_space_not_sep;
	bool force_cat;
	
	// Default settings
	north=NORTH;
	south=SOUTH;
	east=EAST;
	west=WEST;
	
	projtype proj=PROJ;
	
	pixx=PIXX;
	pixy=PIXY;
	pixdx=PIXDX;
	pixdy=PIXDY;
		
	filelegend="";
	
	ifplotcoast=true;
	ifplotbound=false;
	ifplotsweden=false;
		
	if (!processargs(argc,argv,title,minval,maxval,
		interval,nclass,round,ifscale,iflogscale,cscale_request1,cscale_request2,
		outfile,infile,item,itemno,north,south,east,west,proj,
		pixx,pixy,pixdx,pixdy,havepixsize,havepixoffset,
		ifgrid,ifframe,ifn,ifs,ife,ifw,filelegend,ifhlslegend,
		ifplotcoast,ifplotbound,ifplotsweden,
		textsize_title,textsize_legend,linewidth,
		ifhorizlegend,ifvertlegend,orientation,lonitem,lonitemno,latitem,latitemno,
  		nozeroclass,lon0,dens,ifmaskocean,ifmasksweden,havegs,ext,ifxview,
		force_space_is_sep,force_space_not_sep,force_cat))
			abort(argv[0]);
		
	left=PLEFT;
	right=PRIGHT;
	top=PTOP;
	bottom=PBOTTOM;
		
	// else ...

	unixtime(header);
	header=(xtring)"[GMAP  "+header+"]\n\n";
	printf("%s",(char*)header);

	if (!readdata(infile,item,lonitem,latitem,
		itemno,lonitemno,latitemno,dmin,dmax,pixx,pixy,pixdx,pixdy,
  		force_space_is_sep,force_space_not_sep,
		n,s,e,w,ifcat,force_cat)) abort(argv[0]);
	
	if (ifscale || iflogscale) ifcat=false;
		
	if (!havepixsize || !havepixoffset) {
		if (!estimate_pixsize(pixx,pixy,pixdx,pixdy,!havepixsize,!havepixoffset)) {
			if (!havepixsize) {
				printf("Assuming default pixel size (0.5,0.5)\n");
				printf("-pixsize to change\n");
			}
			if (!havepixoffset) {
				printf("Assuming default pixel offset (0.25,0.25)\n");
				printf("-pixoffset to change\n");
			}
		}
		else {
			if (!havepixsize) {
				printf("Pixel size seems to be (%g,%g)\n",pixx,pixy);
				printf("-pixsize to change\n");
			}
			if (!havepixoffset) {
				printf("Guessing pixel offset at (%g,%g)\n",pixdx,pixdy);
				printf("-pixoffset to change\n");
			}
		}
	}

	if (!ifn) north=n;
	if (!ifs) south=s;
	if (!ife) east=e;
	if (!ifw) west=w;
	
	if (north<=south) {
		if (south<=87.0) north=south+1.0;
		else south=north-1.0;
	}

	if (proj==NPOLAR && south<0.0) {
		south=0.0;
		north=90.0;
	}
	if (proj==SPOLAR && north>0.0) {
		north=0.0;
		south=-90.0;
	}

	if (east<=west) {
		if (west<=179) east=west+1.0;
		else west=east-1.0;
	}
		
	if (npixel==1 && !ifscale || dmin==dmax && !ifscale) {
		if (!itemno) {
			printf("Only one data value for item %s\n",(char*)item);
			printf("-s to specify data scale\n");
		}
		else {
			printf("Only one data value for item number %d\n",itemno);
			printf("-s to specify data scale\n");
		}
		
		delete[] pixdata;
		abort(argv[0]);
	}
	
	if (filelegend!="")
		if (!readcolourkey(filelegend,!ifhlslegend)) abort(argv[0]);
	
	
	if (havegs) file=(xtring)TEMPFILE;
	else file=outfile;
	
	if (startbengraph(file)==1) {
		printf("Could not open temporary file for output\n");
		
		delete[] pixdata;
		abort(argv[0]);
	}
	
	if (filelegend!="") readcolourkey(filelegend,!ifhlslegend);
	else if (ifcat && !ifscale) {
		makecatcolourkey(nclass);
		ifscale=true;
	}
	
	printf("Plotting graphics ...\n");
	
	if (!ifscale && (!iflogscale || minval==maxval)) {
		//printf("Setting min/max to %g/%g\n",dmin,dmax);
		minval=dmin;
		maxval=dmax;
	}
	
	if (ifscale && nclass==-1) {
		minval=dmin;
		maxval=dmax;
	}
		
	if (!mapmodel(title,cscale_request1,cscale_request2,minval,maxval,
		interval,nclass,ifscale,iflogscale,round,
		north,south,east,west,proj,pixx,pixy,pixdx,pixdy,
		left,right,top,bottom,ifgrid,ifframe,
		ifplotcoast,ifplotbound,ifplotsweden,
		textsize_title,textsize_legend,linewidth,
		ifhorizlegend,ifvertlegend,orientation,nozeroclass,lon0,dens,
		ifmaskocean,ifmasksweden,ifcat)) {
			
		delete[] pixdata;
		abort(argv[0]);
	}
	
	if (endbengraph()) {
		if (havegs)
			printf("Could not write PostScript output to temporary file\n");
		else
			printf("Could not open %s for output\n",(char*)file);
		
		delete[] pixdata;
		abort(argv[0]);
	};
	
	if (ifxview) {
		if (!callgs(file,outfile,"",true)) {
			
			delete[] pixdata;
			abort(argv[0]);
		}
	}
	
	if (havegs) {
		if (!callgs(file,outfile,ext,false)) {
			
			delete[] pixdata;
			abort(argv[0]);
		}
	}
	
	delete[] pixdata;
	printf("\nOutput is in file %s\n\n",(char*)outfile);
	
	return 0;
};
