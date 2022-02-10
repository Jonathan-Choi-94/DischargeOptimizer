#include "DataProcessor.h"


int GetHalf(int num);
doublematrix OddIndexEliminator(doublematrix data);
std::vector<double> ArrangeInflowData(std::vector<std::vector<std::string>>);

DataProcessor::DataProcessor()
{
	NCreader = new PythonNCreader();
}

DataProcessor::~DataProcessor()
{
	delete NCreader;
}

// This reads K-River input data from DB
bool DataProcessor::ReadKRiverInputData(PostgreSQLConnector* db_connector,
	std::string kdrum_imt_sn, std::string file_path)
{
	// Get imt_dt_vec
	std::vector<std::vector<std::string>> imt_dt_vec;
	std::string get_query_condition = "kdrum_imt_sn = " + kdrum_imt_sn + " and lc_no = 15136";
	imt_dt_vec = db_connector->GetData(
		"imt_dt", "tn_kriver_inpt_data", get_query_condition);
	std::string current_time, current_time_shortform;
	current_time = db_connector->GetNextHour(imt_dt_vec.at(0).at(0),
		std::to_string(g_dataarchive->start_time_index));
	g_dataarchive->opt_start_time = current_time.substr(0, 13);

	// Changing current time in short form to get initial reservoir level
	if (current_time.substr(11, 2) != "00")
	{
		current_time_shortform = "";
		current_time_shortform += current_time.substr(0, 4);
		current_time_shortform += current_time.substr(5, 2);
		current_time_shortform += current_time.substr(8, 2);
		current_time_shortform += current_time.substr(11, 2);
	}
	else
	{
		current_time = db_connector->GetNextHour(imt_dt_vec.at(0).at(0),
			std::to_string(g_dataarchive->start_time_index - 1));
		current_time_shortform = "";
		current_time_shortform += current_time.substr(0, 4);
		current_time_shortform += current_time.substr(5, 2);
		current_time_shortform += current_time.substr(8, 2);
		current_time_shortform += "24";
	}
	
	printf("Getting current time has succeeded \n");
	
	//====================================================================================
	// Get initial reservoir level
	std::map<std::string, std::vector<std::string>> dam_level_map;
	double initial_sjdam_level, initial_jadam_level;
	get_query_condition = "msmt_tm = '" + current_time_shortform + "'";
	dam_level_map = db_connector->GetDataMap(
		"dam_cd", "rsvr_wal", "ti_hdaps_hrdamif", get_query_condition);
	/*initial_sjdam_level = std::stod(dam_level_map.at("4001110").at(0));
	initial_jadam_level = std::stod(dam_level_map.at("4007110").at(0));*/
	initial_sjdam_level = 194.46;
	initial_jadam_level = 103.61;
	g_dataarchive->initial_sjdam_level = initial_sjdam_level;
	g_dataarchive->initial_jadam_level = initial_jadam_level;
	printf("Getting initial sjdam level has succeeded \n");

	//====================================================================================
	// Get reservoir inflow data
	printf("Getting sjdam inflow vec...");
	std::vector<std::vector<std::string>> sjdam_inflow_map;
	get_query_condition = "kdrum_imt_sn = " + kdrum_imt_sn + " and lc_no = 15136";
	sjdam_inflow_map = db_connector->GetData(
		"imt_dt, inflow", "tn_kriver_inpt_data", get_query_condition);
	std::vector<double> sjdam_inflow_vec = ArrangeInflowData(sjdam_inflow_map);
	g_dataarchive->sj_inflow = sjdam_inflow_vec;
	printf("succeeded \n");

	printf("Getting jadam inflow vec...");
	std::vector<std::vector<std::string>> jadam_inflow_map;
	get_query_condition = "kdrum_imt_sn = " + kdrum_imt_sn + " and lc_no = 63561";
	jadam_inflow_map = db_connector->GetData(
		"imt_dt, inflow", "tn_kriver_inpt_data", get_query_condition);
	std::vector<double> jadam_inflow_vec = ArrangeInflowData(jadam_inflow_map);
	g_dataarchive->ja_inflow = jadam_inflow_vec;
	printf("succeeded \n");


	//====================================================================================
	// Get the initial set discharge at Seomjin dam and Juam dam
	std::ifstream file_boundary(file_path + "\\sj_riv_boundary.txt");
	aria::csv::CsvParser parser0(file_boundary);
	int length_discharge_seomjin(241), length_discharge_juam(241);
	std::vector<double> initial_discharge_seomjin, initial_discharge_juam;
	std::vector<std::vector<double>> dam_discharge;
	int i = 0;
	for (auto& row : parser0)
	{
		if (i == 4)
			length_discharge_seomjin = std::stoi(row[0].substr(10, 14));
		else if (i >= 6 && i < 6 + length_discharge_seomjin)
			initial_discharge_seomjin.push_back(std::stod(row[0]));
		else if (i == 11 + length_discharge_seomjin)
			length_discharge_juam = std::stoi(row[0].substr(10, 14));
		else if (i >= 13 + length_discharge_seomjin && i < 13 + length_discharge_seomjin + length_discharge_juam)
			initial_discharge_juam.push_back(std::stod(row[0]));
		i++;
	}
	dam_discharge.push_back(initial_discharge_seomjin);
	dam_discharge.push_back(initial_discharge_juam);
	g_dataarchive->dataset_dam_discharge.push_back(dam_discharge);
	printf("Getting initial set discharge from sj_riv_boundary.txt has succeeded \n");
	
	//====================================================================================
	// Get tributary inflow from Check_Point.OUT
	std::ifstream file_inflow(file_path + "\\Check_Point.OUT");
	aria::csv::CsvParser parser1(file_inflow);
	i = 0;
	std::vector<int> lc_no_vec;
	std::string temp_str;
	std::vector<std::vector<double>> rainfall_vec(387);
	std::vector<std::vector<double>> inflow_vec(387);
	for (auto& row : parser1)
	{
		if (i == 0)
		{
			for (int j = 3; j < row.size(); j++)
			{
				if (j % 2)
				{
					temp_str = row[j].substr(1, 5);
					lc_no_vec.push_back(std::stoi(temp_str));
				}
			}
		}
		else if (i >= 2)
		{
			for (int j = 1; j < row.size(); j++)
			{
				if (j % 2)
				{
					rainfall_vec.at((j - 1) / 2).push_back(std::stod(row[j]));
				}
				else
				{
					inflow_vec.at((j - 2) / 2).push_back(std::stod(row[j]));
				}

			}
		}
		++i;
	}

	std::map<int, std::vector<std::vector<double>>> drum_map;
	std::vector<std::vector<double>> temp_combination(2);
	for (int i = 0; i < lc_no_vec.size(); i++)
	{
		temp_combination.at(0) = rainfall_vec.at(i);
		temp_combination.at(1) = inflow_vec.at(i);
		drum_map.insert(std::make_pair(lc_no_vec.at(i), temp_combination));
	}

	doublematrix river_inflow_vec;
	for (int i = 0; i < g_dataarchive->inflow_drum_vec.size(); i++)
	{
		river_inflow_vec.push_back(drum_map.at(g_dataarchive->inflow_drum_vec.at(i)).at(1));
	}

	g_dataarchive->dataset_Q_in.push_back(river_inflow_vec);
	printf("Getting river inflow vec from Check_Point.OUT has succeeded \n");
	//====================================================================================

	printf("Reading K-River input data in program has succeeded \n");
	return true;
}

