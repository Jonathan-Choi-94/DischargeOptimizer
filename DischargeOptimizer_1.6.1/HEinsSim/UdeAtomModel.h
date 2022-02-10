#ifndef __ATOMIC_H__
#define __ATOMIC_H__
#include "UdeMessage.h"
#include "UdeBaseModel.h"

class UDESIM_EXT_CLASS UdeAtomModel : public UdeBaseModel
{
	bool m_bContinue;
public:
	UdeAtomModel();
	explicit UdeAtomModel(const std::string& name);
	virtual ~UdeAtomModel();

	virtual bool ExtTransFn(const UdeMessage&) { return false; }
	virtual bool IntTransFn() { return false; }
	virtual bool OutputFn(UdeMessage& msg) { return false; }
	virtual TimeType TimeAdvanceFn() { return TimeType( -1 ); }
	void Continue();

	bool GetContinueValue()
	{
		if (m_bContinue)
		{
			m_bContinue = false;
			return true;
		}
		return false;
	}
};
#endif
