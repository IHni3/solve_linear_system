#include "linear_system_functions.h"
#include <stdlib.h>
#include <math.h>


/*  
*Function: currentFieldToNumber
*--------------------
* Takes a single field of type <char pointer> and returns
* a boolean after validating the numerical correctness by
* cycling through every character checking with isdigit().
*
* Arguments
*			 field:			Char pointer containing a string consisting of 
*							a field of the linear system of equations.
*			 nSize:			Size of the specified field (single chars count, except dots).
*			 result:		Double pointer as unique identifier of 
*							a to double converted string.
*
* Returns
*			 1. boolean value 'true', when all variables have been validated.
*			 2. boolean value 'false' otherwise.
*/
bool currentFieldToNumber(const char* field, const int nSize, double* result)
{
	bool bRet = true;

	if (field != NULL && nSize > 0)
	{
		char* str = (char*)malloc(sizeof(char) * (nSize + 1));
		if (str != NULL)
		{
			memcpy(str, field, sizeof(char) * nSize);
			str[nSize] = '\0';

			for (char* current = str; *current != '\0' && bRet == true; current++)
			{
				if (isdigit(*current) == 0 && *current != '.')
				{
					bRet = false;
				}
			}

			double num = atof(str);
			*result = num;
		}

		free(str);
	}

	else
	{
		bRet = false;
	}

	return bRet;
}


/*  
*Function: initVector
*--------------------
* Initializes a vector as a function of the size of the vector.
*
* Arguments
*			 vector:		A vector array with supposedly multiple entries.
*			 nSize:			Size of the specified vector (single chars count, except dots).
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and thus the vector was initialized successfully.
*			 2. boolean value 'false' otherwise.
*/
bool initVector(Vector* vector, const int nSize)
{
	bool bRet = false;

	if (vector != NULL && nSize > 0)
	{
		double* p = (double*)malloc(sizeof(double) * nSize);
		if (p != NULL)
		{
			vector->data = p;
			vector->n = nSize;

			bRet = true;
		}
	}

	return bRet;
}


/*  
*Function: initMatrix
*--------------------
* Initializes a matrix as a function of the size of the matrix.
*
* Arguments
*			 pMatrix:		A two-dimensional Array consisting of n rows and n lines.
*			 nSize:			Size of the specified vector (single chars count, except dots).
*							Should be the same for lines and rows (logically).
*
* Returns
*			 1. boolean value 'true', when all variables have been validated 
*				and thus the matrix was initialized successfully.
*			 2. boolean value 'false' otherwise.
*/
bool initMatrix(Matrix* pMatrix, const int nSize)
{
	bool bRet = true;

	if (pMatrix != NULL && nSize > 0)
	{

		const int nRows = nSize;
		const int nColumns = nSize;

		pMatrix->data = (double**)malloc(nRows * sizeof(double*));

		//printf("initMatrix: matrix created at 0x%p. %d bytes allocated.\n", pMatrix->data, (int)sizeof(pMatrix->data));

		if (pMatrix->data != NULL)
		{
			for (int i = 0; i < nRows; i++)
			{
				pMatrix->data[i] = (double*)malloc(nColumns * sizeof(double));
				//printf("initMatrix: row number %d created: 0x%p to 0x%p\n", i, pMatrix->data[i], pMatrix->data[i] + nColumns * sizeof(double));
				//test init data
				for (int y = 0; y < nColumns; y++)
				{
					//printf("initMatrix: accessing [%d][%d] 0x%p\n", i, y, pMatrix->data[i] + y * sizeof(double));
				}
				if (pMatrix->data[i] == NULL)
					bRet = false;
			}
		}
		else
		{
			bRet = false;
		}

		if (bRet == true)
		{
			pMatrix->n = nSize;
		}
	}
	else
	{
		bRet = false;
	}

	return bRet;
}


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
bool getDimensionsFromFile(const char* cFilename, int* nRows, int* nCols)
{
	bool bRet = false;

	if (cFilename && nRows && nCols)
	{
		FILE* pReadFileHandle = fopen(cFilename, "r");

		if (pReadFileHandle)
		{
			*nRows = 0;
			*nCols = 0;

			int nLastCols = -1;

			bool bIterationSuccess = true;
			char c;
			do
			{
				c = (char)fgetc(pReadFileHandle);
				if (c == ',')
				{
					*nCols = *nCols + 1;
				}
				else if (c == '\n')
				{
					*nCols = *nCols + 1; //da kein delimiter mehr komm am ende

					if (nLastCols != -1 && *nCols != nLastCols)
					{
						//TODO print invalid file
						bIterationSuccess = false;
					}

					nLastCols = *nCols;
					*nCols = 0;
					*nRows = *nRows + 1;
				}
			} while (c != EOF || bIterationSuccess == false);

			bRet = bIterationSuccess;

			if (bRet == true)
				*nCols = nLastCols;

			fclose(pReadFileHandle);
			pReadFileHandle = 0;
		}
	}

	return bRet;
}


