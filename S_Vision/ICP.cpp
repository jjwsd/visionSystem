#include <include/Eigen/Core>
#include <include/Eigen/Geometry>

#include "ICP.h"

void CEst::Pose_Estimation_with_ICP(const std::vector<std::array<float, 2>>& ModelScan, int nOfModelScanPnts,
	const std::vector<std::array<float, 2>>& QueryScan, int nOfQueryScanPnts, dmat Init_T, dmat T)
{
	int i;

	//==============================================================//
	// Initialization
	//==============================================================//
	dmat ModelPnts = newdmat(0, nOfModelScanPnts - 1, 0, 2, &errno);
	dmat QueryPnts = newdmat(0, nOfQueryScanPnts - 1, 0, 2, &errno);

	for (i = 0; i < nOfModelScanPnts; i++)
	{
		ModelPnts.el[i][0] = (double)ModelScan[i][0];
		ModelPnts.el[i][1] = (double)ModelScan[i][1];
		// ModelPnts.el[i][2] = (double)ModelScan[i][2];
	}

	for (i = 0; i < nOfQueryScanPnts; i++)
	{
		QueryPnts.el[i][0] = (double)QueryScan[i][0];
		QueryPnts.el[i][1] = (double)QueryScan[i][1];
		// QueryPnts.el[i][2] = (double)QueryScan[i][2];
	}

	//==============================================================//
	// Perform ICP
	//==============================================================//
	std::vector<std::array<int, 2>> Matches(nOfModelScanPnts);
	int nOfMatches;

	ICP_Point_to_Point(QueryPnts, nOfQueryScanPnts, ModelPnts, nOfModelScanPnts, Init_T,
		&nOfMatches, Matches, T);


	//==============================================================//
	// Release memories
	//==============================================================//	
	freemat(ModelPnts);
	freemat(QueryPnts);
}

