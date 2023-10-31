#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include "LockFreeStack.h"
// 예제3 MemoryExample
// 메모리 할당(Allocator)
// new delete 사용중 쪼개진 메모리를 제대로 관리해 자원 효율을 높히려는 시도 연습
// new와 delete도 operator overloading이 가능하다.
#include "Memory.h"

class Knight
{
public:
	int32 _hp = rand() % 1000;

	// 클래스 안에 넣으면 해당 클래스에게만 적용
	// 버전에 따라 new() 인수를 더 넣어줘야함
	//void* operator new(size_t size)
	//{
	//	cout << " Knight new! " << size << endl; // 디버깅용
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}
	//void operator delete(void* ptr)
	//{
	//	cout << " Knight delete!" << endl;
	//	::free(ptr);
	//}
};

//DECLSPEC_ALIGN(16)
//class Data //:public SListEntry
//{
//public:
//	SListEntry _entry;
//
//	int64 _rand = rand() % 1000;
//};

// MS에서 직접 제공하는 MemoryPool 구조체로 변경
//DECLSPEC_ALIGN(16)
//class Data //:public SListEntry
//{
//public:
//	SLIST_ENTRY _entry; 
//
//	int64 _rand = rand() % 1000;
//};

// 이러면 모든 것에 대해 전역으로 작동(위험)
//void* operator new(size_t size)
//{
//	cout << "new! " << size << endl; // 디버깅용
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//void* operator new[](size_t size)
//{
//	cout << "new[]! " << size << endl; // 디버깅용
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//void operator delete(void* ptr)
//{
//	cout << "delete!" << endl;
//	::free(ptr);
//}
//void operator delete[](void* ptr)
//{
//	cout << "delete[]!" << endl;
//	::free(ptr);
//}

//SLIST_HEADER* GHeader;

int main()
{
	//Knight* knight = new Knight();
	//delete knight;

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);
	//info.dwPageSize;
	//info.dwAllocationGranularity;

	//int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//*test = 100;
	//::VirtualFree(test, 0, MEM_RELEASE);



	//Knight* knight = xnew<Knight>();

	//xdelete(knight);

	// MemoryPool 실습 코드
	// 멀티스레드 제작
	//for (int32 i = 0; i < 5; i++)
	//{
	//	GThreadManager->Launch([]()
	//		{
	//			while (true)
	//			{
	//				XVector<Knight> v(10);

	//				XMap<int32, Knight> m;
	//				m[100] = Knight();

	//				this_thread::sleep_for(10ms);
	//			}
	//	});
	//}
	//GThreadManager->Join();

	//GHeader = new SListHeader();
	//ASSERT_CRASH(((uint64)GHeader % 16) == 0); // 16바이트 정렬 체크
	////SListHeader header;
	////InitializeHead(&header); // 초기화
	//InitializeHead(GHeader);
	////Data* data = new Data();
	////PushEntrySList(&header,(SListEntry*)data);

	////Data* popData = (Data*)PopEntrySList(&header);
	////// 데이터 변환에 유의

	//for (int32 i = 0; i < 3; i++)
	//{
	//	GThreadManager->Launch([]()
	//		{
	//			while (true)
	//			{
	//				Data* data = new Data();
	//				ASSERT_CRASH(((uint64)data % 16) == 0);

	//				PushEntrySList(GHeader, (SListEntry*)data);
	//				this_thread::sleep_for(10ms);
	//			}

	//		});
	//}

	//for (int32 i = 0; i < 2; i++)
	//{
	//	GThreadManager->Launch([]()
	//		{
	//			while (true)
	//			{
	//				Data* pop = nullptr;
	//				pop = (Data*)PopEntrySList(GHeader);
	//				// 데이터를 헤더에서 추출
	//				if(pop)
	//				{
	//					cout << pop->_rand << endl;
	//					delete pop;
	//				}
	//				else// 데이터가 없으며
	//				{
	//					cout << "NONE" << endl;
	//				}
	//			}

	//		});
	//}
	//GThreadManager->Join();
	//GHeader = new SLIST_HEADER();
	//ASSERT_CRASH(((uint64)GHeader % 16) == 0); // 16바이트 정렬 체크
	//SListHeader header;
	//InitializeHead(&header); // 초기화
	//::InitializeSListHead(GHeader);
	//Data* data = new Data();
	//PushEntrySList(&header,(SListEntry*)data);

	//Data* popData = (Data*)PopEntrySList(&header);
	//// 데이터 변환에 유의

	for (int32 i = 0; i < 3; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();
					cout << knight->_hp << endl;
					
					this_thread::sleep_for(10ms);
					xdelete(knight);
				}

			});
	}
	GThreadManager->Join();
}