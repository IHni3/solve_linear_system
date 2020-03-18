#ifndef LINEAR_SYSTEM_FUNCTIONS_H
#define LINEAR_SYSTEM_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define NUMBER_OF_ITERATIONS 100

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


typedef struct node
{
	Vector* vector;
	struct node* next;
} VectorLinkedListNode;

typedef enum
{
	JACOBI = 0, GAUSS_SEIDEL = 1
} Method;

bool currentFieldToNumber(const char* field, const uint32_t nSize, double* result);
bool initVector(Vector* vector, const uint32_t nSize);
bool initMatrix(Matrix* pMatrix, const uint32_t nSize);
bool getDimensionsFromFile(const char* cFilename, uint32_t* nRows, uint32_t* nCols);
bool interpretateDimensions(const uint32_t nRows, const uint32_t nCols, uint32_t* nCoefficients, bool* bResults, bool* bStartVector);
bool insertMatrix(const Matrix* matrix, const uint32_t nRow, const uint32_t nCol, const char* field, const uint32_t nSize);
bool insertVector(const Vector* vector, const uint32_t n, const char* field, const uint32_t nSize);
bool readFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector);
bool load(const char* cfilename, Matrix* A, Vector* b, Vector* x);
VectorLinkedListNode* solve(Method method, Matrix* A, Vector* b, Vector* x,const double e);
VectorLinkedListNode* solveGauss(Matrix* pa, Vector* pb, Vector* px,const double acc);
VectorLinkedListNode* solveJacobi(Matrix* pa, Vector* pb, Vector* px,const double acc);
clock_t startStopwatch();
float stopStopwatch(const clock_t c);

#endif