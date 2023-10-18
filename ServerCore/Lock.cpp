#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
// 예제 16번 부근임
// DeadLock탐지는 Debug 모드에서만 보는 것으로 설정
// Lock을 테스트 하기위해 lock을 잡다보니 무리가 감
// 전역으로 하지않고 TSL영역, thread 단위로 추적함 
// 다만 두 개 이상의 스레드의 호출 순서 병합이 이루어지지 않아 deadlock 상황이 좀 늦게 발견될 여지가 있다.
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif



	// 동일한 스레드가 소유하고 있다면 무조건 성공 시키자
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK)>> 16;
	// readflag만 날려주고 비트를 내려서 복구한다음
	// 해당 값을 비교
	if (LThreadId == lockThreadId)
	{
		_writeCount++; // 바로 카운트 올리고 귀환
		return; 
	}
	// 너무 오래반복할 경우 탈출시킬 것
	const int64 beginTick = ::GetTickCount64();

	// 아무도 소유 및 공유하고 있지 않을 때 , 경합해서 소유권을 얻는다.
	// 즉 모든 비트가 0인 경우
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK); // 내 스레드 번호를 기
	// 16비트 이동 후 더 해주기
	
	while (true)
	{
		for (uint32 sprinCount = 0; sprinCount < MAX_SPIN_COUNT; sprinCount++)
		{
			uint32 expected = EMPTY_FLAG; // EMPTYFLAG (비어있다면)로 확인된다면 
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) // CAS에서 expected 값이 OUTPUT값으로 다시 세팅되는 경우 OUT으로 표시
			{
				// 경합에서 이겼으니
				// 쓰기 카운트 증가=> 재귀적으로 호출시 mutex처럼 크래쉬내지않고
				// 다시 호출시키게 하는대신 카운트를 증가시켜서 추적할 것
				_writeCount++;
				return;
			}
			 
		}
		// 직접 지정한 최대 대기 시간 보다 오래걸리면
		// 만들어 놓은 CRASH 매크로로 오류 발생시키기
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT"); 

		this_thread::yield(); // 소유권 포기

	}
} 
// 즉 해당 매서드에서는 _lockFlag가 완전히 비어있어야만 경합 및 값 교체 기능을 수행하고
// read 부분 16자리에 하나라도 0이 아닌 값이 있다면 수행되지 않는다.

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock이 다 풀리기 전에는 절대 WriteLock을 잡을 수 없게 설계하자
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	// 쓰기 lock을 해제하러 들어왔을때 조건문 시작
	const int32 lockCount = --_writeCount;
	if(lockCount == 0) // 만약 나 혼자 가지고 있을 경우
	{
		_lockFlag.store(EMPTY_FLAG);// 0으로 초기화
	}
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// 동일한 스레드가 소유하고 있다면 바로 성공 처리
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	// readflag만 날려주고 비트를 내려서 복구한다음
	// 해당 값을 비교
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1); // readcount1증가
		return;
	}


	const int64 beginTick = ::GetTickCount64();
	// 아무도 소유하고 있지 않을때 경합해서 공유 카운트를 올린다.
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			// Writeflag와 관련된부분 밀어버리기
			// expected와 맞다면..
			// 어쩌피 W 비트부분 16비트들이 비어있다면 READ_COUNT_MASK로 밀어버렸을 경우
			// 0이 되기 때문에
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return; // COUNT를 1올려준 상태로 바꿔주고 나가기
		}

		// 읽기도 오래걸리면 크래쉬
		// 보통 이ㅜ정도로 오래걸리면 로직 에러가 있을 확률이 높다.
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
		
		// 5천번 안 됐으면 실패	
		this_thread::yield();
	
	}

}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	// 빼기 이전값부터 이미 0이었다면 뭔가 문제가 있는 것
	// 마지막 읽기 락이풀리면서 0이 되야 정상
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
	// 그외 문제가 없다면 readcount 비트에서 1감소된다음 함수 종료
}