void CEst::ICP_Point_to_Point(dmat Pnts1, int nOfPnts1, dmat Pnts2, int nOfPnts2, dmat Init_T,
			      int *nOfMatches, std::vector<std::array<int, 2>>& Matches, dmat Pose)
{
	// INPUT
	// - Pnts1  : Set of stationary points
	// - Pnts2  : Set of moving points
	// - Init_T : Initial alignment of Pnts2 relative to Pnts1

	// OUTPUT
	// - TransformMat : Refined alignment after ICP iteration

	int i;
	int Iter;
	int max_Iter = (int)PAR_icp_max_iter;;

	dmat P1 = newdmat(0,2,0,0,&errno);
	dmat P2 = newdmat(0,2,0,0,&errno);
	dmat Pnts2_Buf      = newdmat(0,nOfPnts2,0,1,&errno);	
	dmat Pnts1_Matches  = newdmat(0,nOfPnts2,0,1,&errno);
	dmat Pnts2_Matches  = newdmat(0,nOfPnts2,0,1,&errno);
	dmat T      = newdmat(0,2,0,2,&errno);
	dmat prev_T = newdmat(0,2,0,2,&errno);

	//=================================================//
	// Initialization	
	//=================================================//
	for(i = 0; i < nOfPnts2; i++) // Moving points
	{
		P1.el[0][0] = Pnts2.el[i][0];	
		P1.el[1][0] = Pnts2.el[i][1];	
		P1.el[2][0] = 1.0;	

		matmul(Init_T, P1, P2);

		Pnts2_Buf.el[i][0] = P2.el[0][0];
		Pnts2_Buf.el[i][1] = P2.el[1][0];
	}

	//=================================================//
	// Iteration
	//=================================================//
	int idx;
	double dist, Q[2];

	Eigen::Matrix3d tf_prev, tf_curr;
	tf_prev << Init_T.el[0][0], Init_T.el[0][1], Init_T.el[0][2], 
		Init_T.el[1][0], Init_T.el[1][1], Init_T.el[1][2],
		Init_T.el[2][0], Init_T.el[2][1], Init_T.el[2][2];
	
	for(Iter = 0; Iter < max_Iter; Iter++)
	{
		//=================================================//
		// Find the nearest neighbor (Tentative correspondence)
		//=================================================//
		*nOfMatches = 0;

		for(i = 0; i < nOfPnts2; i++) // Moving points
		{
			Q[0] = Pnts2_Buf.el[i][0];
			Q[1] = Pnts2_Buf.el[i][1];
			Brute_Force_Search(Q, Pnts1, nOfPnts1, &idx, &dist);
			dist = sqrt(pow(Q[0] - Pnts1.el[idx][0], 2) + 
				    pow(Q[1] - Pnts1.el[idx][1], 2) );

			#ifdef PAR_icp_dmax_match
			if(dist > (double)PAR_icp_dmax_match)
			{
				continue;
			}
			#endif

			Pnts1_Matches.el[*nOfMatches][0] = Pnts1.el[idx][0]; 
			Pnts1_Matches.el[*nOfMatches][1] = Pnts1.el[idx][1]; 

			Pnts2_Matches.el[*nOfMatches][0] = Pnts2.el[i][0];
			Pnts2_Matches.el[*nOfMatches][1] = Pnts2.el[i][1];

			*nOfMatches = *nOfMatches + 1;		
		}

		if(*nOfMatches < 3)
		{
			printf("CEst::ICP_Point_to_Point\n");
			printf("The number of tentative matches is smaller than 3.\n");
			break;
		}

		//=================================================//
		// Compute transformation matrix
		//=================================================//
		Homogeneous_Transformation_Matrix(Pnts2_Matches, Pnts1_Matches, *nOfMatches, T);

		tf_curr << T.el[0][0], T.el[0][1], T.el[0][2], T.el[1][0], T.el[1][1], T.el[1][2], T.el[2][0], T.el[2][1], T.el[2][2];
		Eigen::Matrix3d delta_tf = tf_prev.inverse() * tf_curr;
		// Eigen::Rotation2Dd rot(delta_tf.topLeftCorner<2, 2>());
		Eigen::Rotation2Dd rot(atan2(delta_tf(1,0), delta_tf(0,0)));

		if (delta_tf.topRightCorner<2, 1>().norm() < 1e-6 && fabs(rot.angle()) < 1e-4)
			break;

		tf_prev = tf_curr;

		//=================================================//
		// Tranform the moving points
		//=================================================//
		for(i = 0; i < nOfPnts2; i++)
		{
			P1.el[0][0] = Pnts2.el[i][0];	
			P1.el[1][0] = Pnts2.el[i][1];	
			P1.el[2][0] = 1.0;	

			matmul(T, P1, P2);

			Pnts2_Buf.el[i][0] = P2.el[0][0];
			Pnts2_Buf.el[i][1] = P2.el[1][0];
		}
	}

	// //=================================================//
	// // Find the nearest neighbor (Final correspondence)
	// //=================================================//
	// *nOfMatches = 0;
	// double sum  = 0.0;

	// for(i = 0; i < nOfPnts2; i++)
	// {
	// 	Q[0] = Pnts2_Buf.el[i][0];
	// 	Q[1] = Pnts2_Buf.el[i][1];
	// 	Brute_Force_Search(Q, Pnts1, nOfPnts1, &idx, &dist);
	// 	dist = sqrt(pow(Q[0] - Pnts1.el[idx][0], 2) + 
	// 		    pow(Q[1] - Pnts1.el[idx][1], 2) );

	// 	#ifdef PAR_icp_dmax_match
	// 	if(dist > (double)PAR_icp_dmax_match)
	// 	{
	// 		continue;
	// 	}
	// 	#endif	

	// 	Pnts1_Matches.el[*nOfMatches][0] = Pnts1.el[idx][0]; 
	// 	Pnts1_Matches.el[*nOfMatches][1] = Pnts1.el[idx][1]; 

	// 	Pnts2_Matches.el[*nOfMatches][0] = Pnts2.el[i][0];
	// 	Pnts2_Matches.el[*nOfMatches][1] = Pnts2.el[i][1];

	// 	// Index of correspondence
	// 	Matches[*nOfMatches][0] = idx; // Stationary points
	// 	Matches[*nOfMatches][1] = i;   // Moving points

	// 	*nOfMatches = *nOfMatches + 1;		
		
	// 	sum = sum + dist;
	// }

	// //=================================================//
	// // Compute transformation matrix
	// // - T
	// //=================================================//
	// Homogeneous_Transformation_Matrix(Pnts2_Matches, Pnts1_Matches, *nOfMatches, T);
	
	//=================================================//
	// OUTPUT
	//=================================================//
	matcopy(T,Pose);

	//=================================================//
	// Release memories
	//=================================================//
	freemat(P1);
	freemat(P2);
	freemat(T);
	freemat(prev_T);
	freemat(Pnts2_Buf);
	freemat(Pnts1_Matches);
	freemat(Pnts2_Matches);
}

