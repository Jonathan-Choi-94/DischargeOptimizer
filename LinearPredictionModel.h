#pragma once
#include "UdeLogger.h"
#include "UdeCsModel.h"
#include "DamCorrelation.h"
#include "DataArchive.h"

class LinearPredictionModel : public UdeCsModel
{
public:
	LinearPredictionModel(int& point);
	virtual ~LinearPredictionModel();

	// Engine member functions
	bool DerivativeFn(const double& t, double* y, double* dydt) override;
	bool OutputFn(const double& t, double* y) override;

	// Initializing functions
	void InitializeState(std::vector<double>& init_Q_in,
		double& init_qflow, double& init_level);
	void InitializeInput(int& dataset);
	void InitializeInput(int& dataset, doublematrix new_discharge);
	void SetEventCriteria(std::vector<double>& event_criteria);
	void SetWholeParam(std::vector<double> param);
	
	// Other member functions
	void UpdateInput();
	void ModelStateUpdate(double state);
	void ResetParam_1(std::vector<double>& param);
	void ResetParam_2(int num_model, std::vector<double>& param);
	void Copyqflow(std::vector<double>& q_flow);
	void CopyLevel(std::vector<double>& level);
	
	// member variables 
	int index_point;
	std::vector<std::vector<double>> m_param;
	std::vector<double> result_qflow, result_level, m_event_criteria, m_w, Q_in_upstream;
	int num_flow_model_weight, m_model_state, m_num_range;
	double m_b, Q_in_thisPoint;

	doublematrix Q_in_, dam_discharge;
	int ts;
	double delt, time;
};

