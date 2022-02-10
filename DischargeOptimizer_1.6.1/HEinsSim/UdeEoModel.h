#pragma once
#include "UdeBaseModel.h"
#include "UdeMessage.h"
#include <functional>

#define PORT_MAPPING(_port, _class, _func) AddMapping(_port, std::bind(&_class::_func, this, std::placeholders::_1))
#define DELETE_PREV_EVENT UDEIF->DelPrevEvent(this)
#define DELETE_PREV_EVENT_FEO(_func) UDEIF->DelPrevEvent(_func)
#define NEW_EVENT(_time, _class, _func) UDEIF->ScheduleEvent(_time, this, std::bind(&_class::_func, this, std::placeholders::_1))
#define NEW_EVENT_FEO(_time, _func, _priority) UDEIF->ScheduleEvent(_time, _func, _priority)

class UDESIM_EXT_CLASS UdeEoModel : public UdeBaseModel
{
public:
	UdeEoModel();
	virtual ~UdeEoModel();

	std::map<unsigned int, std::function<void(const UdeMessage &)>> func_map_;

	void AddMapping(unsigned int port, std::function<void(const UdeMessage &)>);
};

