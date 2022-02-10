#include "OptimalDischargeComputer.h"

OptimalDischargeComputer::OptimalDischargeComputer()
{
	//==================================================================================
	// 	   variables (default)
	//==================================================================================
	// The number of discharge data
	length_discharge_ = 20;
	// Change scale of the discharge rate (every iteration)
	scale = 800;
	// Total number of iteration times
	num_iter = 1000;
	// Constraint for changing discharge (if it is 300, the discharge can change with in +- 300)
	discharge_constraint = 800;

	// discharge period in time index form
	discharge_period = (g_dataarchive->simulation_duration - 1) / length_discharge_;

	// Initializing member variables
	std::vector<double> v;
	for (int i = 0; i < 3; i++)
	{
		sj_initshort_discharge.push_back(v);
		sj_original_discharge.push_back(v);
		sj_initset_discharge.push_back(v);
		sj_optimized_discharge.push_back(v);
		sj_reservoir_level_vec.push_back(v);
		ja_initshort_discharge.push_back(v);
		ja_original_discharge.push_back(v);
		ja_initset_discharge.push_back(v);
		ja_optimized_discharge.push_back(v);
		ja_reservoir_level_vec.push_back(v);

		original_danger_vec.push_back(v);
		initset_danger_vec.push_back(v);
		optimized_danger_vec.push_back(v);

		// Setting total discharges of SJ and JA with three scenarios
		sj_original_total_discharge.push_back(0.0);
		ja_original_total_discharge.push_back(0.0);
		original_danger_score.push_back(0.0);
		initset_danger_score.push_back(0.0);
		optimized_danger_score.push_back(0.0);
	}
}

OptimalDischargeComputer::~OptimalDischargeComputer()
{
}

