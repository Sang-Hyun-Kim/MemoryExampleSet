#pragma once

// ObjectPool을 사용하는 이유
// MemoryPool 사용 중 메모리 오염 문제가 발생했을때
// 단일 객체에서 사용중인 메모리 구역일 경우에는 찾을 방도 가 있으나
// 다수의 객체가 공유중이며 한번 사용후 일정 구역이 MemoryPool로 반환되는 경우
// 오염원인을 찾기 어려워진다
// 따라서 객체별로 관리하는 ObjectPool도 같이 운영한다.

#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{

public:
	template<typename... Args>
	static Type* Pop(Args&&... args) // 자료형을 받아 반환
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		// 메모리 꺼내쓰기 + _Stomp가 정의 되지 않았다면 일반 Allocator 사용
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();

#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj)); // 반납할때에는 헤드 제거 후 푸쉬
#endif // _STOMP

		
		

	}
	// 사용과 반납
// shared_ptr<> = make_shared<>() => 기본적인 new/ delete를 사용하기 때문에 Allocator를 사용할 수 없게 됨
// 인자로 넣어 줄 수가 있음
// shared_ptr<Knight> sptr = { ObjectPool<Knight>::Pop(), ObjectPool<Knight>::Push};
// 인자로 넣기보단 아예 자체적인 함수로 만들어서 사용
	static shared_ptr<Type> MakeShared() 
	{
		shared_ptr<Type> ptr = { Pop(),Push };
		return ptr;
	}

private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
		
};
// 특정 Type 전용으로 Pool을 만들어 관리

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };