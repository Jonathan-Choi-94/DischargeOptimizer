#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <map>
typedef std::vector<std::vector<double>> doublematrix;

class DataArchive
{
public:
	DataArchive();
	virtual ~DataArchive();
	void UpdateParam_flow(std::vector<double> param_new, int point);
	void UpdateParam_level(std::vector<double> param_new, int point, int submodel);
	void GetDelay(int& num_interpolation);
	void MapIndices();
	void InitialSetting();

	int delay_max, num_interpolation_, default_dataset, numof_dataset, num_dam,
		num_inflow_seomjin_main, num_inflow_seomjin_boseong, num_inflow_seomjin_main_down,
		num_station_seomjin_main, num_station_seomjin_boseong, num_station_seomjin_main_down,
		num_whole_inflow, station_joining,
		start_time_index, simulation_duration, non_optimizing_duration;
	double time_step, initial_sjdam_level, initial_jadam_level,
		sj_lower_level_limit, sj_upper_level_limit,
		ja_lower_level_limit, ja_upper_level_limit,
		sj_max_level_limit, sj_min_level_limit,
		ja_max_level_limit, ja_min_level_limit,
		sj_max_volume_limit, sj_min_volume_limit,
		ja_max_volume_limit, ja_min_volume_limit;
	std::map<double, int> inflow_point_map;
	std::vector<double> inflow_river_vec, station_river_vec, predict_point,
		sj_inflow, ja_inflow;
	std::vector<std::string> inflow_region_info_vec, station_region_info_vec;
	std::vector<int> inflow_drum_vec, v_int, index_wholetime, train_points,
		IndexStationEqualstoPP, IndexInflowEqualstoPP,
		num_flow_model_weight_vec;
	std::vector<std::vector<int>> EveryIndexInflowEqualstoPP;
	std::vector<std::vector<int>> delay, delay_dam;
	doublematrix matrix_dmnset, param_trained, event_criteria, level_limit;
	std::vector<doublematrix> dataset_dam_discharge, dataset_Q_in,
		dataset_q_flow_true ,dataset_level_true;

	std::string imt_sn, opt_start_time, PYTHONHOME;
};

extern DataArchive* g_dataarchive;
