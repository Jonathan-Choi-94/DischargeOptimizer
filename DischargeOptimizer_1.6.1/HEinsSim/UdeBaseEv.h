#ifndef __EVENT_H__
#define __EVENT_H__
#include "HEinsSim.h"
#include "SimType.h"
#include "TimeType.h"

class UDESIM_EXT_CLASS UdeBaseEv
{
public:

	UdeBaseEv(const TimeType& time, const EvType& type, int priority);
	UdeBaseEv(const TimeType& time, const EvType& type);

	virtual ~UdeBaseEv() = default;

private:
	UdeBaseEv();

public:
	TimeType GetTime() const { return sch_time_; }
	bool IsActive() const { return active_; }
	void Nullify() { active_ = false; }

	EvType GetEvType() const { return ev_type_; }

	void SetPriority(const int& arg_priority)
	{
		priority_ = arg_priority;
	}

	int GetPriority() const
	{
		return priority_;
	}

public:
	TimeType sch_time_;
	bool active_;
	EvType ev_type_;
	int priority_;
};

/// <summary>
/// UdeBaseEv ���� �켱 ���� �񱳸� ���� Comparator ����ü<para/>
/// UdeEngine.cpp���� UdeBaseEv�� �켱���� ť�� ������ ��, �̺�Ʈ�� �켱������ ���߾� �����ϱ� ���� ���
/// </summary>
struct EventComparator
{
	bool operator()(const UdeBaseEv* ev1, const UdeBaseEv* ev2) const
	{
		if (ev1->sch_time_ > ev2->sch_time_)
		{
			return true;
		}
		if (ev1->sch_time_ == ev2->sch_time_)
		{
			if (ev1->GetEvType() > ev2->GetEvType())
				return true;
			if (ev1->GetEvType() == ev2->GetEvType())
			{
				if (ev1->priority_ > ev2->priority_)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}

		return false;
	}
};

#endif
