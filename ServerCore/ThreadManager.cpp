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
	InitTLS(); // 메인 스레드 초기화
}

ThreadManager::~ThreadManager()
{
	Join();

}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock); // 일단 멀티스레드 환경이므로 lock

	// 람다형, 실행시킬 스레드 실행 및 삽입
	_threads.push_back(thread([=]()
		{
			InitTLS(); // TLS 초기화 밑 확보
			callback(); // callback으로 가져온 함수 실행
			DestroyTLS(); // TLS 정리
		}));
} 

// function 라이브러리를 황용 람다 등 온 갖 함수 형태를 받아서 사용
// => 함수 포인터 쓰다 실수하지말고 안전한 라이브러리 사용권장
// 


//ThreadManager 클래스의 vector<thread> 변수에 저장된 thread들을
// 전부 Join() 해준다.
// 소멸자가 실행될때 실행 해주면서 mainthread 종료 전 다른 스레드를 기다림
void ThreadManager::Join()
{
	// 스레드 순회
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	// thread id 초기화
	static Atomic<uint32> SThreadID = 1;
	LThreadId = SThreadID.fetch_add(1); 
	// 1부터 시작, 꾸준히 넣어줄 것
	// 클래스 의 정적 멤버 변수는
	// 클래스의 모든 객체에서 공유됨 => 모든 객체에서 여러 값을 변경 할 수 있음

}

void ThreadManager::DestroyTLS()
{
	// 추후 동적으로 생성되는 TLS를 처리
}


