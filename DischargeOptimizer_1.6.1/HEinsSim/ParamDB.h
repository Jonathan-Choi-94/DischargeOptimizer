#include "HEinsSim.h"
#include <memory>

template <typename T>
class UDESIM_EXT_CLASS ParamMap
{
public:
	std::map<std::string, T> paramMap;

	ParamMap()
	{
	}

	~ParamMap()
	{
	}

	bool GetValue(const std::string& param_id, T& rev_val);
};

class UDESIM_EXT_CLASS ParamObject
{
public:
	ParamObject()
	{
	}

	virtual ~ParamObject()
	{
	}
};

class UDESIM_EXT_CLASS UDEGlobalDB
{
	std::map<std::string, ParamMap<bool>> flag_param_map_;
	std::map<std::string, ParamMap<int>> int_param_map_;
	std::map<std::string, ParamMap<double>> double_param_map_;
	std::map<std::string, ParamMap<std::string>> str_param_map_;
	std::map<std::string, std::map<std::string, std::shared_ptr<ParamObject>>> param_obj_map_;

public:
	UDEGlobalDB()
	= default;

	~UDEGlobalDB()
	= default;

	void RegisterFlagParam(const std::string& param_id, const bool& val, const std::string& mdl_name = "");

	void RegisterIntParam(const std::string& param_id, const int& val, const std::string& mdl_name = "");

	void RegisterDoubleParam(const std::string& param_id, const double& val, const std::string& mdl_name = "");

	void RegisterStrParam(const std::string& param_id, const std::string& val, const std::string& mdl_name = "");

	void RegisterObjParam(const std::string& param_id, ParamObject* val, const std::string& mdl_name = "");

	bool GetFlagValue(const std::string& param_id, bool& ret_value, const std::string& mdl_name = "");

	bool GetIntValue(const std::string& param_id, int& ret_value, const std::string& mdl_name = "");

	bool GetDoubleValue(const std::string& param_id, double& ret_value, const std::string& mdl_name = "");

	bool GetStrValue(const std::string& param_id, std::string& ret_value, const std::string& mdl_name = "");

	bool GetObjValue(const std::string& param_id, ParamObject* ret_value, const std::string& mdl_name = "");
};
