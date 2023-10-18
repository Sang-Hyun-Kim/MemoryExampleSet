#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"


/*--------------------------
	ThreadManager
----------------------------*/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS(); // ���� ������ �ʱ�ȭ
}

ThreadManager::~ThreadManager()
{
	Join();

}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock); // �ϴ� ��Ƽ������ ȯ���̹Ƿ� lock

	// ������, �����ų ������ ���� �� ����
	_threads.push_back(thread([=]()
		{
			InitTLS(); // TLS �ʱ�ȭ �� Ȯ��
			callback(); // callback���� ������ �Լ� ����
			DestroyTLS(); // TLS ����
		}));
} 

// function ���̺귯���� Ȳ�� ���� �� �� �� �Լ� ���¸� �޾Ƽ� ���
// => �Լ� ������ ���� �Ǽ��������� ������ ���̺귯�� ������
// 


//ThreadManager Ŭ������ vector<thread> ������ ����� thread����
// ���� Join() ���ش�.
// �Ҹ��ڰ� ����ɶ� ���� ���ָ鼭 mainthread ���� �� �ٸ� �����带 ��ٸ�
void ThreadManager::Join()
{
	// ������ ��ȸ
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	// thread id �ʱ�ȭ
	static Atomic<uint32> SThreadID = 1;
	LThreadId = SThreadID.fetch_add(1); 
	// 1���� ����, ������ �־��� ��
	// Ŭ���� �� ���� ��� ������
	// Ŭ������ ��� ��ü���� ������ => ��� ��ü���� ���� ���� ���� �� �� ����

}

void ThreadManager::DestroyTLS()
{
	// ���� �������� �����Ǵ� TLS�� ó��
}


