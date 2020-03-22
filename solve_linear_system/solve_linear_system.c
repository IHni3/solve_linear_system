#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "linear_system_functions.h"
//#include "test.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

/*
*Function: clearInputBuffer
*--------------------	
* Clears the input buffer. Delimiting factors are 
* new lines and end of file.
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
void clearInputBuffer()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

/*  
*Function: inputMethodValidation
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
* In this case it is used to validate the given file. Prints out the required time
* for the file opening and validation.
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
	printf("loading file..\n");
	const clock_t t1 = startStopwatch();
		
	bool bRet = load(path, pMatrix, pResultsVector, pStartVector);

	if (bRet)
	{
		const float loadingTimeInS = stopStopwatch(t1);
		printf("file loaded in %.2fs\n", loadingTimeInS);
	}
	
	return bRet;
}

/*  
*Function: userInputPath
*--------------------
* Takes the users input and validates it via sub-function (userInputFileValidation()).
* A valid input is mandatory for the method to continue. Asks the user for input
* until a valid input is given.
*
* Arguments
*			 pMatrix:		 A matrix with n rows and n columns.
*			 pStartVector:	 Contains initial coefficients and iteration vectors.
*			 pResultsVector: Contains resulting coefficients and iteration vectors.
*
* Returns
*			 None.
*/
void userInputPath(Matrix* pMatrix, Vector* pResultsVector,Vector* pStartVector)
{
	bool bRet = false;
	char* buffer = malloc(sizeof(char) * 1024);
	do
	{
		printf("please enter file path of linear system (csv): ");		
		scanf("%1024[^\n]s", buffer);
		clearInputBuffer();
		

		bRet = userInputFileValidation(buffer, pMatrix, pResultsVector, pStartVector);

		if(!bRet)
			printf("given path does not exist or file is invalid!\n\n");

	} while (!bRet);
}

/*  
*Function: userInputMethod
*--------------------
* Asks the user for input until a valid input is given. Takes the users 
* input and validates it via sub-function (userInputFileValidation()).
* A valid input is mandatory for the method to continue.
*
* Arguments
*			 None.
*
* Returns
*			 1. The chosen input as a struct <Method> object.
*/
Method userInputMethod()
{
	bool bRet = false;
	
	int32_t input = 0;
	Method method;
	do
	{
		printf("iteration method:\n1. Jacobi\n2. Gauss-Seidel\n\n");		
		scanf("%d", &input);
		clearInputBuffer();
		

		
		bRet = inputMethodValidation(input, &method);

		if(!bRet)
			printf("input is invalid!\n\n");

	} while (!bRet);

	return method;
}

/*  
*Function: userInputPrintResults
*--------------------
* Validates user input and outputs the results depending on the
* users choice.
*
* Arguments
*			 None.
*
* Returns
*			 1. Boolean value 'true' if the user input was 1.
*			 2. Boolean value 'false' if the user input was 2.
*/
bool userInputPrintResults()
{
	bool bRet = false;

	int32_t input = 0;
	do
	{
		printf("results:\n1. Print all results\n2. Print last result only\n\n");

		scanf("%d", &input);
		clearInputBuffer();

		bRet = input == 1 || input == 2;

		if (!bRet)
			printf("input is invalid!\n");

		printf("\n");

	} while (!bRet);



	return input == 1;
}

/*  
*Function: userInputNewCalcOrExit
*--------------------
* Takes the user input after him deciding between 
* a new calculation and exiting the program. A valid user input
* is required for the function to continue.
*
* Arguments
*			 None.
*
* Returns
*			 1. Boolean value 'false' if the user input was 1.
*			 2. Boolean value 'true' if the user input was 2.
*/
bool userInputNewCalcOrExit()
{
	bool bRet = false;

	int32_t input = 0;
	do
	{
		printf("1. start new calculation\n2. Exit program\n\n");

		scanf("%d", &input);
		clearInputBuffer();

		bRet = input == 1 || input == 2;

		if (!bRet)
			printf("input is invalid!\n\n");

	} while (!bRet);

	return input == 2; //user wants to Exit
}

