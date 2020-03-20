#ifndef LINEAR_SYSTEM_FUNCTIONS_H
#define LINEAR_SYSTEM_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define NUMBER_OF_ITERATIONS 100
#define DELIMITER ','
#define LINE_ENDING '\n'

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

//TODO @aaron
void error(const char* location, const char* msg);

/*  
*Function: addNullTermination
*--------------------
* Adds \0 character to a char pointer at the give position. 
* The purpose is to terminate the string
*
* Arguments
*			 field:			Char pointer containing the string 
*			 pos:			position where the termination is set
*
* Returns
*			 1. boolean value 'true', when all parameters are valid.
*			 2. boolean value 'false' otherwise.
*/
bool addNullTermination(char* field, const uint32_t pos);

/*  
*Function: currentFieldToNumber
*--------------------
* Converts an given string into floating number
*
* Arguments
*			 field:			char pointer containg a string. Allowed Characters '0-9', '.'. 
*			 nSize:			Size of the specified field. Value has to be bigger then zero.
*			 result:		If the function was executed successfully 
*							the parameter contains the value of the 
*							parameter field converted to double.
*
* Returns
*			 1. boolean value 'true', when all parameters are valid.
*			 2. boolean value 'false' otherwise.
*/
bool currentFieldToNumber(const char* field, const uint32_t nSize, double* result);

/*  
*Function: initVector
*--------------------
* Initializes a vector with the number of entries specified in nSize.
*
* Arguments
*			 vector:		Vector to initialize.
*			 nSize:			Number of entries to create.
*
* Returns
*			 1. boolean value 'true', when all parameters are valid, the allocation of memory works 
*				and the given size (nSize) is bigger then zero.
*			 2. boolean value 'false' otherwise.
*/
bool initVector(Vector* vector, const uint32_t nSize);

//TODO @aaron
void freeVector(Vector* pVector);

/*  
*Function: initMatrix
*--------------------
* Initializes a Matrix with n colums and n rows spezified in nSize.
*
* Arguments
*			 pMatrix:		Matrix to initialize.
*			 nSize:			Size for the spezified square-matrix (lines and colums).
*
* Returns
*			 1. boolean value 'true', when all parameters are valid, the allocation of memory works 
*				and the given size (nSize) is bigger then zero.
*			 2. boolean value 'false' otherwise.
*/
bool initMatrix(Matrix* pMatrix, const uint32_t nSize);

//TODO @aaron
void freeMatrix(Matrix* pMatrix);

/*  
*Function: getDimensionsFromFile
*--------------------
* Evaluates the actual size of the file containing the 
* given values for the linear system of equations.
*
* Arguments
*			 cFilename:		Name of the file
*			 nRows:			Amount of rows of given file
*			 nCols:			Amount of lines of given file
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and the correct size of vertical and horizontal 
*			 2. boolean value 'false' otherwise.
*/
bool getDimensionsFromFile(const char* cFilename, uint32_t* nRows, uint32_t* nCols);

/*  
*Function: interpretateDimensions
*--------------------
* Initializes a vector as a function of the size of the vector. Evaluates if either the
* linear system of equations contains either a valid amount of starting vectors, 
* a valid amount of reasonable results, both of which or ultimately none of both.
*
* Arguments
*			 nRows:			Integer containing he number of lines.
*			 nCols:			Integer containing the number of columns.
*			 nCoefficients:	All values representing the coefficients of the linear
*							system of equations. These are tested for proportional 
*							length differences (+1) by this method.
*			 bResults:		Boolean value depending on the presence of 
*							already predefined results of the linear system of equations.
*			 bStartVector:	Boolean value depending on the presence of starting vectors.
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and one of the specifications matches. 
*			 2. boolean value 'false' otherwise.
*/
bool interpretateDimensions(const uint32_t nRows, const uint32_t nCols, uint32_t* nCoefficients, bool* bResults, bool* bStartVector);

/*  
*Function: insertMatrix
*--------------------
* Acts as a wrapper for the insertion of the matrix.
* Automatically converts entry in field of matrix to digits 
* (via sub-function) and fills the field after validation.
*
* Arguments
*			 matrix:		A two-dimensional Array consisting of n rows and n lines.
*			 nRow:			Integer representing the actual position on behalf of the amount of lines.
*			 nCol:			Integer representing the actual position on behalf of the amount of columns.
*			 field:			The unedited field of the matrix returned by functions reading the raw file.
*			 nSize:			Integer consisting of the length of the given field.
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and the matrix has been successfully filled with verified digits only.
*			 2. boolean value 'false' otherwise.
*/
bool insertMatrix(const Matrix* matrix, const uint32_t nRow, const uint32_t nCol, const char* field, const uint32_t nSize);

