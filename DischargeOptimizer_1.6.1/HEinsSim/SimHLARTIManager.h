#pragma once
#include "HEinsSim.h"

/// <summary>
/// HLA/RTI 시뮬레이션을 위해 SMARTAdaptor와 통신에 사용하는 핸들러 클래스 <para />
/// RTI를 통한 시뮬레이션 수행 시 시뮬레이션 시각은 RTI의 TimeAdvanceRequest를 통해 진행한다. <para />
/// 이를 위해 엔진과 SMARTADP 사이에서 엔진의 시간 진행 요청을 SMARTADP로 전달하기 위한 ROHandler가 필요 <para />
/// 이를 구현 할 때 아래의 UdeHLAHandler 클래스를 상속 받아 구현한다. <para />
/// </summary>
class UdeHLARTIHandler
{
public:
	UdeHLARTIHandler() {}
	virtual ~UdeHLARTIHandler() {}

	/**
	 * UdeHLAHandler를 상속한 클래스는 엔진의 시각과 요청 시각을 비교하면서 요청 시각이 엔진의 시각보다 큰 경우
	 * 네트워크 핸들러 등을 통해 SMARTADP를 거쳐 RTI로 시간 진행을 요청하는 구현을 TimeAdvanceRequest() 함수에 구현한다.
	 */
	virtual void TimeAdvanceRequest(double time) {}
};

class SimHLARTIManager
{
public:
	SimHLARTIManager();
	virtual ~SimHLARTIManager();

private:
	bool is_HLARTI_active;	// HLA/RTI 사용 여부
	bool is_RTI_connected;	// RTI와 연결 여부
	bool time_granted;		// RTI로부터 TimeGranted 수신 여부
	double grant_time;		// RTI로부터 수신한 GrantTime
	UdeHLARTIHandler* HLARTI_handler;	// RTI와 통신을 위한 Handler

public:
	void Initialize();

	// 엔진이 사용할 HLAHandler를 세팅
	void SetHLARTIHandler(UdeHLARTIHandler* ude_hla_rti_handler) { HLARTI_handler = ude_hla_rti_handler; }
	void SetIsHLARTIActive(bool tf) { is_HLARTI_active = tf; }
	// 외부 모듈에서 엔진과 RTI가 연동되거나 연동이 끊어졌을 경우 호출
	void SetIsRTIConnected(bool tf) { is_RTI_connected = tf; }
	void SetTimeGranted(bool tf) { time_granted = tf; }
	void SetGrantTime(double g_time) { grant_time = g_time; }
	
	UdeHLARTIHandler& GetHLARTIHandler() const { return *HLARTI_handler; }
	bool GetIsHLARTIActive() const { return is_HLARTI_active; }
	// 엔진과 RTI의 연결 여부 확인
	bool GetIsRTIConnected() const { return is_RTI_connected; }
	bool GetTimeGranted() const { return time_granted; }
	double GetGrantTime() const { return grant_time; }
};