/*  
*Function: interpretateDimensions
*--------------------
* Initializes a vector as a function of the size of the vector. Evaluates if either the
* linear system of equations contains either a valid amount of starting vectors, 
* a valid amount of reasonable results, both of which or ultimately none of both.
*
* Arguments
*			 nRows:			Integer containing he amount of lines.
*			 nCols:			Integer containing the amount of columns.
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
bool interpretateDimensions(const int nRows, const int nCols, int* nCoefficients, bool* bResults, bool* bStartVector)
{
	bool bRet = false;

	if (nCoefficients && bResults && bStartVector)
	{
		if (nRows > 0 && nCols > 0 && nCols <= nRows + 2)
		{
			*nCoefficients = nRows;

			//TODO woher weiß man, dass es nicht den Fall gibt, dass Startvektoren aber keine Lösungen gegeben sind?
			if (nRows == nCols)
			{
				*bResults = false;
				*bStartVector = false;
			}
			else if (nCols == nRows + 1)
			{
				*bResults = true;
				*bStartVector = false;
			}
			else if (nCols == nRows + 2)
			{
				*bResults = true;
				*bStartVector = true;
			}

			bRet = true;
		}
	}

	return bRet;
}

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
bool insertMatrix(const Matrix* matrix, const int nRow, const int nCol, const char* field, const int nSize)
{
	double num = 0;
	bool bRet = currentFieldToNumber(field, nSize, &num);
	if (bRet == true)
	{
		matrix->data[nRow][nCol] = num;
		//printf("matrix.data[%d][%d] = %lf\n", nRow, nCol, num);
	}
	//TODO für was das else? bRet = false? Hat dann bei Funktionsaufruf aber kein exception handling...?
	else
	{
		
	}

	return bRet;
}


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
bool insertVector(const Vector* vector, const int n, const char* field, const int nSize)
{
	double num = 0;
	bool bRet = currentFieldToNumber(field, nSize, &num);
	if (bRet == true)
	{
		vector->data[n] = num;
		//printf("vector.data[%d] = %lf\n", n, num);
	}
	else
	{
		//TODO warum ist das else hier?
	}

	return bRet;
}

/*  
*Function: readFile
*--------------------
* Reads the file and verifies all inputs with the sub-functions above.
* Ulitmately initializes the overall linear system of equations.
*
* Arguments
*			 cFilename:		 String containing the relative path to the file.
*			 pMatrix:		 A two-dimensional Array consisting of n rows and n lines.
*			 pResultsVector: Vector containing the results of the linear system of equations.
*			 pStartVector:	 Vector containing the initial values of the linear system of equations.
*
* Returns
*			 1. boolean value 'true', when the linear system of equations has been successfully initialized.
*			 2. boolean value 'false' otherwise.
*/
bool readFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector)
{
	bool bReturn = false;

	if (pMatrix && pResultsVector && pStartVector)
	{

		unsigned long ulSize = 0;  // Input File size

		int rows = 0, cols = 0;
		getDimensionsFromFile(cFilename, &rows, &cols);
		int nCoefficients = 0;
		bool bResultsVector, bStartVector;
		interpretateDimensions(rows, cols, &nCoefficients, &bResultsVector, &bStartVector);

		//pResultsVector = (Vector*)malloc(sizeof(*pResultsVector));;
		if (bResultsVector)
		{
			bool ret1 = initVector(pResultsVector, nCoefficients);
		}

		//pStartVector = (Vector*)malloc(sizeof(*pStartVector));
		if (bStartVector)
		{
			bool ret2 = initVector(pStartVector, nCoefficients);
		}



		FILE* fpInputFile = fopen(cFilename, "r"); // file open
		if (fpInputFile)
		{
			char c;
			char delimiter = ',';
			char newline = '\n';
			size_t nFieldSize = 10;
			char* currentField = (char*)malloc(nFieldSize * sizeof(currentField));
			if (currentField == NULL)
			{
				return false;
			}
			uint32_t nCharCount = 0;
			uint32_t nFieldCount = 0;
			uint32_t nLineCount = 0;


			//pMatrix = (Matrix*)malloc(sizeof(*pMatrix));
			initMatrix(pMatrix, nCoefficients);

			do
			{
				c = (char)fgetc(fpInputFile);
				//printf("found %c\n", c);

				if (nCharCount >= nFieldSize)
				{
					nFieldSize += 3;
					char* field = (char*)realloc(currentField, nFieldSize);
					currentField = field; //auf null ptr pr�fen
				}

				if (c == delimiter || c == newline)
				{
					if (nFieldCount < pMatrix->n)
						insertMatrix(pMatrix, nLineCount, nFieldCount, currentField, nCharCount);
					else if (bResultsVector == true && nFieldCount == pMatrix->n)
					{
						insertVector(pResultsVector, nLineCount, currentField, nCharCount);
					}
					else if (bStartVector == true && nFieldCount == pMatrix->n + 1)
					{
						insertVector(pStartVector, nLineCount, currentField, nCharCount);
					}

					if (c == delimiter)
					{
						nFieldCount++;
						nCharCount = 0; //reset char count
					}
					else //newline
					{
						nLineCount++;
						nFieldCount = 0;
						nCharCount = 0;
					}
				}
				else
				{
					currentField[nCharCount] = c;
					nCharCount++;
				}

			} while (c != EOF);

			bReturn = true;
		}
	}

	return bReturn;
}

