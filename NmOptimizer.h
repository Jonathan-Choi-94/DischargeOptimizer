#pragma once
#include "NmSimplex.h"
#include "UdeEngine.h"
#include "OptLogger.h"
#include "DataProcessor.h"

class NmOptimizer final : public NmSimplex
{
public:
	NmOptimizer(const double& scale, const double& epsilon, const int& max_iteration,
		double& sj_total_discharge, double& ja_total_discharge,
		std::vector<double>& sj_initshort_discharge, std::vector<double>& ja_initshort_discharge,
		double& discharge_constraint);
	virtual ~NmOptimizer();

	bool PreOptProcess() override;
	void InitFunc(double* params) override;
	void EvaluateFunc() override;
	double GetMeanSquareError() override;
	void RunSimulation(UdeCoupModel* model);
	void CalculateDangerScore();
	std::vector<double> GetOptimizedSJDischarge();
	std::vector<double> GetOptimizedJADischarge();
	std::vector<double> ExpandDischarge(std::vector<double> discharge_short);

	int length_discharge_, length_timeindex_;
	double original_total, danger_score,
		sj_original_total, ja_original_total;
	std::vector<double> param_, sj_new_discharge, ja_new_discharge, danger_score_vec,
		max_level_vec, sj_level, ja_level;
	
	doublematrix new_discharge, reservoir_level;
};