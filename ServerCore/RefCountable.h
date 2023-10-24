#pragma once
// �޸� ���� ����1: Reference Counting 
// �޸� ������ ��ü�� �����ϴ� �޸� ������ �����ϱ� ���� ����
#include "CorePch.h"
class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {} // �޸� Leak ����, �ֻ��� Ŭ������ �Ҹ��ڿ��� virtual�� �ʼ�

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount; // refCount�� �ϳ� ����
		if (refCount == 0)// �ϳ� �ٿ��µ� 0�̸� �޸� �������� ���
		{
			delete this;
		}
		return refCount;
	}
protected:
	int32 _refCount;




};

// �ش� RefCountable Ŭ������ �θ�Ŭ������ ��ӹ޾Ƽ� ����ϰ� �Ǹ�  �޸� ���� ����
// �ڽ��� �����ϰ� �ִ� ����� ����
// ����: Reference �߰�/���� �� �� �������� ������ؼ� ������� ū�ϳ�
//		��Ƽ ������ ȯ���� ��� �Ϲ� ���� ���� ������ �߻��� atomic���� �����ؾ���
//		�Ǵ� RefCount�� ���� ������ �ܼ� ���������ڰ� �ƴ� �������� �������� �ٲ����
//		���� Ÿ���� �������� ������, AddRef()�� �ѹ���(������)���� �̷����������
// => ����Ʈ ����Ʈ�� ����ϴ� ����

template<class T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	//����
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs.ptr); }

	// ����� �Ѱ���ٸ� ���������� �ϱ� ������Refcount ����

	// �̵�
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }

	// �̵��� ��� �������� �̵��ؾ��ϱ� ������ �� �����Ϳ� ���� �����͸�. ������ �����͸� nullptr�� �о��ش�.
	//RefCounting�� �״�� �������� ��

	// ��� ���� ����
	template<class U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }
public:
	// ���翬����
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr) // ���� ���� ��� ��ŵ
		{
			Release(); // �� �� Ǯ���ְ�
			Set(rhs._ptr); // ��� �� �޾ƿ���	
		}
		return *this;
	}

	// �̵� ������
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release(); // ��� �ִ� �� ����
		_ptr = rhs._ptr;// ���� ������ ����(�������� �����;���)
		rhs._ptr = nullptr; // ���� ������ ����
		return *this;
	}

	// �� ������
	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; } // TSharedPtr�� Ư�� ������ ��
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr != ptr; } // TSharedPtr�� Ư�� ������ ��
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	bool operator>(const TSharedPtr& rhs) const { return _ptr > rhs._ptr; }

	// TSharedPtr ��� �� ���� ������Ÿ�� ��ȯ�� �ʿ��� ��
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