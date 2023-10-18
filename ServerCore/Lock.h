#pragma once
// ���� 15:Reader-Writer-Lock �����
// �⺻ �����Ǵ� ǥ�� std::mutex ����  �� �� ���������� lock�� unlock�� ������ lock Ŭ������ ���� �����

// ǥ�� std::lock ���� Ŀ���� lock Ŭ������ ����ϴ� ����
// 1. mutex ��������� lock�� ���� �� ����
//  => �̹� lock �� ���� ���¿��� ���� �Լ��� �� lock�� ��ƾ� �ϴ� ���
//		������ ��������� ���� ���������� �ʿ������� ��찡 ����
// ���� ���� ���� ������ ����� Lock

#include "Types.h"

/*----------------

	RW SpinLock

----------------*/

/*----------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId), Lock�� ȹ���ϰ��ִ� Thread�� ID ����
R : ReadFlag (Shared Lock Count), �����ϰ� ����ϰ��ִ� Read Count�� ����
----------------*/


// W ->W (O) �������� ���� ���� ����
// W->R (O) // ���� �� �б� ���� ����
// R->W (X) // ������ �б� ���� �ȵȴ�.

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000, // �ִ�� ��ٷ��ִ½ð�
		MAX_SPIN_COUNT = 5000, // �������
		WRITE_THREAD_MASK = 0xFFFF'0000, //MASK, wirte�� �ִ� ������ �� 1�� ä����
		READ_COUNT_MASK = 0x0000'FFFF, // read Count �� ����ִ� ��Ʈ�� 1�� ä���ֱ�
		EMPTY_FLAG = 0x0000'0000 // ���� ����ش�.
	};
public:


	void WriteLock(const char* name);// �� �� �ᱸ��
	void WriteUnlock(const char* name); // �� ���� Ǯ��
	void ReadLock(const char* name);  // ���� �� �ᱸ��
	void ReadUnlock(const char* name); // �� �а� Ǯ��

private:
	Atomic<uint32> _lockFlag;
	uint16 _writeCount = 0; // ������ �ʿ䰡 ���� �����尡 ���� ����ϴϱ� �Ϲ� �ڷ������� ����
	// ��Ʈ���� ũ�� �ʿ��� 16
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


