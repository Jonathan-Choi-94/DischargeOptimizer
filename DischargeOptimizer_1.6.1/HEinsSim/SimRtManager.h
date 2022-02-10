#pragma once
#include "HEinsSim.h"

//engine state
enum class UdeEngineState { WAISER_ENGINE_INIT = -1, WAISER_ENGINE_START, WAISER_SIM_START, WAISER_SIM_END, WAISER_SIM_PAUSE };

//real-time manager
class SimRtManager
{
public:
	SimRtManager();
	virtual ~SimRtManager();

public:
	void Initialize();
public:
	void SetEngineState(const UdeEngineState state);
	UdeEngineState GetEngineState();

	void SetRatio(const double& ratio);
	double GetRatio();

	void SetTimeTick(const double& tick);
	double GetTimeTick();

	//@void SetPause(const bool& en);
	//@bool GetPause();

	//@bool GetSimStop();
	//@void SetSimStop();
	//@bool IsEnSimEnd();

	clock_t GetLastClock();
	void UpdateClock();

	//@ SupportHLARTI
	void SetEngStart();
	bool IsEngStart();
	void SetSimStart();
	bool IsSimStart();
	void SetSimPause();
	bool IsSimPause();
	void SetSimEnd();
	bool IsSimEnd();

private:
	UdeEngineState eng_state_;

	double cur_ratio_;
	double cur_sim_tick_;
	//@bool en_pause_;
	double paused_time_;
	//@bool en_sim_end_;

	clock_t sim_start_real_time_;

private:
	std::mutex real_sim_opt_lock_;
};
