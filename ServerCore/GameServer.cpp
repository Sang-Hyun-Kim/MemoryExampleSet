#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>

// 예제3 MemoryExample
// 메모리 할당(Allocator)
// new delete 사용중 쪼개진 메모리를 제대로 관리해 자원 효율을 높히려는 시도 연습
// new와 delete도 operator overloading이 가능하다.
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