#include "linear_system_functions.h"
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

void error(const char* location, const char* msg)
{
#if DEBUG
	printf("error at %s: %s\n", location, msg);
#else
	(void)location; //because unused
	printf("error: %s\n", msg);
#endif // DEBUG

	
}

bool addNullTermination(char* field, const uint32_t pos)
{
	bool bRet = false;
	if (field)
	{
		field[pos] = '\0';
		bRet = true;
	}

	return bRet;
}

bool currentFieldToNumber(const char* field, const uint32_t nSize, double* result)
{
	bool bRet = true;

	if (field && nSize > 0 && result)
	{
		char* endPtr;
		*result = strtod(field, &endPtr);
		if (field == endPtr) //field is NaN
		{				
			bRet = false;
		}
	}
	else
	{
		bRet = false;
	}

	return bRet;
}

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

void freeVector(Vector* pVector)
{
	if (pVector)
	{
		free(pVector->data);
		free(pVector);
	}
}

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

void freeMatrix(Matrix* pMatrix)
{
	if (pMatrix)
	{
		for (uint32_t i = 0; i < pMatrix->n; i++)
		{
			free(pMatrix->data[i]);
		}
		free(pMatrix->data);
		free(pMatrix);
	}
}

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

			uint32_t nLastCols = 0; //columns of the previous iteration
			bool bFirstIteration = true;

			bool bSameNumberOfColumns = true; //checks if there is the same number of columns in each row
			char c = 0, lastC;
			bool bLineEmpty = true;
			do
			{
				lastC = c;
				c = (char)fgetc(pReadFileHandle);
				if (c == DELIMITER)
				{
					*nCols = *nCols + 1;
				}
				else if (c == LINE_ENDING)
				{
					if (bLineEmpty == false)
					{
						*nCols = *nCols + 1; //da kein delimiter mehr - kommt am ende

						if (*nCols != nLastCols && bFirstIteration == false)
						{
							bSameNumberOfColumns = false;
						}
						else if (bFirstIteration)
						{
							bFirstIteration = false;
						}

						nLastCols = *nCols;
						*nCols = 0;
						*nRows = *nRows + 1;

						bLineEmpty = true; //becaue next line will be checked
					}
				}
				else
				{
					bLineEmpty = false;
				}
			} while (c != EOF && bSameNumberOfColumns == true);

			if (lastC != LINE_ENDING) //When last line has no \n
			{
				nRows += 1;
			}

			bRet = bSameNumberOfColumns;

			if (bRet == true) //if success set cols
				*nCols = nLastCols;

			fclose(pReadFileHandle);
			pReadFileHandle = NULL;
		}
	}

	return bRet;
}

