#include "ModelTrainToolBox.h"

double NSE_Calculator(std::vector<double> Q_m, std::vector<double> Q_o);

ModelTrainToolBox::ModelTrainToolBox()
{	
}

ModelTrainToolBox::~ModelTrainToolBox()
{
}

bool ModelTrainToolBox::CheckFlowModelNSE(int dataset)
{
	std::vector<int> index_time = g_dataarchive->index_wholetime;
	double time_step = g_dataarchive->time_step;

	doublematrix q_flow_true, q_flow_pred;
	std::vector<double> v, NSE;

	// getting true level within index_time
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		q_flow_true.push_back(v);
		for (int j : index_time)
		{
			q_flow_true.at(i).push_back(g_dataarchive->dataset_q_flow_true
				.at(dataset).at(i).at(j));
		}
	}

	UDEIF->Reset();
	UdeCoupModel* pred_model = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(pred_model, dataset);
	UDEIF->SetModel(pred_model);

	//set desired simulation time step
	UDEIF->GetCsEngine()->SetSolverTimeStep(time_step);

	//show I/O and coupling information
	UDEIF->ShowModelInfo();

	//run simulation
	UDEIF->Start(time_step * (index_time.size() - 1));

	//print the log to console
	CLOG_PTR->info("simulation ends...");


	// getting pred level and compare true level and pred level
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		LinearPredictionModel* get_result =
			(LinearPredictionModel*)pred_model->child_models_.at(1 + i);
		q_flow_pred.push_back(get_result->result_qflow);

		NSE.push_back(NSE_Calculator(q_flow_pred.at(i), q_flow_true.at(i)));
		std::cout << "Flow model NSE at predict point " << i << " is " << NSE.at(i) << std::endl;
	}

	WriteInFile("Simulation Result_q_flow_true.csv", q_flow_true);
	WriteInFile("Simulation Result_q_flow_pred.csv", q_flow_pred);

	return true;
}

bool ModelTrainToolBox::CheckWholeModelNSE(int dataset)
{
	std::vector<int> index_time = g_dataarchive->index_wholetime;
	double time_step = g_dataarchive->time_step;

	doublematrix level_true, level_pred;
	std::vector<double> v, NSE;

	// getting true level within index_time
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		level_true.push_back(v);
		for (int j : index_time)
		{
			level_true.at(i).push_back(g_dataarchive->dataset_level_true
				.at(dataset).at(i).at(j));
		}
	}

	UDEIF->Reset();
	UdeCoupModel* pred_model = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(pred_model, dataset);
	UDEIF->SetModel(pred_model);

	//set desired simulation time step
	UDEIF->GetCsEngine()->SetSolverTimeStep(time_step);

	//show I/O and coupling information
	UDEIF->ShowModelInfo();

	//run simulation
	UDEIF->Start(time_step * (index_time.size() - 1));

	//print the log to console
	CLOG_PTR->info("simulation ends...");


	doublematrix q_flow_pred;
	g_dataarchive->train_points.clear();
	// getting pred level and compare true level and pred level
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		LinearPredictionModel* get_result = 
			(LinearPredictionModel*)pred_model->child_models_.at(1 + i);
		level_pred.push_back(get_result->result_level);
		q_flow_pred.push_back(get_result->result_qflow);

		NSE.push_back(NSE_Calculator(level_pred.at(i), level_true.at(i)));
		std::cout << "Whole model NSE at predict point " << i << " is " << NSE.at(i) << std::endl;
		if (NSE.at(i) < 0.90)
		{
			g_dataarchive->train_points.push_back(i);
		}
	}

	printf("\n");
	for (int point: g_dataarchive->train_points)
		std::cout << "Train is needed at point" << point << std::endl;
	printf("\n");

	WriteInFile("Simulation Result_level_true.csv", level_true);
	WriteInFile("Simulation Result_level_pred.csv", level_pred);

	return true;
}


// 211016 Y.Choi
// Train flow prediction model with the simple linear regression method 
bool ModelTrainToolBox::TrainFlowModel(std::vector<int>& train_points,
	std::vector<int>& indices_dataset, int& train_interpolation)
{
	LinearRegression linearregression = LinearRegression();
	for (int i : train_points)
	{
		//===============================================================//
		/* ====================== train each model ======================*/
		//===============================================================//
		std::vector<double> flow_true_;
		std::vector<std::vector<double>> inflow(g_dataarchive->num_flow_model_weight_vec.at(i));
		for (int dataset : indices_dataset)
		{
			for (int ti : g_dataarchive->index_wholetime)
			{
				flow_true_.push_back(
					g_dataarchive->dataset_q_flow_true.at(dataset).at(i).at(ti));
				if (g_dataarchive->IndexStationEqualstoPP.at(i) < g_dataarchive->station_joining)
				{
					inflow.at(0).push_back(
						g_dataarchive->dataset_dam_discharge.at(dataset).at(0).
						at(ti - g_dataarchive->delay_dam.at(i).at(0)));
					for (int p = 0; p < inflow.size() - 1; p++)
					{
						inflow.at(p + 1).push_back(
							g_dataarchive->dataset_Q_in.at(dataset).at(p).
							at(ti - g_dataarchive->delay.at(i).at(p)));
					}
				}
				else
				{
					inflow.at(0).push_back(
						g_dataarchive->dataset_dam_discharge.at(dataset).at(0).
						at(ti - g_dataarchive->delay_dam.at(i).at(0)));
					inflow.at(1).push_back(
						g_dataarchive->dataset_dam_discharge.at(dataset).at(1).
						at(ti - g_dataarchive->delay_dam.at(i).at(1)));
					for (int p = 0; p < inflow.size() - 2; p++)
					{
						inflow.at(p + 2).push_back(
							g_dataarchive->dataset_Q_in.at(dataset).at(p).
							at(ti - g_dataarchive->delay.at(i).at(p)));
					}
				}
			}
		}

		std::vector<double> param_;
		linearregression.ReadData(flow_true_, inflow);
		param_ = linearregression.GetModelParams();

		g_dataarchive->UpdateParam_flow(param_, i);
	}

	printf("Training flow model has succeeded \n");
	return true;
}


