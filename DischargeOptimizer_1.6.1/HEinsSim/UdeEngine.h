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
	CsSimEngine cs_sim_engine_;	// 연속 시간 시뮬레이션 엔진
	bool cs_engine_active_;		// 연속 시간 시뮬레이션 사용

	ModelCoupler model_coupler_;	// 모델 결합 관계 관리자
	UDEGlobalDB eng_param_db_;		// 엔진 제공 파라미터 DB

	/* manager 객체 */
	SimRtManager sim_rt_manager_;			// real-time simulation manager
	SimHLARTIManager sim_hlarti_manager_;	// HLA/RTI simulation manager
	SimLicenseManager sim_license_manager_;	// license manager

	/* 시뮬레이션 엔진의 데이터 */
	std::unique_ptr<UdeCoupModel> initial_model_;	// 엔진에 세팅 된 최상위 결합 모델
	std::list<UdePoModel*> po_model_list_;			// PoModel 리스트
	std::priority_queue<UdeBaseEv*, std::vector<UdeBaseEv*>, EventComparator> priority_queue_;	// EventQueue
	TimeType cur_sim_time_;	// 현재 엔진 시각
	TimeType end_time_;		// 시뮬레이션 종료 시각

	/* thread mutex */
	std::mutex po_list_lock_;			// PoModel list mutex
	std::mutex priority_queue_lock_;	// EventQueue mutex 
	std::mutex rti_lock_;				// HLA/RTI mutex

	/* For real time, HLA/RTI simulation 
	* 실시간 시뮬레이션을 위해서는 en_thread_와 en_realtime_sim_을 모두 활성
	* HLA/RTI 시뮬레이션을 위해서는 en_thread_만 활성해도 사용 가능
	*/
	std::thread eng_exec_thread_;	// 엔진 스레드
	bool en_thread_;		// 스레드 모드 사용
	bool en_realtime_sim_;	// real time 시뮬레이션 사용
	bool opt_mode_;			// opt mode 사용

public:
	std::mt19937_64 random_gen_;

public: // 엔진 생성 및 singleton 패턴 관리
	static UdeEngine* GetInstance();
	static void Destroy();
private:
	static UdeEngine* ptr_eng_instance_;
	UdeEngine();
	~UdeEngine();

public: // 모델 셋팅 및 초기화, 로깅
	void Randomize();	// random_gen_ 초기화

	static bool EnableEngDebugLogger();		// 엔진 디버그 로거 활성화
	static bool DisableEngDebugLogger();	// 엔진 디버그 로거 비활성화

	void SetModel(UdeCoupModel*);	// 시뮬레이션 타겟 모델을 엔진에 셋팅
	void SetRunTimeModel();			// 엔진 실행 중 모델 변경 시 호출해 런타임 모델 갱신
	void RemoveRunTimeModel(UdeBaseModel* model);	// 엔진 실행 중 모델 제거시 결합 모델에서 사용
	
	void ShowModelInfo() const;	// 전체 모델에 대해 결합 관계 출력
	void ShowModelInfo(UdeCoupModel* coup_model) const;	// 주어진 결합 모델에 대해 결합 관계 출력

	void Reset();	// 엔진 초기화
	void Reset(UdeCoupModel *ude_coup_model);	// 엔진 초기화와 동시에 모델을 엔진에 셋팅

public: // Int/Ext event의 스케쥴과 실행 함수
	void ScheduleIntEvent(const TimeType& ta, UdeBaseModel* src_model);	// Internal Event의 스케쥴
	void ScheduleExtEvent(const TimeType& scheduled_time, UdeBaseModel* src_model,
		const unsigned int& out_port, void* ptr_msg_value);				// External Event의 스케쥴
	void ScheduleIntEvent(const TimeType& ta, UdeBaseModel* src_model,
		std::function<void(const UdeMessage&)> func);		// PoModel에서 호출하는 Internal Event 스케쥴
	void ScheduleExtEventFromOutter(const TimeType& scheduled_time, UdeBaseModel* dest_model, 
		unsigned int& dest_in_port, void* ptr_msg_value);	// 패킷 등을 통해 외부 모델에서 도착한 External Event를 엔진 외부에서 스케쥴
	void ScheduleExtEventFromOutter(const TimeType& scheduled_time, UdeBaseModel* src_model,
		const unsigned int& src_out_port, UdeBaseModel* dest_model,	unsigned int& dest_in_port,
		void* ptr_msg_value);	// 패킷 등을 통해 외부 모델에서 도착한 External Event를 엔진 외부에서 스케쥴
private:
	bool HandleIntEv(UdeBaseEv* ptr_ev);	// EventQueue의 Internal Event 수행
	bool HandleExtEv(UdeBaseEv* ptr_ev);	// EventQueue의 External Event 수행

public:	// 엔진 실행 및 시뮬레이션 수행을 위한 함수
	bool Start();	// 엔진 실행. 종료 시간은 사용자가 설정한 값 또는 디폴트 값
	bool Start(const TimeType& end_time);	// 엔진 실행. 종료 시간은 주어진 end_time 값으로 설정