void CEst::Brute_Force_Search(double *Query, dmat Candidates, int nOfCandidates, int *idx, double *dist)
{
	int i, idx_;
	double x1, y1;
	double x2, y2;
	double min_dist, dist_;

	x1 = Query[0];            y1 = Query[1];            
	x2 = Candidates.el[0][0]; y2 = Candidates.el[0][1]; 

	min_dist = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
	idx_     = 0;
	
	for(i = 1; i < nOfCandidates; i++)
	{		
		x2    = Candidates.el[i][0];
		y2    = Candidates.el[i][1]; 
		dist_ = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);

		if(dist_ < min_dist)
		{
			min_dist = dist_;
			idx_     = i;
		}
	}

	// OUTPUT
	*dist = sqrt(min_dist);
	*idx  = idx_;
}

//========================================================================================================//
//========================================================================================================//

/*
   Allocates and initializes memory for a double precision Iliffe matrix.
*/
dmat CEst::newdmat (int rs, int re, int cs, int ce, int *error)
{
    double   *p,
            **b;

    int       r,
              rows,
              cols;

    dmat      matrix;

    rows = re - rs + 1;
    cols = ce - cs + 1;
    if (rows <= 0 || cols <= 0) {
	errno = EDOM;
	*error = -1;
	return (matrix);
    }

    /* fill in the bounds for this matrix */
    matrix.lb1 = rs;
    matrix.ub1 = re;
    matrix.lb2 = cs;
    matrix.ub2 = ce;

    /* allocate memory for the row pointers */
    b = (double **) calloc ((unsigned int) rows, (unsigned int) sizeof (double *));
    if (b == 0) {
	errno = ENOMEM;
	*error = -1;
	return (matrix);
    }

    /* adjust for non-zero lower index bounds */
    matrix.el = b -= rs;

    /* allocate memory for storing the actual matrix */
    p = (double *) calloc ((unsigned int) rows * cols, (unsigned int) sizeof (double));
    if (p == 0) {
	errno = ENOMEM;
	*error = -1;
	return (matrix);
    }

    /* keep a reminder where the block is actually located */
    matrix.mat_sto = (char *) p;

    /* adjust for non-zero lower index bounds */
    p -= cs;

    /* fabricate row pointers into the matrix */
    for (r = rs; r <= re; r++) {
	b[r] = p;
	p += cols;
    }

    *error = 0;
    return (matrix);
}

