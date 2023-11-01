#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header);



}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));
	_useCount.fetch_sub(1);
	_reserveCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	// MemoryHeader를  List_Entry로 변환해서 넣어준 것이니  재변활 할때도 형변환이 필요함
	// 상속받은 상태이니 static_cast 사용가능
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)	);

	// 없으면 새로 할당
	// 생성자에서 만들고 시작하는 방식도 있음
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT)); // 16바이트 정렬을 보장해줘야 하기 때문에 ::malloc 대신
		// alligned_malloc 사용
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0); // 디버그용, 사이즈 체크
		_reserveCount.fetch_sub(1);
	}

	_useCount.fetch_add(1);
	

	return memory;
}