private:
	TimeType PreSimulation(const TimeType& min_ev_time);	// 연속 시간 모델의 시뮬레이션 수행
	int EngineStart(bool& ret_ok);	// 실제 엔진 실행 함수. 사용자는 Start API를 통해 간접적으로 실행
	void WaitForThreadStop();		// 엔진이 스레드 모드로 실행 될 때, 메인 스레드보다 먼저 종료 되지 않도록 대기

private: // 시뮬레이션 수행을 위한 엔진 필요 함수
	void MakeInitialAmEvents(UdeCoupModel*);	// 이산사건 모델의 초기 이벤트를 찾아 EventQueue에 추가
	void AddEvToQueue(UdeBaseEv* ptr_ev);		// Schedule 함수에서 생성 된 Event를 EventQueue에 추가
	bool IsQueueEmpty();
	TimeType GetMinIntEvTime(); // 현재 EventQueue에서 가장 먼저 시작되는 이벤트의 시작 시간을 구함
	UdeBaseEv* GetFrontEv();	// 현재 EventQueue에서 가장 우선순위가 높은 이벤트를 구함
	void RemoveQueueEvs();		// 현재 EventQueue에서 가장 우선순위가 높은 이벤트를 제거
	void MakeFatalSimTimeExcept(const TimeType& next_cs_time) const; // 연속 시간 시뮬레이션 수행 후 시간이 현재 엔진 시간보다 작은 경우 예외 발생
public:
	void UpdateQueueForChangedPriority();	// 시뮬레이션 모델의 우선순위가 변경 된 경우, Queue의 Event들의 우선 순위에 반영

public: // 엔진 외부에서 시뮬레이션의 상태 제어 및 엔진의 옵션 설정 및 상태 값 획득을 위한 함수
	CsSimEngine* GetCsEngine() { return &cs_sim_engine_; }
	UdeCoupModel* GetModel() const { return initial_model_.get(); }
	UDEGlobalDB* GetParamDB() { return &eng_param_db_; }

	void SimStart();	// 시뮬레이션 시작. 스레드 모드나 HLA/RTI 시뮬레이션 시 외부에서 해당 함수를 호출해 시뮬레이션을 시작
	void Pause();		// 시뮬레이션 일시정지
	void Resume();		// 시뮬레이션 재시작
	void Stop();		// 시뮬레이션 중단
	bool IsEngineRunning();
	void SetOptMode() { opt_mode_ = true; }		// opt 모드 활성화
	void SetThreadMode() { en_thread_ = true; }	// 스레드 모드 활성화
	void SetEndTime(const TimeType& end_time);	// 시뮬레이션 종료 시각 설정
	TimeType GetCurSimTime() const { return cur_sim_time_; };
	void EnableRealTimeSim(const double& ratio, const double& time_tick);	// real-time 시뮬레이션 설정. 배속과 tick을 설정
	void ChangeRatio(const double& ratio);	// real-time 시뮬레이션 배속 변경
	void ChangeRatio(const double& ratio, const double& time_tick);	// real-time 시뮬레이션 배속과 tick 변경
	double GetSimRatio();

public: //Process-oriented model APIs
	void SendData(UdeBaseModel* itself, unsigned int port, void* msg_value_ptr);
	void AddPoModel(UdePoModel*);
	void ErasePoModel(UdeBaseModel* ptr_po_model);
private:
	void RemoveAllPoModel();

public: // HLA/RTI 사용을 위한 함수
	/* 엔진의 HLA / RTI 연동 모드 활성화.UdeHLARTIHandler를 상속한 핸들러를 기본 핸들러로 세팅해 활성화한다. */
	void SetHLARTIMode(UdeHLARTIHandler* ude_hla_rti_handler);
	/* 엔진이 RTI와 연동되어 TAG(TimeAdvanceGrant)를 수신 가능해졌을 때 외부에서 호출.
	  엔진이 RTI의 TAG를 수신할 수 있도록 조건변수를 활성화한다. */
	void SetRTIConnected();
	/* 엔진과 RTI의 연동이 끊어진 경우 외부에서 호출.
	 * 엔진이 RTI의 TAG 수신을 기다리지 않고 내부의 ROMessage만을 처리하도록 함. */
	void SetRTIDisConnected();
	/* 엔진과 RTI가 정상적으로 연동 되었는지 SimHLARTIManager의 조견변수를 확인 */
	bool IsRTIConnected();
	/* RTI로부터 TAG가 도착한 경우, TAG를 수신한 외부의 콜백 함수에서 본 함수를 호출.
	 * 엔진의 GrantTime을 설정하고 TAG 수신을 알려 엔진의 시간을 진행시키도록 함 */
	void HLATimeGranted(TimeType grant_time);
private:
	// 엔진이 RTI에 시간 진행을 요청하는 함수
	void HLATimeAdvance(TimeType& ref_time);

public: // 엔진의 라이센스 관리를 위한 함수
	void SetLicenseFileLoc(std::string& license_file_loc);	// 엔진의 라이센스 경로 설정
};

extern UDESIM_EXT_CLASS UdeEngine* GetEngInterface();

