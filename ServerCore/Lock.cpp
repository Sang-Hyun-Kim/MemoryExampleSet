#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
// ���� 16�� �α���
// DeadLockŽ���� Debug ��忡���� ���� ������ ����
// Lock�� �׽�Ʈ �ϱ����� lock�� ��ٺ��� ������ ��
// �������� �����ʰ� TSL����, thread ������ ������ 
// �ٸ� �� �� �̻��� �������� ȣ�� ���� ������ �̷������ �ʾ� deadlock ��Ȳ�� �� �ʰ� �߰ߵ� ������ �ִ�.
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif



	// ������ �����尡 �����ϰ� �ִٸ� ������ ���� ��Ű��
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK)>> 16;
	// readflag�� �����ְ� ��Ʈ�� ������ �����Ѵ���
	// �ش� ���� ��
	if (LThreadId == lockThreadId)
	{
		_writeCount++; // �ٷ� ī��Ʈ �ø��� ��ȯ
		return; 
	}
	// �ʹ� �����ݺ��� ��� Ż���ų ��
	const int64 beginTick = ::GetTickCount64();

	// �ƹ��� ���� �� �����ϰ� ���� ���� �� , �����ؼ� �������� ��´�.
	// �� ��� ��Ʈ�� 0�� ���
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK); // �� ������ ��ȣ�� ��
	// 16��Ʈ �̵� �� �� ���ֱ�
	
	while (true)
	{
		for (uint32 sprinCount = 0; sprinCount < MAX_SPIN_COUNT; sprinCount++)
		{
			uint32 expected = EMPTY_FLAG; // EMPTYFLAG (����ִٸ�)�� Ȯ�εȴٸ� 
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) // CAS���� expected ���� OUTPUT������ �ٽ� ���õǴ� ��� OUT���� ǥ��
			{
				// ���տ��� �̰�����
				// ���� ī��Ʈ ����=> ��������� ȣ��� mutexó�� ũ���������ʰ�
				// �ٽ� ȣ���Ű�� �ϴ´�� ī��Ʈ�� �������Ѽ� ������ ��
				_writeCount++;
				return;
			}
			 
		}
		// ���� ������ �ִ� ��� �ð� ���� �����ɸ���
		// ����� ���� CRASH ��ũ�η� ���� �߻���Ű��
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT"); 

		this_thread::yield(); // ������ ����

	}
} 
// �� �ش� �ż��忡���� _lockFlag�� ������ ����־�߸� ���� �� �� ��ü ����� �����ϰ�
// read �κ� 16�ڸ��� �ϳ��� 0�� �ƴ� ���� �ִٸ� ������� �ʴ´�.

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock�� �� Ǯ���� ������ ���� WriteLock�� ���� �� ���� ��������
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	// ���� lock�� �����Ϸ� �������� ���ǹ� ����
	const int32 lockCount = --_writeCount;
	if(lockCount == 0) // ���� �� ȥ�� ������ ���� ���
	{
		_lockFlag.store(EMPTY_FLAG);// 0���� �ʱ�ȭ
	}
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// ������ �����尡 �����ϰ� �ִٸ� �ٷ� ���� ó��
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	// readflag�� �����ְ� ��Ʈ�� ������ �����Ѵ���
	// �ش� ���� ��
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1); // readcount1����
		return;
	}


	const int64 beginTick = ::GetTickCount64();
	// �ƹ��� �����ϰ� ���� ������ �����ؼ� ���� ī��Ʈ�� �ø���.
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			// Writeflag�� ���õȺκ� �о������
			// expected�� �´ٸ�..
			// ��¼�� W ��Ʈ�κ� 16��Ʈ���� ����ִٸ� READ_COUNT_MASK�� �о������ ���
			// 0�� �Ǳ� ������
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return; // COUNT�� 1�÷��� ���·� �ٲ��ְ� ������
		}

		// �б⵵ �����ɸ��� ũ����
		// ���� �̤������� �����ɸ��� ���� ������ ���� Ȯ���� ����.
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
		
		// 5õ�� �� ������ ����	
		this_thread::yield();
	
	}

}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	// ���� ���������� �̹� 0�̾��ٸ� ���� ������ �ִ� ��
	// ������ �б� ����Ǯ���鼭 0�� �Ǿ� ����
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
	// �׿� ������ ���ٸ� readcount ��Ʈ���� 1���ҵȴ��� �Լ� ����
}
