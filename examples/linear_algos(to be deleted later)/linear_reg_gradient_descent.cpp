#include <bits/stdc++.h>
#define NUM_ITERATIONS 1500
#define ALPHA 0.01
#define PARAMETERS 2

using namespace std;

double dot_product(double x_com[PARAMETERS], double theta[PARAMETERS])
{
	double sum = 0;
	for(int i=0; i<PARAMETERS; i++)
	{
		sum += x_com[i]*theta[i];
	}
	return sum;
}

double compute_cost(double x_com[100][PARAMETERS], double y_com[100], double theta[PARAMETERS], int num_elements)
{
	double prediction = 0, sqErrors = 0, sum = 0, sqError = 0;

	for(int i=0; i<num_elements; i++)
	{
		prediction = dot_product(x_com[i], theta);
		sqError = prediction - y_com[i];
		sum += sqError*sqError;
	}
	sum = sum/(2*num_elements);
	return sum;
}

void gradient_descent(double x_com[100][PARAMETERS], double y_com[100], double theta[PARAMETERS], int num_elements, double cost[NUM_ITERATIONS])
{
	double prediction = 0, errors[PARAMETERS] = {0}, error_sum[PARAMETERS] = {0};

	for(int j=0; j<NUM_ITERATIONS; j++)
	{
		for(int i=0; i<num_elements; i++)
		{
			prediction = dot_product(x_com[i],theta) - y_com[i];
			for(int k=0; k<PARAMETERS; k++)
			{
				error_sum[k] += prediction*x_com[i][k];
			}
		}

		for(int k=0; k<PARAMETERS; k++)
		{
			theta[k] = theta[k] - (ALPHA*error_sum[k])/num_elements;
		}

		cost[j] = compute_cost(x_com, y_com, theta, num_elements);
	}
}

int main()
{
	double x_com[100][PARAMETERS] = {0}, y_com[100] = {0}, theta[PARAMETERS] = {0}, cost[NUM_ITERATIONS] = {0};
	int num_elements = 0;

	cout << "Enter the number of elements" << endl;
	cin >> num_elements;

	cout << "Enter the x components" << endl;

	for(int j=0; j<PARAMETERS-1; j++)
	{
		for(int i=0; i< num_elements; i++)
			cin >> x_com[i][j];
	}

	cout << "Enter the y components" << endl;

	for(int i=0; i< num_elements; i++)
		cin >> y_com[i];

	// intialising the constant array
	for(int i=0;i< num_elements; i++)
		x_com[i][PARAMETERS-1] = 1;

	gradient_descent(x_com, y_com, theta, num_elements, cost);
}