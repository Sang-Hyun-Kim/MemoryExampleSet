#pragma once

// 전역으로 사용할 Manager 클래스 생성 후 관리 할 것이다.
// 전역으로 사용하는 객체들 관리
extern class ThreadManager* GThreadManager;
extern class Memory* GMemory;
//// 추가적인 Manager 클래스 등장시 매니저끼리의순서 관리
//// 생성,삭제 순서를 관리해야한다.
//
//class CoreGlobal
//{
//public:
//	CoreGlobal();
//	~CoreGlobal();
//
//
//};
// cpp로 옮김

// 데드락 탐지
extern class DeadLockProfiler* GDeadLockProfiler;