#pragma once
#include "HEinsSim.h"
#include "UdeBaseModel.h"

class UDESIM_EXT_CLASS OptEvalFunc
{
public:
	OptEvalFunc();
	virtual ~OptEvalFunc();

	// Initialize
	void SetParamValues(const double* param_values);
	void SetTimePoints(const std::vector<TimeType>& time_points); 
	void ClearTimePoints(); 	

	// called before simulation
	bool IsParamMatch(const std::map<std::string, double>& param_map) const; 
	bool IsProbeDataMatch(const std::map<std::string, double>& probe_data) const; 
	bool IsProbeDataMatch(const std::map<std::string, std::vector<double>> & probe_data) const; 
	

	// for logging
	void ShowParamInfo() const; 
	void ShowProbingData() const;
	static void LogOptimizerParams(const std::map<std::string, double>& param_map);
	static void LogOptimizerParams(const std::map<std::string, std::vector<double>> & param_map);
	static void LogOptimizerProbeData(const std::map<std::string, double>& param_map);
	static void LogOptimizerProbeData(const std::map<std::string, std::vector<double>> & param_map);

	// engine calls for the simulation y recording
	void RecordTimedData(const TimeType& cur_time);
	void RecordData();

	// Optimization 
	void GetSimResult(std::map<std::string, double> & sim_y_values) const; 
	void GetSimResult(std::map<std::string, std::vector<double> > & timed_sim_y_values) const; 

	// observed parameters and y values 
	std::map<std::string, std::pair<UdeBaseModel *, double *>> init_params_;
	std::map<std::string, std::pair<UdeBaseModel *, double *>> y_values_;

	// Vector to store the time points of observed variables 
	std::vector<TimeType> time_points_;

	// simulation result
	std::map<std::string, double > sim_y_values_;
	std::map<std::string, std::vector<double> > timed_sim_y_values_;

	bool is_time_point_exist_{false};

protected:
	// simulation runtime data recording
	void SetRecordReady(const TimeType& cur_time);
	//save the probing data at the current time
	void SaveCurProbingStates();
	//save the probing data after interpolation
	void SaveInterpolation(const TimeType& cur_time, const double& time_diff);
	//save the last probing data for interpolation
	void SaveLastRecordData(const TimeType & cur_time);

	unsigned int opt_time_index_{0};
	//Map to store the pair of name and observed variables

	std::map<std::string, double> last_probe_data_;
	TimeType last_time_{0.0};
};

