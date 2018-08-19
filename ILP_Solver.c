#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "gurobi_c.h"

/*
 * return the index of cell with row r, column c and value v
 *
 * unsigned int	N	-	number of rows/columns.
 * unsigned int	row	-	Row number (between 0 and N-1).
 * unsigned int col	-	Column number (between 0 and N-1).
 * unsigned int	val	-	cell value (between 0 and N-1).
 */
unsigned int get_index(unsigned int N, unsigned int row, unsigned int col, unsigned int val){
	unsigned int res = (row*N*N) + (col*N) + val;
	return res;
}

/* Used for adding constraints for non-empty cells:
 * Iterate on all N^2 cells, and for each non-empty cell(row,col) (value between 1 and N) with a value of val -
 * set valuesArr[get_index(N,row,col,val)] to TRUE.
 *
 * Board*			boardPtr		-	A pointer a game board.
 * unsigned int* 	valuesArr		-	An array that indicates cell values for non-empty cells.
 * 										(if getCell(row,col)->value == val, then valuesArr[get_index(N,row,col,val-1)] = TRUE)
 */
void setValues(Board* boardPtr, unsigned int* valuesArr) {
	unsigned int row;
	unsigned int col;
	int val;
	unsigned int N = boardPtr->m * boardPtr->n;
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			val = getCell(boardPtr,row,col)->value-1;
			if(val > -1) { /* cell is not empty */
				valuesArr[get_index(N,row,col,val)] = TRUE;
			}
		}
	}
}



unsigned int createGurobiEnvironment(GRBenv *env, GRBmodel **model){
	unsigned int error;
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	GRBsetintparam(GRBgetenv(*model),"OutputFlag",0);
	return FALSE; /* No errors */
}



unsigned int initObjectiveFunction(GRBenv *env, GRBmodel **model, unsigned int N, char* vtype){
	unsigned int	N3 = N*N*N;
	unsigned int	r,c,v;
	unsigned int	index;
	unsigned int	error;

	for(r=0; r < N; r++) {
		for(c = 0; c < N; c++) {
			for(v = 0; v < N; v++){
				index = get_index(N,r,c,v);
				vtype[index]= GRB_BINARY;
			}
		}
	}

	/* add variables to model */
	error = GRBaddvars(*model, N3, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* has an error */
	}


	/* Change objective sense to maximization */
	error = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	/* update the model - to integrate new variables */
	error = GRBupdatemodel(*model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}
	return FALSE; /* No errors */
}


