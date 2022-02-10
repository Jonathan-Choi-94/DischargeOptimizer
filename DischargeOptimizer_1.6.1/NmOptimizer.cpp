#include "NmOptimizer.h"
#include <numeric>

std::string Numbering(int i);

NmOptimizer::NmOptimizer(const double& scale, const double& epsilon, const int& max_iteration,
	double& sj_total_discharge, double& ja_total_discharge,
	std::vector<double>& sj_initshort_discharge, std::vector<double>& ja_initshort_discharge,
	double& discharge_constraint)
	: NmSimplex(scale, epsilon, max_iteration)
{
	// Setting basic informations
	length_discharge_ = sj_initshort_discharge.size();
	sj_original_total = sj_total_discharge;
	ja_original_total = ja_total_discharge;

	// discharge vector (param_)
	param_.assign(sj_initshort_discharge.size() + ja_initshort_discharge.size(), 0.0);

	// Set target score
	std::vector<double> target_score{ 1.0 };
	SetObservedData("danger_score", target_score);

	// set SJ initial parameter
	for (int i = 0; i < sj_initshort_discharge.size(); i++)
	{
		double discharge_ = sj_initshort_discharge.at(i);
		param_.at(i) = sj_initshort_discharge.at(i);
		SetInitParamValue("Q" + Numbering(i + 1), discharge_);
		ConstraintFunc constraint_func = [discharge_, discharge_constraint](const double& value)
		{
			if (value > 1868.0)
				return 1868.0;
			else if (value < 0.0)
				return 0.0;
			else if (value < discharge_ - discharge_constraint)
				return discharge_ - discharge_constraint;
			else if (value > discharge_ + discharge_constraint)
				return discharge_ + discharge_constraint;
			else
				return value;
		};
		AddConstraint("Q" + Numbering(i + 1), constraint_func);
	}

	// set JA initial parameter
	for (int i = 0; i < ja_initshort_discharge.size(); i++)
	{
		double discharge_ = ja_initshort_discharge.at(i);
		param_.at(i + length_discharge_) = ja_initshort_discharge.at(i);
		SetInitParamValue("Q" + Numbering(i + length_discharge_ + 1), discharge_);
		ConstraintFunc constraint_func = [discharge_, discharge_constraint](const double& value)
		{
			if (value > 1200.0)
				return 1200.0;
			else if (value < 0.0)
				return 0.0;
			else if (value < discharge_ - discharge_constraint)
				return discharge_ - discharge_constraint;
			else if (value > discharge_ + discharge_constraint)
				return discharge_ + discharge_constraint;
			else
				return value;
		};
		AddConstraint("Q" + Numbering(i + length_discharge_ + 1), constraint_func);
	}
}

NmOptimizer::~NmOptimizer()
{
}

bool NmOptimizer::PreOptProcess() {
	//OPT_CON_PTR->info("NM simplex optimization starts");
	return true;
};

