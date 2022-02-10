#pragma once
#include "HEinsSim.h"

/// <summary>
/// HLA/RTI �ùķ��̼��� ���� SMARTAdaptor�� ��ſ� ����ϴ� �ڵ鷯 Ŭ���� <para />
/// RTI�� ���� �ùķ��̼� ���� �� �ùķ��̼� �ð��� RTI�� TimeAdvanceRequest�� ���� �����Ѵ�. <para />
/// �̸� ���� ������ SMARTADP ���̿��� ������ �ð� ���� ��û�� SMARTADP�� �����ϱ� ���� ROHandler�� �ʿ� <para />
/// �̸� ���� �� �� �Ʒ��� UdeHLAHandler Ŭ������ ��� �޾� �����Ѵ�. <para />
/// </summary>
class UdeHLARTIHandler
{
public:
	UdeHLARTIHandler() {}
	virtual ~UdeHLARTIHandler() {}

	/**
	 * UdeHLAHandler�� ����� Ŭ������ ������ �ð��� ��û �ð��� ���ϸ鼭 ��û �ð��� ������ �ð����� ū ���
	 * ��Ʈ��ũ �ڵ鷯 ���� ���� SMARTADP�� ���� RTI�� �ð� ������ ��û�ϴ� ������ TimeAdvanceRequest() �Լ��� �����Ѵ�.
	 */
	virtual void TimeAdvanceRequest(double time) {}
};

class SimHLARTIManager
{
public:
	SimHLARTIManager();
	virtual ~SimHLARTIManager();

private:
	bool is_HLARTI_active;	// HLA/RTI ��� ����
	bool is_RTI_connected;	// RTI�� ���� ����
	bool time_granted;		// RTI�κ��� TimeGranted ���� ����
	double grant_time;		// RTI�κ��� ������ GrantTime
	UdeHLARTIHandler* HLARTI_handler;	// RTI�� ����� ���� Handler

public:
	void Initialize();

	// ������ ����� HLAHandler�� ����
	void SetHLARTIHandler(UdeHLARTIHandler* ude_hla_rti_handler) { HLARTI_handler = ude_hla_rti_handler; }
	void SetIsHLARTIActive(bool tf) { is_HLARTI_active = tf; }
	// �ܺ� ��⿡�� ������ RTI�� �����ǰų� ������ �������� ��� ȣ��
	void SetIsRTIConnected(bool tf) { is_RTI_connected = tf; }
	void SetTimeGranted(bool tf) { time_granted = tf; }
	void SetGrantTime(double g_time) { grant_time = g_time; }
	
	UdeHLARTIHandler& GetHLARTIHandler() const { return *HLARTI_handler; }
	bool GetIsHLARTIActive() const { return is_HLARTI_active; }
	// ������ RTI�� ���� ���� Ȯ��
	bool GetIsRTIConnected() const { return is_RTI_connected; }
	bool GetTimeGranted() const { return time_granted; }
	double GetGrantTime() const { return grant_time; }
};