// This reads K-River output data from .nc file
bool DataProcessor::ReadKRiverOutputData(std::string file_path)
{
	doublematrix level_true, q_flow_true;
	std::vector<doublematrix> dataset_;
	
	if (!NCreader->OutputRead(file_path + "\\sj_riv.nc", dataset_))
	{
		throw std::runtime_error("[DataProcessor::DataRead] Error during NCread");
	}

	// Reading q_flow & level_true
	q_flow_true = g_dataarchive->matrix_dmnset;
	level_true = g_dataarchive->matrix_dmnset;
	for (int i = 0; i < g_dataarchive->IndexStationEqualstoPP.size(); i++)
	{
		q_flow_true.at(i) = dataset_.at(0).at(g_dataarchive->IndexStationEqualstoPP.at(i));
		level_true.at(i) = dataset_.at(1).at(g_dataarchive->IndexStationEqualstoPP.at(i));
	}

	// Adding the read data into the DataArchive
	g_dataarchive->dataset_q_flow_true.push_back(q_flow_true);
	g_dataarchive->dataset_level_true.push_back(level_true);

	printf("Reading K-River output data in program has succeeded \n");
	return true;
}

// This initialization initializes whole model with set discharge
bool DataProcessor::InitializeWholeModel(UdeCoupModel* pred_model, int& dataset)
{
	// Defining initial inputs
	std::vector<double> init_Q_in;
	double init_qflow, init_level;

	// Initializing state of every prediction model
	for (int i = 0; i < g_dataarchive->IndexInflowEqualstoPP.size(); i++)
	{
		if (g_dataarchive->IndexStationEqualstoPP.at(i) < g_dataarchive->station_joining)
		{
			// Setting the initial discharge inflence at the predict_point 
			init_Q_in.push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(0)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(0)));
			// Setting upstream tributary inflow & this_point inflow at the predict_point 
			for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 1 + 1; j++)
			{
				// Setting the initial tributary inflow inflence at the predict_point
				init_Q_in.push_back(g_dataarchive->dataset_Q_in.at(dataset).at(j)
					.at(g_dataarchive->start_time_index - g_dataarchive->delay.at(i).at(j)));
			}
		}
		else
		{
			// Setting the initial discharge inflence at the predict_point 
			init_Q_in.push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(0)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(0)));
			init_Q_in.push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(1)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(1)));
			// Setting upstream tributary inflow & this_point inflow at the predict_point
			for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 2 + 1; j++)
			{
				// Setting the initial tributary inflow inflence at the predict_point
				init_Q_in.push_back(g_dataarchive->dataset_Q_in.at(dataset).at(j)
					.at(g_dataarchive->start_time_index - g_dataarchive->delay.at(i).at(j)));
			}
		}
		
		// Setting initial state
		init_qflow = g_dataarchive->dataset_q_flow_true.at(dataset).at(i).
			at(g_dataarchive->start_time_index);
		init_level = g_dataarchive->dataset_level_true.at(dataset).at(i).
			at(g_dataarchive->start_time_index);

		// Initializing models
		ReservoirPredictionModel* initreservoirset = 
			(ReservoirPredictionModel*)pred_model->child_models_.at(0);
		LinearPredictionModel* initmodelset =
			(LinearPredictionModel*)pred_model->child_models_.at(1 + i);
		initreservoirset->Initialize(dataset);
		initmodelset->InitializeInput(dataset);
		initmodelset->SetEventCriteria(g_dataarchive->event_criteria.at(i));
		initmodelset->SetWholeParam(g_dataarchive->param_trained.at(i));
		initmodelset->InitializeState(init_Q_in, init_qflow, init_level);

		init_Q_in.clear();
	}

	return true;
}

