#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "gurobi_c.h"


/********** Private method declarations **********/

unsigned int	get_index(unsigned int, unsigned int, unsigned int, unsigned int);
unsigned int	createGurobiEnvModel(GRBenv*, GRBmodel**);
unsigned int	initObjectiveFunction(GRBenv*, GRBmodel**, unsigned int, char*);
unsigned int	addConstraints(GRBenv*, GRBmodel*, Board*, unsigned int, unsigned int);
void			updateSolution(Board*, double*, unsigned int);

/******* End of private method declarations ******/



/************************* Public methods *************************/

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

	/* some helpful variables */
	unsigned int	m = boardPtr->m;
	unsigned int	n = boardPtr->n;
	unsigned int	N = m*n;
	unsigned int	N3 = N*N*N;
	unsigned int	ret = TRUE;				/* return value */

	/* Add variables */
	/* variable types (binary). */
	vtype = (char*)malloc(N3*sizeof(char));
	if(vtype == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}


	sol = (double*)calloc(N*N*N ,sizeof(double));
	if(sol == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}


	/*--------- step1: Create environment and model ------------------*/
	error = createGurobiEnvModel(env,&model);
	if(error) {
		ret = -1;
	}
	/*------------------- step2: Objective function ------------------*/
	if(ret) { /* no errors */
		error = initObjectiveFunction(env,&model,N,vtype);
		if(error) {
			ret = -1;
		}
	}

	/* ------------------- step3: Add constraints ------------------- */
	if(ret) { /* no errors */
		error = addConstraints(env,model,boardPtr,m,n);
		if(error) {
			ret = -1;
		}
	}
	/* ---------------------- step4: Optimize ----------------------- */
	/* Optimize model - need to call this before calculation */
	if(ret) { /* no errors */
		error = GRBoptimize(model);
		if (error) {
			printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
			ret = -1;
		}
	}
	/* --------- step5: Get optimization status (feasible/not feasible) ------------------ */
	if(ret) { /* no errors */
		error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (error) {
			printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
			ret = -1;
		}
	}
	/* --------- step6: get the solution - the assignment to each variable --------------- */
	if(ret) { /* no errors */
		if(optimstatus == GRB_OPTIMAL) { /* Model is feasible. Solution found. */
			error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N3, sol);
			if (error) {
				printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
				ret = -1;
			}
			ret = TRUE; /* solution found */
			updateSolution(solBoardPtr,sol,N); /* Update solution board to the values assigned by the optimized model */
		}
		else { /* Model is infeasible. No solution found. */
			ret = FALSE;
		}
	}

	/* Free model and environment */
	GRBfreemodel(model);
	GRBfreeenv(env);
	/* Free allocated space */
	free(sol);
	free(vtype);

	return ret;
}

/********************** End of public methods *********************/



/************************* Private methods *************************/

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


/*
 * Creates a Gurobi environment and an empty model.
 * Also sets OutputFlag to 0, so there will be no prints to the console from Gurobi.
 * Returns TRUE iff there were errors while using Gurobi.
 *
 * GRBenv* 		env			-	Gurobi envorinment.
 * GRBmodel**	modelPtr	-	A pointer to a Gurobi model.
 */
unsigned int createGurobiEnvModel(GRBenv *env, GRBmodel **modelPtr){
	unsigned int error;
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, modelPtr, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	GRBsetintparam(GRBgetenv(*modelPtr),"OutputFlag",0);
	return FALSE; /* No errors */
}


/*
 * Initializes the objective function.
 * The objective function will be: Maximize 0.
 * The number of variables is set to be N^3 (x_(r,c,v) \forall r,c,v \in {0,...,N^3}).
 *
 * GRBenv* 		env			-	Gurobi envorinment.
 * GRBmodel**	modelPtr	-	A pointer to a Gurobi model.
 * unsigned int	N			-	number of rows/columns.
 * char*		vtype		-	variable types (binary) (size: N^3).
 */
unsigned int initObjectiveFunction(GRBenv *env, GRBmodel **modelPtr, unsigned int N, char* vtype){
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

	/* add variables to modelPtr */
	error = GRBaddvars(*modelPtr, N3, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* has an error */
	}


	/* Change objective sense to maximization */
	error = GRBsetintattr(*modelPtr, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}

	/* update the modelPtr - to integrate new variables */
	error = GRBupdatemodel(*modelPtr);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		return TRUE; /* Has an error */
	}
	return FALSE; /* No errors */
}


/*
 * Adds all of the constraints needed for the model.
 * Constraint Types:
 * 	Type A: each cell has exactly  1 value.
 * 	type B: each column has exactly one of each value contained within it.
 * 	Type C: each row has exactly one of each value contained within it.
 * 	Type D: each block has exactly one of each value contained within it.
 * 	Type E: each non-empty cell has one value, that is known beforehand.
 *
 * GRBenv* 		env			-	Gurobi envorinment.
 * GRBmodel*	model		-	Gurobi model.
 * Board*		boardPtr	-	A pointer a game board.
 * unsigned int	m			-	number of rows in each block on the board.
 * unsigned int	n			-	number of columns in each block on the board.
 */
unsigned int addConstraints(GRBenv *env, GRBmodel *model, Board* boardPtr, unsigned int m, unsigned int n) {
	unsigned int 	r,c,v;
	unsigned int	N = m*n;
	unsigned int 	index, error;
	unsigned int	block;				/* for type D: Block constraints:*/
	unsigned int	block_r, block_c;	/* for type D: Block constraints:*/
	int*			ind;				/* stores the indices of the row/col/block constraints of the model (size: N).*/
	double*			coefs;				/* stores the coefficients of the row/col/block constraints of the model (size: N).*/
	int				nonEmptyCellInd[1];	/* for type E: non-empty cell: stores the index of a non-empty cell constraint. */
	double			nonEmptyCellCoef[1];/* for type E: non-empty cell: stores the coefficient of a non-empty cell constraint.*/
	unsigned int	ret = FALSE;		/* return value. returns TRUE iff there WERE errors while adding constraints. */

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
	/* Type A: each cell has exactly  1 value */
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
				ret = TRUE;
			}
		}
	}

	/* Type B: each column has exactly one of each value contained within it. */
	if(!ret) {/* no errors yet */
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
					ret = TRUE;
				}
			}
		}
	}

	/* Type C: each row has exactly one of each value contained within it. */
	if(!ret) {/* no errors yet */
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
					ret = TRUE;
				}
			}
		}
	}


	/* Type D: each block has exactly one of each value contained within it. */
	if(!ret) {/* no errors yet */
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
					ret = TRUE;
				}
			}
		}
	}

	/* Type E: each non-empty cell has one value, that is known beforehand. */
	if(!ret) {/* no errors yet */
		nonEmptyCellCoef[0] = 1; /* Set the coefficient of all non-empty cell constraints to 1 */
		for(r = 0; r < N; r++) {
			for(c = 0; c < N; c++) {
				v = getCell(boardPtr,r,c)->value; /* value between 0-N, 0 means empty cell. */
				if(v > 0) { /* cell(r,c) is not empty */
					index = get_index(N,r,c,v-1); /* here a value of v translates to v-1 in the indices. */
					nonEmptyCellInd[0] = index;
					/* Add constraint */
					error = GRBaddconstr(model, 1, nonEmptyCellInd, nonEmptyCellCoef, GRB_EQUAL,1.0, NULL);
					if (error) {
						printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						ret = TRUE;
					}
				}
			}
		}
	}
	free(coefs);
	free(ind);
	return ret;
}


/*
 * Updates the solution board according to the optimized solution to the model.
 *
 * Board*		solBoardPtr	-	A pointer to the solution board. used to update the solution board.
 * double*		sol			-	stores the solution values of the model (size: N^3).
 * unsigned int	N			-	number of rows/columns.
 */
void updateSolution(Board* solBoardPtr, double* sol, unsigned int N) {
	unsigned int r,c,v;
	unsigned int index;
	for(r = 0; r < N; r++) {
		for(c = 0; c < N; c++) {
			for(v = 0; v < N; v++) {
				index = get_index(N,r,c,v);
				if(sol[index] == 1) {
					setCellVal(solBoardPtr,r,c,v+1);
					break;
				}
			}
		}
	}
}

/********************** End of private methods *********************/
