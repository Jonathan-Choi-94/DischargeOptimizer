#include "LinearPredictionModel.h"

LinearPredictionModel::LinearPredictionModel(int& i)
{
	//set name
	SetName("LinearPredictionModel");

	//set number of state
	SetCsStateNum(2);

	num_flow_model_weight = g_dataarchive->num_flow_model_weight_vec.at(i);
	index_point = i;
	time = 0.0; ts = 0;
	delt = g_dataarchive->time_step;

	// initializing variables with the garbage value
	m_model_state = 0;
	m_num_range = 0;
	m_b = 0.0;
	Q_in_thisPoint = 0.0;
	Q_in_upstream.assign(num_flow_model_weight, 0.0);
}

LinearPredictionModel::~LinearPredictionModel()
= default;

bool LinearPredictionModel::DerivativeFn(const double& t, double* y, double* dydt)
{
	dydt[0] = 0;
	dydt[1] = 0;
	return true;
}

bool LinearPredictionModel::OutputFn(const double& t, double* y)
{
	time = t; 
	UpdateInput();

	double temp_q_flow, temp_level;

	for (int i = 0; i < 10; i++)
	{
		// Updating continuous state variable
		temp_q_flow = 0;
		for (auto j = 0; j < Q_in_upstream.size(); j++)
		{
			temp_q_flow += m_w[j] * Q_in_upstream[j];
		}
		temp_q_flow += m_b;

		temp_level = 0;
		temp_level += m_param.at(m_model_state).at(0) * temp_q_flow
			+ m_param.at(m_model_state).at(1);

		// Updating model state
		int old_model_state = m_model_state;
		ModelStateUpdate(temp_level);
		if (m_model_state == old_model_state)
			break;
	}
	
	// Saving the result y[0]->q_flow, y[1]->level 
	y[0] = temp_q_flow; y[1] = temp_level;
	result_qflow.push_back(y[0]);
	result_level.push_back(y[1]);
	
	return true;
}

//=================================================================================================
// Initialize State
void LinearPredictionModel::InitializeState(std::vector<double>& init_Q_in,
	double& init_qflow, double& init_level)
{
	// set model initial state value
	InitCsStateValue(0, 0.0); // y[0] q_flow
	InitCsStateValue(1, init_level); // y[1] Water Level (h)

	// initializing model state
	ModelStateUpdate(init_level);

	// split inflow into upstream and thispoint 
	std::vector<double> init_Q_in_ = init_Q_in;
	Q_in_thisPoint = init_Q_in_.back();
	init_Q_in_.pop_back();
	Q_in_upstream = init_Q_in_;

	// clear the result saved
	result_qflow.clear();
	result_level.clear();
	result_qflow.push_back(init_qflow);
	result_level.push_back(init_level);

}

// Initialize input with the specific dataset in DataArchive
void LinearPredictionModel::InitializeInput(int& dataset)
{
	// Setting discharge at dams
	dam_discharge.clear();
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		dam_discharge.push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(i));
	}

	// Setting Q_in with Q_in(dataset)
	Q_in_ = g_dataarchive->dataset_Q_in.at(dataset);
}

// Initialize input with the new discharges at dam
void LinearPredictionModel::InitializeInput(int& dataset, doublematrix new_discharge)
{
	// Setting discharge at dams
	dam_discharge.clear();
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		dam_discharge.push_back(new_discharge.at(i));
	}

	// Setting Q_in with Q_in(dataset)
	Q_in_ = g_dataarchive->dataset_Q_in.at(dataset);
}

void LinearPredictionModel::SetEventCriteria(std::vector<double>& event_criteria)
{
	m_event_criteria.clear();
	for (int i = 0; i < event_criteria.size(); i++)
	{
		m_event_criteria.push_back(event_criteria.at(i));
	}
	m_num_range = event_criteria.size() + 1;
}

void LinearPredictionModel::SetWholeParam(std::vector<double> param)
{
	m_w = std::vector<double>(num_flow_model_weight);
	m_param = std::vector<std::vector<double>>(m_num_range);

	std::vector<double> param_set = param;
	// setting model 1 parameters (Q_in -> q_flow model) 
	for (int i = 0; i < num_flow_model_weight; i++)
	{
		m_w[i] = param_set.front();
		param_set.erase(param_set.begin());
	}
	m_b = param_set.front();
	param_set.erase(param_set.begin());

	// setting model 2 parameters (q_flow -> level model)
	for (int i = 0; i < m_num_range; i++)
	{
		m_param.at(i).clear();
		for (int j = 0; j < 2; j++)
		{
			m_param.at(i).push_back(param_set.front());
			param_set.erase(param_set.begin());
		}
	}
}

