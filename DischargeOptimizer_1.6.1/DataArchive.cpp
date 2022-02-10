#include "DataArchive.h"
#include "parser.hpp"

DataArchive::DataArchive()
{
	time_step = 1.0;
	numof_dataset = 0;
	default_dataset = 0;
	num_dam = 2;
	PYTHONHOME = "";

	InitialSetting();
}

DataArchive::~DataArchive()
{
}

void DataArchive::UpdateParam_flow(std::vector<double> param_new, int point)
{
	for (int i = 0; i < param_new.size(); i++)
	{
		param_trained.at(point).erase(param_trained.at(point).begin()
			+ i);
		param_trained.at(point).emplace(param_trained.at(point).begin()
			+ i, param_new[i]);
	}
}

void DataArchive::UpdateParam_level(std::vector<double> param_new, int point, int submodel)
{
	for (int i = 0; i < param_new.size(); i++)
	{
		param_trained.at(point).erase(param_trained.at(point).begin() 
			+ num_flow_model_weight_vec.at(point) + 1 + (2 * submodel) + i);
		param_trained.at(point).emplace(param_trained.at(point).begin() 
			+ num_flow_model_weight_vec.at(point) + 1 + (2 * submodel) + i, param_new[i]);
	}
}

void DataArchive::GetDelay(int& num_interpolation)
{
	// Setting the delay using average velocity 
	for (int j = 0; j < predict_point.size(); j++)
	{
		delay.push_back(v_int);
		delay_dam.push_back(v_int);
		for (int i = 0; i <= IndexInflowEqualstoPP.at(j); i++)
		{
			if (i < num_inflow_seomjin_main ||
				i >= num_inflow_seomjin_main + num_inflow_seomjin_boseong)
			{
				delay[j].push_back(round(num_interpolation *
					(inflow_river_vec[i] - predict_point[j]) / (3.7869 * 3600 / 1000)));
			}
			else
			{
				// (Boseong river joins with Seomjin river at 61km from the end point of Seomjin river)
				// Delay calculation from stations at Boseong river 
				delay[j].push_back(round(num_interpolation *
					(61 + inflow_river_vec[i] - predict_point[j]) / (3.7869 * 3600 / 1000)));
			}
		}

		// Add the delay from Seomjin dam to predict_point[j]
		delay_dam.at(j).push_back(round(num_interpolation *
			(136.0 - predict_point[j]) / (3.7869 * 3600 / 1000)));
		// Add the delay from Juam dam to predict_point[j]
		// It applies when the predict_point is at the main_down region
		if (IndexStationEqualstoPP.at(j) < station_joining)
		{
			delay_dam.at(j).push_back(0);
		}
		else
		{
			delay_dam.at(j).push_back(round(num_interpolation *
				(26.0 + 61.0 - predict_point[j]) / (3.7869 * 3600 / 1000)));
		}
			
	}
	 
	// delay_max is the delay applied between the last predict point and Seomjin dam
	delay_max = delay_dam.back().front();
}


