#pragma once

#include "Allocator.h"
class MemoryPool;

/*
	MemoryPool
*/


class Memory
{
	enum
	{
		// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048/256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();
	
	void* Allocate(int32 size);
	void Release(void* ptr);
private:
	// 다수의 메모리풀 보관 장소
	vector<MemoryPool*> _pools;
	// 메모리 크기 <-> 메모리풀
	// O(1)
	// 빠르게 찾기위한 Helper(Tool) 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];

};

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	// 여기까지만 만들면 메모리가 할당은 해주는데 객체 생성자/소멸자를 호출하지 않는 문제가 발생함

	// 메모리가 할당된 상태에서 그 메모리 위에다가 객체의 생성자를 호출하는 문법이
	// Placement new라는 문법이라 함
	// 이러면 또 인자를 넣는 생성자 구현을 고려해야함
	// C++11이전에는 생성자를 경우의 수만큼 전부 따로 구현해줬었으나
	// variable template 기능을 한번에 구현 가능

	new(memory)Type(forward<Args>(args)...);

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// 소멸자 호출
	obj->~Type(); // 객체를 대상으로 소멸자 호출
	PoolAllocator::Release(obj);
}

template<typename Type>
shared_ptr<Type> MakeShared() // 메모리 풀도 ObjectPool 처럼 makeshared를 위한 작업이 필요함
{
	return shared_ptr<Type>{ xnew<Type>(), xdelete<Type>};
}