void CEst::Homogeneous_Transformation_Matrix(dmat XYZ_A,dmat XYZ_B,int PntNum,
					     dmat Transformation_Mtx)
{
	// OUTPUT
	// - B_T_A : Coordinates system {A} defined with respect to {B}

	int i,j,k;
	double sum_x;
	double sum_y;
	dmat Rotation_Mtx;
	dmat Translation_Vec;
	dmat A_mean_position_vec;
	dmat B_mean_position_vec;
	dmat A_relative_position_vec;
	dmat B_relative_position_vec;
	dmat Correlation_Mtx;
	dmat SUM;
	dmat temp_mtx;
	dmat temp_mtx1;
	dmat temp_vec;
	dmat temp_vec1;
	dmat temp_vec_trans;
	
	Rotation_Mtx    = newdmat(0,1,0,1,&errno);
	Translation_Vec	= newdmat(0,1,0,0,&errno);
		
	A_mean_position_vec = newdmat(0,1,0,0,&errno);
	B_mean_position_vec = newdmat(0,1,0,0,&errno);
	
	A_relative_position_vec = newdmat(0,PntNum-1,0,1,&errno);
	B_relative_position_vec = newdmat(0,PntNum-1,0,1,&errno);

	Correlation_Mtx = newdmat(0,1,0,1,&errno);
	SUM = newdmat(0,1,0,1,&errno);	
	temp_mtx  = newdmat(0,1,0,1,&errno);	
	temp_mtx1 = newdmat(0,1,0,1,&errno);	
	temp_vec  = newdmat(0,0,0,1,&errno);	
	temp_vec1 = newdmat(0,1,0,0,&errno);	
	temp_vec_trans = newdmat(0,1,0,0,&errno);
	
	/* Compute mean position vector of the points A */
	sum_x = 0.0;
	sum_y = 0.0;

	for(i = 0; i < PntNum; i++)
	{
		sum_x = sum_x + XYZ_A.el[i][0];	
		sum_y = sum_y + XYZ_A.el[i][1];
	}

	A_mean_position_vec.el[0][0] = sum_x/double(PntNum);
	A_mean_position_vec.el[1][0] = sum_y/double(PntNum);
	
	/* Compute mean position vector of the points B */
	sum_x = 0.0;
	sum_y = 0.0;

	for(i = 0; i < PntNum; i++)
	{
		sum_x = sum_x + XYZ_B.el[i][0];	
		sum_y = sum_y + XYZ_B.el[i][1];
	}

	B_mean_position_vec.el[0][0] = sum_x/double(PntNum);
	B_mean_position_vec.el[1][0] = sum_y/double(PntNum);

	/* Compute relative position vector w.r.t the mean position vector */
	for(i = 0; i < PntNum; i++)
	{
		A_relative_position_vec.el[i][0] = XYZ_A.el[i][0] - A_mean_position_vec.el[0][0];
		A_relative_position_vec.el[i][1] = XYZ_A.el[i][1] - A_mean_position_vec.el[1][0];

		B_relative_position_vec.el[i][0] = XYZ_B.el[i][0] - B_mean_position_vec.el[0][0];
		B_relative_position_vec.el[i][1] = XYZ_B.el[i][1] - B_mean_position_vec.el[1][0];
	}
	
	/* Compute correlation matrix */
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 2; j++)
		{
			SUM.el[i][j] = 0.0;
		}
	}

	for(i = 0; i < PntNum; i++)
	{
		temp_vec.el[0][0] = A_relative_position_vec.el[i][0];		
		temp_vec.el[0][1] = A_relative_position_vec.el[i][1];
	
		temp_vec_trans.el[0][0] = B_relative_position_vec.el[i][0];		
		temp_vec_trans.el[1][0] = B_relative_position_vec.el[i][1];
		
		matmul(temp_vec_trans,temp_vec,temp_mtx); // 2*2 matrix

		for(j = 0; j < 2; j++)
		{
			for(k = 0; k < 2; k++)
			{
				SUM.el[j][k] = SUM.el[j][k] + temp_mtx.el[j][k];
			}
		}
	}
	
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 2; j++)
		{
			Correlation_Mtx.el[i][j] = SUM.el[i][j]/double(PntNum);
		}
	}

	/* Compute rotation matrix */
	double **L, *eig_val, **eig_vec;
	double determinant;
	dmat U;
	dmat V;
	dmat V_Trans;
	
	L       = mat_new_forRecipe(2,2);
	eig_val = vec_new_forRecipe(2);   
	eig_vec = mat_new_forRecipe(2,2);

	U       = newdmat(0,1,0,1,&errno);	
	V       = newdmat(0,1,0,1,&errno);		
	V_Trans = newdmat(0,1,0,1,&errno);	
	
	for(i = 1; i <= 2; i++)
	{
		for(j = 1; j <= 2; j++)
		{
			L[i][j] = Correlation_Mtx.el[i-1][j-1];
		}
	}
	
	svdcmp(L,2,2,eig_val,eig_vec);  // Singular Value Decomposition	

	for(i = 1; i <= 2; i++)
	{
		for(j = 1; j <= 2; j++)
		{
			U.el[i-1][j-1] = L[i][j];	// Right eigen vector
			V.el[i-1][j-1] = eig_vec[i][j];	// Left eigen vector
		}
	}
	
	transpose(V, V_Trans);
	matmul(U,V_Trans,temp_mtx);
	determinant = matinvert(temp_mtx,temp_mtx1);
	
	temp_mtx.el[0][0] = 1.0;
	temp_mtx.el[0][1] = 0.0;
	
	temp_mtx.el[1][0] = 0.0;
	temp_mtx.el[1][1] = determinant;
	
	matmul(U,temp_mtx,temp_mtx1);
	matmul(temp_mtx1,V_Trans,Rotation_Mtx);  // Rotation matrix

	/* Compute translation vector */
	matmul(Rotation_Mtx,A_mean_position_vec,temp_vec1);  
	
	Translation_Vec.el[0][0] = B_mean_position_vec.el[0][0] - temp_vec1.el[0][0];	
	Translation_Vec.el[1][0] = B_mean_position_vec.el[1][0] - temp_vec1.el[1][0];
	
	/* Transformation matrix */
	/* B_T_A		 */
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 2; j++)
		{
			Transformation_Mtx.el[i][j] = Rotation_Mtx.el[i][j];
		}
	
		Transformation_Mtx.el[i][2] = Translation_Vec.el[i][0];
		Transformation_Mtx.el[2][i] = 0.0;
	}
	Transformation_Mtx.el[2][2] = 1.0;
	
	freemat(Rotation_Mtx);
	freemat(Translation_Vec);
	freemat(A_mean_position_vec);
	freemat(B_mean_position_vec);
	freemat(A_relative_position_vec);
	freemat(B_relative_position_vec);
	freemat(Correlation_Mtx);
	freemat(SUM);
	freemat(U);
	freemat(V);
	freemat(V_Trans);
	freemat(temp_vec);
	freemat(temp_vec1);
	freemat(temp_vec_trans);
	freemat(temp_mtx);
	freemat(temp_mtx1);
	mat_delete_forRecipe(L,2,2);
	mat_delete_forRecipe(eig_vec,2,2);
	vec_delete_forRecipe(eig_val);
}