bool interpretateDimensions(const uint32_t nRows, const uint32_t nCols, uint32_t* nCoefficients, bool* bResults, bool* bStartVector)
{
	bool bRet = false;

	if (nCoefficients && bResults && bStartVector)
	{
		if (nRows > 0 && nCols > 0 && nCols >= nRows && nCols <= nRows + 2) //between cols = rows and cols = rows + 2
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

bool initVariablesForReadFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector)
{
	if (pMatrix && pResultsVector && pStartVector)
	{
		uint32_t rows = 0, cols = 0;
		const bool bRetGetDimFromFile = getDimensionsFromFile(cFilename, &rows, &cols);
		if (!bRetGetDimFromFile)
		{
			error(AT, "gathering of file dimensions failed!");
			return false;
		}

		uint32_t nCoefficients = 0;
		bool bResultsVectorGiven, bStartVectorGiven;

		const bool bInterpretateDimensions = interpretateDimensions(rows, cols, &nCoefficients, &bResultsVectorGiven, &bStartVectorGiven);
		if (!bInterpretateDimensions)
		{
			error(AT, "the dimensions of the given file are incorrect! accected file format: (ai1, ai2, ..., ain, bi[, xi])");
			return false;
		}

		if (bResultsVectorGiven)
		{
			const bool bInitVector = initVector(pResultsVector, nCoefficients);
			if (!bInitVector)
			{
				error(AT, "initialization of results-vector failed!");
				return false;
			}
		}
		else
		{
			error(AT, "no results-vector given in file!");
			return false;
		}

		if (bStartVectorGiven)
		{
			const bool bInitVector = initVector(pStartVector, nCoefficients);
			if (!bInitVector)
			{
				error(AT, "initialization of start-vector failed!");
				return false;
			}
		}
		else
		{
			pStartVector->n = 0;
			pStartVector->data = NULL;
		}

		const bool bInitMatrix = initMatrix(pMatrix, nCoefficients);
		if (!bInitMatrix)
		{
			error(AT, "initialization of matrix failed!");
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool readFile(const char* cFilename, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector)
{
	if (pMatrix && pResultsVector && pStartVector)
	{
		const bool bInitVariablesForReadFile = initVariablesForReadFile(cFilename, pMatrix, pResultsVector, pStartVector);
		if (!bInitVariablesForReadFile)
		{
			error(AT, "initialization of variables for reading file failed!");
			return false;
		}

		FILE* fpInputFile = fopen(cFilename, "r");
		if (!fpInputFile)
		{
			error(AT, "failed to open the specified file!");
			return false;
		}
		else
		{
			char c;
			size_t nFieldSize = 10;
			char* currentField = (char*)malloc(nFieldSize * sizeof(currentField));
			if (currentField == NULL)
			{
				error(AT, "memory allocation failed!");
				return false;
			}
			uint32_t nCharCount = 0;
			uint32_t nFieldCount = 0;
			uint32_t nLineCount = 0;

			bool bLineEmpty = true;

			do
			{
				c = (char)fgetc(fpInputFile);

				//da falls das feld abgeschlossen ist (newline oder delimiter) noch die null terminierung hinzugefügt wird
				if (nCharCount + 1 >= nFieldSize)
				{
					nFieldSize += 3;
					char* field = (char*)realloc(currentField, nFieldSize);
					if(field)
						currentField = field;
					else
					{
						error(AT, "memory reallocation failed!");
						return false;
					}
				}

				if (c == DELIMITER || c == LINE_ENDING)
				{
					if (bLineEmpty == false)
					{
						bool bAddNullTermination = addNullTermination(currentField, nCharCount);
						if (!bAddNullTermination)
						{
							error(AT, "adding null-termination to field failed!");
							return false;
						}

						if (nFieldCount < pMatrix->n)
						{
							const bool bInsertMatrix = insertMatrix(pMatrix, nLineCount, nFieldCount, currentField, nCharCount);
							if (!bInsertMatrix)
							{
								error(AT, "inserting value into matrix failed!");
								return false;
							}
						}
						else if (pResultsVector->n > 0 && nFieldCount == pMatrix->n)
						{
							const bool bInsertVector = insertVector(pResultsVector, nLineCount, currentField, nCharCount);
							if (!bInsertVector)
							{
								error(AT, "inserting value into result-vector failed!");
								return false;
							}
						}
						else if (pStartVector->n > 0 && nFieldCount == pMatrix->n + 1)
						{
							const bool bInsertVector = insertVector(pStartVector, nLineCount, currentField, nCharCount);
							if (!bInsertVector)
							{
								error(AT, "inserting value into start-vector failed!");
								return false;
							}
						}

						memset(currentField, ' ', nFieldSize); //clear field to erase null termination. because otherwise the values could be shortened

						if (c == DELIMITER)
						{
							nFieldCount++;
							nCharCount = 0; //reset char count
						}
						else //newline
						{
							nLineCount++;
							nFieldCount = 0;
							nCharCount = 0;

							bLineEmpty = true;
						}
					}
				}
				else
				{
					bLineEmpty = false;

					currentField[nCharCount] = c;
					nCharCount++;
				}

			} while (c != EOF);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool load(const char* cfilename, Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector)
{
	return readFile(cfilename, pMatrix, pResultVector, pStartVector);
}

VectorLinkedListNode* solve(Method method, Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
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

bool checkAccReached(const Vector* pVector1, const Vector* pVector2, const double acc)
{
	bool bRet = false;
	if (acc >= 0 && pVector1 && pVector2 && pVector1->data && pVector2->data && pVector1->n == pVector2->n)
	{
		double accDiff;
		for (uint32_t i = 0; i < pVector1->n; i++)
		{
			//calculate difference btw. last two results
			accDiff = fabs(pVector1->data[i] - pVector2->data[i]);

			//check difference against accuracy
			if (accDiff >= acc)
			{
				bRet = false;
				break;
			}
			else
			{
				bRet = true;
			}
		}
	}
	return bRet;
}

bool copyVectorData(const Vector* pSrc, Vector* pDest)
{
	bool bRet = false;
	if (pSrc && pSrc->data && pDest && pDest->data && pSrc->n == pDest->n)
	{
		for (uint32_t i = 0; i < pSrc->n; i++)
		{
			pDest->data[i] = pSrc->data[i];
		}
		bRet = true;
	}

	return bRet;
}

bool clearVectorData(Vector* pVector)
{
	bool bRet = false;
	if (pVector && pVector->data)
	{
		for (uint32_t i = 0; i < pVector->n; i++)
		{
			pVector->data[i] = 0.f;
		}
		bRet = true;
	}

	return bRet;
}

VectorLinkedListNode* solveGauss(Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
{
	VectorLinkedListNode* pStartNode = NULL;

	if (pMatrix && pMatrix->data && pResultVector && pResultVector->data && pStartVector && acc >= 0.f)
	{
		const uint32_t n = pMatrix->n;

		if (pStartVector->n <= 0)
		{
			const bool bInitVector = initVector(pStartVector, n);
			if (!bInitVector)
			{
				error(AT, "initialization of start-vector failed!");
				return NULL;
			}

			const bool bclearVectorData = clearVectorData(pStartVector);
			if (!bclearVectorData)
			{
				error(AT, "clearing start-vector failed!");
				return NULL;
			}
		}

		//bool for ending do/while
		bool accReached = false;

		//iteration counter
		uint32_t counter = 0;

		//cache for gauss seidel algorithm
		double sum;

		VectorLinkedListNode* pCurNode = NULL;

		do
		{
			for (uint32_t i = 0; i < n; i++)
			{
				sum = pResultVector->data[i];

				for (uint32_t j = 0; j < n; j++)
				{
					if (i != j)
					{
						sum = sum - pMatrix->data[i][j] * pStartVector->data[j];
					}

					pStartVector->data[i] = sum / pMatrix->data[i][i]; //Hauptdiagonale
				}
			}
			
			if(pCurNode) //check if last result exists
				accReached = checkAccReached(pStartVector, pCurNode->vector, acc);


			if (pCurNode == NULL)
			{
				pStartNode = addVectorToLinkedList(pCurNode, pStartVector);
				pCurNode = pStartNode;
			}
			else
			{
				pCurNode = addVectorToLinkedList(pCurNode, pStartVector);
			}

			counter++;

		} while (!accReached && counter < NUMBER_OF_ITERATIONS);

	}

	return pStartNode;
}

VectorLinkedListNode* solveJacobi(Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
{
	VectorLinkedListNode* pStartNode = NULL;

	if (pMatrix && pMatrix->data && pResultVector && pResultVector->data && pStartVector && acc >= 0.f)
	{
		uint32_t n = pMatrix->n;

		if (pStartVector->n <= 0)
		{
			const bool bInitVector = initVector(pStartVector, n); 
			if (!bInitVector)
			{
				error(AT, "initialization of start-vector failed!");
				return NULL;
			}
			const bool bClearVectorData = clearVectorData(pStartVector); 
			if (!bClearVectorData)
			{
				error(AT, "clearing start-vector data failed!");
				return NULL;
			}
		}

		uint32_t nCounter = 0;
		VectorLinkedListNode* pCurNode = NULL;
		bool bAccReached = false;	

		Vector* pTempResultVector = (Vector*)malloc(sizeof(*pTempResultVector));
		if (pTempResultVector == NULL)
		{
			error(AT, "memory allocation failed!");
			return NULL;
		}

		const bool bInitVector = initVector(pTempResultVector, n);
		if (!bInitVector)
		{
			error(AT, "initialization of vector failed!");
			return NULL;
		}

		do
		{			
			const bool bCopyResToTmp = copyVectorData(pResultVector, pTempResultVector);
			if (!bCopyResToTmp)
			{
				error(AT, "copying of vector failed!");
				return NULL;
			}

			for (uint32_t x = 0; x < n; x++)
			{

				for (uint32_t y = 0; y < n; y++)
				{
					if (x != y)
					{
						pTempResultVector->data[x] = pTempResultVector->data[x] - pMatrix->data[x][y] * pStartVector->data[y];
					}
				}
				pTempResultVector->data[x] = pTempResultVector->data[x] / pMatrix->data[x][x];
			}

			if(pCurNode) //check if last result exists
				bAccReached = checkAccReached(pStartVector, pTempResultVector, acc);

			bool bCopyVectorTmpToRes = copyVectorData(pTempResultVector, pStartVector);
			if (!bCopyVectorTmpToRes)
			{
				error(AT, "copying of vector failed!");
				return NULL;
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

			nCounter++;
			
		} while (!bAccReached && nCounter < NUMBER_OF_ITERATIONS);

		freeVector(pTempResultVector);
	}

	return pStartNode;
}

clock_t startStopwatch()
{
	return clock();
}

float stopStopwatch(const clock_t c)
{
	const clock_t t2 = clock();

	float diff = ((float)(t2 - c) / (float)CLOCKS_PER_SEC);

	return diff;
}
