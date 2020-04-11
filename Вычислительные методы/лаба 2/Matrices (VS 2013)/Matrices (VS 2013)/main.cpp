#include<iostream>
#include<string>

int findMaxIndex(float** a, int startIndex)
{
	int maxIndex = startIndex;

	for (int j = startIndex; j < 3; j++)
	{
		if (a[startIndex][maxIndex] < a[startIndex][j])	maxIndex = j;
	}

	return maxIndex;
}

void swapLines(float*** a, float** b, int i, int j)
{
	float bufB;
	float* bufA;

	bufA = (*a)[i];
	(*a)[i] = (*a)[j];
	(*a)[j] = bufA;

	bufB = (*b)[i];
	(*b)[i] = (*b)[j];
	(*b)[j] = bufB;
}

void makeZeros(float*** a, float** b, int currentLine)
{
	for (int j = 2; j >= currentLine; j--)
	{
		if (j != currentLine)
		{
			float multiplier = (*a)[j][currentLine] / (*a)[currentLine][currentLine];
			for (int k = 2; k >= currentLine; k--)
			{
				(*a)[j][k] -= (*a)[currentLine][k] * multiplier;
			}
			(*b)[j] -= (*b)[currentLine] * multiplier;
		}
		else
		{
			float multiplier = 1 / (*a)[currentLine][currentLine];
			for (int k = j; k < 3; k++)
			{
				(*a)[j][k] *= multiplier;
			}
			(*b)[j] *= multiplier;
		}
	}
}

float* gauss(float** a, float* b)
{
	float* result = new float[3];

	float** aLocal = new float*[3];
	float* bLocal = new float[3];
	for (int i = 0; i < 3; i++)
	{
		aLocal[i] = new float[3];
		for (int j = 0; j < 3; j++)
		{
			aLocal[i][j] = a[i][j];
		}
		bLocal[i] = b[i];
	}

	int maxIndex;

	std::cout << "Start Gauss method" << std::endl;

	for (int i = 0; i < 3; i++)
	{
		/*Find maximal element in column*/
		maxIndex = findMaxIndex(aLocal, i);

		/*Swap lines*/
		swapLines(&aLocal, &bLocal, i, maxIndex);

		/*making zeros below i line*/
		makeZeros(&aLocal, &bLocal, i);
	}

	/*solving system*/
	for (int i = 2; i >= 0; i--)
	{
		float sum = 0;
		for (int j = 2; j > i; j--)
		{
			sum += result[j] * aLocal[i][j];
		}
		result[i] = (bLocal[i] - sum) / aLocal[i][i];
	}

	return result;
}

float calcNorm(float* result, float* resultPrev)
{
	float norm = fabs(result[0] - resultPrev[0]);

	for (int i = 0; i < 3; i++)
	{
		if (fabs(result[i] - resultPrev[i]) > norm)
			norm = fabs(result[i] - resultPrev[i]);
		resultPrev[i] = result[i];
	}

	return norm;
}

void outputMiddleResultIteration(float* result, int counter)
{
	std::cout << "Solution(Iteration " << counter << "): " << result[0] << "\t" << result[1] << "\t" << result[2] << "\t" << std::endl;
}

float* iter(float** a, float* b, float precise)
{
	float norm = 0, divider = 1, maxDiff = 0;
	float* result = new float[3]{0, 0, 0};
	float* resultPrev = new float[3]{ 0, 0, 0 };
	float** newArr = new float*[3];
	int counter = 0;

	int maxIndex = 0;
	float** aLocal = new float*[3];
	float* bLocal = new float[3];
	for (int i = 0; i < 3; i++)
	{
		aLocal[i] = new float[3];
		for (int j = 0; j < 3; j++)
		{
			aLocal[i][j] = a[i][j];
		}
		bLocal[i] = b[i];
	}

	std::cout << "Start iteration method" << std::endl;
	/*Calculating norm (WTF!!!)*/
	for (int i = 0; i < 3; i++)
	{
		float sum = 0;
		for (int j = 0; j < 3; j++)
		{
			sum += fabs(aLocal[i][j]);
		}
		if (norm < sum)
			norm = sum;
	}
	std::cout << "Norm for iteration method: " << norm << std::endl;

	for (int i = 0; i < 3; i++)
	{
		/*Find maximal element in column*/
		maxIndex = findMaxIndex(aLocal, i);

		/*Swap lines*/
		swapLines(&aLocal, &bLocal, i, maxIndex);
	}


	do {
		counter++;
		for (int i = 0; i < 3; i++)
		{
			result[i] = bLocal[i];
			for (int j = 0; j < 3; j++)
			{
				if (i != j)
					result[i] -= aLocal[i][j] * resultPrev[j];
			}
			result[i] /= aLocal[i][i];
		}

		norm = calcNorm(result, resultPrev);

		outputMiddleResultIteration(result, counter);
	} while (norm > precise);

	std::cout << "Iterations: " << counter << std::endl << std::endl;

	return result;
}

void outputResult(std::string methodName, float* result)
{
	std::cout << "Solution(" << methodName << "): " << result[0] << "\t" << result[1] << "\t" << result[2] << "\t" << std::endl << std::endl;
}

int main()
{
	float** a = new float*[3];
	a[0] = new float[3]{ 2, 15, 1 };
	a[1] = new float[3]{12, 2, 3 };
	a[2] = new float[3]{1, 2, 16};
	float* b = new float[3]{35, 25, 53};
	float* solExact = new float[3]{ 1, 2, 3 };
	float* solGauss, *solIteration;

	outputResult("Exact", solExact);

	solGauss = gauss(a, b);
	outputResult("Gauss", solGauss);

	solIteration = iter(a, b, 0.1f);
	outputResult("Iterations", solIteration);

	system("pause");
	return 0;
}