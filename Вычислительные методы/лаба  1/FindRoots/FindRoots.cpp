// FindRoots.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

double Power(double x, int pow)
{
	double result = 0;

	for (int i = 1; i <= pow; i++)
	{
		if (i == 1) result = 1;

		result *= x;
	}

	return result;
}

double Function(double x)
{
	return -Power(x, 4) + 15 * Power(x, 2) + 12 * x - 10;
}

double DFunction(double x)
{
	return -4 * Power(x, 3) + 30 * x + 12;
}

double DDFunction(double x)
{
	return -12 * Power(x, 2) + 30;
}

double Phi(double x, double lambda)
{
	return x + lambda * Function(x);
}

double Newton(double a, double b, double precision)
{
	double solution, prevSolution;
	int counter = 0;

	if (Function(a) * DDFunction(a) > 0) solution = a;
	else solution = b;

	do
	{
		counter++;
		prevSolution = solution;
		solution = solution - Function(solution) / DFunction(solution);
	} while (abs(solution - prevSolution) >= precision);

	std::cout << "Iterations: " << counter << std::endl;


	return solution;
}

double Iteration(double a, double b, double precision)
{
	double solution = a, prevSolution, lambda = -1/45.0;
	int counter = 0;

	if (Function(a) > Function(b)) lambda *= -1;

	do
	{
		counter++;
		prevSolution = solution;
		solution = Phi(solution, lambda);
	} while (abs(solution - prevSolution) >= precision);

	std::cout << "Iterations: " << counter << std::endl;

	return solution;
}

double Chords(double a, double b, double precision)
{
	double solution, prevSolution, commonPoint;
	int counter = 0;

	if (Function(a) * DDFunction(a) >= 0)
	{
		commonPoint = a;
		solution = b;
	}
	else
	{
		commonPoint = b;
		solution = a;
	}

	do
	{ 
		counter++;
		prevSolution = solution;
		solution = solution - (commonPoint - solution) * Function(solution) / (Function(commonPoint) - Function(solution));
	} while (abs(solution - prevSolution) >= precision);

	std::cout << "Iterations: " << counter << std::endl;

	return solution;
}

int main()
{
	double precision = 0.01;

	std::cout << "Precalculated roots: -3.207, -1.465, 0.512, 4.160" << std::endl << std::endl;

	std::cout << "Newton method, precision = " << precision << std::endl;
	std::cout << "Root 1: " << Newton(-4, -3, precision) << std::endl;
	std::cout << "Root 2: " << Newton(-1.5, -1, precision) << std::endl;
	std::cout << "Root 3: " << Newton(-1, 1, precision) << std::endl;
	std::cout << "Root 4: " << Newton(4, 5, precision) << std::endl << std::endl;

	std::cout << "Iteration method, precision = " << precision << std::endl;
	std::cout << "Root 1: " << Iteration(-4, -3, precision) << std::endl;
	std::cout << "Root 2: " << Iteration(-1.5, -1, precision) << std::endl;
	std::cout << "Root 3: " << Iteration(-1, 1, precision) << std::endl;
	std::cout << "Root 4: " << Iteration(4, 5, precision) << std::endl << std::endl;

	std::cout << "Chords method, precision = " << precision << std::endl;
	std::cout << "Root 1: " << Chords(-4, -3, precision) << std::endl;
	std::cout << "Root 2: " << Chords(-1.5, -1, precision) << std::endl;
	std::cout << "Root 3: " << Chords(-1, 1, precision) << std::endl;
	std::cout << "Root 4: " << Chords(4, 5, precision) << std::endl << std::endl;
}