unsigned int addConstraints(GRBenv *env, GRBmodel *model, unsigned int m, unsigned int n, unsigned int* valuesArr) {
	unsigned int 	r,c,v;
	unsigned int	N = m*n;
	unsigned int	block;				/* for type D: Block constraints:*/
	unsigned int	block_r, block_c;	/* for type D: Block constraints:*/
	unsigned int 	index;
	unsigned int	error;
	int*			ind;				/* stores the indices of the row/col/block constraints of the model (size: N).*/
	double*			coefs;				/* stores the coefficients of the row/col/block constraints of the model (size: N).*/
	int				nonEmptyCellInd[1];	/* for type E: non-empty cell: stores the index of a non-empty cell constraint. */
	double			nonEmptyCellCoef[1];/* for type E: non-empty cell: stores the coefficient of a non-empty cell constraint.*/

	/* Initialize coefficients for the constraints */
	coefs = (double*)malloc(N*sizeof(double));
	for(r = 0; r < N; r++) {
		coefs[r] = 1;
	}
	if(coefs == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
	/* indices for the constraints */
	ind = (int*)malloc(N*sizeof(int));
	if(ind == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
	/*printf("	TypeA\n");*/
	/* type A: each cell has exactly  1 value */
	for(r = 0; r < N; r++) {
		for(c = 0; c < N; c++) {
			for(v = 0; v < N; v++) {
				index = get_index(N,r,c,v);
				ind[v] = index;
			}
			/* Add constraint:*/
			error = GRBaddconstr(model, N, ind, coefs, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return TRUE;
			}
		}
	}

	/* type B: each column has exactly one of each value contained within it. */
/*	printf("	TypeB\n");*/
	for(v = 0; v < N; v++) {
		for(c = 0; c < N; c++) {
			for(r = 0; r < N; r++) {
				index = get_index(N,r,c,v);
				ind[r] = index;
			}
			/* Add constraint:*/
			error = GRBaddconstr(model, N, ind, coefs, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return TRUE;
			}
		}
	}

	/* type C: each row has exactly one of each value contained within it. */
/*	printf("	TypeC\n");*/
	for(v = 0; v < N; v++) {
		for(r = 0; r < N; r++) {
			for(c = 0; c < N; c++) {
				index = get_index(N,r,c,v);
				ind[c] = index;
			}
			/* Add constraint:*/
			error = GRBaddconstr(model, N, ind, coefs, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return TRUE;
			}
		}
	}


	/* type D: each block has exactly one of each value contained within it. */
/*	printf("	TypeD\n");*/
	for(v = 0; v < N; v++) {
		for(block = 0; block < N; block++) {
			/* block's top-left cell's coordinations */
			r = (block/m)*m;
			c = (block%m)*n;
			for(block_r = 0; block_r < m; block_r++) {
				for(block_c = 0; block_c < n; block_c++) {
					index = get_index(N, r+block_r, c+block_c, v);
					ind[block_r*n+block_c] = index; /* ind inside the block*/
				}
			}
			/* Add constraint:*/
			error = GRBaddconstr(model, N, ind, coefs, GRB_EQUAL, 1.0, NULL);
			if (error) {
				return TRUE;
			}
		}
	}

	/* type E: each non empty cell has exactly one of each value contained within it. */
	nonEmptyCellCoef[0] = 1; /* Set the coefficient of all non-empty cell constraints to 1 */
/*	printf("	TypeE\n");*/
	for(r = 0; r < N; r++) {
		for(c = 0; c < N; c++) {
			for(v = 0; v < N; v++) {
				index = get_index(N,r,c,v);
				if(valuesArr[index] != 0) { /* if cell[r,c] has a value of v, add constraint */
					nonEmptyCellInd[0] = index;
					/* Add constraint:*/
					error = GRBaddconstr(model, 1, nonEmptyCellInd, nonEmptyCellCoef, GRB_EQUAL,1.0, NULL);
					if (error) {
						printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						return TRUE;
					}
				}
			}
		}
	}
	free(coefs);
	free(ind);
	return FALSE; /* No errors */
}


/*
 *  Solve a game board and update its solution board,
 *  using Integer Linear Programming (ILP) with the Gurobi library.
 *  return TRUE iff board is solvable.
 *
 *  Board*	boardPtr	-	A pointer a game board.
 *  Board*	solBoardPtr	-	A pointer to the solution board. used to update the solution board.
 */
int ilpSolver(Board* boardPtr, Board* solBoardPtr) {
	/* Gurobi variables */
	GRBenv			*env   = NULL;
	GRBmodel		*model = NULL;
	int				error = 0;
	int				optimstatus;		/* TRUE iff model is feasible */
	char*			vtype;				/* variable types (binary) (size: N^3).*/
	double*			sol;				/* stores the solution values of the model (size: N^3).*/
	unsigned int*	valuesArr;			/* An array that indicates cell values for non-empty cells (size: N^3). */

	/* some helpful variables */
	unsigned int	m = boardPtr->m;
	unsigned int	n = boardPtr->n;
	unsigned int	N = m*n;
	unsigned int	N3 = N*N*N;
	unsigned int	r,c,v,index;
	unsigned int	ret;					/* return value */

	/* Add variables */
	/* variable types (binary). */
/*	printf("0\n");*/
	vtype = (char*)malloc(N3*sizeof(char));
	if(vtype == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}

	valuesArr = (unsigned int*)calloc(N*N*N ,sizeof(unsigned int));
	if(valuesArr == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}
	setValues(boardPtr,valuesArr);

	sol = (double*)calloc(N*N*N ,sizeof(double));
	if(sol == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}


	/*--------- step1: create environment ------------------*/
/*	printf("step1\n");*/
	/* Create environment - log file is mip1.log */
	error = createGurobiEnvironment(env,&model);
	if(error) {
		return -1;
	}
	/*--------- step2: Objective function ------------------*/
/*	printf("step2\n");*/
	error = initObjectiveFunction(env,&model,N,vtype);
	if(error) {
		return -1;
	}

	/* --------- step3: add constraints ------------------ */
/*	printf("step3\n");*/
	error = addConstraints(env,model,m,n,valuesArr);
	if(error) {
		return -1;
	}
	/* --------- step4: optimize ------------------ */
	/* Optimize model - need to call this before calculation */
/*	printf("step4\n");*/
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

	/* --------- step5: Get optimization status (feasible/not feasible) ------------------ */
/*	printf("step5\n");*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}


	/* --------- step6: get the solution - the assignment to each variable --------------- */
/*	printf("step6\n");*/
	if(optimstatus == GRB_OPTIMAL) { /* Model is feasible */
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N3, sol);
		if (error) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
			return -1;
		}
	}

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		for(r = 0; r < N; r++) {
			for(c = 0; c < N; c++) {
				for(v = 0; v < N; v++) {
					index = get_index(N,r,c,v);
					if(sol[index] == 1) {
						setCellVal(solBoardPtr,r,c,v+1);
						break;
						/*					  has_value = TRUE;
					  printf("%d ", v+1);
					  break;*/
					}
				}
				/*			  if(!has_value)
				  printf("0 ");
			  has_value = FALSE;*/
			}
			/*		  printf("\n");*/
		}
		ret = TRUE;
	}

	/* no solution found */
	else {
		ret = FALSE;
	}

	/* IMPORTANT !!! - Free model and environment */
	GRBfreemodel(model);
	GRBfreeenv(env);
	/* Free allocated space */
	free(valuesArr);
	free(sol);
	free(vtype);

	return ret;
}
