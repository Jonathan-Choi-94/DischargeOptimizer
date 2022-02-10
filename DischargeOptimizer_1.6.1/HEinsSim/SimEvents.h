#pragma once
#include "UdeBaseEv.h"
#include "UdeBaseModel.h"
#include <functional>
#include <memory>

class ModelSchEnv : public UdeBaseEv
{
public:
	ModelSchEnv(const TimeType& time, UdeBaseModel* src_model, UdeBaseModel* exec_model, const int priority)
		: UdeBaseEv(time, INT_EV_TYPE, priority)
		  , exec_model_(exec_model)
		  , src_model_(src_model)
	{
	}

	virtual ~ModelSchEnv() = default;

public:
	UdeBaseModel* GetExecModel() const { return exec_model_; }
	UdeBaseModel* GetSrcModel() const { return src_model_; }

protected:
	UdeBaseModel* exec_model_;
	UdeBaseModel* src_model_;
};


class EomEvent final : public ModelSchEnv
{
public:
	EomEvent(const TimeType& _time, UdeBaseModel* src_model, UdeBaseModel* exec_model,
	         std::function<void(const UdeMessage&)> func, const int & priority)
		: ModelSchEnv(_time, src_model, exec_model, priority)
		  , ptr_fn(std::move(func))
	{
	}

public:
	std::function<void(const UdeMessage&)> GetFunc() const { return ptr_fn; }


public:
	std::function<void(const UdeMessage&)> ptr_fn;
};

class ExtTrigEv : public UdeBaseEv
{
public:
	ExtTrigEv(const TimeType& time, UdeBaseModel* src_model, const unsigned int &src_out_port,
	      UdeBaseModel* dest_model, unsigned int & dest_in_port, std::shared_ptr<void>& msg_ptr)
		: UdeBaseEv(time, EXT_EV_TYPE, 0)
		  , src_model_(src_model)
		  , dest_model_(dest_model)
		  , src_out_port_(src_out_port)
		  , dest_in_port_(dest_in_port)
	{
		if (msg_ptr)
		{
			msg_ptr_ = msg_ptr;
		}
	}

	virtual ~ExtTrigEv() = default;

public:
	UdeBaseModel* GetSrcModel() const { return src_model_; }
	UdeBaseModel* GetDestModel() const { return dest_model_; }
	unsigned int GetSrcOutPort() const { return src_out_port_; }
	unsigned int GetDestInPort() const { return dest_in_port_; }
	void* GetMsg() const { return msg_ptr_.get(); }

public:
	UdeBaseModel* src_model_;
	UdeBaseModel* dest_model_;
	unsigned int src_out_port_;
	unsigned int dest_in_port_;

	std::shared_ptr<void> msg_ptr_;
};