/*  
*Function: userInputAccuracy
*--------------------
* Returns the accuracy defined by the user input. Specified value
* has to be equal or bigger than zero.
*
* Arguments
*			 None.
*
* Returns
*			 1. A double value representing the users 
*				desired accuracy for the calculation.
*/
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
			printf("specified accuracy is invalid. Value has to be equal or bigger than zero!\n\n");

	} while (!bRet);

	return inputAcc;
}

/*  
*Function: printCurrentNode
*--------------------
* Prints out the current node of a vector. Accuracy of 
* the output is 10 characters post-decimal per value.
*
* Arguments
*			 node:			A struct <VectorLinkedListNode> object, that is 
*							implicitly defined by the struct <Vector> (values 
*							represented as an array [double pointer]).
*
* Returns
*			 None.
*/
void printCurrentNode(const VectorLinkedListNode* node)
{
	if (node && node->vector)
		for (uint32_t i = 0; i < node->vector->n; i++)
			printf("[%.10lf]\t", node->vector->data[i]);
}

/*  
*Function: printResults
*--------------------
* Prints out the results of the previous transacted calculation.
* The linear system of equations does not converge if the counter 
* reaches the error bound before finishing the output of the calculation.
*
* Arguments
*			 pResults:		A struct <VectorLinkedListNode> object, that is 
*							implicitly defined by the struct <Vector> (values 
*							represented as an array [double pointer]).
*
* Returns
*			 None.
*/
void printResults(const VectorLinkedListNode* pResults)
{
	printf("iterations:\n");
	if (pResults)
	{
		uint32_t count = 1;
		for (const VectorLinkedListNode* it = pResults; it != NULL; it = it->next)
		{
			printf("%d:\t", count);
			printCurrentNode(it);
			//TODO kann durch printNewLine() ersetzt werden
			printf("\n");

			count++;
		}
		if (count >= NUMBER_OF_ITERATIONS)
			printf("note: iteration limit was reached!\n");
	}
	else
	{
		printf("no resuls found!\n");
	}
	
}

/*  
*Function: printLastResult
*--------------------
* Finds the last node and prints out the last results of the calculation.
* The linear system of equations does not converge if the counter 
* reaches the error bound before finishing the output of the calculation.
*
* Arguments
*			 pResults:		A struct <VectorLinkedListNode> object, that is 
*							implicitly defined by the struct <Vector> (values 
*							represented as an array [double pointer]).
*
* Returns
*			 None.
*/
void printLastResult(const VectorLinkedListNode* pResults)
{
	printf("last result:\n");
	if (pResults)
	{
		//Find last Node
		const VectorLinkedListNode* node = pResults;
		uint32_t count = 0;
		while (node->next != NULL)
		{
			node = node->next;
			count++;
		}	

		printCurrentNode(node);
		//TODO kann durch printNewLine() ersetzt werden
		printf("\n");
		if (count >= NUMBER_OF_ITERATIONS)
			printf("note: Iteration limit was reached!\n");
	}
	else
	{
		printf("no resuls found!\n");
	}
}

/*  
*Function: printNewLine
*--------------------
*  Simply prints a new line where the function is called.
*
* Arguments
*			 None.
*
* Returns
*			 None.
*/
void printNewLine()
{
	printf("\n");
}

int main()
{
	//testAll();
	bool bUserExit = false;
	do
	{
		printf("this program solves linear systems of equations with Jacobi or Gauss-Seidel algorithm.\n\n");

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
			VectorLinkedListNode* results = solve(method, pMatrix, pResultsVector, pStartVector, acc);
			if (results == NULL)
				printf("something went wrong, no results found!");

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
			freeMatrix(pMatrix);
			freeVector(pResultsVector);
			freeVector(pStartVector);
			pMatrix = NULL;
			pResultsVector = NULL;
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