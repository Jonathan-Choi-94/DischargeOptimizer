#pragma once
#include "ModelCoupler.h"
#include "ParamDB.h"
#include "SimRtManager.h"
#include "SimHLARTIManager.h"
#include "SimLicenseManager.h"
#include "OptEvalFunc.h"

#include "CsSimEngine.h"
#include "UdePoModel.h"
#include "UdeEoModel.h"

class UDESIM_EXT_CLASS UdeEngine : public OptEvalFunc
{
private:
	CsSimEngine cs_sim_engine_;	// ���� �ð� �ùķ��̼� ����
	bool cs_engine_active_;		// ���� �ð� �ùķ��̼� ���

	ModelCoupler model_coupler_;	// �� ���� ���� ������
	UDEGlobalDB eng_param_db_;		// ���� ���� �Ķ���� DB

	/* manager ��ü */
	SimRtManager sim_rt_manager_;			// real-time simulation manager
	SimHLARTIManager sim_hlarti_manager_;	// HLA/RTI simulation manager
	SimLicenseManager sim_license_manager_;	// license manager

	/* �ùķ��̼� ������ ������ */
	std::unique_ptr<UdeCoupModel> initial_model_;	// ������ ���� �� �ֻ��� ���� ��
	std::list<UdePoModel*> po_model_list_;			// PoModel ����Ʈ
	std::priority_queue<UdeBaseEv*, std::vector<UdeBaseEv*>, EventComparator> priority_queue_;	// EventQueue
	TimeType cur_sim_time_;	// ���� ���� �ð�
	TimeType end_time_;		// �ùķ��̼� ���� �ð�

	/* thread mutex */
	std::mutex po_list_lock_;			// PoModel list mutex
	std::mutex priority_queue_lock_;	// EventQueue mutex 
	std::mutex rti_lock_;				// HLA/RTI mutex

	/* For real time, HLA/RTI simulation 
	* �ǽð� �ùķ��̼��� ���ؼ��� en_thread_�� en_realtime_sim_�� ��� Ȱ��
	* HLA/RTI �ùķ��̼��� ���ؼ��� en_thread_�� Ȱ���ص� ��� ����
	*/
	std::thread eng_exec_thread_;	// ���� ������
	bool en_thread_;		// ������ ��� ���
	bool en_realtime_sim_;	// real time �ùķ��̼� ���
	bool opt_mode_;			// opt mode ���

public:
	std::mt19937_64 random_gen_;

public: // ���� ���� �� singleton ���� ����
	static UdeEngine* GetInstance();
	static void Destroy();
private:
	static UdeEngine* ptr_eng_instance_;
	UdeEngine();
	~UdeEngine();

public: // �� ���� �� �ʱ�ȭ, �α�
	void Randomize();	// random_gen_ �ʱ�ȭ

	static bool EnableEngDebugLogger();		// ���� ����� �ΰ� Ȱ��ȭ
	static bool DisableEngDebugLogger();	// ���� ����� �ΰ� ��Ȱ��ȭ

	void SetModel(UdeCoupModel*);	// �ùķ��̼� Ÿ�� ���� ������ ����
	void SetRunTimeModel();			// ���� ���� �� �� ���� �� ȣ���� ��Ÿ�� �� ����
	void RemoveRunTimeModel(UdeBaseModel* model);	// ���� ���� �� �� ���Ž� ���� �𵨿��� ���
	
	void ShowModelInfo() const;	// ��ü �𵨿� ���� ���� ���� ���
	void ShowModelInfo(UdeCoupModel* coup_model) const;	// �־��� ���� �𵨿� ���� ���� ���� ���

	void Reset();	// ���� �ʱ�ȭ
	void Reset(UdeCoupModel *ude_coup_model);	// ���� �ʱ�ȭ�� ���ÿ� ���� ������ ����

public: // Int/Ext event�� ������� ���� �Լ�
	void ScheduleIntEvent(const TimeType& ta, UdeBaseModel* src_model);	// Internal Event�� ������
	void ScheduleExtEvent(const TimeType& scheduled_time, UdeBaseModel* src_model,
		const unsigned int& out_port, void* ptr_msg_value);				// External Event�� ������
	void ScheduleIntEvent(const TimeType& ta, UdeBaseModel* src_model,
		std::function<void(const UdeMessage&)> func);		// PoModel���� ȣ���ϴ� Internal Event ������
	void ScheduleExtEventFromOutter(const TimeType& scheduled_time, UdeBaseModel* dest_model, 
		unsigned int& dest_in_port, void* ptr_msg_value);	// ��Ŷ ���� ���� �ܺ� �𵨿��� ������ External Event�� ���� �ܺο��� ������
	void ScheduleExtEventFromOutter(const TimeType& scheduled_time, UdeBaseModel* src_model,
		const unsigned int& src_out_port, UdeBaseModel* dest_model,	unsigned int& dest_in_port,
		void* ptr_msg_value);	// ��Ŷ ���� ���� �ܺ� �𵨿��� ������ External Event�� ���� �ܺο��� ������
private:
	bool HandleIntEv(UdeBaseEv* ptr_ev);	// EventQueue�� Internal Event ����
	bool HandleExtEv(UdeBaseEv* ptr_ev);	// EventQueue�� External Event ����

public:	// ���� ���� �� �ùķ��̼� ������ ���� �Լ�
	bool Start();	// ���� ����. ���� �ð��� ����ڰ� ������ �� �Ǵ� ����Ʈ ��
	bool Start(const TimeType& end_time);	// ���� ����. ���� �ð��� �־��� end_time ������ ����
private:
	TimeType PreSimulation(const TimeType& min_ev_time);	// ���� �ð� ���� �ùķ��̼� ����
	int EngineStart(bool& ret_ok);	// ���� ���� ���� �Լ�. ����ڴ� Start API�� ���� ���������� ����
	void WaitForThreadStop();		// ������ ������ ���� ���� �� ��, ���� �����庸�� ���� ���� ���� �ʵ��� ���

private: // �ùķ��̼� ������ ���� ���� �ʿ� �Լ�
	void MakeInitialAmEvents(UdeCoupModel*);	// �̻��� ���� �ʱ� �̺�Ʈ�� ã�� EventQueue�� �߰�
	void AddEvToQueue(UdeBaseEv* ptr_ev);		// Schedule �Լ����� ���� �� Event�� EventQueue�� �߰�
	bool IsQueueEmpty();
	TimeType GetMinIntEvTime(); // ���� EventQueue���� ���� ���� ���۵Ǵ� �̺�Ʈ�� ���� �ð��� ����
	UdeBaseEv* GetFrontEv();	// ���� EventQueue���� ���� �켱������ ���� �̺�Ʈ�� ����
	void RemoveQueueEvs();		// ���� EventQueue���� ���� �켱������ ���� �̺�Ʈ�� ����
	void MakeFatalSimTimeExcept(const TimeType& next_cs_time) const; // ���� �ð� �ùķ��̼� ���� �� �ð��� ���� ���� �ð����� ���� ��� ���� �߻�
public:
	void UpdateQueueForChangedPriority();	// �ùķ��̼� ���� �켱������ ���� �� ���, Queue�� Event���� �켱 ������ �ݿ�

public: // ���� �ܺο��� �ùķ��̼��� ���� ���� �� ������ �ɼ� ���� �� ���� �� ȹ���� ���� �Լ�
	CsSimEngine* GetCsEngine() { return &cs_sim_engine_; }
	UdeCoupModel* GetModel() const { return initial_model_.get(); }
	UDEGlobalDB* GetParamDB() { return &eng_param_db_; }

	void SimStart();	// �ùķ��̼� ����. ������ ��峪 HLA/RTI �ùķ��̼� �� �ܺο��� �ش� �Լ��� ȣ���� �ùķ��̼��� ����
	void Pause();		// �ùķ��̼� �Ͻ�����
	void Resume();		// �ùķ��̼� �����
	void Stop();		// �ùķ��̼� �ߴ�
	bool IsEngineRunning();
	void SetOptMode() { opt_mode_ = true; }		// opt ��� Ȱ��ȭ
	void SetThreadMode() { en_thread_ = true; }	// ������ ��� Ȱ��ȭ
	void SetEndTime(const TimeType& end_time);	// �ùķ��̼� ���� �ð� ����
	TimeType GetCurSimTime() const { return cur_sim_time_; };
	void EnableRealTimeSim(const double& ratio, const double& time_tick);	// real-time �ùķ��̼� ����. ��Ӱ� tick�� ����
	void ChangeRatio(const double& ratio);	// real-time �ùķ��̼� ��� ����
	void ChangeRatio(const double& ratio, const double& time_tick);	// real-time �ùķ��̼� ��Ӱ� tick ����
	double GetSimRatio();

public: //Process-oriented model APIs
	void SendData(UdeBaseModel* itself, unsigned int port, void* msg_value_ptr);
	void AddPoModel(UdePoModel*);
	void ErasePoModel(UdeBaseModel* ptr_po_model);
private:
	void RemoveAllPoModel();

public: // HLA/RTI ����� ���� �Լ�
	/* ������ HLA / RTI ���� ��� Ȱ��ȭ.UdeHLARTIHandler�� ����� �ڵ鷯�� �⺻ �ڵ鷯�� ������ Ȱ��ȭ�Ѵ�. */
	void SetHLARTIMode(UdeHLARTIHandler* ude_hla_rti_handler);
	/* ������ RTI�� �����Ǿ� TAG(TimeAdvanceGrant)�� ���� ���������� �� �ܺο��� ȣ��.
	  ������ RTI�� TAG�� ������ �� �ֵ��� ���Ǻ����� Ȱ��ȭ�Ѵ�. */
	void SetRTIConnected();
	/* ������ RTI�� ������ ������ ��� �ܺο��� ȣ��.
	 * ������ RTI�� TAG ������ ��ٸ��� �ʰ� ������ ROMessage���� ó���ϵ��� ��. */
	void SetRTIDisConnected();
	/* ������ RTI�� ���������� ���� �Ǿ����� SimHLARTIManager�� ���ߺ����� Ȯ�� */
	bool IsRTIConnected();
	/* RTI�κ��� TAG�� ������ ���, TAG�� ������ �ܺ��� �ݹ� �Լ����� �� �Լ��� ȣ��.
	 * ������ GrantTime�� �����ϰ� TAG ������ �˷� ������ �ð��� �����Ű���� �� */
	void HLATimeGranted(TimeType grant_time);
private:
	// ������ RTI�� �ð� ������ ��û�ϴ� �Լ�
	void HLATimeAdvance(TimeType& ref_time);

public: // ������ ���̼��� ������ ���� �Լ�
	void SetLicenseFileLoc(std::string& license_file_loc);	// ������ ���̼��� ��� ����
};

extern UDESIM_EXT_CLASS UdeEngine* GetEngInterface();

