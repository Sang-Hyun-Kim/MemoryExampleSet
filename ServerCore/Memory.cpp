#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// 첫번째 범위에 대한 순회
	// 첫번째 범위 내에 있는 단위마다 특정 메모리 풀을 참조하도록 사용
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 두번째 범위
	for (size=1024; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 세번째 범위
	for (size = 2048; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 최종 풀카운트는 48개
}

// 메모리 풀이 해제 된다는 것은 프로그램이 끝나는 것을 의미해서
// 굳이 구현할필요는 없지만 예방책으로 메모리 할당 해제 구현
Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
	{
		delete pool;
	}
	_pools.clear();
}

// 헤더를 포함하지 않은 사용하고 싶은 메모리 사이즈 반환
void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// 메모리 풀에서 꺼내와서 할당
		header = _poolTable[allocSize]->Pop();

	}

	return MemoryHeader::AttachHeader(header,allocSize);
}

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		//메모리 풀링 최대 크기를 벗어나면 일반 해제
		::_aligned_free(header);
	}
	else // 작은 크기->풀에 반납
	{
		_poolTable[allocSize]->Push(header);
	} 
}
