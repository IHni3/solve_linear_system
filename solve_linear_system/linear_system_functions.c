#include "linear_system_functions.h"
#include <stdlib.h>
#include <math.h>

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

bool interpretateDimensions(const int nRows, const int nCols, int* nCoefficients, bool* bResults, bool* bStartVector)
{
	bool bRet = false;

	if (nCoefficients && bResults && bStartVector)
	{
		if (nRows > 0 && nCols > 0 && nCols <= nRows + 2)
		{
			*nCoefficients = nRows;

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

bool insertMatrix(const Matrix* matrix, const int nRow, const int nCol, const char* field, const int nSize)
{
	double num = 0;
	bool bRet = currentFieldToNumber(field, nSize, &num);
	if (bRet == true)
	{
		matrix->data[nRow][nCol] = num;
		//printf("matrix.data[%d][%d] = %lf\n", nRow, nCol, num);
	}
	else
	{

	}

	return bRet;
}

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

	}

	return bRet;
}

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

bool load(const char* cfilename, Matrix* A, Vector* b, Vector* x)
{
	return readFile(cfilename, A, b, x);
}

//void solve(Method method, Matrix* A, Vector* b, Vector* x, double e)
//{
//
//}

// return absolute differnce value of two inserst
bool vectorAbs(const double* a, const double* b, const int nSize, double* result)
{								//@Tim : Ich weiß das wir prüfen wollen, ob man überhaupt einen Betrag berechnen kann, aber wie bekommen wir den Wert dann hier raus?
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

		vectorAbs(preX, px->data, n, &precision); //TODO: r�ckgabewert pr�fen
												   //@Tim : Was stellen wir jetzt mit dem Bool an?
		printf("Iterations: %d, Precision: %.11f\n", iteration, precision);

		for (int i = 1; i <= n; i++)
		{
			printf("[x%d] = %.5f\n\n", i, px->data[i]);
		}
	}
}

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