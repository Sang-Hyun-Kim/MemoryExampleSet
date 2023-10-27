#pragma once
// 할당 정책 적용
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
// 버그 체크에 유용
// 다만 배우 작은 변수도 4096배수로 할당되기 때문에 디버그때만 사용하는 것을 유의
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 }; // Window에서 메모리 할당을 해주는 함수인 VirtualAlloc의 두번째 인수가 Page 단위로
	// 인자를 받기 때문에 PAGE_SIZE의 배수로 기입해 줄 것이다.
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);

};

//----------------
// PoolAllocator
//----------------
// 버그 체크에 유용
// 다만 배우 작은 변수도 4096배수로 할당되기 때문에 디버그때만 사용하는 것을 유의
class PoolAllocator
{
	enum { PAGE_SIZE = 0x1000 }; // Window에서 메모리 할당을 해주는 함수인 VirtualAlloc의 두번째 인수가 Page 단위로
	// 인자를 받기 때문에 PAGE_SIZE의 배수로 기입해 줄 것이다.
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);

};
//----------------
// STL  Allocator
//----------------

// 커스텀 Allocator가 STL에 활용 될 수 있도록
// vector의 경우 옵션값으로 allocator 클래스를 인자로 넣어줄 수 있음
template<typename T>
class StlAllocator
{
public:
	// 템플릿 타입의 자료형 명시
	using value_type = T;

	StlAllocator() {} // 생성자 호출

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t count)
	{
		// 메모리 사이즈 계산
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(Xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		Xrelease(ptr);
	}
};