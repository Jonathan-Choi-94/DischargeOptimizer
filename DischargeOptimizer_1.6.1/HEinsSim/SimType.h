#ifndef __SIM_TYPE_H__
#define __SIM_TYPE_H__
#include <limits>
#include <utility>

#define NOMINMAX

const unsigned int ERROR_PORT = std::numeric_limits<unsigned int>::max();
const unsigned int OUTTER_MODEL_PORT = std::numeric_limits<unsigned int>::max()-1;

enum PortType { kInPort, kOutPort };

enum ModelType
{
	M_TYPE_ERROR,
	COUPLED_TYPE,
	DEVS_ATOMIC_TYPE,
	EO_ATOMIC_TYPE,
	PO_ATOMIC_TYPE,
	DT_ATOMIC_TYPE,
	CS_ATOMIC_TYPE
};

enum EvType { EV_TYPE_UNKNOWN, INT_EV_TYPE, EXT_EV_TYPE };

enum EngState { READY, RUN, OK_SUCCESS, OK_FAIL };

#define POOLING_TIME 1
#define UDEIF		GetEngInterface()
#define PARAM_DB_PTR UDEIF->GetParamDB()
#define SLEEP_MIL_SEC(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

#define EV_TIME_UNKNOWN -99999.0
const unsigned int EV_PRIORITY_UNKNOWN = std::numeric_limits<int>::max();


#endif
