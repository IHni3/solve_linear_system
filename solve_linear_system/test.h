#if 0

#include "linear_system_functions.h"

#define THROW_EXCEPTION printf("TEST: Check failed in function \"%s\", Line \"%d\"\n", __FUNCTION__, __LINE__)

bool test_currentFieldToNumber()
{

	double result = 0.0;
	const char* field3 = "";
	const char* field4 = "";

	bool ret1 = currentFieldToNumber(NULL, 0, NULL);
	if (ret1 == true)
	{
		THROW_EXCEPTION;
		return false;
	}

	bool ret2 = currentFieldToNumber("1,21", -1, &result);
	if (ret2 == true && result == 1.21)
	{
		THROW_EXCEPTION;
		return false;
	}

	bool ret3 = currentFieldToNumber("1.21", sizeof("1.21"), &result);
	if (!ret3 == true && !result == 1.21)
	{
		THROW_EXCEPTION;
		return false;
	}

	bool ret4 = currentFieldToNumber("121", sizeof("121"), &result);
	if (!ret4 == true && !result == 121)
	{
		THROW_EXCEPTION;
		return false;
	}


	bool ret5 = currentFieldToNumber("194480735604831", sizeof("194480735604831"), &result);
	if (!ret5 == true && !result == 194480735604831)
	{
		THROW_EXCEPTION;
		return false;
	}

	bool ret6 = currentFieldToNumber("abc", sizeof("abc"), &result);
	if (ret6 == true)
	{
		THROW_EXCEPTION;
		return false;
	}

	char arr[] = { '1', '2' };
	bool ret7 = currentFieldToNumber(arr, sizeof(char) * 2, &result);
	if (ret7 == false && result != 12)
	{
		THROW_EXCEPTION;
		return false;
	}

	return ret1 & ret2 & ret3 & ret4 & ret5;
}

bool test_initVector()
{
	bool bRet = true;
	Vector* vectHeap = NULL;
	Vector vectStack;
	bRet &= !initVector(NULL, -1);
	bRet &= !initVector((Vector*)malloc(5), -1);
	bRet &= initVector((Vector*) malloc(sizeof(Vector)), 500);
	bRet &= !initVector(vectHeap, 1);

	bRet &= initVector(&vectStack, 10);

	for (uint32_t i = 0; i < vectStack.n; i++)
			vectStack.data[i] = 1.2405;

	if (bRet == false)
	{
		THROW_EXCEPTION;
	}

	free(vectHeap);
	vectHeap = NULL;
	
	return bRet;
}

bool test_initMatrix()
{
	bool bRet = true;
	Matrix* matrixHeap = NULL;
	Matrix matrixStack;
	bRet &= !initMatrix(NULL, -1);
	bRet &= !initMatrix((Matrix*)malloc(5), -1);
	bRet &= initMatrix((Matrix*)malloc(sizeof(Matrix)), 50);
	bRet &= !initMatrix(matrixHeap, 1);

	bRet &= initMatrix(&matrixStack, 10);
	for (uint32_t i = 0; i < matrixStack.n; i++)
		for (uint32_t j = 0; j < matrixStack.n; j++)
			matrixStack.data[i][j] = 1.2405;

	if (bRet == false)
	{
		THROW_EXCEPTION;
	}

	free(matrixHeap);
	matrixHeap = NULL;

	return bRet;
}

bool test_getDimensionsFromFile()
{
	uint32_t rows, cols;
	bool ret = !getDimensionsFromFile(NULL, NULL, NULL);
	ret &= getDimensionsFromFile("C:\\Testdaten\\konv3.csv", &rows, &cols);

	return ret;

}

bool testAll()
{
	bool ret3 = test_initMatrix();
	bool ret2 = test_initVector();
	bool ret1 = test_currentFieldToNumber();

	bool ret4 = test_getDimensionsFromFile();

	return ret1 & ret2 & ret3;
}
#endif