void OptimalDischargeComputer::Run()
{
	//std::string change_variable_or_not = "N";
	//std::cout << "Do you change variables? (Y/N) (default = N) ";
	//std::cin >> change_variable_or_not;

	//==================================================================================
	// 	   variables to change
	//==================================================================================
	//if (change_variable_or_not == "Y")
	//{
	//	// The number of discharge data
	//	std::cout << "Discharge data length? (default = 10) ";
	//	if (!(std::cin >> length_discharge_))
	//		throw std::runtime_error("[OptimalDischargeComputer] Wrong discharge length format");
	//	// Change scale of the discharge rate (every iteration)
	//	std::cout << "Change scale? (default = 100) ";
	//	if (!(std::cin >> scale))
	//		throw std::runtime_error("[OptimalDischargeComputer] Wrong change scale format");
	//	// Total number of iteration times
	//	std::cout << "Iteration times? (default = 1000) ";
	//	if (!(std::cin >> num_iter))
	//		throw std::runtime_error("[OptimalDischargeComputer] Wrong iteration time format");
	//	// Constraint for changing discharge (if it is 300, the discharge can change with in +- 300)
	//	std::cout << "Discharge constraint? (default = 300) ";
	//	if (!(std::cin >> discharge_constraint))
	//		throw std::runtime_error("[OptimalDischargeComputer] Wrong discharge constraint format");
	//	// Setting initial discharge as trapezoid shape or not
	//	std::cout << "Trapezoid mode on or off? (default = off) ";
	//}	
	//==================================================================================

	// Setting original discharge (default)
	sj_original_full_discharge = g_dataarchive->dataset_dam_discharge
		.at(g_dataarchive->default_dataset).at(0);
	ja_original_full_discharge = g_dataarchive->dataset_dam_discharge
		.at(g_dataarchive->default_dataset).at(1);

	// Setting original discharge with 0: conservative, 1: moderate, 2: aggressive scenario
	for (int i = g_dataarchive->start_time_index; i < 241; i++)
	{
		sj_original_discharge.at(0).push_back(sj_original_full_discharge.at(i) * 1.1);
		sj_original_discharge.at(1).push_back(sj_original_full_discharge.at(i) * 1.0);
		sj_original_discharge.at(2).push_back(sj_original_full_discharge.at(i) * 0.9);
		ja_original_discharge.at(0).push_back(ja_original_full_discharge.at(i) * 1.1);
		ja_original_discharge.at(1).push_back(ja_original_full_discharge.at(i) * 1.0);
		ja_original_discharge.at(2).push_back(ja_original_full_discharge.at(i) * 0.9);
	}
	
	// Setting the initshort discharge from originally set data
	for (int i = 0; i < length_discharge_; i++)
	{	 
		sj_initshort_discharge.at(0).push_back(sj_original_discharge.at(0).at(i * discharge_period));
		sj_initshort_discharge.at(1).push_back(sj_original_discharge.at(1).at(i * discharge_period));
		sj_initshort_discharge.at(2).push_back(sj_original_discharge.at(2).at(i * discharge_period));
		ja_initshort_discharge.at(0).push_back(ja_original_discharge.at(0).at(i * discharge_period));
		ja_initshort_discharge.at(1).push_back(ja_original_discharge.at(1).at(i * discharge_period));
		ja_initshort_discharge.at(2).push_back(ja_original_discharge.at(2).at(i * discharge_period));
	}

	// Calculate original total discharges (0: conservative, 1: moderate, 2: aggressive)
	for (int i = 0; i < 3; i++)
	{ 
		sj_original_total_discharge.at(i) = std::accumulate(
			sj_original_discharge.at(i).begin(), sj_original_discharge.at(i).end(), 0.0) * 3600;
		ja_original_total_discharge.at(i) = std::accumulate(
			ja_original_discharge.at(i).begin(), ja_original_discharge.at(i).end(), 0.0) * 3600;
	}

	// Do optimization for three times (0: conservative, 1: moderate, 2: aggressive)
	clock_t start, end;
	start = clock();
	for (int i = 0; i < 3; i++)
	{
		Optimize(i);
	}
	end = clock();
	taken_time = static_cast<double>(end - start) / 1000;
	std::cout << "최적화 3번 수행하는데에 걸린 시간[s] : " << taken_time << std::endl;
	

	// Calculate initset, optimized total discharges (0: conservative, 1: moderate, 2: aggressive)
	for (int i = 0; i < 3; i++)
	{
		sj_initset_total_discharge.push_back(std::accumulate(
			sj_initset_discharge.at(i).begin(), sj_initset_discharge.at(i).end(), 0.0) * 3600);
		sj_optimized_total_discharge.push_back(std::accumulate(
			sj_optimized_discharge.at(i).begin(), sj_optimized_discharge.at(i).end(), 0.0) * 3600);
		ja_initset_total_discharge.push_back(std::accumulate(
			ja_initset_discharge.at(i).begin(), ja_initset_discharge.at(i).end(), 0.0) * 3600);
		ja_optimized_total_discharge.push_back(std::accumulate(
			ja_optimized_discharge.at(i).begin(), ja_optimized_discharge.at(i).end(), 0.0) * 3600);
	}

	WriteResultFile();
}

void OptimalDischargeComputer::Optimize(int i)
{
	// Defining combined discharge
	doublematrix sj_ja_discharge;

	// Defining and running the optimizer
	NmOptimizer optimizer(scale, 1e-10, num_iter,
		sj_original_total_discharge.at(i), ja_original_total_discharge.at(i),
		sj_initshort_discharge.at(i), ja_initshort_discharge.at(i), discharge_constraint);
	optimizer.Run();

	// Set initset & optimized discharge vectors
	sj_initset_discharge.at(i) = optimizer.ExpandDischarge(sj_initshort_discharge.at(i));
	sj_optimized_discharge.at(i) =
		optimizer.ExpandDischarge(optimizer.GetOptimizedSJDischarge());
	ja_initset_discharge.at(i) = optimizer.ExpandDischarge(ja_initshort_discharge.at(i));
	ja_optimized_discharge.at(i) =
		optimizer.ExpandDischarge(optimizer.GetOptimizedJADischarge());

	// Run the original simulation and calculate the danger score
	sj_ja_discharge.clear(); 
	sj_ja_discharge.push_back(sj_original_discharge.at(i)); 
	sj_ja_discharge.push_back(ja_original_discharge.at(i));
	UdeCoupModel* sim_model_0 = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(sim_model_0,
		g_dataarchive->default_dataset, sj_ja_discharge);
	optimizer.RunSimulation(sim_model_0);
	optimizer.CalculateDangerScore();
	original_danger_vec.at(i) = optimizer.danger_score_vec;
	original_danger_score.at(i) = optimizer.danger_score;

	// Run the initially set discharge simulation and calculate the danger score
	sj_ja_discharge.clear();
	sj_ja_discharge.push_back(sj_initset_discharge.at(i));
	sj_ja_discharge.push_back(ja_initset_discharge.at(i));
	UdeCoupModel* sim_model_1 = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(sim_model_1
		, g_dataarchive->default_dataset, sj_ja_discharge);
	optimizer.RunSimulation(sim_model_1);
	optimizer.CalculateDangerScore();
	initset_danger_vec.at(i) = optimizer.danger_score_vec;
	initset_danger_score.at(i) = optimizer.danger_score;

	// Run the optimized simulation and calculate the danger score
	sj_ja_discharge.clear();
	sj_ja_discharge.push_back(sj_optimized_discharge.at(i));
	sj_ja_discharge.push_back(ja_optimized_discharge.at(i));
	UdeCoupModel* sim_model_2 = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(sim_model_2,
		g_dataarchive->default_dataset, sj_ja_discharge);
	optimizer.RunSimulation(sim_model_2);
	optimizer.CalculateDangerScore();
	optimized_danger_vec.at(i) = optimizer.danger_score_vec;
	optimized_danger_score.at(i) = optimizer.danger_score;

	// Get reservoir_level_vec
	ReservoirPredictionModel* get_reservoir_level_vec;
	get_reservoir_level_vec = (ReservoirPredictionModel*)sim_model_2->child_models_.at(0);
	get_reservoir_level_vec->CopyLevel(0, sj_reservoir_level_vec.at(i));
	get_reservoir_level_vec->CopyLevel(1, ja_reservoir_level_vec.at(i));

	//print the log to console
	CLOG_PTR->info("simulation ends...");

	UDEIF->Reset();
}

