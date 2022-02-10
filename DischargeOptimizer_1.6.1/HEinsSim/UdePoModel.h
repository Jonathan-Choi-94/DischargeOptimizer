#ifndef __POMODEL_H__
#define __POMODEL_H__
#include "UdeBaseModel.h"

#define SEND_MSG(_port, _msg) SendData(this, _port, (void*)_msg)

class UDESIM_EXT_CLASS UdePoModel :
	public UdeBaseModel
{
public:
	UdePoModel();
	~UdePoModel();

	virtual void Work()
	{
	}

	std::mutex mtx_lock_;

	bool b_end_, b_suspended_, b_scheduled_, b_wait_, b_changed_, b_advance_time_;
	bool b_wait_scheduled_, b_initial_flag_;
	bool b_first_wait_;

	bool WaitUntil();
	bool WaitUntil( const TimeType& time );
	bool AdvanceTime( const TimeType& time );
	bool Suspend();

	virtual bool SetData(unsigned int _port, void* _data);

	//bool isAlive();
};

#endif
