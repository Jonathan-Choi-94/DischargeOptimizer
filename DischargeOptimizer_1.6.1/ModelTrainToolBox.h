#pragma once
#include <iostream>
#include <numeric>
#include <fstream>
#include "OptLogger.h"
#include "UdeEngine.h"
#include "DataArchive.h"
#include "DataProcessor.h"
#include "PredPointCoupledModel.h"
#include "LinearPredictionModel.h"
#include "LinearRegression.h"

class ModelTrainToolBox
{
public:
	ModelTrainToolBox();
	virtual ~ModelTrainToolBox();
	
	bool CheckFlowModelNSE(int dataset);
	bool CheckWholeModelNSE(int dataset);
	bool TrainFlowModel(std::vector<int>& train_points, std::vector<int>& indices_dataset, int& train_interpolation);
	bool TrainLevelModel(std::vector<int>& train_points, std::vector<int>& indices_dataset, int& train_interpolation);
	bool WriteInFile(std::string, doublematrix);
	bool ClassifyTimeIndex(int& dataset, int& point, int& total_submodel,
		std::vector<std::vector<int>>& timeindex_group);
};