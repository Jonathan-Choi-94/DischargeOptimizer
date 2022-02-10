#pragma once

#include "UdeLogger.h"
#include "UdeCsModel.h"
#include "DamCorrelation.h"
#include "DataArchive.h"

class ReservoirPredictionModel : public UdeCsModel
{
public:
	ReservoirPredictionModel();
	virtual ~ReservoirPredictionModel();

	// Engine member function
	bool DerivativeFn(const double& t, double* y, double* dydt) override;
	bool OutputFn(const double& t, double* y) override;

	// Initializing functions
	void Initialize(int& dataset);
	void Initialize(int& dataset, doublematrix new_discharge);

	// Other member functions
	void UpdateDamLevel();
	double CalculateReservoirLevel(int& index_dam, double& Volume, double h_pred1, double h_pred2);
	void CopyLevel(int index_dam, std::vector<double>& level);


	// Member variables
	doublematrix dam_discharge,
		reservoir_level, reservoir_volume, reservoir_inflow;
	int ts, num_dam;
	double delt;
};