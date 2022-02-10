#pragma once
#include "UdeCsModel.h"

/**
 *  CS(Continuous-system)-generated event type  
 *  Model, output src_port, value pointer
 */
//typedef std::tuple<UdeCsModel *, unsigned int, void*> CSGenEvType;
struct CsGenEvType
{
	TimeType time;
	UdeCsModel* ptr_model;
	int src_port;
	void* ptr_value;
};


class UDESIM_EXT_CLASS CsSimEngine
{
public:
	CsSimEngine();
	~CsSimEngine();

	//enroll the continuous model 
	void SetCsModels(std::vector<UdeCsModel*>& models);

	//set time step of numerical solver
	void SetSolverTimeStep(const double& time_step);

	//@박종현 21.05.28 연속시간 엔진에도 이산사건 엔진의 sim_rt_manager 설정
	void SetSimRtManager(SimRtManager& sim_rt_manager);

	//get time step
	TimeType GetCsTimeStep() const;

	//update connected input CS values based on generated CS outputs 
	void DeliverCsOutput();

	/**
	 * generate continuous-time or event-type outputs of continuous models  
	 * \param cs_gen_events: generated event lists forwarding discrete event models
	 * \param time_step: time to calculate the next time 
	 * returns the minimum time of cs events
	 */
	TimeType GetAdvancedCsTime(std::list<CsGenEvType>& cs_gen_events, const double& time_step);
	void ReCsPropagate( const TimeType& min_advanced_time, const double& new_time_step );

	//get continuous time of CS engine
	TimeType GetCsCurrentTime() const;

	//init the CS engine
	void Initialize();

	TimeType PreSimulation(const TimeType& current_time, const TimeType& next_ev_time,
	                       std::list<CsGenEvType>& cs_gen_ev_list, const bool& opt_mode_on);

	//rest engine variables 
	void Reset();

private:
	//to exact the generated events of a CS model  
	void SetCsEvents(std::list<CsGenEvType>& cs_gen_events, const double& next_cs_time, UdeCsModel* ptr_cs_model);
private:
	//vector of all continuous models
	std::vector<UdeCsModel *> approx_cs_models_;
	std::vector<UdeCsModel *> inter_ev_gen_models_;
	std::vector<UdeCsModel *> cs_models_;

	//current time of CS engine 
	TimeType cur_cs_time_;
	//time step for numerical solver 
	TimeType time_step_;

	//@박종현 21.05.28 sim_rt_manager(이산사건 엔진에서 생성 된 객체 참조)
	SimRtManager* sim_rt_manager_;

};
