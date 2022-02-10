#include "ReservoirPredictionModel.h"

ReservoirPredictionModel::ReservoirPredictionModel()
{
	//set name
	SetName("ReservoirPredictionModel");

	//set number of state
	SetCsStateNum(2);

	// Set the number of dams
	num_dam = 2;

	// Setting initial reservoir level, volume and inflow
	std::vector<double> v;
	for (int i = 0; i < num_dam; i++)
	{
		reservoir_level.push_back(v);
		reservoir_volume.push_back(v);
		reservoir_inflow.push_back(v);
	}
	
	reservoir_level.at(0).push_back(g_dataarchive->initial_sjdam_level);
	reservoir_level.at(1).push_back(g_dataarchive->initial_jadam_level);
	reservoir_volume.at(0).push_back(Seomjin::GetVolume(reservoir_level.at(0).front()));
	reservoir_volume.at(1).push_back(Juam::GetVolume(reservoir_level.at(1).front()));
	reservoir_inflow.at(0) = g_dataarchive->sj_inflow;
	reservoir_inflow.at(1) = g_dataarchive->ja_inflow;
	
	ts = 0;
	delt = g_dataarchive->time_step;
}

ReservoirPredictionModel::~ReservoirPredictionModel()
= default;

bool ReservoirPredictionModel::DerivativeFn(const double& t, double* y, double* dydt)
{
	dydt[0] = 0;
	dydt[1] = 0;
	return true;
}

bool ReservoirPredictionModel::OutputFn(const double& t, double* y)
{
	ts = g_dataarchive->start_time_index + static_cast<int> (round(t / delt / 0.1)) * 0.1;
	UpdateDamLevel();
	 
	return true;
}

// Initialize input with the specific dataset in DataArchive
void ReservoirPredictionModel::Initialize(int& dataset)
{
	dam_discharge.clear();
	reservoir_level.clear();
	reservoir_volume.clear();

	// Setting initial reservoir level, volume and inflow
	std::vector<double> v;
	for (int i = 0; i < num_dam; i++)
	{
		reservoir_level.push_back(v);
		reservoir_volume.push_back(v);
	}
	reservoir_level.at(0).push_back(g_dataarchive->initial_sjdam_level);
	reservoir_level.at(1).push_back(g_dataarchive->initial_jadam_level);
	reservoir_volume.at(0).push_back(Seomjin::GetVolume(reservoir_level.at(0).front()));
	reservoir_volume.at(1).push_back(Juam::GetVolume(reservoir_level.at(1).front()));

	// Setting discharge at dams
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		dam_discharge.push_back(g_dataarchive->dataset_dam_discharge.at(dataset).at(i));
	}
}

// Initialize input with the new discharges at dam
void ReservoirPredictionModel::Initialize(int& dataset, doublematrix new_discharge)
{
	dam_discharge.clear();
	reservoir_level.clear();
	reservoir_volume.clear();

	// Setting initial reservoir level, volume and inflow
	std::vector<double> v;
	for (int i = 0; i < num_dam; i++)
	{
		reservoir_level.push_back(v);
		reservoir_volume.push_back(v);
	}
	reservoir_level.at(0).push_back(g_dataarchive->initial_sjdam_level);
	reservoir_level.at(1).push_back(g_dataarchive->initial_jadam_level);
	reservoir_volume.at(0).push_back(Seomjin::GetVolume(reservoir_level.at(0).front()));
	reservoir_volume.at(1).push_back(Juam::GetVolume(reservoir_level.at(1).front()));

	// Setting discharge at dams
	for (int i = 0; i < g_dataarchive->num_dam; i++)
	{
		dam_discharge.push_back(new_discharge.at(i));
	}
}

