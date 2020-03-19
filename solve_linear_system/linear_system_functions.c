#include "linear_system_functions.h"
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

bool addNullTermination(char* field, const uint32_t pos)
{
	bool bRet = false;
	if (field && pos >= 0)
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
					else if (bFirstIteration)
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


VectorLinkedListNode* solveJacobi(Matrix* pMatrix, Vector* pResultVector, Vector* pStartVector, const double acc)
{

	VectorLinkedListNode* startNode = NULL;

	if (pMatrix && pResultVector && pStartVector && acc >= 0)
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
		do
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
				//printf("Diagonal X[%d%d] has value of /%f \n", i,i, pMatrix->data[i][i]); 

				for (uint32_t j = 0; j < n; j++)
				{
					if (i != j)
					{
						sum = sum - pMatrix->data[i][j] * preX[j];
					}
				}
				pStartVector->data[i] = sum / pMatrix->data[i][i];
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
			iteration++;
		} while (iteration < NUMBER_OF_ITERATIONS && acc > curAcc);

	}

	return startNode;
}


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
				pStartVector->data[i] = 0.f;
			}
		}

		//diccerence between last two resutls
		double accDiff;
		//bool for ending do/while
		bool accReached = false;

		//iteration counter
		uint32_t counter = 0;

		//cache for gauss seidl algorithm
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

			//iterate through rows, to check, if accuracy is reached
			for (uint32_t i = 0; i < n; i++)
			{
				//calculate difference btw. last two results
				double prevValue = 0;
				if (pCurNode)
				{
					pCurNode->vector->data[i];
				}
				accDiff = fabs(pStartVector->data[i] - prevValue);

				//ckeck difference against accuracy
				if (accDiff > acc)
				{
					accReached = false;
					break;
				}
				else
				{
					accReached = true;
				}
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

			counter++;
		} while (!accReached && counter <= NUMBER_OF_ITERATIONS);

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
