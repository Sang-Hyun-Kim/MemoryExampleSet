#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include "LockFreeStack.h"
// ����3 MemoryExample
// �޸� �Ҵ�(Allocator)
// new delete ����� �ɰ��� �޸𸮸� ����� ������ �ڿ� ȿ���� �������� �õ� ����
// new�� delete�� operator overloading�� �����ϴ�.
#include "Memory.h"

class Knight
{
public:
	int32 _hp = rand() % 1000;

	// Ŭ���� �ȿ� ������ �ش� Ŭ�������Ը� ����
	// ������ ���� new() �μ��� �� �־������
	//void* operator new(size_t size)
	//{
	//	cout << " Knight new! " << size << endl; // ������
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

// MS���� ���� �����ϴ� MemoryPool ����ü�� ����
//DECLSPEC_ALIGN(16)
//class Data //:public SListEntry
//{
//public:
//	SLIST_ENTRY _entry; 
//
//	int64 _rand = rand() % 1000;
//};

// �̷��� ��� �Ϳ� ���� �������� �۵�(����)
//void* operator new(size_t size)
//{
//	cout << "new! " << size << endl; // ������
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//void* operator new[](size_t size)
//{
//	cout << "new[]! " << size << endl; // ������
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

	// MemoryPool �ǽ� �ڵ�
	// ��Ƽ������ ����
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
	//ASSERT_CRASH(((uint64)GHeader % 16) == 0); // 16����Ʈ ���� üũ
	////SListHeader header;
	////InitializeHead(&header); // �ʱ�ȭ
	//InitializeHead(GHeader);
	////Data* data = new Data();
	////PushEntrySList(&header,(SListEntry*)data);

	////Data* popData = (Data*)PopEntrySList(&header);
	////// ������ ��ȯ�� ����

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
	//				// �����͸� ������� ����
	//				if(pop)
	//				{
	//					cout << pop->_rand << endl;
	//					delete pop;
	//				}
	//				else// �����Ͱ� ������
	//				{
	//					cout << "NONE" << endl;
	//				}
	//			}

	//		});
	//}
	//GThreadManager->Join();
	//GHeader = new SLIST_HEADER();
	//ASSERT_CRASH(((uint64)GHeader % 16) == 0); // 16����Ʈ ���� üũ
	//SListHeader header;
	//InitializeHead(&header); // �ʱ�ȭ
	//::InitializeSListHead(GHeader);
	//Data* data = new Data();
	//PushEntrySList(&header,(SListEntry*)data);

	//Data* popData = (Data*)PopEntrySList(&header);
	//// ������ ��ȯ�� ����

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