#include "LinearRegression.h"
#include <iostream>


LinearRegression::LinearRegression()
{
}

LinearRegression::~LinearRegression()
{
}

void LinearRegression::ReadData(std::vector<double> y_vec, std::vector<std::vector<double>> x_vec)
{
	y.resize(y_vec.size(), 1);
	x.resize(y_vec.size(), x_vec.size() + 1);

	for (int i = 0; i < y.rows(); i++)
		y(i) = y_vec.at(i);

	for (int j = 0; j < x.cols(); j++)
	{
		if (j != 0)
		{
			for (int i = 0; i < x.rows(); i++)
				x(i, j) = x_vec.at(j - 1).at(i);
		}
		else
		{
			for (int i = 0; i < x.rows(); i++)
				x(i, j) = 1;
		}
		
	}
}

std::vector<double> LinearRegression::GetModelParams()
{
	MatrixXd coefficients(x.cols(), 1);
	coefficients = (x.transpose() * x).inverse() * x.transpose() * y;

	std::vector<double> params(coefficients.size());
	for (int i = 0; i < coefficients.size() - 1; i++)
	{
		params.at(i) = coefficients(i + 1);
	}
	params.back() = coefficients(0);
	
	return params;
}