void DataArchive::MapIndices()
{
	for (double pp : predict_point)
	{
		if (pp >= station_river_vec.at(num_station_seomjin_main - 1))
		{
			 IndexStationEqualstoPP.push_back(std::find(station_river_vec.begin(),
				 station_river_vec.begin() + num_station_seomjin_main - 1,
				pp) - station_river_vec.begin());
		}
		else
		{
			IndexStationEqualstoPP.push_back(std::find(station_river_vec.begin()
				+ num_station_seomjin_main + num_station_seomjin_boseong,
				station_river_vec.end(),
				pp) - station_river_vec.begin());
		}
	}

	for (double pp : predict_point)
	{
		if (pp >= inflow_river_vec.at(num_inflow_seomjin_main - 1))
		{
			for (int ip = 0; ip < num_inflow_seomjin_main; ip++)
			{
				if (pp == inflow_river_vec.at(ip))
				{
					IndexInflowEqualstoPP.push_back(ip);
					break;
				}
			}
		}
		else
		{
			for (int ip = num_inflow_seomjin_main + num_inflow_seomjin_boseong;
				ip < num_whole_inflow; ip++)
			{
				if (pp == inflow_river_vec.at(ip))
				{
					IndexInflowEqualstoPP.push_back(ip);
					break;
				}

			}
		}
	}


	double pp;
	for (int i = 0; i < predict_point.size(); i++)
	{
		pp = predict_point.at(i);
		EveryIndexInflowEqualstoPP.push_back(v_int);
		if (pp >= inflow_river_vec.at(num_inflow_seomjin_main - 1))
		{
			for (int ip = 0; ip < num_inflow_seomjin_main; ip++)
			{
				if (pp == inflow_river_vec.at(ip))
				{
					EveryIndexInflowEqualstoPP.at(i).push_back(ip);
				}
			}
		}
		else
		{
			for (int ip = num_inflow_seomjin_main + num_inflow_seomjin_boseong;
				ip < num_whole_inflow; ip++)
			{
				if (pp == inflow_river_vec.at(ip))
				{
					EveryIndexInflowEqualstoPP.at(i).push_back(ip);
				}
					
			}
		}

		if (IndexStationEqualstoPP.at(i) < station_joining)
			num_flow_model_weight_vec.at(i) = IndexInflowEqualstoPP.at(i) + 1;
		else
			num_flow_model_weight_vec.at(i) = IndexInflowEqualstoPP.at(i) + 2;
	}
}

