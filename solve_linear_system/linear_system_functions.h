#ifndef LINEAR_SYSTEM_FUNCTIONS_H
#define LINEAR_SYSTEM_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct
{
	uint32_t n;
	double** data;
} Matrix;
typedef struct
{
	uint32_t n;
	double* data;
} Vector;
typedef enum
{
	JACOBI = 0, GAUSS_SEIDEL = 1
} Method;

bool currentFieldToNumber(const char* field, const int nSize, double* result);
bool initVector(Vector* vector, const int nSize);
bool initMatrix(Matrix* pMatrix, const int nSize);
bool getDimensionsFromFile(const char* cFilename, int* nRows, int* nCols);
bool interpretateDimensions(const int nRows, const int nCols, int* nCoefficients, bool* bResults, bool* bStartVector);
bool insertMatrix(const Matrix* matrix, const int nRow, const int nCol, const char* field, const int nSize);
bool insertVector(const Vector* vector, const int n, const char* field, const int nSize);
bool readFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector);
bool load(const char* cfilename, Matrix* A, Vector* b, Vector* x);
//void solve(Method method, Matrix* A, Vector* b, Vector* x, double e)
//{
//
//}
void solveGauss(Matrix* pa, Vector* pb, Vector* px, double acc);
void solveJacobi(Matrix* pa, Vector* pb, Vector* px, double acc);
clock_t startStopwatch();
float stopStopwatch(const clock_t c);

#endif