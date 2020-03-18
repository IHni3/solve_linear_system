#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "linear_system_functions.h"
//#include "test.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


/*
*Function: cleanInputBuffer
*--------------------
* keine Referenzierung
*/
void clearInputBuffer()
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }
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
bool inputMethodValidation(const int32_t input, Method* returnValue)
{
	bool bRet = false;
	if(returnValue != NULL)
	{
		if (input == 1)
		{
			*returnValue = JACOBI;
			bRet = true;
		}
		else if (input == 2)
		{
			*returnValue = GAUSS_SEIDEL;
			bRet = true;
		}
	}
	return bRet;
}


/*  
*Function: userInputFileValidation
*--------------------
* Acts as a wrapper and calls method 'load'; returns the boolean outcome.
* In this case it is used to validate the given file.
*
* Arguments
*			 path:			 Path to specified file.
*			 pMatrix:		 A matrix with n rows and n columns.
*			 pStartVector:	 Contains initial coefficients and iteration vectors.
*			 pResultsVector: Contains resulting coefficients and iteration vectors.
*
* Returns
*			 1. boolean value 'true', when given path exists, file is readable and valid.
*			 2. boolean value 'false' otherwise.
*/
bool userInputFileValidation(const char* path, Matrix* pMatrix, Vector* pResultsVector, Vector* pStartVector)
{
	bool bRet;

	printf("loading file..\n");
	const clock_t t1 = startStopwatch();
		
	bRet = load(path, pMatrix, pResultsVector, pStartVector);

	if (bRet)
	{
		const float loadingTimeInS = stopStopwatch(t1);
		printf("file loaded in %.2fs\n", loadingTimeInS);
	}
	
	return bRet;
}


void userInputPath(Matrix* pMatrix, Vector* pResultsVector,Vector* pStartVector)
{
	bool bRet = false;
	char* buffer = malloc(sizeof(char) * 1024);
	do
	{
		printf("please enter file path of linear system (csv): ");		
		scanf("%1023s", buffer);
		clearInputBuffer();
		

		bRet = userInputFileValidation(buffer, pMatrix, pResultsVector, pStartVector);

		if(!bRet)
			printf("given path does not exist or file is invalid!\n\n");

	} while (!bRet);
}

Method userInputMethod()
{
	bool bRet = false;
	
	int32_t input = 0;
	Method method;
	do
	{
		printf("iteration method:\n1. jacobi\n2. gauss-seidel\n\n");		
		scanf("%d", &input);
		clearInputBuffer();
		

		
		bRet = inputMethodValidation(input, &method);

		if(!bRet)
			printf("input is invalid!\n\n");

	} while (!bRet);

	return method;
}

bool userInputPrintResults()
{
	bool bRet = false;

	int32_t input = 0;
	Method method;
	do
	{
		printf("results:\n1. print all results\n2. print only last result\n\n");

		scanf("%d", &input);
		clearInputBuffer();
		
		


		bRet = input == 1 || input == 2;

		if (!bRet)
			printf("input is invalid!\n\n");

	} while (!bRet);

	return input == 1;
}

bool userInputNewCalcOrExit()
{
	bool bRet = false;

	int32_t input = 0;
	Method method;
	do
	{
		printf("1. start new calculation\n2. exit program\n\n");

		scanf("%d", &input);
		clearInputBuffer();
		
		


		bRet = input == 1 || input == 2;

		if (!bRet)
			printf("input is invalid!\n\n");

	} while (!bRet);

	return input == 2; //user wants to Exit
}

double userInputAccuracy()
{
	bool bRet = false;
	
	double inputAcc = -1.f;
	do
	{
		printf("accuracy: ");

		scanf("%lf", &inputAcc);
		clearInputBuffer();
		
		

		bRet = inputAcc >= 0;

		if(!bRet)
			printf("given accuracy is invalid. Value has to be equal or bigger then zero!\n\n");

	} while (!bRet);
}

void printCurrentNode(const VectorLinkedListNode* node)
{
	if (node && node->vector)
		for (int i = 0; i < node->vector->n; i++)
			printf("[%.10lf] ", node->vector->data[i]);
}

void printResults(const VectorLinkedListNode* pResults)
{
	if (pResults)
	{
		uint32_t count = 1;
		for (const VectorLinkedListNode* it = pResults; it != NULL; it = it->next)
		{
			printf("%d:", count);
			printCurrentNode(it);
			printf("\n");

			count++;
		}
	}
}
void printLastResult(const VectorLinkedListNode* pResults)
{
	if (pResults)
	{
		//Find last Node
		const VectorLinkedListNode* node = pResults;
		while (node->next != NULL)
		{
			node = node->next;
		}

		printCurrentNode(node);
	}
}

void printNewLine()
{
	printf("\n");
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

int main(const int argc, char** argv)
{
	//testAll();
	bool bUserExit = false;
	do
	{
		printf("this program solves linear systems of equations with jacobi or gauss-seidel algorithm\n\n");

		Matrix* pMatrix = (Matrix*)malloc(sizeof(*pMatrix));
		Vector* pResultsVector = (Vector*)malloc(sizeof(*pResultsVector));
		Vector* pStartVector = (Vector*)malloc(sizeof(*pStartVector));

		if (pMatrix && pResultsVector && pStartVector)
		{		
			userInputPath(pMatrix, pResultsVector, pStartVector);
			printNewLine();
			
			Method method = userInputMethod();
			printNewLine();
			
			double acc = userInputAccuracy();
			printNewLine();

			printf("solving linear system..\n");
			const clock_t t1 = startStopwatch();
			VectorLinkedListNode* results = solve(method, pMatrix, pResultsVector, pStartVector, acc); //TODO auf rückgabewert prüfen		

			const float solvingTimeInS = stopStopwatch(t1);
			printf("solved in %.2fs\n", solvingTimeInS);

			printNewLine();

			bool bPrintAll = userInputPrintResults();
			if (bPrintAll)
				printResults(results);
			else
				printLastResult(results);

			printNewLine();

			bUserExit = userInputNewCalcOrExit();

			//Cleanup
			free(pMatrix);
			pMatrix = NULL;
			free(pResultsVector);
			pResultsVector = NULL;
			free(pStartVector);
			pStartVector = NULL;

			printNewLine();
			printNewLine();
		}
		else
		{
			printf("memory allocation failed! Maybe there is no free RAM left..\n");
		}
	} while (!bUserExit);
}