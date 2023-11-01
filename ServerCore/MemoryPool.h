#pragma once

// 동일한 크기의 메모리를 모아서 관리

enum
{
	SLIST_ALIGNMENT = 16
};

//디버깅을 도와줄 메모리 헤더
//----------------
// Memory Header
//----------------

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	// 메모리를 받아준 뒤 헤더를 기입해주는 함수
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);// placement new
		return reinterpret_cast<void*>(++header); //C++ 포인터 연산상 header만큼의 크기를 지나서 헤더가 붙어있는 데이터를 반환 할 것이다.
		// [MemoryHeader][data]에서 data 시작 위치 반환
	}

	// 메모리를 다사용했으면 반납할때 헤더를 반납하는 함수
	static MemoryHeader* DetachHeader(void* ptr)
	{
		// 헤더 추출, void 포인터를 MemoryHeader 포인터로 캐스팅 후 그 크기만큼 빼주기
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}


	int32 allocSize;
};
/*
	MemoryPool
*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	// 메모리를 다 사용했으면 해제하지 않고 Pool에 반납
	void				 Push(MemoryHeader* ptr);
	// 메모리가 필요해서 사용하려고 한다면 Header를 추출해서 사용
	MemoryHeader*		 Pop();
private:
	SLIST_HEADER	_header; // LockFreeStack 관리
	// 다수의 MemoryPool이 각각의 존재가 담당하고있는 크기 
	int32			_allocSize = 0;
	atomic<int32>	_useCount = 0; // Memory Pool에서 담당하는  Memory 개수
	atomic<int32> _reserveCount = 0; // MemoryPool에 저장된 개수 카운팅(디버깅용)
};

//
//DECLSPEC_ALIGN(SLIST_ALIGNMENT)
//class MemoryPool
//{
//public:
//	MemoryPool(int32 allocSize);
//	~MemoryPool();
//
//	// 메모리를 다 사용했으면 해제하지 않고 Pool에 반납
//	void				 Push(MemoryHeader* ptr);
//	// 메모리가 필요해서 사용하려고 한다면 Header를 추출해서 사용
//	MemoryHeader* Pop();
//private:
//	// 다수의 MemoryPool이 각각의 존재가 담당하고있는 크기 
//	int32 _allocSize = 0;
//	atomic<int32> _allocCount = 0; // Memory Pool에서 담당하는  Memory 개수
//
//	USE_LOCK;
//	queue<MemoryHeader*> _queue;
//
//
//};
