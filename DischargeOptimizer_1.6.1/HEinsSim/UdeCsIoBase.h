#pragma once
#include "UdeBaseModel.h"

class UDESIM_EXT_CLASS UdeCsIoBase : public UdeBaseModel
{
public:
	typedef std::pair<UdeCsIoBase *, unsigned int> CsBasePortPair;

	UdeCsIoBase();
	explicit UdeCsIoBase(const ModelType& type);
	UdeCsIoBase(const std::string& name, const ModelType& type);

	bool AddCsInPort( const unsigned& in_port_id, const std::string& port_name );
	bool AddCsOutPort( const unsigned& out_port_id, const std::string& port_name );

	bool HasCsInPort( const unsigned& in_port_id );
	bool HasCsInPort(const std::string& in_port_name);

	bool HasCsOutPort( const unsigned& out_port_id );
	bool HasCsOutPort(const std::string& out_port_name);

	void RemoveCsInPort( const unsigned& in_port_id );
	void RemoveCsOutPort( const unsigned& out_port_id );

	std::string GetCsInPortName(const int& port_id);
	int GetCsInPortId(const std::string& in_port_name);

	std::string GetCsOutPortName(const int& port_id);
	int GetCsOutPortId(const std::string& out_port_name);

	virtual void ShowCsIoSummary();
	virtual void ShowCsDestSummary();

	void SetCsInValue( const unsigned& in_port_id, const double& val );

public: //called by engine 
	void ResetCsDestModelPortPair();
	bool AddCsDestModelPortPair(const unsigned int& src_out_port, const CsBasePortPair& dest_port);
	bool GetCsDestModelPortVec(const unsigned int& src_out_port, std::vector<CsBasePortPair>& dest_pos_vec);

private:
	bool IsCsInPortNameNew(const std::string& port_name);
	bool IsCsOutPortNameNew(const std::string& port_name);

	void AllocCsInput( const unsigned& in_port_id );
	void AllocCsOutput( const unsigned& in_port_id );

protected:
	//continuous src_port
	std::map<unsigned int, std::string> cs_in_port_map_;
	std::map<unsigned int, std::string> cs_out_port_map_;

	std::map<unsigned int, double> input_set_;
	std::map<unsigned int, double> output_set_;

	std::map<unsigned int, std::vector<CsBasePortPair>> cs_dest_poses_;
};


typedef UdeCsIoBase::CsBasePortPair CSModelPortPair;
