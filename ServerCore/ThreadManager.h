#pragma once
// thread 관리
// 추가적인 기능이 필요하면 더 작성
#include <thread>
#include <functional>
#include <vector>

/*--주석을 통한 클래스 구분 관리
	ThreadManager
----------------------------*/


class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	// 특정함수를 callback하면서 스레드를 실행하는 기능
	void Launch(function<void(void)> callback);
	void Join(); // 우리가 std::thread.join() 사용시 실행되는 함수

	static void InitTLS(); // 핵심, thread 생성시 TLS 영역 생성
	static void DestroyTLS(); //


private:
	Mutex			_lock;
	vector<thread> _threads;

};