// Writing the optimization result in the csv file
void OptimalDischargeComputer::WriteResultFile()
{
	std::ofstream file;
	file.open("Optimization Result.csv");
	for (int i = 0; i < sj_optimized_discharge.at(0).size() + 2; i++)
	{
		if (i == 0)
		{
			file << "Conservative Discharges" << ",,,,,,"
				<< "Moderate Discharges" << ",,,,,,"
				<< "Aggressive Discharges" << ",,,,,,"
				<< "Conservative Total Discharges" << ",,,,,,"
				<< "Moderate Total Discharges" << ",,,,,,"
				<< "Aggressive Total Discharges" << ",,,,,,"
				<< "Conservative Total Discharges";
		}
		else if (i == 1)
		{
			for (int j = 0; j < 6; j++)
			{
				file << "SJ_Original" << "," << "SJ_Initset" << "," << "SJ_Optimized" << ","
					<< "JA_Original" << "," << "JA_Initset" << "," << "JA_Optimized" << ",";
			}
			file << "Taken time for optimization";
		}
		else if (i == 2) 
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_total_discharge.at(j) << ","
					<< sj_initset_total_discharge.at(j) << ","
					<< sj_optimized_total_discharge.at(j) << ","
					<< ja_original_total_discharge.at(j) << ","
					<< ja_initset_total_discharge.at(j) << ","
					<< ja_optimized_total_discharge.at(j) << ",";
			}
			file << taken_time;
		}
		else if (i == 4)
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			file << "Conservative Danger Average" << ",,,,"
				<< "Moderate Danger Average" << ",,,,"
				<< "Aggressive Danger Average";
		}
		else if (i == 5)
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			for (int j = 0; j < 3; j++)
			{
				file << "Original" << "," << "InitSet" << ","
					<< "Optimized" << "," << "Difference" << ",";
			}
		}
		else if (i == 6)
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			for (int j = 0; j < 3; j++)
			{
				file << original_danger_score.at(j) << ","
					<< initset_danger_score.at(j) << ","
					<< optimized_danger_score.at(j) << ","
					<< optimized_danger_score.at(j) - original_danger_score.at(j) << ",";
			}
		}
		else if (i == 8)
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			file << "Conservative Danger Vector" << ",,,,"
				<< "Moderate Danger Vector" << ",,,,"
				<< "Aggressive Danger Vector";
		}
		else if (i == 9)
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			for (int j = 0; j < 3; j++)
			{
				file << "Original" << "," << "InitSet" << ","
					<< "Optimized" << "," << "Difference" << ",";
			}
		}
		else if (i >= 10 && i < 10 + original_danger_vec.at(0).size())
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
			for (int j = 0; j < 3; j++)
			{
				file << original_danger_vec.at(j).at(i - 10) << ","
					<< initset_danger_vec.at(j).at(i - 10) << ","
					<< optimized_danger_vec.at(j).at(i - 10) << ","
					<< optimized_danger_vec.at(j).at(i - 10) - original_danger_vec.at(j).at(i - 10) << ",";
			}
		}
		else
		{
			for (int j = 0; j < 3; j++)
			{
				file << sj_original_discharge.at(j).at(i - 2) << ","
					<< sj_initset_discharge.at(j).at(i - 2) << ","
					<< sj_optimized_discharge.at(j).at(i - 2) << ","
					<< ja_original_discharge.at(j).at(i - 2) << ","
					<< ja_initset_discharge.at(j).at(i - 2) << ","
					<< ja_optimized_discharge.at(j).at(i - 2) << ",";
			}
		}
		file << "\n";
	}
	file.close();
}