// This initialization is for the discharge optimization with both sj and ja 
bool DataProcessor::InitializeWholeModel(UdeCoupModel* pred_model, int& dataset, doublematrix new_discharge)
{
	// Defining initial inputs 
	std::vector<double> init_Q_in;
	double init_qflow, init_level;

	std::vector<double> v;
	doublematrix discharge_for_update;
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		discharge_for_update.push_back(v);
		for (int j = 0; j < g_dataarchive->non_optimizing_duration; j++)
		{
			discharge_for_update.at(i).push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(i).at(j));
		}
		for (int j = 0; j < g_dataarchive->simulation_duration; j++)
		{
			discharge_for_update.at(i).push_back(new_discharge.at(i).at(j));
		}
	}

	// Initializing state of every prediction model
	for (int i = 0; i < g_dataarchive->IndexInflowEqualstoPP.size(); i++)
	{
		if (g_dataarchive->IndexStationEqualstoPP.at(i) < g_dataarchive->station_joining)
		{
			// Setting the initial discharge inflence at the predict_point 
			init_Q_in.push_back(discharge_for_update.at(0)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(0)));
			// Setting upstream tributary inflow & this_point inflow at the predict_point 
			for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 1 + 1; j++)
			{
				// Setting the initial tributary inflow inflence at the predict_point
				init_Q_in.push_back(g_dataarchive->dataset_Q_in.at(dataset).at(j)
					.at(g_dataarchive->start_time_index - g_dataarchive->delay.at(i).at(j)));
			}
		}
		else
		{
			// Setting the initial discharge inflence at the predict_point 
			init_Q_in.push_back(discharge_for_update.at(0)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(0)));
			init_Q_in.push_back(discharge_for_update.at(1)
				.at(g_dataarchive->start_time_index - g_dataarchive->delay_dam.at(i).at(1)));
			// Setting upstream tributary inflow & this_point inflow at the predict_point
			for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 2 + 1; j++)
			{
				// Setting the initial tributary inflow inflence at the predict_point
				init_Q_in.push_back(g_dataarchive->dataset_Q_in.at(0).at(j)
					.at(g_dataarchive->start_time_index - g_dataarchive->delay.at(i).at(j)));
			}
		}

		// Setting initial state
		init_qflow = g_dataarchive->dataset_q_flow_true.at(dataset).at(i).at(g_dataarchive->start_time_index);
		init_level = g_dataarchive->dataset_level_true.at(dataset).at(i).at(g_dataarchive->start_time_index);

		// Initializing models
		ReservoirPredictionModel* initreservoirset =
			(ReservoirPredictionModel*)pred_model->child_models_.at(0);
		LinearPredictionModel* initmodelset =
			(LinearPredictionModel*)pred_model->child_models_.at(1 + i);
		initreservoirset->Initialize(dataset, discharge_for_update);
		initmodelset->InitializeInput(dataset, discharge_for_update);
		initmodelset->SetEventCriteria(g_dataarchive->event_criteria.at(i));
		initmodelset->SetWholeParam(g_dataarchive->param_trained.at(i));
		initmodelset->InitializeState(init_Q_in, init_qflow, init_level);

		init_Q_in.clear();
	}

	return true;
}

