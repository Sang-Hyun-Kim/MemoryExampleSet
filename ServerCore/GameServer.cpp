#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>

// ����3 MemoryExample
// �޸� �Ҵ�(Allocator)
// new delete ����� �ɰ��� �޸𸮸� ����� ������ �ڿ� ȿ���� �������� �õ� ����
// new�� delete�� operator overloading�� �����ϴ�.
#include "Memory.h"

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}
	~Knight()
	{
		cout << "~Knight()" << endl;
	}

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

int main()
{
	//Knight* knight = new Knight();
	//delete knight;

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);
	//info.dwPageSize;
	//info.dwAllocationGranularity;

	int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	*test = 100;
	::VirtualFree(test, 0, MEM_RELEASE);



	Knight* knight = xnew<Knight>();

	xdelete(knight);

}