/*  
*Function: insertVector
*--------------------
* Acts as a wrapper for the insertion of the vector.
* Automatically converts entry in field of matrix to digits 
* (via sub-function) and fills the field after validation.
*
* Arguments
*			 vector:		A one-dimensional Array consisting of the values of the starting vector.
*			 n:				Indication of the current line.
*			 field:			The unedited field of the matrix returned by functions reading the raw file.
*			 nSize:			Integer consisting of the length of the given field.
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and the vector has been successfully filled with verified digits only.
*			 2. boolean value 'false' otherwise.
*/
bool insertVector(const Vector* vector, const uint32_t n, const char* field, const uint32_t nSize);

/*  
*Function: readFile
*--------------------
* Reads the file and verifies all inputs with the sub-functions above.
* Ulitmately initializes the overall linear system of equations.
*
* Arguments
*			 cFilename:		 String containing the relative pMatrixth to the file.
*			 pMatrix:		 A two-dimensional Array consisting of n rows and n lines.
*			 pResultsVector: Vector containing the results of the linear system of equations.
*			 pStartVector:	 Vector containing the initial values of the linear system of equations.
*
* Returns
*			 1. boolean value 'true', when the linear system of equations has been successfully initialized.
*			 2. boolean value 'false' otherwise.
*/
bool readFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector);

/*  
*Function: load
*--------------------
* Acts as a wrapper for the function 'readFile'.
*
* Arguments
*			 cFilename:		String containing the relative pMatrixth to the file.
*			 A:				A two-dimensional Array consisting of n rows and n lines.
*			 b:				Vector containing the results of the linear system of equations.
*			 x:				Vector containing the starting values of the linear system of equations.
*
* Returns
*			 1. The return of 'readFile'.
*/
bool load(const char* cfilename, Matrix* A, Vector* b, Vector* x);

/*  
*Function: solve
*--------------------
* Wrapper acting as a logical switch for 
* the solving methods (selection between user inputs).
*
* Arguments
*			 method:			An ENUM containing both solving methods 
*								assigned to distinguishable figures.
*			 pResultVector:		The resulting vectors.
*			 pStartVector: 		The starting vectors.
*			 acc:				The accuracy of the calculation.
*
* Returns
*			 All vectors as a linked list.
*/
VectorLinkedListNode* solve(Method method, Matrix* A, Vector* b, Vector* x,const double e);

/*  
*Function: addVectorToLinkedList
*--------------------
* Adds a given vector to a linked list after 
* validating the initialization.
*
* Arguments
*			 pPrevNode:			The current node of the vector which 
*								will then be the previous node.
*			 pSaveVector:		The starting vector of the next node.
*
* Returns
*			 Starting vector of the next node considering the 
*			 previous node as a node.
*/
VectorLinkedListNode* addVectorToLinkedList(VectorLinkedListNode* pPrevNode, const Vector* pSaveVector);

//TODO @aaron
bool checkAccReached(const Vector* pVector1, const Vector* pVector2, const double acc);

//TODO @aaron
bool copyVectorData(const Vector* pSrc, Vector* pDest);

//TODO @aaron
bool clearVectorData(Vector* pVector);

/*  
*Function: solveGauss
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the gauss-seidel iteration method.
*
* Arguments
*			 pMatrix:			A matrix with n rows and n lines.
*			 pResultVector:		The resulting vectors.
*			 pStartVector: 		The starting vectors.
*			 acc:				The accuracy of the calculation.
*
* Returns
*			 None.
*/
VectorLinkedListNode* solveGauss(Matrix* pa, Vector* pb, Vector* px,const double acc);

/*  
*Function: solveJacobi
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the jacobi iteration method.
*
* Arguments
*			 pMatrix:			A matrix with n rows and n lines.
*			 pResultVector:		The resulting vectors.
*			 pStartVector: 		The starting vectors.
*			 acc:				The accuracy of the calculation.
*
* Returns
*			 None.
*/
VectorLinkedListNode* solveJacobi(Matrix* pa, Vector* pb, Vector* px,const double acc);

/*
*Function: startStopwatch
*--------------------
* Returns a timestamp resembling the current time.
*
* Arguments
*			 None.
*
* Returns
*			 1. A timestamp resembling the current time.
*/
clock_t startStopwatch();

/*
*Function: stopStopwatch
*--------------------
* Returns the number of clock ticks elapsed since the program was launched.
*
* Arguments
*			 c:					First timestamp (starting value)
*								to calculate the difference.		
*
* Returns
*			 1. The difference between the timestamps as a float in seconds.
*/
float stopStopwatch(const clock_t c);

#endif