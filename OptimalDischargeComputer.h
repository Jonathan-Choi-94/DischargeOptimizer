#pragma once
#include "UdeEngine.h"
#include "DataArchive.h"
#include "DataProcessor.h"
#include "NmOptimizer.h"
#include <iostream>
#include <numeric>
#include <fstream>
#include "PostgreSQLConnector.h"

class OptimalDischargeComputer
{
public:
	OptimalDischargeComputer();
	virtual ~OptimalDischargeComputer();
	
	void Run();
	void Optimize(int num_mode);
	void WriteResultFile();
	void PrintOutToDB(PostgreSQLConnector*);
	
	
	int length_discharge_, num_iter, discharge_period;
	double scale, discharge_constraint, taken_time;

	std::vector<double> 
		sj_original_full_discharge, ja_original_full_discharge,
		sj_original_total_discharge, sj_initset_total_discharge, sj_optimized_total_discharge,
		ja_original_total_discharge, ja_initset_total_discharge, ja_optimized_total_discharge,
		original_danger_score, initset_danger_score, optimized_danger_score;

	doublematrix 
		sj_initshort_discharge, ja_initshort_discharge,
		sj_original_discharge, ja_original_discharge,
		sj_initset_discharge, ja_initset_discharge,
		sj_optimized_discharge, ja_optimized_discharge,
		sj_reservoir_level_vec, ja_reservoir_level_vec,
		original_danger_vec, initset_danger_vec, optimized_danger_vec;
};