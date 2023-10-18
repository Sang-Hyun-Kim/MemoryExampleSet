#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

// �߰����� Manager Ŭ���� ����� �Ŵ��������Ǽ��� ����
// ����,���� ������ �����ؾ��Ѵ�.

class CoreGlobal
{
public:
	
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GDeadLockProfiler;
	}

}GCoreGlobal;

// example ���� �������� �����ܿ��� �ű�
// CoreGlobal coreGlobal;