void OptimalDischargeComputer::PrintOutToDB(PostgreSQLConnector* db_connector)
{
	std::string query_form, 
		discharge_time, discharge_str_sj, discharge_str_ja, 
		dam_wal_str, num_mode;

	for (int j = 0; j < 3; j++)
	{
		num_mode = std::to_string(j + 1);
		// Write conservative optimized discharge at Seomjin dam
		query_form = g_dataarchive->imt_sn +
			", currval('seq_optmz_imt_sn'), '4001110', 'SRO00" + num_mode + "', ";
		for (int i = 0; i < g_dataarchive->non_optimizing_duration; i++)
		{
			discharge_time = db_connector->GetNextHour(g_dataarchive->opt_start_time, std::to_string(i - g_dataarchive->start_time_index));
			dam_wal_str = std::to_string(0.0);
			discharge_str_sj = std::to_string(sj_original_full_discharge.at(i));
			db_connector->WriteData("tn_ai_optmz_dcwtr", query_form
				+ "to_timestamp('" + discharge_time + "','YYYY.MM.DD.HH24'), "
				+ discharge_str_sj + "," + dam_wal_str);
		}
		for (int i = 0; i < g_dataarchive->simulation_duration; i++)
		{
			discharge_time = db_connector->GetNextHour(g_dataarchive->opt_start_time, std::to_string(i));
			dam_wal_str = std::to_string(sj_reservoir_level_vec.at(j).at(i));
			discharge_str_sj = std::to_string(sj_optimized_discharge.at(j).at(i));
			db_connector->WriteData("tn_ai_optmz_dcwtr", query_form
				+ "to_timestamp('" + discharge_time + "','YYYY.MM.DD.HH24'), "
				+ discharge_str_sj + "," + dam_wal_str);
		}

		// Write conservative optimized discharge at Juam dam
		query_form = g_dataarchive->imt_sn +
			", currval('seq_optmz_imt_sn'), '4007110', 'SRO00" + num_mode + "', ";
		for (int i = 0; i < g_dataarchive->non_optimizing_duration; i++)
		{
			discharge_time = db_connector->GetNextHour(g_dataarchive->opt_start_time, std::to_string(i - g_dataarchive->start_time_index));
			dam_wal_str = std::to_string(0.0);
			discharge_str_ja = std::to_string(ja_original_full_discharge.at(i));
			db_connector->WriteData("tn_ai_optmz_dcwtr", query_form
				+ "to_timestamp('" + discharge_time + "','YYYY.MM.DD.HH24'), "
				+ discharge_str_ja + "," + dam_wal_str);
		}
		for (int i = 0; i < g_dataarchive->simulation_duration; i++)
		{
			discharge_time = db_connector->GetNextHour(g_dataarchive->opt_start_time, std::to_string(i));
			dam_wal_str = std::to_string(ja_reservoir_level_vec.at(j).at(i));
			discharge_str_ja = std::to_string(ja_optimized_discharge.at(j).at(i));
			db_connector->WriteData("tn_ai_optmz_dcwtr", query_form
				+ "to_timestamp('" + discharge_time + "','YYYY.MM.DD.HH24'), "
				+ discharge_str_ja + "," + dam_wal_str);
		}
		
		std::string print_log;
		print_log.append("Writing optimized data ");
		print_log.append(num_mode);
		print_log.append(" in DB has succeeded \n");
		printf(print_log.c_str());
	}
}