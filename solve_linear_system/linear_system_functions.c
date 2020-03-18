#include "linear_system_functions.h"
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#ifdef _MSC_VER
	#pragma warning(disable:4996)
#endif

/*  
*Function: addNullTermination
*--------------------
* Adds \0 character to a char pointer at the give position. 
* The purpose is to terminate the string
*
* Arguments
*			 field:			Char pointer containing a string 
*			 pos:			position where the termination is set
*
* Returns
*			 1. boolean value 'true', when all variables have been validated.
*			 2. boolean value 'false' otherwise.
*/
bool addNullTermination(char* field, const uint32_t pos)
{
	bool bRet = false;
	if(field && pos >= 0)
	{
		field[pos] = '\0';
		bRet = true;
	}

	return bRet;	
}


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
bool currentFieldToNumber(const char* field, const uint32_t nSize, double* result)
{
	bool bRet = true;

	if (field && nSize > 0 && result)
	{
		for (const char* current = field; *current != '\0' && bRet == true; current++)
		{
			if (isdigit(*current) == 0 && *current != '.')
			{
				bRet = false;
			}
		}

		double num = atof(field);
		*result = num;
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
bool initVector(Vector* vector, const uint32_t nSize)
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
bool initMatrix(Matrix* pMatrix, const uint32_t nSize)
{
	bool bRet = true;

	if (pMatrix != NULL && nSize > 0)
	{

		const uint32_t nRows = nSize;
		const uint32_t nColumns = nSize;

		pMatrix->data = (double**)malloc(nRows * sizeof(double*));

		if (pMatrix->data != NULL)
		{
			for (uint32_t i = 0; i < nRows; i++)
			{
				pMatrix->data[i] = (double*)malloc(nColumns * sizeof(double));
				
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
bool getDimensionsFromFile(const char* cFilename, uint32_t* nRows, uint32_t* nCols)
{
	bool bRet = false;

	if (cFilename && nRows && nCols)
	{
		FILE* pReadFileHandle = fopen(cFilename, "r");

		if (pReadFileHandle)
		{
			*nRows = 0;
			*nCols = 0;

			uint32_t nLastCols = 0; //cols of the previous iteration
			bool bFirstIteration = true;

			bool bSameColumns = true; //checks if there is the same number of columns in each row
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
					
					if (*nCols != nLastCols && bFirstIteration == false)
					{
						//TODO pruint32_t invalid file
						bSameColumns = false;
					}
					else if(bFirstIteration)
					{
						bFirstIteration = false;
					}

					nLastCols = *nCols;
					*nCols = 0;
					*nRows = *nRows + 1;
				}
			} while (c != EOF || bSameColumns == false);

			bRet = bSameColumns;

			if (bRet == true) //if success set cols
				*nCols = nLastCols;

			fclose(pReadFileHandle);
			pReadFileHandle = NULL;
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
bool interpretateDimensions(const uint32_t nRows, const uint32_t nCols, uint32_t* nCoefficients, bool* bResults, bool* bStartVector)
{
	bool bRet = false;

	if (nCoefficients && bResults && bStartVector)
	{
		if (nRows > 0 && nCols > 0 && nCols <= nRows + 2) //+2 because of startvector and resultvector
		{
			*nCoefficients = nRows;

			if (nRows == nCols) //no startvector and resultvector
			{
				*bResults = false;
				*bStartVector = false;
			}
			else if (nCols == nRows + 1)  //no startvector
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
bool insertMatrix(const Matrix* matrix, const uint32_t nRow, const uint32_t nCol, const char* field, const uint32_t nSize)
{
	double num = 0;

	bool bRet = false;

	bool bFieldToNumber = currentFieldToNumber(field, nSize, &num);
	if (bFieldToNumber && matrix && matrix->data)
	{
		matrix->data[nRow][nCol] = num;
		bRet = true;
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
bool insertVector(const Vector* vector, const uint32_t n, const char* field, const uint32_t nSize)
{
	bool bRet = false;

	if (vector && n >= 0 && field)
	{
		double num = 0;
		bool bFieldToNumber = currentFieldToNumber(field, nSize, &num);
		if (bFieldToNumber == true)
		{
			vector->data[n] = num;
			bRet = true;
		}
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
*			 cFilename:		 String containing the relative pMatrixth to the file.
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

		uint32_t rows = 0, cols = 0;
		getDimensionsFromFile(cFilename, &rows, &cols); //TODO rückgabewert


		uint32_t nCoefficients = 0;
		bool bResultsVector, bStartVector;
		interpretateDimensions(rows, cols, &nCoefficients, &bResultsVector, &bStartVector); //TODO rückgabewert

		//pResultsVector = (Vector*)malloc(sizeof(*pResultsVector));;
		if (bResultsVector)
		{
			bool ret1 = initVector(pResultsVector, nCoefficients); //TODO nullprüfung
		}
		else
		{
			pResultsVector->n = 0;
			pResultsVector->data = NULL;
		}

		//pStartVector = (Vector*)malloc(sizeof(*pStartVector));
		if (bStartVector)
		{
			bool ret2 = initVector(pStartVector, nCoefficients); //TODO null prüfung
		}
		else
		{
			pStartVector->n = 0;
			pStartVector->data = NULL;
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
			initMatrix(pMatrix, nCoefficients); //TODO Rückgabewert

			do
			{
				c = (char)fgetc(fpInputFile);
				//printf("found %c\n", c);

				if (nCharCount + 1 >= nFieldSize) //da falls das feld abgeschlossen ist (newline oder delimiter) noch die null terminirung hinzugefügt wird
				{
					nFieldSize += 3;
					char* field = (char*)realloc(currentField, nFieldSize);
					currentField = field; //auf null ptr pr�fen
				}

				if (c == delimiter || c == newline)
				{
					//printf("before currentField=%s\n", currentField);
					bool success = addNullTermination(currentField, nCharCount); //TODO return value

					//printf("currentField=%s\n", currentField);

					if (nFieldCount < pMatrix->n)
						insertMatrix(pMatrix, nLineCount, nFieldCount, currentField, nCharCount); //TODO Returnvalue
					else if (bResultsVector == true && nFieldCount == pMatrix->n)
					{
						insertVector(pResultsVector, nLineCount, currentField, nCharCount); //TODO Returnvalue
					}
					else if (bStartVector == true && nFieldCount == pMatrix->n + 1)
					{
						insertVector(pStartVector, nLineCount, currentField, nCharCount); //TODO Returnvalue
					}

					memset(currentField, ' ', nFieldSize); //clear field to erase null termination. because otherwise the values could be shortened

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
*			 cFilename:		String containing the relative pMatrixth to the file.
*			 A:				A two-dimensional Array consisting of n rows and n lines.
*			 b:				Vector containing the results of the linear system of equations.
*			 x:				Vector containing the initial values of the linear system of equations.
*
* Returns
*			 1. The return of 'readFile'.
*/
bool load(const char* cfilename, Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector)
{
	return readFile(cfilename, pMatrix, pResultVector, pStartVector);
}

VectorLinkedListNode* solve(Method method, Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector,const double acc)
{
	VectorLinkedListNode* vec = NULL;

	switch (method)
	{
		case JACOBI:
			vec = solveJacobi(pMatrix, pResultVector, pStartVector, acc);
			break;
		
		case GAUSS_SEIDEL:
			vec = solveGauss(pMatrix, pResultVector, pStartVector, acc);
			break;
	}

	return vec;
}


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
bool vectorAbs(const double* a, const double* b, const uint32_t nSize, double* result)
{				
	//TODO @Tim : Ich weiß dass wir prüfen wollen, ob man überhaupt einen Betrag berechnen kann, aber wie bekommen wir den Wert dann hier raus?
	if (a && b && result)
	{
		double value = 0;
		for (uint32_t i = 0; i < nSize; i++)
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


VectorLinkedListNode* addVectorToLinkedList(VectorLinkedListNode* pPrevNode, const Vector* pSaveVector)
{
	VectorLinkedListNode* node = NULL;
	if (pSaveVector)
	{
		node = (VectorLinkedListNode*)malloc(sizeof(VectorLinkedListNode));

		if (node)
		{
			node->next = NULL;
			node->vector = (Vector*)malloc(sizeof(Vector));
			bool bSuccess = initVector(node->vector, pSaveVector->n);
			if (bSuccess)
			{
				node->vector->n = pSaveVector->n;
				for (uint32_t i = 0; i < node->vector->n; i++)
					node->vector->data[i] = pSaveVector->data[i];
			}

			if (pPrevNode == NULL) //startNode
			{
				return node;
			}
			else
			{
				pPrevNode->next = node;
			}
		}		
	}

	return node;
}


/*  
*Function: solveJacobi
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the jacobi iteration method.
*
* Arguments
*			 pMatrix:		A matrix with n rows and n lines.
*			 pResultVector:		The starting vectors.
*			 pStartVector: 		The resulting vectors.
*			 acc:		The accuracy of the calculation.
*
* Returns
*			 None.
*/
VectorLinkedListNode* solveJacobi(Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
{
	
	VectorLinkedListNode* startNode = NULL;

	if(pMatrix && pResultVector && pStartVector && acc >= 0)
	{
		const uint32_t n = pMatrix->n;

		if (pStartVector->n <= 0)
		{
			initVector(pStartVector, n);
			for (uint32_t i = 0; i < n; i++)
			{
				pStartVector->data[i] = 0;
			}
		}

		//TODO auf VECTOR umstellen
		//cache for old x Values for checking difference
		double* preX = (double*)malloc(sizeof(double) * n);
		if (preX == NULL)
		{
			return NULL;
		}

		uint32_t iteration = 0;

		double curAcc = -1;
		for (uint32_t i = 0; i < n; i++)
		{
			printf("b[%d]-value is : %lf \n", i, pResultVector->data[i]);
		}
		
		VectorLinkedListNode* curNode = NULL;
		for (uint32_t iteration = 0; iteration < NUMBER_OF_ITERATIONS && acc > curAcc; iteration++)
		{
			for (uint32_t i = 0; i < n; i++)
			{
				preX[i] = pStartVector->data[i];
			}

			double sum;
			for (uint32_t i = 0; i < n; i++)
			{
				sum = 0;
				double d = pMatrix->data[i][i];
				printf("Diagonal X[%d%d] has value of /%f \n", i,i, pMatrix->data[i][i]); 

				for (uint32_t j = 0; j < n; j++)
				{
					if (i != j)
					{
						sum += pMatrix->data[i][j] * preX[j];
					}
				}
				pStartVector->data[i] = 1 / d * sum;
			}

			vectorAbs(preX, pStartVector->data, n, &curAcc); //TODO: rueckgabewert pruefen

			if (curNode == NULL)
			{
				startNode = addVectorToLinkedList(curNode, pStartVector);
				curNode = startNode;
			}
			else
			{
				curNode->next = addVectorToLinkedList(curNode, pStartVector);
			}

			printf("Iterations: %d, curAcc: %.11f\n", iteration, curAcc);

			//for (uint32_t i = 1; i <= n; i++)
			//{
			//	printf("[x%d] = %.5f\n\n", i, pStartVector->data[i]);
			//}
		}

	}

	return startNode;
}


/*  
*Function: solveGauss
*--------------------
* Takes the matrix, resulting vectors and starting vectors as arguments.
* Calculates the linear system equation based on the gauss-seidel iteration method.
*
* Arguments
*			 pMatrix:		A matrix with n rows and n lines.
*			 pResultVector:		The starting vectors.
*			 pStartVector: 		The resulting vectors.
*			 acc:		The accuracy of the calculation.
*
* Returns
*			 None.
*/
VectorLinkedListNode* solveGauss(Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
{
	VectorLinkedListNode* pStartNode = NULL;

	if (pMatrix && pResultVector && pStartVector && acc >= 0.f)
	{
		const uint32_t n = pMatrix->n;

		if (pStartVector->n <= 0)
		{
			initVector(pStartVector, n);  //TODO retval
			for (uint32_t i = 0; i < n; i++)
			{
				pStartVector->data[i] = 0;
			}
		}

		//cache for old x Values for checking difference
		double* preX = (double*)malloc(sizeof(double) * n);
		if (preX == NULL)
		{
			return NULL;
		}
		//diccerence between last two resutls
		double accDiff;
		//bool for ending do/while
		uint32_t accReached = 0;

		//iteration counter
		uint32_t counter = 0;

		//cache for gauss seidl algorithm
		double d;

		VectorLinkedListNode* pCurNode = NULL;

		do
		{
			counter++;

			//gauss seidl algorithm
			for (uint32_t i = 0; i < n; i++)
			{
				preX[i] = pStartVector->data[i];
			}

			for (uint32_t j = 0; j < n; j++)
			{
				d = pResultVector->data[j];

				for (uint32_t i = 0; i < n; i++)
				{
					if (j != i)
					{

						d -= pMatrix->data[j][i] * pStartVector->data[i];
					}

					pStartVector->data[j] = d / pMatrix->data[j][j]; //Hauptdiagonale
				}
			}

			//iterate through rows, to ckeck, if accuracy is reached
			for (uint32_t i = 0; i < n; i++) {

				//calculate difference btw. last two results
				accDiff = pStartVector->data[i] - preX[i];
				//turn negative difference, positive for ckecking 
				if (accDiff < 0) accDiff *= -1;

				//ckeck difference against accuracy
				if (accDiff > acc) accReached = 0;
				else accReached = 1;
			}

			if (pCurNode == NULL)
			{
				pStartNode = addVectorToLinkedList(pCurNode, pStartVector);
				pCurNode = pStartNode;
			}
			else
			{
				pCurNode = addVectorToLinkedList(pCurNode, pStartVector);
			}

		} while (!accReached && counter <= NUMBER_OF_ITERATIONS);

	}

	return pStartNode;
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
*			 1. The difference between the timestamps as a float in seconds.
*/
float stopStopwatch(const clock_t c)
{
	const clock_t t2 = clock();

	float diff = ((float)(t2 - c) / (float)CLOCKS_PER_SEC);

	return diff;
}