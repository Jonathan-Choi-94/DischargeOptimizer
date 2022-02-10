#pragma once
#include "UdeCsIoBase.h"
#include "UdeMessage.h"

class UdeMessage;
typedef std::pair<unsigned int, void *> OutPortValuePair;

enum UDESIM_EXT_CLASS CSEvDetectMode
{
	//Approimate the generation times of events as an identical time 
	//in a time-step advancement
	APPROXIMATE_MODE = 0,
	//Approimate the generation times of events
	INTER_EV_GEN_MODE = 1,
};
// An 190905 RK4, EULER 
enum CsEquation { RK4, EULER };
// An 190905 time_step_
enum CsTimeStep 
{
	
	SOLVER_TIME_STEP = 0,
	MODEL_TIME_STEP = 1
};
class UDESIM_EXT_CLASS UdeCsModel : public UdeCsIoBase
{
public:
	UdeCsModel();
	UdeCsModel(const std::string& name);
	virtual ~UdeCsModel();
	double *dydx_, *dym_, *dyt_, *yt_;
	//get advanced time step
	double h_;

public:
	virtual bool EsConverter(const UdeMessage& msg);
	virtual bool DerivativeFn(const double& t, double* y, double* dydt);
	virtual bool OutputFn(const double& t, double* y);
	virtual bool SeConverter(const double& t, UdeMessage& msg);
	virtual bool IsInterEvDetected(double& ev_gen_time, const double& t);

	/**
	 * set number of continuous time state
	 * \param state_size 
	 */
	void SetCsStateNum( const int& state_size );
	//assign the initial values of continuous state by index  
	void InitCsStateValue(const int& cs_state_index, const double& initial_value) const;
	//set cont. output value  
	void SetCsOutValue(const int& out_port_id, const double& value);
	//get CS state pointer 
	double* GetCsStatePtr(const int& cs_state_id) const;
	//get current CS state value
	double GetCsStateValue(const int& cs_state_id) const;
	//get past CS state value
	double GetPastCsStateValue(const int& cs_state_id) const;
	//get past CS time
	double GetPastTime() const;
	//get advanced CS time
	double GetAdvancedTimeStep() const;;
	//set the model to generate an event between cs time points 
	void SetInterEvGenMode();

protected:
	int state_num_; // m_output_num, m_input_num;
	double *state_set_; // current CS state
	double *state_set_old_; // past CS state 

public: 
	// //called by engine to notify the advanced time step
	void SetAdvancedTimeStep(const double& cur_h) { h_ = cur_h; }

public:
	void Propagate(const double& t, const double& h);
	TimeType MakeCsEvent(const double& t, std::list<OutPortValuePair>& out_port_value_pair);
	//deliver the continuous-time output
	void DeliverCsOutput(const double& t);
	void RePropagate(double t, double h);

public: //numerical solvers
	void Rk4(double* y_in, double t, double h, double* y_out);
	void Euler(double* y_in, double t, double h, double* y_out);

private:
	std::map<unsigned int, double> input_set_old_;
	std::map<unsigned int, double> output_set_old_;
	double past_time_;

public:
	//set the model to generate an event between cs time points 
	CSEvDetectMode ev_detect_mode_;
	// An 190905 time_step_
	CsTimeStep cs_time_step;
	double time_step;

public: 
	// An @@ equation Select
	void SetCsEquation(const CsEquation& e);
	void SetCsTimeStep(const CsTimeStep& e);
	void SetTimeStep(const double& t);

private : 
	// An @@ equation Select
	CsEquation csEquation;
};
