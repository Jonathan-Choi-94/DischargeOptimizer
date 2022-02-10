#pragma once
#include <Eigen/Dense>
#include "parser.hpp"

using Eigen::MatrixXd;

class LinearRegression
{
private:
	MatrixXd y, x;

public:
	LinearRegression();
	virtual ~LinearRegression();

	void ReadData(std::vector<double>, std::vector<std::vector<double>>);
	std::vector<double> GetModelParams();
	
};