#ifndef __MSG_H__
#define __MSG_H__
#include "HEinsSim.h"
#include "TimeType.h"

class UDESIM_EXT_CLASS UdeMessage
{
public:
	UdeMessage();
	explicit UdeMessage(const TimeType& time);
	explicit UdeMessage(const double & time);
	~UdeMessage();

	//get generation time of message 
	TimeType GetTime() const;
	void SetPortValue(const unsigned int& port, void* value_ptr);

	//get value with src_port
	void GetPortValuePair(unsigned int& port, void* value_ptr) const;

	//get a src_port num of first data 
	unsigned int GetPort() const;

	//get a value of first data 
	void* GetValue() const;

	//called by the engine to get the src_port-value pairs
	void ExtractPortValues(std::list<std::pair<unsigned int, void *>>& port_val_pair_list);

	//time and port pair to string for debugging
	std::string ToString();

	//change the message time by user considering the delay 
	void SetDelay(const TimeType& time);

private:
	TimeType time_;
	std::list<std::pair<unsigned int, void *>> port_value_pairs_;
};
#endif
