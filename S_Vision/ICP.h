#ifndef _EST_H
#define _EST_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <array>

#include <fstream>
#include <iostream>

using namespace std;

#define FALSE_0  0
#define TRUE_1   1

// ICP
#define PAR_icp_max_iter 			30
#define PAR_icp_dmax_match  			3.0 // [pixel]

// RANSAC
#define PAR_ransac_max_iter    			10
#define PAR_ransac_dmax_match 		       	3.0 // [pixel]

typedef struct
{
    int       lb1,
              ub1,
              lb2,
              ub2;
    char     *mat_sto;
    double  **el;
} dmat;

class CEst
{
public:

	dmat  newdmat (int rs, int re, int cs, int ce, int *error);
	// rs : row start
	// re : row end
	// cs : column start
	// ce : column end

	// Matix operations
	void print_mat (dmat mat);	
	int  matmul (dmat a, dmat b, dmat c);	//  Multiplication Matrix
	int  matcopy(dmat A, dmat RSLT);        //  Copy Matrix
	int  transpose (dmat A, dmat ATrans);   //  Transpose Matrix
	double matinvert (dmat a, dmat b);      //  Inverse Matrix  
	void Homogeneous_Transformation_Matrix(dmat XYZ_A, dmat XYZ_B, int PntNum,
			                       dmat Transformation_Mtx);
	
	// Singular Value Decomposition (SVD)
	double** mat_new_forRecipe(int nx, int ny);
	double*  vec_new_forRecipe(int n);
	void     mat_delete_forRecipe(double **m, int nx, int ny);
	void	 vec_delete_forRecipe(double *m);
	void	 svdcmp(double **a, int m, int n, double w[], double **v);

	// Standard ICP (Iterative Closest Point)
	void ICP_Point_to_Point(dmat Pnts1, int nOfPnts1, dmat Pnts2, int nOfPnts2,
			        dmat Init_T, int *nOfMatches, std::vector<std::array<int, 2>>& Matches, dmat Pose);
	void Brute_Force_Search(double *Query, dmat Candidates, int nOfCandidates, int *idx, double *dist);

	void Pose_Estimation_with_ICP(const std::vector<std::array<float, 2>>& ModelScan, int nOfModelScanPnts,
		const std::vector<std::array<float, 2>>& QueryScan, int nOfQueryScanPnts, dmat Init_T, dmat T);
};

#define freemat(m) free((m).mat_sto) ; free((m).el)

/* Singular Value Decomposition */
static double at,bt,ct;
static int iminarg1,iminarg2;
static double maxarg1, maxarg2;

#define pythag(a,b) ((at=fabs(a)) > (bt=fabs(b))?\
(ct=bt/at, at*sqrt(1.0 + ct*ct)) : (bt ? (ct=at/bt, bt*sqrt(1.0 + ct*ct)): 0.0))  // for the SVD
	
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))  // for the SVD	

#define FMAX(a,b) (maxarg1=(a), maxarg2=(b) , (maxarg1)>(maxarg2) ?\
(maxarg1): (maxarg2))  // for the SVD

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))  // for the SVD	

#endif
