#pragma once
// thread ����
// �߰����� ����� �ʿ��ϸ� �� �ۼ�
#include <thread>
#include <functional>
#include <vector>

/*--�ּ��� ���� Ŭ���� ���� ����
	ThreadManager
----------------------------*/


class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	// Ư���Լ��� callback�ϸ鼭 �����带 �����ϴ� ���
	void Launch(function<void(void)> callback);
	void Join(); // �츮�� std::thread.join() ���� ����Ǵ� �Լ�

	static void InitTLS(); // �ٽ�, thread ������ TLS ���� ����
	static void DestroyTLS(); //


private:
	Mutex			_lock;
	vector<thread> _threads;

};

