#pragma once
// 예제 15:Reader-Writer-Lock 만들기
// 기본 제공되는 표준 std::mutex 보다  좀 더 유동적으로 lock과 unlock이 가능한 lock 클래스를 직접 만들것

// 표준 std::lock 말고 커스텀 lock 클래스를 사용하는 이유
// 1. mutex 재귀적으로 lock을 잡을 수 없음
//  => 이미 lock 이 잡힌 상태에서 다음 함수가 또 lock을 잡아야 하는 경우
//		안전한 기능이지만 정말 제한적으로 필요해지는 경우가 생김
// 추후 서버 개발 예제에 사용할 Lock

#include "Types.h"

/*----------------

	RW SpinLock

----------------*/

/*----------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId), Lock을 획득하고있는 Thread의 ID 저장
R : ReadFlag (Shared Lock Count), 공유하고 사용하고있는 Read Count를 저장
----------------*/


// W ->W (O) 연속적인 쓰기 락은 가능
// W->R (O) // 쓰기 후 읽기 락은 가능
// R->W (X) // 하지만 읽기 락은 안된다.

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000, // 최대로 기다려주는시간
		MAX_SPIN_COUNT = 5000, // 몇번돌지
		WRITE_THREAD_MASK = 0xFFFF'0000, //MASK, wirte가 있는 앞쪽을 다 1로 채워줌
		READ_COUNT_MASK = 0x0000'FFFF, // read Count 가 들어있는 비트를 1로 채워주기
		EMPTY_FLAG = 0x0000'0000 // 전부 비워준다.
	};
public:


	void WriteLock(const char* name);// 쓸 때 잠구기
	void WriteUnlock(const char* name); // 다 쓰고 풀기
	void ReadLock(const char* name);  // 읽을 때 잠구기
	void ReadUnlock(const char* name); // 다 읽고 풀기

private:
	Atomic<uint32> _lockFlag;
	uint16 _writeCount = 0; // 경합할 필요가 없고 스레드가 각자 사용하니까 일반 자료형으로 선언
	// 비트수는 크게 필요없어서 16
};

/*----------------
	LockGuards
-----------------*/

// RAII
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) :
		_lock(lock), _name(name) { _lock.ReadLock(name); }

	~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) :
		_lock(lock), _name(name) { _lock.WriteLock(name); }

	~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};