// Update dam level
void ReservoirPredictionModel::UpdateDamLevel()
{
	for (int i = 0; i < num_dam; i++)
	{
		if (ts < dam_discharge.at(i).size())
		{ 
			double next_reservoir_volume = reservoir_volume.at(i).back() +
				(reservoir_inflow.at(i).at(ts) - dam_discharge.at(i).at(ts)) * 3600;
			reservoir_volume.at(i).push_back(next_reservoir_volume);
			double next_reservoir_level = CalculateReservoirLevel
			(i, next_reservoir_volume, reservoir_level.at(i).back() + 1, reservoir_level.at(i).back() - 1);
			reservoir_level.at(i).push_back(next_reservoir_level);
		}
	}
}

// This function calculates reservoir level with volume by regula-falsi method (numerical method)
double ReservoirPredictionModel::CalculateReservoirLevel(int& index_dam, double& Volume, double h_pred1, double h_pred2)
{
	if (index_dam == 0)
	{
		auto fun = [&Volume](double h) ->double {return (Seomjin::GetVolume(h) - Volume); };

		double err_limit = 1e-10;
		double h_new, current_err;
		if (Volume <= g_dataarchive->sj_min_volume_limit)
			return g_dataarchive->sj_min_level_limit;
		else if (Volume >= g_dataarchive->sj_max_volume_limit)
			return g_dataarchive->sj_max_level_limit;

		while (fun(h_pred1) * fun(h_pred2) > 0)
		{
			if (fun(h_pred1) > 0)
			{
				h_pred1 += -1;
			}
			else
			{
				h_pred2 += 1;
			}
		}

		int index_update = 1;
		for (int i = 0; i < 1000; i++)
		{
			h_new = h_pred2 - fun(h_pred2) * (h_pred1 - h_pred2) / (fun(h_pred1) - fun(h_pred2));
			switch (index_update)
			{
			case 1:
				current_err = abs((h_new - h_pred1) / h_pred1);
				break;
			case 2:
				current_err = abs((h_new - h_pred2) / h_pred2);
				break;
			}
			if (current_err < err_limit)
			{
				break;
			}

			if (fun(h_pred1) * fun(h_new) > 0)
			{
				h_pred1 = h_new;
				index_update = 1;
			}
			else
			{
				h_pred2 = h_new;
				index_update = 2;
			}
		}

		return h_new;
	}
	else if (index_dam == 1)
	{
		auto fun = [&Volume](double h) ->double {return (Juam::GetVolume(h) - Volume); };

		double err_limit = 1e-10;
		double h_new, current_err;
		 
		if (Volume <= g_dataarchive->ja_min_volume_limit)
			return g_dataarchive->ja_min_level_limit;
		else if (Volume >= g_dataarchive->ja_max_volume_limit)
			return g_dataarchive->ja_max_level_limit;

		while (fun(h_pred1) * fun(h_pred2) > 0)
		{
			if (fun(h_pred1) > 0)
			{
				h_pred1 += -1;
			}
			else
			{
				h_pred2 += 1;
			}
		}

		int index_update = 1;
		for (int i = 0; i < 1000; i++)
		{
			h_new = h_pred2 - fun(h_pred2) * (h_pred1 - h_pred2) / (fun(h_pred1) - fun(h_pred2));
			switch (index_update)
			{
			case 1:
				current_err = abs((h_new - h_pred1) / h_pred1);
				break;
			case 2:
				current_err = abs((h_new - h_pred2) / h_pred2);
				break;
			}
			if (current_err < err_limit)
			{
				break;
			}

			if (fun(h_pred1) * fun(h_new) > 0)
			{
				h_pred1 = h_new;
				index_update = 1;
			}
			else
			{
				h_pred2 = h_new;
				index_update = 2;
			}
		}

		return h_new;
	}
}

void ReservoirPredictionModel::CopyLevel(int index_dam, std::vector<double>& level)
{
	level.clear();
	for (int i = 0; i < reservoir_level.at(index_dam).size(); i++)
		level.push_back(reservoir_level.at(index_dam).at(i));
}