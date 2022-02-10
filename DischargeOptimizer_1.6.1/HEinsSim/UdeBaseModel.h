#pragma once
#include "HEinsSim.h"
#include "UdeBaseEv.h"

struct DeleteObj
{
	template <typename T>
	void operator()(T* ptr) const
	{
		delete ptr;
		ptr = nullptr;
	}
};

class UDESIM_EXT_CLASS UdeBaseModel
{
public:
	typedef std::pair<UdeBaseModel *, unsigned int> DestPortType;
	typedef std::tuple<UdeBaseModel *, unsigned int, double *> CsDestPortType;

	UdeBaseModel();
	explicit UdeBaseModel(const ModelType& model_type);
	UdeBaseModel(const std::string& model_name, const ModelType& model_type);
	virtual ~UdeBaseModel();

public:
	void SetName(const std::string& name);
	std::string GetName() const { return model_name_; }
	ModelType GetType() const { return type_; }

	void SetPriority( const int& priority ) { this->priority_ = priority; }
	int GetPriority() const { return priority_; }

	bool AddInPort( const unsigned& in_port_id, const std::string& port_name );
	bool AddOutPort( const unsigned& out_port_id, const std::string& port_name );

	bool HasInPort( const unsigned& in_port_id );
	bool HasInPort(const std::string& in_port_name);

	bool HasOutPort( const unsigned& out_port_id );
	bool HasOutPort(const std::string& out_port_name);

	void RemoveInPort( const unsigned& in_port_id );
	void RemoveOutPort( const unsigned& out_port_id );

	std::string GetInPortName(const unsigned& port_id) { return in_port_map_[port_id]; }
	int GetInPortId(const std::string& in_port_name);

	std::string GetOutPortName(const unsigned& port_id) { return out_port_map_[port_id]; }
	int GetOutPortId(const std::string& out_port_name);

	// @Park ������Ʈ ���� �� ������Ʈ�� ���� �� Ŀ�ø� ������ ���� ���� ���� in/out port map ȹ��
	std::map<unsigned int, std::string> GetInPortMap() { return in_port_map_; }
	std::map<unsigned int, std::string> GetOutPortMap() { return out_port_map_; }

	void SetParentModel( UdeBaseModel* model ) { parent_model_ = model; }
	UdeBaseModel* GetParentModel() const { return parent_model_; }


	// @Park ��Ÿ�� ������Ʈ ���� �Լ�. �θ� �𵨿��� �ڱ��ڽ��� �����ϴ� ��츦 ����.
	void RemoveComponent(const std::string&);
	void RemoveComponent(UdeBaseModel*);
private:
	// @Park �˻��Ϸ��� �𵨿��� �־��� �̸��� ���� ã�� �Լ�. �� ã�� ��� �θ� �𵨿��� �˻��ϴ� ��� �Լ�.
	UdeBaseModel* FindCompoentFromParentRecursive(const std::string& exec_model_name, UdeBaseModel* search_model);

public:
	virtual void ShowIoSummary();
	virtual void ShowDestSummary();

	void SetLastModelSchEv(UdeBaseEv* ptr_ev) { prev_ev_ptr_ = ptr_ev; }
	UdeBaseEv* GetLastModelSchEv() const { return prev_ev_ptr_; }

	void ResetDestModelPortPair();
	bool AddDestModelPortPair(const unsigned int& src_out_port, const DestPortType& dest_port );
	bool GetDestModelPortVec(const unsigned int& src_out_port, std::vector<DestPortType>& dest_pos_vec);
	bool RemoveDestModelPort(const UdeBaseModel* model);
	//optimization
	/**
	 * \brief AddOptParam
	 * \param param_name: the name of initial parameter
	 * \param param_ptr: the value ptr of initial pramter 
	 * \return 
	 */
	void AddOptParam(const std::string& param_name, double* param_ptr);
	/**
	 * \brief 
	 * \param probe_data_name : the probing y value name
	 * \param ptr_probe_data  : the probing y value pointer
	 */
	void AddProbeData(const std::string& probe_data_name, double* ptr_probe_data);

private:
	bool IsInPortNameNew(const std::string& port_name );
	bool IsOutPortNameNew(const std::string& port_name );

protected:
	std::string model_name_;
	ModelType type_;
	UdeBaseModel* parent_model_;

	//continuous src_port
	std::map<unsigned int, std::string> in_port_map_;
	std::map<unsigned int, std::string> out_port_map_;
	std::map<unsigned int, std::vector<DestPortType>> dest_poses_;

public: //for optimization
	std::map<std::string, double *> opt_param_map_;
	std::map<std::string, double *> probe_data_map_;

	int priority_;
	UdeBaseEv* prev_ev_ptr_;
};

typedef UdeBaseModel::DestPortType ModelPortPair;
