#ifndef __COUPLED_H__
#define __COUPLED_H__
#include "UdeCsModel.h"

//class UDESIM_EXT_CLASS UdeEngine;
class UDESIM_EXT_CLASS UdeCoupModel : public UdeCsIoBase
{
public:
	typedef std::pair<ModelPortPair, ModelPortPair> CouplingType;
	typedef std::pair<CSModelPortPair, CSModelPortPair> CSCouplingType;

	std::vector<UdeBaseModel *> child_models_;

	UdeCoupModel();
	explicit UdeCoupModel(const std::string& name);
	virtual ~UdeCoupModel();

	void AddComponent(UdeBaseModel*);
	// @An 모델 실행 중 컴포넌트, 커플링 삭제
	void RemoveComponentInCoupModel(UdeBaseModel*);
	void AddCoupling(UdeBaseModel*, const unsigned int&, UdeBaseModel*, const unsigned int&);
	void AddCoupling(UdeBaseModel*, const std::string&, UdeBaseModel*, const std::string&);
	void AddCsCoupling(UdeBaseModel*, const unsigned int&, UdeBaseModel*, const unsigned int&);
	void AddCsCoupling(UdeBaseModel*, const std::string&, UdeBaseModel*, const std::string&);

	// @An 모델 실행 중 컴포넌트, 커플링 추가
	void AddRuntimeComponent(UdeBaseModel*);
	void AddRuntimeCoupling(UdeBaseModel*, const unsigned int&, UdeBaseModel*, const unsigned int&);
	
	bool IsChildModel(UdeBaseModel*);
	UdeBaseModel* FindComponent(const std::string& name);
	// @Park 결합 모델의 모든 자식 모델에서 주어진 이름의 모델을 찾는 재귀 함수
	UdeBaseModel* FindAllChildComponent(const std::string& name);

	void SetPriority(int num, ...);
	void SetPriority(const std::vector<std::string>& models);

	void ShowIoSummary() override;

private:
	bool SaveCouplingToMap(std::map<ModelPortPair, std::vector<ModelPortPair>>& coup_map, const CouplingType& coupling);
	bool SaveCsCouplingToMap(std::map<CSModelPortPair, std::vector<CSModelPortPair>>& coup_map,
	                         const CSCouplingType& coupling);
	// @Park 모델 실행 중 커플링 삭제
	void RemoveRuntimeCoupling(UdeBaseModel*);
	void RemoveCouplingFromMapByKey(std::map<ModelPortPair, std::vector<ModelPortPair>>& coup_map, std::vector<ModelPortPair>& rm_model_port_vec);
	void RemoveCouplingFromMapByValue(std::map<ModelPortPair, std::vector<ModelPortPair>>& coup_map, std::vector<ModelPortPair>& rm_model_port_vec);

public:
	// coupled model to inner atomic
	std::map<ModelPortPair, std::vector<ModelPortPair>> cin_to_atm_in_;
	std::map<ModelPortPair, std::vector<ModelPortPair>> atm_out_to_atm_in_;
	std::map<ModelPortPair, std::vector<ModelPortPair>> atm_out_to_cout_;
	// 커플 모델 
	std::vector<std::pair<int, std::pair<std::map<UdeBaseModel*, unsigned int>, std::map<UdeBaseModel*, unsigned int>>>> coup_model_relation;

	std::map<CSModelPortPair, std::vector<CSModelPortPair>> cs_cin_to_atm_in_;
	std::map<CSModelPortPair, std::vector<CSModelPortPair>> cs_atm_out_to_atm_in_;
	std::map<CSModelPortPair, std::vector<CSModelPortPair>> cs_atm_out_to_cout_;
};

#endif