int CEst::matmul (dmat a, dmat b, dmat c)
{
    int       i,
              j,
              k,
              broff,
              croff,
              ccoff,		/* coordinate origin offsets */
              mem_alloced,
              error;

    double    t;

    dmat      d;		/* temporary workspace matrix */

    if (a.ub2 - a.lb2 != b.ub1 - b.lb1
	|| a.ub1 - a.lb1 != c.ub1 - c.lb1
	|| b.ub2 - b.lb2 != c.ub2 - c.lb2) {
	errno = EDOM;
	return (-1);
    }

    if (a.mat_sto != c.mat_sto && b.mat_sto != c.mat_sto) {
	d = c;
	mem_alloced = FALSE_0;
    } else {
	d = newdmat (c.lb1, c.ub1, c.lb2, c.ub2, &error);
	if (error) {
	    fprintf (stderr, "Matmul: out of storage.\n");
	    errno = ENOMEM;
	    return (-1);
	}
	mem_alloced = TRUE_1;
    }

    broff = b.lb1 - a.lb2;	/* B's row offset from A */
    croff = c.lb1 - a.lb1;	/* C's row offset from A */
    ccoff = c.lb2 - b.lb2;	/* C's column offset from B */
    for (i = a.lb1; i <= a.ub1; i++)
	for (j = b.lb2; j <= b.ub2; j++) {
	    t = 0.0;
	    for (k = a.lb2; k <= a.ub2; k++)
		t += a.el[i][k] * b.el[k + broff][j];
	    d.el[i + croff][j + ccoff] = t;
	}

    if (mem_alloced) {
	for (i = c.lb1; i <= c.ub1; i++)
	    for (j = c.lb2; j <= c.ub2; j++)
		c.el[i][j] = d.el[i][j];
	freemat (d);
    }

    return (0);
}


/*
	AUHTHER: Soonyong Park
	DISCRIPTION: matrix memory allocation for the singular value decomposition
*/
double** CEst::mat_new_forRecipe(int nx, int ny)
{
	int i;	  	         //  ------> x
	double **m;		 //	 |
  	m = new double *[ny+1];	 //	 |
				 //	 y
	
	for (i=0; i < ny+1; i++)
	{
		m[i] = new double[nx+1];
	}
	
	return m;
}

void CEst::mat_delete_forRecipe(double **m, int nx, int ny)
{
	int i;

	for (i=0; i < ny+1; i++)
	{
		delete [] m[i];
	}
	delete [] m;
}

/*
	AUHTHER: Soonyong Park
	DISCRIPTION: vector memory allocation for the singular value decomposition
*/
double* CEst::vec_new_forRecipe(int n)
{
	double *m;		 //	 |
  	m = new double[n+1];	 //	 |

	return m;
}

void CEst::vec_delete_forRecipe(double *m)
{
	delete [] m;
}