//=================================================================================================
// Other Member Functions
// 
// Update input
void LinearPredictionModel::UpdateInput()
{
	// Setting current ts (time index)
	ts = g_dataarchive->start_time_index + static_cast<int> (round(time / delt / 0.1)) * 0.1;

	int i = index_point;
	// Setting and sending the inflow data to prediction model 
	std::vector<int> delay_vec = g_dataarchive->delay.at(i);
	std::vector<int> delay_dam_vec = g_dataarchive->delay_dam.at(i);

	// Filling the Q_in_upstream
	// ex. when predicting 4th point, Q_in_upstream is Q_in[0:3][time_step]
	Q_in_upstream.clear();
	Q_in_thisPoint = 0.0;

	// Check whether discharge from Juam dam is included or not
	if (g_dataarchive->IndexStationEqualstoPP.at(i) < g_dataarchive->station_joining)
	{
		Q_in_upstream.push_back(dam_discharge.at(0).at(ts - delay_dam_vec.at(0)));
		for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 1; j++)
		{
			if (ts < Q_in_.at(j).size())
			{
				// Setting the upstream inflow with delay
				int ts_delayed = ts - delay_vec.at(j);
				Q_in_upstream.push_back(Q_in_.at(j).at(ts_delayed));
			}
			else
			{
				Q_in_upstream.push_back(0.0);
			}
		}
	}
	else
	{
		Q_in_upstream.push_back(dam_discharge.at(0).at(ts - delay_dam_vec.at(0)));
		Q_in_upstream.push_back(dam_discharge.at(1).at(ts - delay_dam_vec.at(1)));
		for (int j = 0; j < g_dataarchive->num_flow_model_weight_vec.at(i) - 2; j++)
		{
			if (ts < Q_in_.at(j).size())
			{
				// Setting the upstream inflow with delay
				int ts_delayed = ts - delay_vec.at(j);
				Q_in_upstream.push_back(Q_in_.at(j).at(ts_delayed));
			}
			else
			{
				Q_in_upstream.push_back(0.0);
			}
		}
	}


	if (ts < Q_in_.at(0).size())
	{
		// Setting this point inflow
		for (int ip : g_dataarchive->EveryIndexInflowEqualstoPP.at(i))
		{
			Q_in_thisPoint += Q_in_.at(ip).at(ts);
		}
	}
	else
	{
		Q_in_thisPoint = 0.0;
	}
}

// Update model state : distinguish by comparing with event_criteria
void LinearPredictionModel::ModelStateUpdate(double state)
{
	if (state < m_event_criteria[0])
	{
		m_model_state = 0;
	}
	else if (state > m_event_criteria.back())
	{
		m_model_state = m_event_criteria.size();
	}
	else
	{
		int i_model = 1;
		for (int i = 0; i < m_event_criteria.size() - 1; i++)
		{
			if (state >= m_event_criteria.at(i) && state < m_event_criteria.at(i + 1))
			{
				m_model_state = i_model;
			}
			++i_model;
		}
	}
}

void LinearPredictionModel::ResetParam_1(std::vector<double>& param)
{
	// Reset model 1 parameters (Q_in -> q_flow model)
	for (int i = 0; i < param.size() - 1; i++)
	{
		m_w.at(i) = param.at(i);
	}
	m_b = param.back();
}

void LinearPredictionModel::ResetParam_2(int submodel, std::vector<double>& param)
{
	// Reset model 2 [num_model] parameters (q_flow -> level model)
	for (int j = 0; j < 3; j++)
	{
		m_param.at(submodel).at(j) = param.at(j);
	}
}

void LinearPredictionModel::Copyqflow(std::vector<double>& q_flow)
{
	q_flow.clear();
	for (int i = 0; i < result_qflow.size(); i++)
		q_flow.push_back(result_qflow.at(i));
}

void LinearPredictionModel::CopyLevel(std::vector<double>& level)
{
	level.clear();
	for (int i = 0; i < result_level.size(); i++)
		level.push_back(result_level.at(i));
}


