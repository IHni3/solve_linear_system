#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "linear_system_functions.h"
//#include "test.h"


/*  
*Function: input
*--------------------
* Validates all inputs and returns a buffer (minimum size of path).
* User does have unlimited amount of attempts to provide valid inputs.
*
* Arguments
*			 text:			Output to guide the user (indicating the path).
*			 errortext:		Output if an error occurs.
*			 buffer:		Contains the size of the buffer.
*			 size:			Fixed integer size of 512.
*			 inputIsValid:	Consortium of variables that must be true 
*							for the buffer size to be returned.
*			 num:			Generic integer containing starting vectors.
*
* Returns
*			 1. buffer (minimum size of path), when all variables have been validated.
*/
char* userInput(const char* text, const char* errortext, char* buffer, const size_t size, bool (*inputIsValid)(const char* c, void* returnValue, int, ...), int num, ...)
{
	bool bRet = false;
	do
	{

		printf("%s",text);
		//TODO problem wenn mehr als size
		fgets(buffer, size, stdin);
		strtok(buffer, "\n");

		va_list args;
		va_start(args, num);
		bRet = inputIsValid(buffer, (void*) num, args);
		va_end(args);

		if (!bRet)
		{
			printf("%s", errortext);
		}

	} while (!bRet);

	return buffer;
}


/*  
*Function: inputPath
*--------------------
* Acts as a wrapper and calls method 'load'; returns the boolean outcome.
*
* Arguments
*			 c:				 Path to specified file.
*			 pMatrix:		 A matrix with n rows and n columns.
*			 pStartVector:	 Contains initial coefficients and iteration vectors.
*			 pResultsVector: Contains resulting coefficients and iteration vectors.
*
* Returns
*			 1. boolean value 'true', when all variables have been validated.
*			 2. boolean value 'false' otherwise.
*/
bool inputPath(const char* c, Matrix* pMatrix, Vector* pStartVector, Vector* pResultsVector)
{
	return load(c, pMatrix, pStartVector, pResultsVector);
}


/*  
*Function: inputMode
*--------------------	
* Returns the method to be used for solving
* the linear system of equations based on the 
* user input.
*
* Arguments
*			 c:				variable acting as a logic switch.
*
*			 returnValue:	all available ENUM values within
*							the ENUM 'Method'.
*			 
*
* Returns
*			1: boolean value 'true' and ENUM value 'JACOBI',
*			   if input (c) is 1.
*			2: boolean value 'true' and ENUM value 'GAUSS_SEIDEL,
*			   if input (c) is 2.
*			3: boolean value 'false' otherwise.
*/
bool inputMethodValidation(const char* c, Method* returnValue)
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


/*  
*Function: fileExists
*--------------------	
* Checks the accessibility of a given file.
*
* Arguments
*			 filename:		Path to specified file.
*
* Returns
*			 1. boolean value 'true', if the file exists.
*			 2. boolean value 'false' otherwise.
*/
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


/*
*Function: cleanInputBuffer
*--------------------	
* keine Referenzierung
*/
//TODO würde in clearInputBuffer umbenennen, falls die Funktion drin bleibt
void cleanInputBuffer()
{
	while(getchar() != EOF);
}


/*  
*Function: main
*--------------------	
* Main function which calls sub-functions.
* Possesses the logic to handle fundamental method calls.
*
* Arguments
*			 numberOfArguments:		Length of the argument vector.
*			 argv:					Array of character pointers.
*
* Returns
*			 None.
*/
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
		bool bRetX = load("konv3.csv", pMatrix, pResultsVector, pStartVector);
		float timediff2 = stopStopwatch(t2);
		printf("time auslesen: %.2fs\n", timediff2);
		//solveJacobi(pMatrix, pResultsVector, pStartVector, 0);
		solveGauss(pMatrix, pResultsVector, pStartVector, 0);

		float timeDiff = stopStopwatch(t1);

		printf("time: %.2fs\n", timeDiff);
		printf("this program solves linear systems of equations with jacobi or gauss-seidel algorithm\n\n");

		char* path = malloc(sizeof(char) * 512);
		userInput("please enter file path of linear system (csv): ",
			"given path does not exist or file is invalid!\n", path,
			512, inputPath, pMatrix, pResultsVector, pStartVector);		

		char pInputMethodBuffer[1];
		Method chosenInputMethod;
		userInput("iteration method:\n1. jacobi\n2. gauss-seidel\n\n", "",
			  pInputMethodBuffer, sizeof(pInputMethodBuffer), inputMethodValidation, &chosenInputMethod);

		
		


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