/*
	AUHTHER: Soonyong Park
	DISCRIPTION: singular value decomposition
*/
void CEst::svdcmp(double **a, int m, int n, double w[], double **v)
{
	// 입력 //		
	// a : SVD할 matrix
	// m : SVD할 matrix의 행 개수
	// n : SVD할 matrix의 열 개수

	// 출력 //
	// w : eigen value
	// v : eigen vector

	// SVD의 계산
	// Recipe의 original Source를 그대로 이용 (i=1서부터 시작)
	// w(eigen values)와 v(eigen vectors)는 외부에서 memory allocation
	// "pythag"함수 필요함
	// A = U*W*Vt
	// "eigstr"을 이용하여 W(singular value)와 Vt를 큰순서에서 작은 sorting할 수 있음
	
	int flag,i,its,j,jj,k,l,nm;
	double anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1 = vec_new_forRecipe(n);
	g = scale = anorm = 0.0;

	for(i=1; i<=n; i++)
	{
		l = i+1;
		rv1[i] = scale*g;
		g = s = scale = 0.0;

		if(i <= m)
		{
			for(k=i; k<=m; k++)
			{
				scale += (double)fabs(a[k][i]);
			}
			
			if(scale)
			{
				for(k=i; k<=m; k++)
				{
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}

				f = a[i][i];
				g = -(double)SIGN(sqrt(s),f);
				h = f*g-s;
				a[i][i] = f-g;

				for(j=l; j<=n; j++)
				{
					for(s=0.0,k=i; k<=m; k++)
					{
						s += a[k][i]*a[k][j];
					}
					
					f=s/h;
					
					for(k=i; k<=m; k++)
					{
						a[k][j] += f*a[k][i];
					}
				}

				for(k=i; k<=m; k++)
				{
					a[k][i] *= scale;
				}
			}
		}

		w[i] = scale *g;
		g = s = scale = 0.0;

		if(i <= m && i != n)
		{
			for(k=l; k<=n; k++)
			{
				scale += (double)fabs(a[i][k]);
			}

			if(scale)
			{
				for(k=l; k<=n; k++)
				{
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				
				f = a[i][l];
				g = -(double)SIGN(sqrt(s),f);
				h = f*g-s;
				a[i][l] = f-g;
				
				for(k=l; k<=n; k++)
				{
					rv1[k] = a[i][k]/h;
				}

				for(j=l; j<=m; j++)
				{
					for(s=0.0,k=l; k<=n; k++)
					{
						s += a[j][k]*a[i][k];
					}

					for(k=l; k<=n; k++)
					{
						a[j][k] += s*rv1[k];
					}
				}

				for(k=l; k<=n; k++)
				{
					a[i][k] *= scale;
				}
			}
		}
		anorm=(double)FMAX(anorm,((double)fabs(w[i])+(double)fabs(rv1[i])));
	}

	for(i=n; i>=1; i--)
	{
		if(i < n)
		{
			if(g)
			{
				for(j=l; j<=n; j++)
				{
					v[j][i] = (a[i][j]/a[i][l])/g;
				}

				for(j=l; j<=n; j++)
				{
					for(s=0.0,k=l; k<=n; k++)
					{
						s += a[i][k]*v[k][j];
					}

					for(k=l; k<=n; k++)
					{
						v[k][j] += s*v[k][i];
					}
				}
			}

			for(j=l; j<=n; j++)
			{
				v[i][j] = v[j][i] = 0.0;
			}
		}

		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	for (i = IMIN(m,n); i>=1; i--)
	{
		l = i+1;
		g = w[i];

		for(j=l; j<=n; j++)
		{
			a[i][j] = 0.0;
		}

		if(g)
		{
			g = 1.0f/g;

			for(j=l; j<=n; j++)
			{
				for(s=0.0,k=l; k<=m; k++)
				{
					s += a[k][i]*a[k][j];
				}

				f = (s/a[i][i])*g;

				for(k=i; k<=m; k++)
				{
					a[k][j] += f*a[k][i];
				}
			}

			for(j=i; j<=m; j++)
			{
				a[j][i] *= g;
			}
		} 
		else
		{
			for (j=i;j<=m;j++) a[j][i]=0.0;
		}

		++a[i][i];
	}

	for(k=n; k>=1; k--)
	{
		for(its=1; its<=30; its++)
		{
			flag = 1;
			for(l=k; l>=1; l--)
			{
				nm = l-1;
				if ((double)(fabs(rv1[l])+anorm) == anorm)
				{
					flag=0;
					break;
				}

				if ((double)(fabs(w[nm])+anorm) == anorm) 
				{
					break;
				}
			}

			if(flag)
			{
				c = 0.0;
				s = 1.0;

				for(i=l; i<=k; i++)
				{
					f = s*rv1[i];
					rv1[i] = c*rv1[i];

					if ((double)(fabs(f)+anorm) == anorm) 
					{
						break;
					}

					g = w[i];
					h = pythag(f,g);
					w[i] = h;
					h = 1.0f/h;
					c = g*h;
					s = -f*h;

					for(j=1; j<=m; j++)
					{
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y*c+z*s;
						a[j][i] = z*c-y*s;
					}
				}
			}

			z = w[k];

			if(l == k)
			{
				if(z < 0.0)
				{
					w[k] = -z;

					for(j=1; j<=n; j++)
					{
						v[j][k] = -v[j][k];
					}
				}
				break;
			}

			if(its == 30) 
			{
				break; //AfxMessageBox("no convergence in 30 svdcmp iterations", MB_OK);
			}

			x = w[l];
			nm = k-1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0f*h*y);
			g = pythag(f,1.0);
			f = ((x-z)*(x+z)+h*((y/(f+(double)SIGN(g,f)))-h))/x;
			c = s = 1.0;

			for(j=l; j<=nm; j++)
			{
				i = j+1;
				g = rv1[i];
				y = w[i];
				h = s*g;
				g = c*g;
				z = pythag(f,h);
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x*c+g*s;
				g = g*c-x*s;
				h = y*s;
				y *= c;

				for(jj=1; jj<=n; jj++)
				{
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x*c+z*s;
					v[jj][i] = z*c-x*s;
				}

				z = pythag(f,h);
				w[j] = z;

				if(z)
				{
					z = 1.0f/z;
					c = f*z;
				    s = h*z;
				}

				f = c*g+s*y;
				x = c*y-s*g;

				for(jj=1; jj<=m; jj++)
				{
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y*c+z*s;
					a[jj][i] = z*c-y*s;
				}
			}

			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}

	delete [] rv1;
}


/*
   Generate the transpose of a dynamic Iliffe matrix.
*/
int CEst::transpose (dmat A, dmat ATrans)
{
    int       i,
              j,
              rowsize,
              colsize,
              error;

    double  **a = A.el,
            **atrans = ATrans.el,
              temp;

    dmat      TMP;

    rowsize = A.ub1 - A.lb1;
    colsize = A.ub2 - A.lb2;
    if (rowsize < 0 || rowsize != ATrans.ub2 - ATrans.lb2 ||
	colsize < 0 || colsize != ATrans.ub1 - ATrans.lb1) {
	errno = EDOM;
	return (-1);
    }

    if (A.mat_sto == ATrans.mat_sto
	&& A.lb1 == ATrans.lb1
	&& A.lb2 == ATrans.lb2) {
	for (i = 0; i <= rowsize; i++)
	    for (j = i + 1; j <= colsize; j++) {
		temp = a[A.lb1 + i][A.lb2 + j];
		atrans[A.lb1 + i][A.lb2 + j] = a[A.lb1 + j][A.lb2 + i];
		atrans[A.lb1 + j][A.lb2 + i] = temp;
	    }
    } else if (A.mat_sto == ATrans.mat_sto) {
	TMP = newdmat (ATrans.lb1, ATrans.ub1, ATrans.lb2, ATrans.ub2, &error);
	if (error)
	    return (-2);

	for (i = 0; i <= rowsize; i++)
	    for (j = 0; j <= colsize; j++) {
		TMP.el[ATrans.lb1 + j][ATrans.lb2 + i] =
		 a[A.lb1 + i][A.lb2 + j];
	    }

	matcopy (TMP, ATrans);
	freemat (TMP);
    } else {
	for (i = 0; i <= rowsize; i++)
	    for (j = 0; j <= colsize; j++)
		atrans[ATrans.lb1 + j][ATrans.lb2 + i]
		 = a[A.lb1 + i][A.lb2 + j];
    }

    return (0);
}

/*
   Copy dynamic Iliffe matrix A into RSLT.
   Bounds need not be the same but the dimensions must be.
*/
int CEst::matcopy (dmat A, dmat RSLT)
{
    int       i,
              j,
              rowsize,
              colsize;

    double  **a = A.el,
            **rslt = RSLT.el;

    rowsize = A.ub1 - A.lb1;
    colsize = A.ub2 - A.lb2;
    if (rowsize != RSLT.ub1 - RSLT.lb1 || colsize != RSLT.ub2 - RSLT.lb2) {
	errno = EDOM;
	return (-1);
    }

    for (i = 0; i <= rowsize; i++)
	for (j = 0; j <= colsize; j++)
	    rslt[RSLT.lb1 + i][RSLT.lb2 + j] = a[A.lb1 + i][A.lb2 + j];

    return (0);
}

/*
   Print an Iliffe matrix out to stdout.
*/
void CEst::print_mat (dmat mat)
{
    int       i,j;

    fprintf (stdout, "         ");
    for (i = mat.lb2; i <= mat.ub2; i++)
	fprintf (stdout, "  %7d", i);
    fprintf (stdout, "\n");

    for (j = mat.lb1; j <= mat.ub1; j++) {
	fprintf (stdout, "  %7d", j);
	for (i = mat.lb2; i <= mat.ub2; i++)
	    fprintf (stdout, "  %7.2lf", mat.el[j][i]);
	fprintf (stdout, "\n");
    }
}

/*
   In-place Iliffe matrix inversion using full pivoting.
   The standard Gauss-Jordan method is used.
   The return values are the determinant and the b = inverse(a).
*/

#define PERMBUFSIZE 100		/* Mat bigger than this requires calling calloc. */

double CEst::matinvert (dmat a, dmat b)
{
    int       i,
              j,
              k,
             *l,
             *m, 
              permbuf[2 * PERMBUFSIZE],
              mem_alloced;

    double    det,
              biga,
              hold;

	dmat      c;                                 // added by PSY@KIST

	c = newdmat(a.lb1,a.ub1,a.lb2,a.ub2,&errno); // added by PSY@KIST
	matcopy(a,c);						         // added by PSY@KIST

    if (a.lb1 != a.lb2 || a.ub1 != a.ub2) {
	errno = EDOM;
	return (0.0);
    }

    /* Allocate permutation vectors for l and m, with the same origin as the matrix. */
    if (a.ub1 - a.lb1 + 1 <= PERMBUFSIZE) {
	l = permbuf;
	mem_alloced = FALSE_0;
    } else {
	l = (int *) calloc ((unsigned int) 2 * (a.ub1 - a.lb1 + 1), (unsigned int) sizeof (int));
	if (l == 0) {
	    fprintf (stderr, "matinvert: can't get working storage.\n");
	    errno = ENOMEM;
	    return (0.0);
	}
	mem_alloced = TRUE_1;
    }

    l -= a.lb1;
    m = l + (a.ub1 - a.lb1 + 1);

    det = 1.0;
    for (k = a.lb1; k <= a.ub1; k++) {
	l[k] = k;
	m[k] = k;
	biga = a.el[k][k];

	/* Find the biggest element in the submatrix */
	for (i = k; i <= a.ub1; i++)
	    for (j = k; j <= a.ub2; j++)
		if (fabs (a.el[i][j]) > fabs (biga)) {
		    biga = a.el[i][j];
		    l[k] = i;
		    m[k] = j;
		}

	/* Interchange rows */
	i = l[k];
	if (i > k)
	    for (j = a.lb2; j <= a.ub2; j++) {
		hold = -a.el[k][j];
		a.el[k][j] = a.el[i][j];
		a.el[i][j] = hold;
	    }

	/* Interchange columns */
	j = m[k];
	if (j > k)
	    for (i = a.lb1; i <= a.ub1; i++) {
		hold = -a.el[i][k];
		a.el[i][k] = a.el[i][j];
		a.el[i][j] = hold;
	    }

	/* Divide column by minus pivot (value of pivot element is contained in biga). */
	if (biga == 0.0)
	    return (0.0);

	for (i = a.lb1; i <= a.ub1; i++)
	    if (i != k)
		a.el[i][k] /= -biga;

	/* Reduce matrix */
	for (i = a.lb1; i <= a.ub1; i++)
	    if (i != k) {
		hold = a.el[i][k];
		for (j = a.lb2; j <= a.ub2; j++)
		    if (j != k)
			a.el[i][j] += hold * a.el[k][j];
	    }

	/* Divide row by pivot */
	for (j = a.lb2; j <= a.ub2; j++)
	    if (j != k)
		a.el[k][j] /= biga;

	det *= biga;		/* Product of pivots */
	a.el[k][k] = 1.0 / biga;
    }				/* K loop */

    /* Final row & column interchanges */
    for (k = a.ub1 - 1; k >= a.lb1; k--) {
	i = l[k];
	if (i > k)
	    for (j = a.lb2; j <= a.ub2; j++) {
		hold = a.el[j][k];
		a.el[j][k] = -a.el[j][i];
		a.el[j][i] = hold;
	    }

	j = m[k];
	if (j > k)
	    for (i = a.lb1; i <= a.ub1; i++) {
		hold = a.el[k][i];
		a.el[k][i] = -a.el[j][i];
		a.el[j][i] = hold;
	    }
    }

    if (mem_alloced)
	free (l + a.lb1);

	matcopy(a,b); // added by PSY@KIST
	matcopy(c,a); // added by PSY@KIST

    return det; // return determinant
}
