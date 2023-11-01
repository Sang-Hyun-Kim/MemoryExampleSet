#pragma once

// ObjectPool�� ����ϴ� ����
// MemoryPool ��� �� �޸� ���� ������ �߻�������
// ���� ��ü���� ������� �޸� ������ ��쿡�� ã�� �浵 �� ������
// �ټ��� ��ü�� �������̸� �ѹ� ����� ���� ������ MemoryPool�� ��ȯ�Ǵ� ���
// ���������� ã�� ���������
// ���� ��ü���� �����ϴ� ObjectPool�� ���� ��Ѵ�.

#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{

public:
	template<typename... Args>
	static Type* Pop(Args&&... args) // �ڷ����� �޾� ��ȯ
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		// �޸� �������� + _Stomp�� ���� ���� �ʾҴٸ� �Ϲ� Allocator ���
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
		s_pool.Push(MemoryHeader::DetachHeader(obj)); // �ݳ��Ҷ����� ��� ���� �� Ǫ��
#endif // _STOMP

		
		

	}
	// ���� �ݳ�
// shared_ptr<> = make_shared<>() => �⺻���� new/ delete�� ����ϱ� ������ Allocator�� ����� �� ���� ��
// ���ڷ� �־� �� ���� ����
// shared_ptr<Knight> sptr = { ObjectPool<Knight>::Pop(), ObjectPool<Knight>::Push};
// ���ڷ� �ֱ⺸�� �ƿ� ��ü���� �Լ��� ���� ���
	static shared_ptr<Type> MakeShared() 
	{
		shared_ptr<Type> ptr = { Pop(),Push };
		return ptr;
	}

private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
		
};
// Ư�� Type �������� Pool�� ����� ����

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };