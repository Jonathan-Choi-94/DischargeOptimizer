#include "PredPointCoupledModel.h"

PredPointCoupledModel::PredPointCoupledModel()
{
	SetName("PredPointCoupledModel");
	//========================================================================
	/* Setting models and coupling */
	//========================================================================
	// generating and setting models 
	
	UdeCsModel* reservoirpredictionmodel = new ReservoirPredictionModel();
	AddComponent(reservoirpredictionmodel);

	for (int i = 0; i < g_dataarchive->predict_point.size(); i++)
	{
		UdeCsModel* linearpredictionmodel = nullptr;
		// Generating the new prediction model with model class 
		linearpredictionmodel = new LinearPredictionModel(i);
		
		// Add model to the engine
		AddComponent(linearpredictionmodel);
	}
}

PredPointCoupledModel::~PredPointCoupledModel()
{
}