// This is executed with the generator of DataArchive class
void DataArchive::InitialSetting()
{
	// Setting the whole time index
	start_time_index = 20;
	simulation_duration = 240 - start_time_index + 1;
	non_optimizing_duration = start_time_index;
	for (int i = start_time_index; i <= 240; i++)
	{
		index_wholetime.push_back(i);
	}

	// Get whole K-river stations
	std::ifstream kriver_whole_station_info("kriver_whole_station.csv");
	aria::csv::CsvParser parser0_1(kriver_whole_station_info);
	int i = 0;
	for (auto& row : parser0_1)
	{
		if (i > 0)
		{
			station_region_info_vec.push_back(row[0]);
			station_river_vec.push_back(std::stod(row[1]));
		}
		++i;
	}

	// Get inflow points
	std::ifstream kdurm_kriver_connect_info("kdrum_kriver_connect_arranged.csv");
	aria::csv::CsvParser parser0_2(kdurm_kriver_connect_info);
	i = 0;
	for (auto& row : parser0_2)
	{
		if (i > 0)
		{
			inflow_region_info_vec.push_back(row[0]);
			inflow_river_vec.push_back(std::stod(row[1]));
			inflow_drum_vec.push_back(std::stoi(row[2]));
		}
		++i;
	}

	num_station_seomjin_main = count(station_region_info_vec.begin(),
		station_region_info_vec.end(), "Seomjin-main");
	num_station_seomjin_boseong = count(station_region_info_vec.begin(),
		station_region_info_vec.end(), "Seomjin-boseong");
	num_station_seomjin_main_down = count(station_region_info_vec.begin(),
		station_region_info_vec.end(), "Seomjin-main_down");
	station_joining = num_station_seomjin_main + num_station_seomjin_boseong;

	num_inflow_seomjin_main = count(inflow_region_info_vec.begin(),
		inflow_region_info_vec.end(), "Seomjin-main");
	num_inflow_seomjin_boseong = count(inflow_region_info_vec.begin(),
		inflow_region_info_vec.end(), "Seomjin-boseong");
	num_inflow_seomjin_main_down = count(inflow_region_info_vec.begin(),
		inflow_region_info_vec.end(), "Seomjin-main_down");
	num_whole_inflow = num_inflow_seomjin_main + num_inflow_seomjin_boseong +
		num_inflow_seomjin_main_down;

	// Set prediction points (main river station no.)
	//predict_point = { 130.2, 127.723, 126, 109.2, 108.113, 104.2, 97.6, 93.2, 89.2, 79.88,
	//	74.6, 60.4, 54.6, 53.6, 47.555, 39.6, 33.5, 24, 14.797 };
	predict_point = { 130.2, 127.723, 126, 109.2, 108.113, 104.2, 97.6, 93.2, 89.2,
		74.6, 54.6, 53.6, 33.5, 24, 14.797 };
	num_flow_model_weight_vec.assign(predict_point.size(), 0);
	MapIndices();

	// Dam level limit
	sj_upper_level_limit = 197.70;
	sj_lower_level_limit = 175.00;
	ja_upper_level_limit = 110.50;
	ja_lower_level_limit = 85.0;
	sj_max_level_limit = 200.00; sj_max_volume_limit = 1e6 * 510.3;
	sj_min_level_limit = 141.00; sj_min_volume_limit = 1e6 * 16.8;
	ja_max_level_limit = 115.00; ja_max_volume_limit = 1e6 * 647.7;
	ja_min_level_limit = 50.00; ja_min_volume_limit = 1e6 * 8.5;
	

	// Assigning the in & out data size
	matrix_dmnset = std::vector<std::vector<double>>(predict_point.size());

	int DefaultInterpolation = 1;
	GetDelay(DefaultInterpolation);

	std::ifstream limitsavedfile("StationInfo_BankHeight_EventCriteria.csv");
	aria::csv::CsvParser parser1(limitsavedfile);
	std::vector<double> temp_height_vec, temp_criteria_vec;
	doublematrix temp_height_criteria_matrix;
	std::map<double, doublematrix> station_info_map;
	i = 0;
	for (auto& row : parser1)
	{
		if (i > 0)
		{
			temp_height_vec.push_back(std::stod(row[1]));
			temp_height_vec.push_back(std::stod(row[2]));
			temp_criteria_vec.push_back(std::stod(row[3]));
			temp_criteria_vec.push_back(std::stod(row[4]));
			temp_criteria_vec.push_back(std::stod(row[5]));
			temp_criteria_vec.push_back(std::stod(row[6]));
			temp_height_criteria_matrix.push_back(temp_height_vec);
			temp_height_criteria_matrix.push_back(temp_criteria_vec);
			station_info_map.insert(std::make_pair
			(std::stod(row[0]), temp_height_criteria_matrix));
			temp_height_vec.clear();
			temp_criteria_vec.clear();
			temp_height_criteria_matrix.clear();
		}
		++i;
	}

	std::ifstream paramsavedfile("ParamSetforcxx.csv");
	aria::csv::CsvParser parser2(paramsavedfile);
	std::vector<double> temp_param_vec;
	std::map<double, std::vector<double>> station_param_map;
	i = 0;
	for (auto& row : parser2)
	{
		if (i > 0)
		{
			for (int j = 1; j < row.size(); j++)
				temp_param_vec.push_back(std::stod(row[j]));
			station_param_map.insert(std::make_pair
			(std::stod(row[0]), temp_param_vec));
			temp_param_vec.clear();
		}
		++i;
	}


	double pp = 0;
	std::vector<double> v;
	for (i = 0; i < predict_point.size(); i++)
	{
		pp = predict_point.at(i);
		event_criteria.push_back(v);
		level_limit.push_back(v);
		param_trained.push_back(v);
		
		for (auto iter = station_info_map.begin(); iter != station_info_map.end(); ++iter)
		{
			if (iter->first == pp)
			{
				for (double j : iter->second.at(0))
					level_limit.at(i).push_back(j);
				for (double j : iter->second.at(1))
					event_criteria.at(i).push_back(j);
			}

		}

		for (int j = 0;
			j < num_flow_model_weight_vec.at(i) + 1; j++)
			param_trained.at(i).push_back(1.0);

		for (auto iter = station_param_map.begin(); iter != station_param_map.end(); ++iter)
		{
			if (iter->first == pp)
			{
				for (int j = 0;
					j < 2 * (event_criteria.at(i).size() + 1);
					j++)
					param_trained.at(i).push_back(iter->second.at(j));
			}
		}
	}
}

	
	