/*  
*Function: load
*--------------------
* Acts as a wrapper for the function 'readFile'.
*
* Arguments
*			 cFilename:		String containing the relative path to the file.
*			 A:				A two-dimensional Array consisting of n rows and n lines.
*			 b:				Vector containing the results of the linear system of equations.
*			 x:				Vector containing the initial values of the linear system of equations.
*
* Returns
*			 1. The return of 'readFile'.
*/
bool load(const char* cfilename, Matrix* A, Vector* b, Vector* x)
{
	return readFile(cfilename, A, b, x);
}

//TODO entfernen oder hat diese Funktion später noch einen Nutzen?
//void solve(Method method, Matrix* A, Vector* b, Vector* x, double e)
//{
//
//}


/*  
*Function: vectorAbs
*--------------------
* Returns the absolute difference value of two inserts.
*
* Arguments
*			 a:		 	First double value for the calculation.
*			 b:		 	Second double value for the calculation.
*			 nSize: 	Member of the matrix structure.
*			 result:	The calculation result.
*
* Returns
*			 1. boolean value 'true', when the result was calculated successfully.
*			 2. boolean value 'false' otherwise.
*/
bool vectorAbs(const double* a, const double* b, const int nSize, double* result)
{				
	//TODO @Tim : Ich weiß dass wir prüfen wollen, ob man überhaupt einen Betrag berechnen kann, aber wie bekommen wir den Wert dann hier raus?
	if (a && b && result)
	{
		double value = 0;
		for (int i = 0; i < nSize; i++)
		{
			value += fabs(a[i] - b[i]);
		}
		*result = value;
		return true;
	}
	else
	{
		return false;
	}
}


