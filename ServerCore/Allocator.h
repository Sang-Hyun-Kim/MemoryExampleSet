#pragma once
// �Ҵ� ��å ����
//----------------
// BaseAllocator
//----------------
class BaseAllocator
{

public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);

};

//----------------
// StompAllocator
//----------------
// ���� üũ�� ����
// �ٸ� ��� ���� ������ 4096����� �Ҵ�Ǳ� ������ ����׶��� ����ϴ� ���� ����
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 }; // Window���� �޸� �Ҵ��� ���ִ� �Լ��� VirtualAlloc�� �ι�° �μ��� Page ������
	// ���ڸ� �ޱ� ������ PAGE_SIZE�� ����� ������ �� ���̴�.
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);

};

//----------------
// PoolAllocator
//----------------
// ���� üũ�� ����
// �ٸ� ��� ���� ������ 4096����� �Ҵ�Ǳ� ������ ����׶��� ����ϴ� ���� ����
class PoolAllocator
{
	enum { PAGE_SIZE = 0x1000 }; // Window���� �޸� �Ҵ��� ���ִ� �Լ��� VirtualAlloc�� �ι�° �μ��� Page ������
	// ���ڸ� �ޱ� ������ PAGE_SIZE�� ����� ������ �� ���̴�.
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);

};
//----------------
// STL  Allocator
//----------------

// Ŀ���� Allocator�� STL�� Ȱ�� �� �� �ֵ���
// vector�� ��� �ɼǰ����� allocator Ŭ������ ���ڷ� �־��� �� ����
template<typename T>
class StlAllocator
{
public:
	// ���ø� Ÿ���� �ڷ��� ���
	using value_type = T;

	StlAllocator() {} // ������ ȣ��

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t count)
	{
		// �޸� ������ ���
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(Xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		Xrelease(ptr);
	}
};