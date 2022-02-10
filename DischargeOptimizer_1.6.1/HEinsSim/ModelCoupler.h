#pragma once
#include "UdeCoupModel.h"

class ModelCoupler final {
public:
	ModelCoupler();

	/**
	 * Constructor for model coupler
	 * \param opt_mode_on: enables to fetch the inner optimization settings of models 
	 */
	explicit ModelCoupler(const bool& opt_mode_on);
	~ModelCoupler();

	bool FlattenModel(UdeCoupModel* out_most_coup_model);
	bool GetCsModels(std::vector<UdeCsModel *>& cs_models);

	// @An Runtime �� �߰� �� �� Ŀ�ø�
	bool FlattenRuntimeModel(UdeCoupModel* out_most_coup_model);
	// @An Runtime �� ����
	bool RemoveRuntimeModelPort(UdeCoupModel* out_most_coup_model, UdeBaseModel* model);

	// @������ 21.05.31 ModelCoupler Reset. ������ Reset �� �ݺ� ���� �� ModelCoupler�� Reset�� ȣ��
	void ModelCouplerReset();

private:
	void MakeInitCoupling(UdeCoupModel* coup_model); // @An
	void MakeCoupling(UdeCoupModel* coup_model); // @An
	void MakeCsCoupling(UdeCoupModel* coup_model);
	void UpdateOptData(UdeBaseModel* each_child);
	void SetPriorityAndCsModels(UdeCoupModel* coup_model);

	void ShowUnConnSource();
	//void ShowModelInfo(UdeCoupModel * m);

	//APIs for make coupling
	bool IdentifyCoupling(); // @An
	bool IdentifyCsCoupling();

	// �� �� ���� 
	void AtomicToAtomic(const ModelPortPair in_model_port_pair, const ModelPortPair prev_am, const int type);
	// ���� Ŀ�� �� �˻� 
	void AtomicToCouple(const ModelPortPair in_model_port_pair, const ModelPortPair prev_am, const int type);

	bool IsPrevAmExist(const ModelPortPair& in_model_port_pair, ModelPortPair& prev_am);
	bool IsPrevCsExist(const CSModelPortPair& in_model_port_pair,
	                   CSModelPortPair& prev_cs);

	void ReflectCouplingInit(
		const std::map<ModelPortPair, std::vector<ModelPortPair>>& inner_coupling);
	void ReflectCoupling(
		const std::map<ModelPortPair, std::vector<ModelPortPair>>& inner_coupling);
	void ReflectCsCoupling(
		const std::map<CSModelPortPair, std::vector<CSModelPortPair>>& inner_coupling);
	
public:
	unsigned int alloc_priority_;
	std::map<ModelPortPair, std::vector<ModelPortPair>> un_conn_cio_to_atomic_ins_;
	std::map<ModelPortPair, ModelPortPair> next_to_prev_conn_;

	std::map<CSModelPortPair, std::vector<CSModelPortPair>> un_conn_cs_cio_to_atomic_ins_;
	std::map<CSModelPortPair, CSModelPortPair> next_to_prev_cs_conn_;

	// @An �� �� Map
	std::map<std::string, std::vector<std::pair<int, std::pair<std::map<UdeBaseModel*, unsigned int>, std::map<UdeBaseModel*, unsigned int>>>>> coup_model_relation;

	//identified CS model
	std::vector<UdeCsModel *> cs_model_vec_;

	//state and probe data for optimization
	std::map<std::string, std::pair<UdeBaseModel *, double *>> opt_param_map_;
	std::map<std::string, std::pair<UdeBaseModel*, double *>> probe_data_map_;

private:
	bool opt_mode_on_;
	bool opt_data_duplicated_;
};
