#pragma once
#include "TimeType.h"
#include "UdeBaseModel.h"

#include <vector>
#include <map>

class UDESIM_EXT_CLASS OptDataDB
{
public:
	OptDataDB();
	~OptDataDB();
	void SetReadyForOpt();

	//APIs called by engine  
	void InitRecordTimedData(const TimeType& cur_time);
	void RecordTimedData(const TimeType& cur_time);

	double GetMeanSquareError();
	TimeType GetFinalTime();

	void SetObservedTimes(const std::vector<double>& vec_times);
	void AddObservedData(const std::string& val_name, const std::vector<double>& vec_data);
	void Reset(); 

	//get state values 
	int GetProveDataNum() const;
	double GetProbeStateValue(const int& index);

	int GetOptStateNum() const;
	int GetOptStateIndex(const std::string& opt_state_name);
	void GetOptNameModelPtrPair(std::pair<std::string, UdeBaseModel *>& ret_pair, const int& index);

public:
	//APIs to check whether opt. data is ready 
	bool IsOptDataReady();
	//APIs to check the validity (length) of saved probed data during simulation 
	bool IsSavedProbedDataValid() const;

private:
	bool IsAllOptMapReady() const; //To check the opt. maps are ready
	bool IsProbeDataObserved() const; //Probing data is observed? 
	//Is length of observed data is match to the observed times  
	bool IsObDataLengthValid() const;

	/*APIs to save the simulation data */
	//save the probing data at the current time
	void SaveCurProbingStates();
	//save the probing data after interpolation
	void SaveInterpolation(const TimeType& cur_time, const double& time_diff);
	//save the last probing data for interpolation
	void SaveAsLastProbedStates(const TimeType& cur_time);

public:
	//Map to store the pair of name and optimizing variables
	std::map<std::string, double *> init_params_;
	std::map<std::string, double *> y_values_;
	std::map<std::string, double > init_param_values_;

	//Map to store the pair of name and observed variables
	std::map<std::string, std::vector<double>> observed_state_map_;

	//Vector to store the time points of observed variables 
	std::vector<TimeType> time_points_;

	std::map<std::string, double > sim_y_values_;
	std::map<std::string, std::vector<double> > timed_sim_y_values_;

	//Map to store the pair of name and observed variables
	std::map<std::string, std::vector<double>> timed_sim_y_values_;

private:
	unsigned int opt_time_index_;
	//Map to store the pair of name and observed variables

	std::map<std::string, double> last_probe_data_;

	TimeType last_time_;
};