void NmOptimizer::InitFunc(double* params) {
	//파라미터 값 갱신
	std::vector<double> sj_current_discharge, ja_current_discharge;
	for (int i = 0; i < length_discharge_; i++)
	{
		sj_current_discharge.push_back(params[i]);
	}
	for (int i = length_discharge_; i < 2 * length_discharge_; i++)
	{
		ja_current_discharge.push_back(params[i]);
	}

	// total time at each periods in second 
	double conversion_rate = (g_dataarchive->simulation_duration - 1) / length_discharge_ * 3600; // [s]

	// current total discharge
	double sj_current_total = std::accumulate(
		sj_current_discharge.begin(), sj_current_discharge.end(), 0.0) * conversion_rate;
	double ja_current_total = std::accumulate(
		ja_current_discharge.begin(), ja_current_discharge.end(), 0.0) * conversion_rate;

	// difference between the original total discharge and current total discharge
	double sj_diff_total = sj_original_total - sj_current_total;
	double ja_diff_total = ja_original_total - ja_current_total;

	double diff_addition = 0.0;
	
	// Adjust discharges at Seomjin with the difference of the total discharge
	if (sj_diff_total > 0)
	{
		// discharge addition amount for each discharge value to make the same total
		diff_addition = sj_diff_total / conversion_rate / length_discharge_;
		for (int i = 0; i < length_discharge_; i++)
		{

			params[i] = params[i] + diff_addition;
			param_.at(i) = params[i];
		}
	}
	else
	{
		// discharge addition amount for each discharge value to make the same total
		diff_addition = sj_diff_total / conversion_rate / length_discharge_;
		std::vector<int> index_nonzero;
		for (int i = 0; i < length_discharge_; i++)
		{
			if (params[i] + diff_addition < 0)
			{
				sj_diff_total += (params[i] * conversion_rate);
				params[i] = 0.0;
				param_.at(i) = params[i];
			}
			else
			{
				index_nonzero.push_back(i);
			}
		}
		diff_addition = sj_diff_total / conversion_rate / index_nonzero.size();
		for (int i : index_nonzero)
		{
			params[i] = params[i] + diff_addition;
			param_.at(i) = params[i];
		}
	}

	// Adjust discharges at Juam with the difference of the total discharge
	if (ja_diff_total > 0)
	{
		// discharge addition amount for each discharge value to make the same total
		diff_addition = ja_diff_total / conversion_rate / length_discharge_;
		for (int i = length_discharge_; i < 2 * length_discharge_; i++)
		{

			params[i] = params[i] + diff_addition;
			param_.at(i) = params[i];
		}
	}
	else
	{
		// discharge addition amount for each discharge value to make the same total
		diff_addition = ja_diff_total / conversion_rate / length_discharge_;
		std::vector<int> index_nonzero;
		for (int i = length_discharge_; i < 2 * length_discharge_; i++)
		{
			if (params[i] + diff_addition < 0)
			{
				ja_diff_total += (params[i] * conversion_rate);
				params[i] = 0.0;
				param_.at(i) = params[i];
			}
			else
			{
				index_nonzero.push_back(i);
			}
		}
		diff_addition = ja_diff_total / conversion_rate / index_nonzero.size();
		for (int i : index_nonzero)
		{
			params[i] = params[i] + diff_addition;
			param_.at(i) = params[i];
		}
	}

	// Checking whether if the total discharges the same or not
	sj_current_total = std::accumulate(param_.begin(), param_.end() - length_discharge_, 0.0)
		* conversion_rate;
	ja_current_total = std::accumulate(param_.begin() + length_discharge_, param_.end(), 0.0)
		* conversion_rate;
	sj_diff_total = sj_original_total - sj_current_total;
	ja_diff_total = ja_original_total - ja_current_total;

	// Setting discharges after the adjustment
	sj_current_discharge.clear();
	ja_current_discharge.clear();
	for (int i = 0; i < length_discharge_; i++)
	{
		sj_current_discharge.push_back(params[i]);
	}
	for (int i = length_discharge_; i < 2 * length_discharge_; i++)
	{
		ja_current_discharge.push_back(params[i]);
	}

	// Full-range 방류량 설정
	new_discharge.clear(); 
	new_discharge.push_back(ExpandDischarge(sj_current_discharge));
	new_discharge.push_back(ExpandDischarge(ja_current_discharge));
};

void NmOptimizer::EvaluateFunc() 
{
	// Defining new opt_model
	UdeCoupModel* opt_model = new PredPointCoupledModel();
	g_dataprocessor->InitializeWholeModel(opt_model, g_dataarchive->default_dataset, new_discharge);

	// 시뮬레이션 실행
	RunSimulation(opt_model);

	// 위험점수 계산
	CalculateDangerScore();

	// Update the calculated danger score in the optimization-engine
	observed_y_values["danger_score"].clear();
	observed_y_values["danger_score"].push_back(danger_score);
};

// Mean Square Error 계산
double NmOptimizer::GetMeanSquareError() 
{
	std::vector<double> vec_square_err;
	std::map<std::string, std::vector<double> > sim_y_values;

	// get simulation results 
	sim_y_values = observed_y_values;
	//UDEIF->GetSimResult(sim_y_values);

	for (auto& each_pair : timed_observed_y_values_)
	{
		auto square_err = 0.0;
		auto& observed_queue = sim_y_values[each_pair.first];

		for (size_t i = 0; i < each_pair.second.size(); ++i)
		{
			const auto diff = each_pair.second.at(i) - observed_queue.at(i);
			square_err += diff * diff;
		}

		square_err /= each_pair.second.size();

		vec_square_err.push_back(square_err);
	}

	const auto sum_square_err = std::accumulate(vec_square_err.begin(), vec_square_err.end(), 0.0);
	return sum_square_err / vec_square_err.size();
};

