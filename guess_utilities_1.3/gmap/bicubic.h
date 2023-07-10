


void bcucof(double y[4],double y1[4],double y2[4],double y12[4],double d1,double d2,double c[4][4]) {

	// Calculate coefficients for a bilinear interpolation
	// Ref: Press, W.H., Flannery, B.P., Teukolsky, S.A., Vetterling, W.T. 1989.
	//        Numerical Recipes. Cambridge University Press, Cambridge, pp. 99
	
	double cl[16],x[16];
	const double wt[16][16]={
		1,0,-3,2,0,0,0,0,-3,0,9,-6,2,0,-6,4,
		0,0,0,0,0,0,0,0,3,0,-9,6,-2,0,6,-4,
		0,0,0,0,0,0,0,0,0,0,9,-6,0,0,-6,4,
		0,0,3,-2,0,0,0,0,0,0,-9,6,0,0,6,-4,
		0,0,0,0,1,0,-3,2,-2,0,6,-4,1,0,-3,2,
		0,0,0,0,0,0,0,0,-1,0,3,-2,1,0,-3,2,
		0,0,0,0,0,0,0,0,0,0,-3,2,0,0,3,-2,
		0,0,0,0,0,0,3,-2,0,0,-6,4,0,0,3,-2,
		0,1,-2,1,0,0,0,0,0,-3,6,-3,0,2,-4,2,
		0,0,0,0,0,0,0,0,0,3,-6,3,0,-2,4,-2,
		0,0,0,0,0,0,0,0,0,0,-3,3,0,0,2,-2,
		0,0,-1,1,0,0,0,0,0,0,3,-3,0,0,-2,2,
		0,0,0,0,0,1,-2,1,0,-2,4,-2,0,1,-2,1,
		0,0,0,0,0,0,0,0,0,-1,2,-1,0,1,-2,1,
		0,0,0,0,0,0,0,0,0,0,1,-1,0,0,-1,1,
		0,0,0,0,0,0,-1,1,0,0,2,-2,0,0,-1,1  };
		
	double xx,d1d2=d1*d2;
	int i,j,k,el;
	for (i=0;i<4;i++) {
		x[i]=y[i];
		x[i+4]=y1[i]*d1;
		x[i+8]=y2[i]*d2;
		x[i+12]=y12[i]*d1d2;
	}
	for (i=0;i<16;i++) {
		xx=0.0;
		for (k=0;k<16;k++) {
			xx+=wt[k][i]*x[k];
		}
		cl[i]=xx;
	}
	el=0;
	for (i=0;i<4;i++) {
		for (j=0;j<4;j++) {
			c[i][j]=cl[el++];
		}
	}
}

void bcuint(double dx,double dy,double x,double y,double c[4][4],double& z) {

	// Bilinear interpolation
	// Ref: Press, W.H., Flannery, B.P., Teukolsky, S.A., Vetterling, W.T. 1989.
	//        Numerical Recipes. Cambridge University Press, Cambridge, pp. 99

	// x,y = displacement of target grid point from BL of known grid cell
	
	double t=x/dx;
	double u=y/dy;
	int i;
	
	z=0.0;
	for (i=3;i>=0;i--)
		z=t*z+((c[i][3]*u+c[i][2])*u+c[i][1])*u+c[i][0];
}