/*  
*Function: solveJacobi
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the jacobi iteration method.
*
* Arguments
*			 pa:		A matrix with n rows and n lines.
*			 pb:		The starting vectors.
*			 px: 		The resulting vectors.
*			 acc:		The accuracy of the calculation.
*
* Returns
*			 None.
*/
void solveJacobi(Matrix* pa, Vector* pb, Vector* px, double acc)
{
	const int n = pa->n;

	if (px->n <= 0)
	{
		initVector(px, n);
		for (int i = 0; i < n; i++)
		{
			px->data[i] = 0;
		}
	}

	//cache for old x Values for checking difference
	double* preX = (double*)malloc(sizeof(double) * n);
	if (preX == NULL)
	{
		return;
	}

	int iteration = 1;
	double d;
	double precision = -1;     // @Tim : Verstehe nicht wie du "precision" verwenden willst
	for (int i = 0; i < n; i++)
	{
		printf("b[%d]-value is : %d \n", i, pb->data[i]);
	}

	for (int iteration = 0; iteration < 100 && acc > precision; iteration++)
	{
		//copyVector(b, x, dimension);

		double sum;
		for (int i = 0; i < n; i++)
		{
			sum = 0;
			double d = pa->data[i][i];
			//printf("Diagonal X[%d%d] has value of /%f \n", i,i, pa->data[i][i]); 

			for (int j = 0; j < n; j++)
			{
				if (i != j)
				{
					sum += pa->data[i][j] * preX[j];
				}
			}
			px->data[i] = 1 / d * sum;
		}
		double precision = 0;			//@Tim: siehe oben

		vectorAbs(preX, px->data, n, &precision); //TODO: rueckgabewert pruefen
												   //@Tim : Was stellen wir jetzt mit dem Bool an?
		printf("Iterations: %d, Precision: %.11f\n", iteration, precision);

		for (int i = 1; i <= n; i++)
		{
			printf("[x%d] = %.5f\n\n", i, px->data[i]);
		}
	}
}


/*  
*Function: solveGauss
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the gauss-seidel iteration method.
*
* Arguments
*			 pa:		A matrix with n rows and n lines.
*			 pb:		The starting vectors.
*			 px: 		The resulting vectors.
*			 acc:		The accuracy of the calculation.
*
* Returns
*			 None.
*/
void solveGauss(Matrix* pa, Vector* pb, Vector* px, double acc)
{

	const int n = pa->n;

	if (px->n <= 0)
	{
		initVector(px, n);
		for (int i = 0; i < n; i++)
		{
			px->data[i] = 0;
		}
	}

	//cache for old x Values for checking difference
	double* preX = (double*)malloc(sizeof(double) * n);
	if (preX == NULL)
	{
		return;
	}
	//diccerence between last two resutls
	double accDiff;
	//bool for ending do/while
	int accReached = 0;

	//iteration counter
	int counter = 0;

	//cache for gauss seidl algorithm
	double d;

	do
	{
		counter++;

		//gauss seidl algorithm
		for (int i = 0; i < n; i++)
		{
			preX[i] = px->data[i];
		}

		for (int j = 0; j < n; j++)
		{
			d = pb->data[j];

			for (int i = 0; i < n; i++)
			{
				if (j != i)
				{

					d -= pa->data[j][i] * px->data[i];
				}

				px->data[j] = d / pa->data[j][j]; //Hauptdiagonale
			}
		}

		//iterate through rows, to ckeck, if accuracy is reached
		for (int i = 0; i < n; i++) {

			//calculate difference btw. last two results
			accDiff = px->data[i] - preX[i];
			//turn negative difference, positive for ckecking 
			if (accDiff < 0) accDiff *= -1;

			//ckeck difference against accuracy
			if (accDiff > acc) accReached = 0;
			else accReached = 1;
		}
		//print results
		//printf("%02d: [%.2f] [%.2f] [%.2f]\n", counter, px->data[0], px->data[1], px->data[2]);
		printf("%02d: [%lf] [%lf] [%lf]\n", counter, px->data[0], px->data[1], px->data[2]);

	} while (!accReached && counter <= 100);
}

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
clock_t startStopwatch()
{
	return clock();
}


/*
*Function: stopStopwatch
*--------------------
* Returns the number of clock ticks elapsed since the program was launched.
*
* Arguments
*			 c:			First timestamp (starting value) to calculate the difference.		
*
* Returns
*			 1. The difference between the timestamps as a float.
*/
float stopStopwatch(const clock_t c)
{
	const clock_t t2 = clock();

	float diff = ((float)(t2 - c) / (float)CLOCKS_PER_SEC);

	return diff;
}