doublematrix DataProcessor::Interpolation(doublematrix data, int num_divide)
{
	std::vector<double> v;
	doublematrix data_ip;
	for (int i = 0; i < data.size(); i++)
	{
		data_ip.push_back(v);
		if (data.at(i).size() != 0)
		{
			for (int t = 0; t < data.at(i).size() - 1; t++)
			{
				for (int j = 0; j < num_divide; j++)
				{
					data_ip.at(i).push_back(
						((num_divide - j) * data.at(i).at(t) + j * data.at(i).at(t + 1))
						/ num_divide
					);
				}
			}
			data_ip.at(i).push_back(data.at(i).back());
		}

	}

	return data_ip;
}

doublematrix OddIndexEliminator(doublematrix data)
{
	int point = data.size();
	
	for (int j = 0; j < point; j++)
	{
		int original_num_time = data.at(j).size();
		// Erasing the element with odd indices (num_element : 241->121)
		for (int i = 1; i < 1 + GetHalf(original_num_time); i++)
		{
			data.at(j).erase(data.at(j).begin() + i);
		}
	}

	return data;
}


int GetHalf(int num)
{
	int num_return;
	if (num % 2)
	{
		num_return = (num - 1) / 2;
	}
	else
	{
		num_return = num / 2;
	}

	return num_return;
}

std::vector<double> ArrangeInflowData(std::vector<std::vector<std::string>> river_inflow_map)
{
	std::vector<double> inflow_vec;
	for (std::string inflow : river_inflow_map.at(1))
		inflow_vec.push_back(std::stod(inflow));

	return inflow_vec;
}