bool bicubic(double dx,double dy,double z[4][4],bool known[4][4],int dens,double* result) {

	// Computes bicubic interpolation for dens x dens grid intersections equally
	// spaced between corners of target grid cell for an array of 4 x 4 known grid points
	
	//  +----+----+----+
	//  |    |    |    |
	//  +----+----+----+
	//  |    | *  |    |   * = target grid cell
	//  +----+----+----+
	//  |    |    |    |
	//  +----+----+----+
	
	// z[column][row] = input values at 16 equally-spaced grid points
	// known[column][row] = true if value supplied for given input grid point
	// dens = number of grid intersections for interpolation between each known grid point
	// result[column*dens*4+row] = interpolated values at grid intersections
	// dx = distance between original grid points in horizontal direction
	// dy = distance between original grid points in vertical direction
	
	// Returns false if interpolation could not be done
	
	double dzdx[4]; // derivative : change in z for a given change in x
	double dzdy[4]; // derivative : change in z for a given change in y 
	double d2zdxdy[4]; // cross derivative
	double zz[4]; // value
	double zd;
	int j,k;
	double c[4][4];
	double x,y;
	
	// Corners of target grid cell MUST be known
	
	if (!(known[1][1] && known[1][2] && known[2][1] && known[2][2])) return false;
	
	// Compute derivatives for target grid cell
	// Press et al. p 98
	// order is : 0=BL; 1=BR; 2=TR; 3=TL
	
	if (known[0][1]) dzdx[0]=(z[2][1]-z[0][1])/(2.0*dx);
	else dzdx[0]=(z[2][1]-z[1][1])/dx;
	
	if (known[1][0]) dzdy[0]=(z[1][2]-z[1][0])/(2.0*dy);
	else dzdy[0]=(z[1][2]-z[1][1])/dy;
	
	if (known[2][0] && known[0][2] && known[0][0])
		d2zdxdy[0]=(z[2][2]-z[2][0]-z[0][2]+z[0][0])/(4.0*dx*dy);
	else
		d2zdxdy[0]=(z[2][2]-z[2][1]-z[1][2]+z[1][1])/(dx*dy);
	
	// -------------------
	
	if (known[3][1]) dzdx[1]=(z[3][1]-z[1][1])/(2.0*dx);
	else dzdx[1]=(z[2][1]-z[1][1])/dx;
	
	if (known[2][0]) dzdy[1]=(z[2][2]-z[2][0])/(2.0*dy);
	else dzdy[1]=(z[2][2]-z[2][1])/dy;
	
	if (known[3][2] && known[3][0] && known[1][0])
		d2zdxdy[1]=(z[3][2]-z[3][0]-z[1][2]+z[1][0])/(4.0*dx*dy);
	else
		d2zdxdy[1]=d2zdxdy[0];
		
	// -------------------
	
	if (known[3][2]) dzdx[2]=(z[3][2]-z[1][2])/(2.0*dx);
	else dzdx[2]=(z[2][2]-z[1][2])/dx;
	
	if (known[2][3]) dzdy[2]=(z[2][3]-z[2][1])/(2.0*dy);
	else dzdy[2]=(z[2][2]-z[2][1])/dy;
	
	if (known[3][3] && known[3][1] && known[1][3])
		d2zdxdy[2]=(z[3][3]-z[3][1]-z[1][3]+z[1][1])/(4.0*dx*dy);
	else
		d2zdxdy[2]=d2zdxdy[0];
		
	// -------------------
	
	if (known[0][2]) dzdx[3]=(z[2][2]-z[0][2])/(2.0*dx);
	else dzdx[3]=(z[2][2]-z[1][2])/dx;
	
	if (known[1][3]) dzdy[3]=(z[1][3]-z[1][1])/(2.0*dy);
	else dzdy[3]=(z[1][2]-z[1][1])/dy;
	
	if (known[2][3] && known[0][3] && known[0][1])
		d2zdxdy[3]=(z[2][3]-z[2][1]-z[0][3]+z[0][1])/(4.0*dx*dy);
	else
		d2zdxdy[3]=d2zdxdy[0];
	
	// Transfer values for target grid cell
	
	zz[0]=z[1][1];
	zz[1]=z[2][1];
	zz[2]=z[2][2];
	zz[3]=z[1][2];
	
	//printf("zz[0]=%g [1]=%g [2]=%g [3]=%g\n",zz[0],zz[1],zz[2],zz[3]);
	//for (j=0;j<4;j++) {
	//	printf("dzdx[%d]=%g dzdy[%d]=%g d2zdxdy[%d]=%g\n",j,dzdx[j],j,dzdy[j],j,d2zdxdy[j]);
	//}
	
		
	// Call bcucof
	
	bcucof(zz,dzdx,dzdy,d2zdxdy,dx,dy,c);

	// Call bcuint for each interpolation point in the central grid cell
	
	for (j=0;j<dens;j++) {
		
		x=dx/(double)dens*(j+0.5);
		
		for (k=0;k<dens;k++) {
		
			y=dy/(double)dens*(k+0.5);
			
			bcuint(dx,dy,x,y,c,zd);
			
			result[j*dens+k]=zd;
			
			//if (zd>100.0) printf("(%g,%g)=%g\n",x,y,zd);
		}
	}
	
	return true;
}