#pragma once
#include "UdeCoupModel.h"
#include "ReservoirPredictionModel.h"
#include "LinearPredictionModel.h"

class PredPointCoupledModel : public UdeCoupModel
{
public:
	PredPointCoupledModel();
	virtual ~PredPointCoupledModel();
};

