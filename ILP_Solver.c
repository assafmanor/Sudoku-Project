#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "gurobi_c.h"


unsigned int get_index(unsigned int N, unsigned int r, unsigned int c, unsigned int v){
	unsigned int res = (r*N*N) + (c*N) + v;
	return res;
}


void setValues(Board* boardPtr, unsigned int* valuesArr) {
	unsigned int row;
	unsigned int col;
	int val;
	unsigned int N = boardPtr->m * boardPtr->n;
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			val = getCell(boardPtr,row,col)->value-1;
			if(val > -1) { /* cell is not empty */
				valuesArr[get_index(N,row,col,val)] = 1;
			}
		}
	}
}


/* This example formulates and solves the following simple MIP model:

     maximize    x +   y + 2 z
     subject to  x + 2 y + 3 z <= 4
                 x +   y       >= 1
     x, y, z binary
*/
int IlpSolver(Board* boardPtr) {
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  double*    sol;
  int*       ind;
  double*    val;
  char*      vtype;
  int       optimstatus;
  double    objval;

  unsigned int*	valuesArr;
  unsigned int	m = boardPtr->m;
  unsigned int	n = boardPtr->n;
  unsigned int	N = m*n;
  unsigned int	N3 = N*N*N;
  unsigned int	r,c,v,index;
  unsigned		block;
  unsigned int	block_r, block_c;


  valuesArr = (unsigned int*)calloc(N*N*N ,sizeof(unsigned int)); /* TODO: free at the end */
  setValues(boardPtr,valuesArr);



/*--------- step1: create environment ------------------*/
  /* Create environment - log file is mip1.log */
  error = GRBloadenv(&env, "mip1.log");
  if (error) {
	  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Create an empty model named "mip1" */
  error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
  if (error) {
	  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

/*--------- step2: Objective function ------------------*/
  /* Add variables */
/* TODO: check malloc/calloc failure */

  /* variable types - for x,y,z (cells 0,1,2 in "vtype") */
  vtype = (char*)malloc(N3*sizeof(char));
  /* Initialize coefficients for the constraints */
  val = (double*)malloc(N*sizeof(double));

  for(r=0; r < N; r++) {
  		for(c = 0; c < N; c++) {
  			for(v = 0; v < N; v++){
  				index = get_index(N,r,c,v);
				vtype[index]= GRB_BINARY;
  			}
  		}
  		val[r] = 1;
  }

  /* add variables to model */
  error = GRBaddvars(model, N3, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);
  if (error) {
	  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }


  /* Change objective sense to maximization */
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* update the model - to integrate new variables */
  error = GRBupdatemodel(model);
  if (error) {
	  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /*--------- step3: init vals ------------------*/
  /* indices for the constraints */
  ind = (int*)malloc(N*sizeof(int));


  /* --------- step4: constraints ------------------ */
  /* type A: each cell has exactly  1 value */
  for(r = 0; r < N; r++) {
	  for(c = 0; c < N; c++) {
		  for(v = 0; v < N; v++) {
			  index = get_index(N,r,c,v);
			  ind[v] = index;
		  }
		  /* Add constraint:*/
		  error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, NULL);
		    if (error) {
		  	  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
		  	  return -1;
		    }
	  }
  }

  /* type B: each column has exactly one of each value contained within it. */
  for(v = 0; v < N; v++) {
	  for(c = 0; c < N; c++) {
		  for(r = 0; r < N; r++) {
			  index = get_index(N,r,c,v);
			  ind[r] = index;
		  }
		  /* Add constraint:*/
		  error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, NULL);
		    if (error) {
		  	  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
		  	  return -1;
		    }
	  }
  }

  /* type C: each row has exactly one of each value contained within it. */
  for(v = 0; v < N; v++) {
	  for(r = 0; r < N; r++) {
		  for(c = 0; c < N; c++) {
			  index = get_index(N,r,c,v);
			  ind[c] = index;
		  }
		  /* Add constraint:*/
		  error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, NULL);
		    if (error) {
		  	  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
		  	  return -1;
		    }
	  }
  }


  /* type D: each block has exactly one of each value contained within it. */
  for(v = 0; v < N; v++) {
	  for(block = 0; block < N; block++) {
		  /* block's top-left cell's coordinations */
		  r = (block/m)*m;
		  c = (block%m)*n;
		  for(block_r = 0; block_r < n; block_r++) {
			  for(block_c = 0; block_c < m; block_c++) {
				  index = get_index(N, r+block_r, c+block_c, v);
				  ind[c] = index;
			  }
		  }
		  /* Add constraint:*/
		  error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, NULL);
		    if (error) {
		  	  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
		  	  return -1;
		    }
	  }
  }



  /*error = GRBaddconstr(model, 3, ind, val, GRB_LESS_EQUAL, 4.0, "c0");
  if (error) {
	  printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }*/

  /* add constraint to model - note size 3 + operator GRB_LESS_EQUAL */
  /* -- equation value (4.0) + unique constraint name */
  /*error = GRBaddconstr(model, 3, ind, val, GRB_LESS_EQUAL, 4.0, "c0");
  if (error) {
	  printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }*/

  /* Second constraint: x + y >= 1 */
  ind[0] = 0; ind[1] = 1;
  val[0] = 1; val[1] = 1;

  /* add constraint to model - note size 2 + operator GRB_GREATER_EQUAL */
  /* -- equation value (1.0) + unique constraint name */
  error = GRBaddconstr(model, 2, ind, val, GRB_GREATER_EQUAL, 1.0, "c1");
  if (error) {
	  printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Optimize model - need to call this before calculation */
  error = GRBoptimize(model);
  if (error) {
	  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Write model to 'mip1.lp' - this is not necessary but very helpful */
  error = GRBwrite(model, "mip1.lp");
  if (error) {
	  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Get solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) {
	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* get the objective -- the optimal result of the function */
  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) {
	  printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* get the solution - the assignment to each variable */
  /* 3-- number of variables, the size of "sol" should match */
  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, 3, sol);
  if (error) {
	  printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* print results */
  printf("\nOptimization complete\n");

  /* solution found */
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);
    printf("  x=%.0f, y=%.0f, z=%.0f\n", sol[0], sol[1], sol[2]);
  }
  /* no solution found */
  else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Model is infeasible or unbounded\n");
  }
  /* error or calculation stopped */
  else {
    printf("Optimization was stopped early\n");
  }

  /* IMPORTANT !!! - Free model and environment */
  GRBfreemodel(model);
  GRBfreeenv(env);

  return 0;
}
