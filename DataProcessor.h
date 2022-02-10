#pragma once
#include <vector>
#include <string>
#include "DataArchive.h"
#include "PredPointCoupledModel.h"
#include "PythonNCreader.h"
#include "PostgreSQLConnector.h"
#include "parser.hpp"

class DataProcessor
{
public:
	DataProcessor();
	virtual ~DataProcessor();

	bool ReadKRiverInputData(PostgreSQLConnector* db_connector,
		std::string kdrum_imt_sn, std::string file_path);
	bool ReadKRiverOutputData(std::string file_path);
	bool InitializeWholeModel(UdeCoupModel* pred_model, int& dataset);
	bool InitializeWholeModel(UdeCoupModel* pred_model, int& dataset, doublematrix new_discharge);
	doublematrix Interpolation(doublematrix data, int num_divide);

	PythonNCreader* NCreader;
};

extern DataProcessor* g_dataprocessor;
