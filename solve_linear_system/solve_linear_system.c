#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "linear_system_functions.h"
//#include "test.h"


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
bool userInputFileValidation(const char* path, Matrix* pMatrix, Vector* pStartVector, Vector* pResultsVector)
{
	return load(path, pMatrix, pStartVector, pResultsVector);
}


const char* userInputPath(Matrix* pMatrix, Vector* pResultsVector,Vector* pStartVector)
{
	bool bRet = false;
	char* buffer = malloc(sizeof(char) * 1024);
	do
	{
		printf("please enter file path of linear system (csv): ");
		scanf("%1023s", buffer);

		printf("%s", buffer);
		clearInputBuffer();
		printf("clearInputBuffer();");

		bRet = userInputFileValidation(buffer, pMatrix, pStartVector, pResultsVector);
		printf("userInputFileValidation(buffer, pMatrix, pStartVector, pResultsVector);");

		if(!bRet)
			printf("given path does not exist or file is invalid!\n");

	} while (!bRet);
}

Method userInputMethod()
{
	bool bRet = false;
	
	int32_t input;
	do
	{
		printf("iteration method:\n1. jacobi\n2. gauss-seidel\n\n");
		scanf("%d", input);
		clearInputBuffer();

		Method method;
		bRet = inputMethodValidation(input, &method);

		if(!bRet)
			printf("input is invalid!\n");

	} while (!bRet);
}

double userInputAccuracy()
{
	bool bRet = false;
	
	double inputAcc;
	do
	{
		printf("accuracy: ");
		scanf("%lf", inputAcc);
		clearInputBuffer();

		bRet = inputAcc >= 0;

		if(!bRet)
			printf("given accuracy is invalid. Value has to be equal or bigger then zero!\n");

	} while (!bRet);
}







/*
*Function: cleanInputBuffer
*--------------------	
* keine Referenzierung
*/
void clearInputBuffer()
{
	//while(getchar() != EOF);
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

		Matrix* pMatrix = (Matrix*)malloc(sizeof(*pMatrix));
		Vector* pResultsVector = (Vector*)malloc(sizeof(*pResultsVector));
		Vector* pStartVector = (Vector*)malloc(sizeof(*pStartVector));

		//TODO auf nullptr prüfen

		//TODO remove later produces error if not
		pResultsVector->n = 0;
		pStartVector->n = 0;

		printf("this program solves linear systems of equations with jacobi or gauss-seidel algorithm\n\n");

		const char* filepath = userInputPath(pMatrix, pResultsVector, pStartVector);
		Method method = userInputMethod();
		double acc = userInputAccuracy();

		VectorLinkedListNode* result = solve(method, pMatrix, pResultsVector, pStartVector, acc); //TODO auf rückgabewert prüfen

		for(VectorLinkedListNode* it = result; it != NULL; it = it->next)
		{
			for(int i = 0; i < it->vector->n; i++)
				printf("[%lf] ", it->vector->data[i]);
		}

		
		
		//Cleanup
		free(pMatrix);
		pMatrix = NULL;
		free(pResultsVector);
		pResultsVector = NULL;
		free(pStartVector);
		pStartVector = NULL;
}