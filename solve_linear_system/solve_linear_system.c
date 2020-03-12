#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "linear_system_functions.h"

//#include "test.h"

char* input(const char* text, const char* errortext, char* buffer,const size_t size, bool (*inputIsValid)(const char* c, void* returnValue, int, ...), int num, ...)
{
	bool bRet = false;
	do
	{
		printf("%s",text);
		fgets(buffer, size, stdin); //TODO problem wenn mehr als size
		strtok(buffer, "\n");

		va_list args;
		va_start(args, num);
		bRet = inputIsValid(buffer, num, args);
		va_end(args);

		if (!bRet)
		{
			printf("%s", errortext);
		}
	} while (!bRet);

	return buffer;
}

bool inputPath(const char* c, Matrix* pMatrix, Vector* pStartVector, Vector* pResultsVector)
{
	return load(c, pMatrix, pStartVector, pResultsVector);
}
bool inputMode(const char* c, Method* returnValue)
{
	if (strcmp(c, "1") == 0)
	{
		*returnValue = JACOBI;
		return true;
	}
	else if (strcmp(c, "2") == 0)
	{
		*returnValue = GAUSS_SEIDEL;
		return true;
	}
	else
	{
		return false;
	}
}

bool fileExists(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if(file)
	{
		fclose(file);
		return true;
	}
	return false;
}

void cleanInputBuffer()
{
	while(getchar() != EOF);
}

int main(const int numberOfArguments, char** argv)
{
	//testAll();

		clock_t t1 = startStopwatch();
		Matrix* pMatrix = (Matrix*)malloc(sizeof(*pMatrix));
		Vector* pResultsVector = (Vector*)malloc(sizeof(*pResultsVector));
		Vector* pStartVector = (Vector*)malloc(sizeof(*pStartVector));

		//remove later produces error if not
		pResultsVector->n = 0;
		pStartVector->n = 0;

		clock_t t2 = startStopwatch();
		bool bRetX = load("C:\\Testdaten\\konv500.csv", pMatrix, pResultsVector, pStartVector);
		float timediff2 = stopStopwatch(t2);
		printf("time auslesen: %.2fs\n", timediff2);
		//solveJacobi(pMatrix, pResultsVector, pStartVector, 0);
		solveGauss(pMatrix, pResultsVector, pStartVector, 0);

		float timeDiff = stopStopwatch(t1);

		printf("time: %.2fs\n", timeDiff);
		printf("this program solves linear systems with jacobi or gauss-seiles algorithm\n\n");

		char* path = malloc(sizeof(char) * 512);
		input("please enter file path of linear system (csv): ",
			"given path does not exist or file is invalid!\n", path,
			512, inputPath, pMatrix, pResultsVector, pStartVector);		

		char pInputMode[2];
		Method inputMethod;
		input("iteration method:\n1. jacobi\n2. gauss-seidel\n\n",
			"", pInputMode,
			2, inputMode, &inputMethod);

		
		


		//switch (method)
		//{
		//	case JACOBI:
		//		solveJacobi(pMatrix, pResultsVector, pStartVector, 0);
		//		break;
		//	case GAUSS_SEIDEL:
		//		solveGauss(pMatrix, pResultsVector, pStartVector, 0);
		//		break;
		//}

		
		
		free(pMatrix);
		pMatrix = NULL;
		free(pResultsVector);
		pResultsVector = NULL;
		free(pStartVector);
		pStartVector = NULL;



		//else if (numberOfArguments == 2)
		//{
		//	const char* method = argv[1];
		//	if (strcmp(method, "jakobi"))
		//	{
		//		printf("using jakobi algorithm\n");
		//	}
		//	else if (strcmp(method, "gauss-seidel"))
		//	{
		//		printf("using gauss-seidel algorithm\n");
		//	}
		//	else
		//	{
		//		printf("invalid argument\n");
		//	}
		//}
		//else
		//{
		//	printf("invalid number of arguments\n");
		//}
}