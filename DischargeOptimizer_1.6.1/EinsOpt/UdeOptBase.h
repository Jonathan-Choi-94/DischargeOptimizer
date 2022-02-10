#pragma once
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include "EinsOpt.h"
#include "TimeType.h"
//#include "UdeCoupModel.h"

//Function type for constraints 
typedef std::function<double (double)> ConstraintFunc;

class UDEOPT_EXT_CLASS UdeOptBase
{
public:
	UdeOptBase();
	virtual ~UdeOptBase();

public:
	// //add the observed times  
	// bool SetObservedTimes(double* times, const int& data_size);
	// bool SetObservedTimes(const std::vector<double>& vec_time);

	//add the observed data 
	void SetObservedData(const std::string& y_data_name, const double& value);
	void SetObservedData(const std::string& y_data_name,
	                     const std::vector<double>& values);

	//set the initial value of an optimizing state 
	void SetInitParamValue(const std::string& state_name, const double& value);

	// remove all initial value of optimizing states
	void AddTimePoints(const std::vector<double> & time_points); 

	// add the constraint function
	void AddConstraint(const std::string& val_name, const ConstraintFunc& func);

	// Reset all constraints and observed data,time series 
	void Reset();

	// get the different between observed data and simulated data
	virtual bool PreOptProcess();

	// get the different between observed data and simulated data
	virtual void InitFunc(double* params) = 0;

	// get the different between observed data and simulated data
	virtual void EvaluateFunc() = 0;

	// get the MSE
	virtual double GetMeanSquareError()=0;

	// get the different between observed data and simulated data
	virtual double GetError(double* params);

	// optimization run
	virtual bool Run() = 0; 

protected:
	// apply the each optimization values  
	void ApplyConstraint(double* params);

	// get value string from values for logging 
	std::string GetParamArrayString(double* values) const;

	// get the assigned and initial opt. state values 
	void InitParamsWithConstraints(double* ret_param_array);

	void UpdateConstraintMapIndex();

	std::map<std::string, std::pair<ConstraintFunc, int> > constraint_map_;
	std::map<std::string, double> init_param_maps_;

	// Vector to store the time points of observed variables 
	std::vector<TimeType> time_points_;

	std::map<std::string, double > observed_y_values_;
	std::map<std::string, std::vector<double> > timed_observed_y_values_;
	std::map<std::string, std::vector<double> > observed_y_values;
	
	// number of optimizing parameters/states 
	int num_param_{0};
	// number of optimizing parameters/states 
	size_t copy_size_{0};
	// time point exists
	bool is_time_point_exist_{false};
};

#define MAX_ALLOW_OPT_SIZE 20