bool ModelTrainToolBox::TrainLevelModel(std::vector<int>& train_points,
	std::vector<int>& indices_dataset, int& train_interpolation)
{
	LinearRegression linearregression = LinearRegression();
	std::map<int, std::vector<std::vector<int>>> timegroup_set;
	std::vector<std::vector<double>> training_input(1);
	std::vector<double> training_output;

	for (int point : train_points)
	{
		// Getting the total number of the submodel at each point
		int total_submodel = 1 + g_dataarchive->event_criteria.at(point).size();
		for (int dataset : indices_dataset)
		{
			std::vector<std::vector<int>> timeindex_group(total_submodel);
			ClassifyTimeIndex(dataset, point, total_submodel, timeindex_group);
			timegroup_set.insert({ dataset, timeindex_group });
		}


		//===============================================================//
		/* ==================== train each sub-model ====================*/
		//===============================================================//
		for (int submodel = 0;
			submodel < total_submodel; submodel++)
		{
			int exist_or_not = 0;
			std::map<int, std::vector<int>> indices_time;
			for (int dataset : indices_dataset)
			{
				indices_time.insert({ dataset, timegroup_set.at(dataset).at(submodel) });
				exist_or_not += !(indices_time.at(dataset).empty());
			}
			// if index_time is not empty, run the model training
			if (exist_or_not > 0)
			{
				for (int dataset : indices_dataset)
				{
					for (int ti : indices_time.at(dataset))
					{
						training_output.push_back(
							g_dataarchive->dataset_level_true.at(dataset).at(point).at(ti));
						training_input.at(0).push_back(
							g_dataarchive->dataset_q_flow_true.at(dataset).at(point).at(ti));
					}
				}
				
				// Training submodel
				std::vector<double> param_;
				linearregression.ReadData(training_output, training_input);
				param_ = linearregression.GetModelParams();
				g_dataarchive->UpdateParam_level(param_, point, submodel);

				training_output.clear();
				training_input.at(0).clear();
				indices_time.clear();
			}
		}

		timegroup_set.clear();
	}

	printf("Training level model has succeeded \n");
	return true;
}


bool ModelTrainToolBox::WriteInFile(std::string file_name, doublematrix data)
{
	std::ofstream file;
	file.open(file_name);
	for (int i = 0; i < data.at(0).size(); i++)
	{
		for (int j = 0; j < data.size(); j++)
		{
			file << data.at(j).at(i) << ",";
		}
		file << "\n";
	}
	file.close();

	return true;
}

// Classifying the time index belongs to each sub-model
bool ModelTrainToolBox::ClassifyTimeIndex(int& dataset, int& point, int& total_submodel,
	std::vector<std::vector<int>>& timeindex_group)
{
	std::vector<double> level_vec = g_dataarchive->dataset_level_true.at(dataset).at(point);

	// distinguish whether level at each time step belongs to event_criteria
	for (int j = g_dataarchive->delay_max;
		j < level_vec.size(); j++)
	{
		// First(0) group assigned
		if (level_vec.at(j) <
			g_dataarchive->event_criteria.at(point).at(0))
		{
			timeindex_group.at(0).push_back(j);
		}
		// Last(.back) group assigned
		else if (level_vec.at(j) >=
			g_dataarchive->event_criteria.at(point).back())
		{
			timeindex_group.back().push_back(j);
		}
		// else groups assigned
		else
		{
			for (int submodel = 1; submodel < total_submodel - 1;
				submodel++)
			{
				if (level_vec.at(j) >=
					g_dataarchive->event_criteria.at(point).at(submodel - 1) &&
					level_vec.at(j) <
					g_dataarchive->event_criteria.at(point).at(submodel))
				{
					timeindex_group.at(submodel).push_back(j);
				}
			}
		}
	}

	return true;
}

double NSE_Calculator(std::vector<double> Q_m, std::vector<double> Q_o)
{
	std::vector<double> numerator, denominator;
	double Q_o_average = std::accumulate(Q_o.begin(), Q_o.end(), 0.0)/Q_o.size();
	double NSE = 0;
	
	if (Q_m.size() == Q_o.size())
	{
		for (int i = 0; i < Q_m.size(); i++)
		{
			numerator.push_back(pow(Q_m.at(i) - Q_o.at(i), 2));
			denominator.push_back(pow(Q_o.at(i) - Q_o_average, 2));
		}
		NSE = 1 - std::accumulate(numerator.begin(), numerator.end(), 0.0) /
			std::accumulate(denominator.begin(), denominator.end(), 0.0);
	}
	else
	{
		throw std::runtime_error("[ModelTrainToolBox::NSE_Caclulator] Different size between true and pred");
	}

	return NSE;
}