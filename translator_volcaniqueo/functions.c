#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
Function subtracts two matrices and returns result as a new matrix. 
Args: 1) ID of first matrix 
      2) ID of second matrix 
      3) # rows of matrices 
      4) # columns of matrices
Returns: Resulting matrix.
*/
double **mm_sub(double **A, double **B, int n, int m){
    double ** C = (double **) calloc(n, sizeof(double *));
    for (int i = 0; i < n; i++) {
        C[i] = (double *) calloc(m, sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
    return C;
}

/*
Function adds two matrices and returns result as a new matrix. 
Args: 1) ID of first matrix 
      2) ID of second matrix 
      3) # rows of matrices 
      4) # columns of matrices
Returns: Resulting matrix.
*/
double **mm_add(double **A, double **B, int n, int m){
    double ** C = (double **) calloc(n, sizeof(double *));
    for (int i = 0; i < n; i++) {
        C[i] = (double *) calloc(m, sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

/*
Function multiples two matrices and returns result as a new matrix. 
Args: 1) ID of first matrix 
      2) ID of second matrix 
      3) # rows of first matrix 
      4) # columns of first matrix
      5) # rows of second matrix      
Returns: Resulting matrix.
*/
double **mm_mup( double **A, double **B, int dimA1, int dimA2, int dimB2){
    double ** C = (double **) calloc(dimA1, sizeof(double *));
    for (int i = 0; i < dimA1; i++) {
        C[i] = (double *) calloc(dimB2, sizeof(double));
    }
    int k;
    for (int i = 0; i < dimA1; i++) {
        for (int j = 0; j < dimB2; j++) {
            for(int k = 0; k < dimA2; k++){
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

/*
Function multiples elements of a matrix with a scalar. 
Args: 1) ID of matrix 
      2) double type scalar to multiply the matrix. 
      3) # rows of matrix 
      4) # columns of matrix      
Returns: Resulting matrix.
*/
double **ms_mup( double **A, double b, int n, int m){
    double ** C = (double **) calloc(n, sizeof(double *));
    for (int i = 0; i < n; i++) {
        C[i] = (double *) calloc(m, sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            C[i][j] = b * A[i][j];
        }
    }
    return C;
}

/*
Function transposes matrix specified. 
Args: 1) ID of the matrix 
      2) # rows of first matrix 
      3) # columns of first matrix      
Returns: Resulting matrix.
*/
double **tr( double **A, int n, int m){
    double ** C = (double **) calloc(m, sizeof(double *));
    for (int i = 0; i < m; i++) {
        C[i] = (double *) calloc(n, sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            C[j][i] = A[i][j];
        }
    }
    return C;
}

/*
Function takes square root of the number specified. 
Args: double type number.    
Returns: Square root of the number specified. 
*/
double sqrt_c(double sca) {
    double ans = sqrt(sca);
    return ans;
}

/*
Function returns appropriate expression between arguments after necessary checks.
Args: Four expressions of double type.  
Returns: double exp. 
*/
double choose(double exp1, double exp2, double exp3, double exp4){
    if(fabs(exp1-0.0) <=  10e-7){
        return exp2;
    }else if(exp1 > 0.0){
        return exp3;
    }else if(exp1 < 0.0){
        return exp4;
    }
}

/*
Prints a line containing twelve '-' characters. 
Args: 
Returns: void
*/
void printsep(){
    printf("------------\n");
}

/*
Prints a line containing double expression specified. 
Args: double expr_s
Returns: void
*/
void print_s(double expr_s){
    if( (expr_s - (int) expr_s) < __DBL_EPSILON__){
        printf("%d\n", (int)expr_s);
    }
    else{
    printf("%.6f\n", expr_s);
    }
}

/*
Prints elements of the matrix specified. 
Args: Pointer accesing elements of the matrix.
Returns: void
*/
void print_m(double **A, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if( (A[i][j] - (int) A[i][j]) < __DBL_EPSILON__){
                printf("%d ", (int)A[i][j]);
            }else{
                printf("%.6f ", A[i][j]);
            }
        }
        printf("\n");
    }
}

/*
Function used to declare a matrix/vector with correct c language specifications.
Args: 
      1) ID of matrix/vector.
      2) # rows of matrix/vector.
      3) # rows of matrix. (Should be 1 in order to declare a vector.)
Returns: void
*/
void declare_var(double ***pA, int n, int m){
    *pA = (double **) calloc(n, sizeof(double *));
    for (int i = 0; i < n; i++) {
        (*pA)[i] = (double *) calloc(m, sizeof(double));
    }
}

/*
Function used to assign elements of a matrix/vector.
Args: 
      1) ID of matrix/vector.
      2) Array of doubles to be assigned as matrix/vector elements.
      3) # rows of matrix/vector.
      4) # rows of matrix. (Should be 1 in order to declare a vector.)
Returns: void
*/
void assign_var(double ***pA, double *elms , int n, int m){
    for(int i = 0; i < n; i++ ){
        for(int j = 0; j < m; j++){
            (*pA)[i][j] = elms[(i*m)+j];
        }
    }
}
