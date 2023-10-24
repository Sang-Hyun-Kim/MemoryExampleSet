#pragma once
// 메모리 관리 예제1: Reference Counting 
// 메모리 해제된 객체를 참조하는 메모리 오염을 방지하기 위한 수단
#include "CorePch.h"
class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {} // 메모리 Leak 예방, 최상위 클래스의 소멸자에는 virtual이 필수

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount; // refCount를 하나 줄임
		if (refCount == 0)// 하나 줄였는데 0이면 메모리 해제까지 담당
		{
			delete this;
		}
		return refCount;
	}
protected:
	int32 _refCount;




};

// 해당 RefCountable 클래스를 부모클래스로 상속받아서 사용하게 되면  메모리 오염 방지
// 자신을 참조하고 있는 대상을 인지
// 단점: Reference 추가/해제 둘 다 수동으로 해줘야해서 까먹으면 큰일남
//		멀티 스레드 환경일 경우 일반 변수 사용시 문제가 발생함 atomic으로 래핑해야함
//		또는 RefCount에 대한 연산을 단순 증감연산자가 아닌 원자적인 연산으로 바꿔야함
//		또한 타겟을 가져오는 과정과, AddRef()가 한번에(원자적)으로 이루어지지않음
// => 스마트 포인트를 사용하는 이유

template<class T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	//복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs.ptr); }

	// 복사로 넘겨줬다면 관리해줘어야 하기 때문에Refcount 증가

	// 이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }

	// 이동의 경우 소유권을 이동해야하기 때문에 내 포인터에 상대방 포인터를. 상대방의 포인터를 nullptr로 밀어준다.
	//RefCounting은 그대로 가져오게 됨

	// 상속 관계 복사
	template<class U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }
public:
	// 복사연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr) // 같은 것일 경우 스킵
		{
			Release(); // 내 것 풀어주고
			Set(rhs._ptr); // 상대 것 받아오기	
		}
		return *this;
	}

	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release(); // 들고 있는 것 해제
		_ptr = rhs._ptr;// 상대방 포인터 저장(소유권을 가져와야함)
		rhs._ptr = nullptr; // 상대방 포인터 해제
		return *this;
	}

	// 비교 연산자
	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; } // TSharedPtr와 특정 포인터 비교
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr != ptr; } // TSharedPtr와 특정 포인터 비교
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	bool operator>(const TSharedPtr& rhs) const { return _ptr > rhs._ptr; }

	// TSharedPtr 사용 중 기존 포인터타입 반환이 필요할 때
	T* operator*() { return _ptr; }
	const T* operator*() { return _ptr; }

	operator T* () const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

	T* _ptr = nullptr;
};