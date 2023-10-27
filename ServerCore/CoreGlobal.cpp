#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"

ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

// 추가적인 Manager 클래스 등장시 매니저끼리의순서 관리
// 생성,삭제 순서를 관리해야한다.

class CoreGlobal
{
public:
	
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;
	}

}GCoreGlobal;

// example 예제 전역변수 생성단에서 옮김
// CoreGlobal coreGlobal;