void NmOptimizer::RunSimulation(UdeCoupModel* model)
{
	// Reset the engine (Delete every set model)
	UDEIF->Reset();

	// Initialize and set whole model with the new discharge
	UDEIF->SetModel(model);

	// Set time step in the simulation engine
	UDEIF->GetCsEngine()->SetSolverTimeStep(g_dataarchive->time_step);

	// Show I/O and coupling information
	UDEIF->ShowModelInfo();

	// Run simulation
	UDEIF->Start(g_dataarchive->time_step * (g_dataarchive->index_wholetime.size() - 1));

	ReservoirPredictionModel* get_reservoir_result =
		(ReservoirPredictionModel*)model->child_models_.at(0);
	reservoir_level.clear();
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		reservoir_level.push_back(get_reservoir_result->reservoir_level.at(i));
	}

	// Get results at each predict point
	double max_level;
	std::vector<double> level_pred_;
	max_level_vec.clear();
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		LinearPredictionModel* get_result =
			(LinearPredictionModel*)model->child_models_.at(1 + i);
		get_result->CopyLevel(level_pred_);

		max_level = *max_element(level_pred_.begin(), level_pred_.end());
		max_level_vec.push_back(max_level);
	}
}

void NmOptimizer::CalculateDangerScore()
{
	// Calculate the danger score at each predict point 
	double max_limit, min_limit;
	danger_score_vec.clear();
	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		max_limit = g_dataarchive->level_limit.at(i).at(0);
		min_limit = g_dataarchive->level_limit.at(i).at(1);

		// Calculating danger score at each point
		danger_score_vec.push_back((max_level_vec.at(i) - min_limit) / (max_limit - min_limit) * 100);
	}
	// Calculate average of the downstream danger score
	danger_score = std::accumulate(danger_score_vec.begin(), danger_score_vec.end(), 0.0) /
		danger_score_vec.size();

	// Get max and min reservoir levels
	double max_sj_level = *max_element(reservoir_level.at(0).begin(), reservoir_level.at(0).end());
	double max_ja_level = *max_element(reservoir_level.at(1).begin(), reservoir_level.at(1).end());
	double min_sj_level = *min_element(reservoir_level.at(0).begin(), reservoir_level.at(0).end());
	double min_ja_level = *min_element(reservoir_level.at(1).begin(), reservoir_level.at(1).end());

	// If the reservoir level exceeds limit, regard it as a dangerous situation which must not happen.
	if (max_sj_level > g_dataarchive->sj_upper_level_limit ||
		min_sj_level < g_dataarchive->sj_lower_level_limit)
	{
		danger_score += 1000;
		//printf("Seomjin reservoir level changed beyond the limit\n");
	}
	if (max_ja_level > g_dataarchive->ja_upper_level_limit ||
		min_ja_level < g_dataarchive->ja_lower_level_limit)
	{
		danger_score += 1000;
		//printf("Juam reservoir level changed beyond the limit\n");
	}
}

std::vector<double> NmOptimizer::GetOptimizedSJDischarge()
{
	std::vector<double> final_param_vec = GetFinalParams();
	std::vector<double> result_vec;
	for (int i = 0; i < length_discharge_; i++)
		result_vec.push_back(final_param_vec.at(i));
	return result_vec;
}

std::vector<double> NmOptimizer::GetOptimizedJADischarge()
{
	std::vector<double> final_param_vec = GetFinalParams();
	std::vector<double> result_vec;
	for (int i = length_discharge_; i < 2 * length_discharge_; i++)
		result_vec.push_back(final_param_vec.at(i));
	return result_vec;
}

// This expands short discharge to full length discharge vector
std::vector<double> NmOptimizer::ExpandDischarge(std::vector<double> discharge_short)
{
	std::vector<double> discharge_full;
	int length_short = discharge_short.size();
	int num_timestep = g_dataarchive->simulation_duration;
	for (int i = 0; i < length_short; i++)
	{
		for (int j = 0; j < num_timestep / length_short; j++)
		{
			discharge_full.push_back(discharge_short.at(i));
		}
	}
	discharge_full.push_back(discharge_short.back());

	return discharge_full; 
}

std::string Numbering(int i)
{
	if (i >= 0 && i < 10)
		return ("00" + std::to_string(i));
	else if (i >= 10 && i < 100)
		return ("0" + std::to_string(i));
	else if (i >= 100 && i < 1000)
		return (std::to_string(i));
	else
		throw("[NmOptimizer] This input is not supported.");
}