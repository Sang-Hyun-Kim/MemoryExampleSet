#pragma once

// ������ ũ���� �޸𸮸� ��Ƽ� ����

enum
{
	SLIST_ALIGNMENT = 16
};

//������� ������ �޸� ���
//----------------
// Memory Header
//----------------

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	// �޸𸮸� �޾��� �� ����� �������ִ� �Լ�
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);// placement new
		return reinterpret_cast<void*>(++header); //C++ ������ ����� header��ŭ�� ũ�⸦ ������ ����� �پ��ִ� �����͸� ��ȯ �� ���̴�.
		// [MemoryHeader][data]���� data ���� ��ġ ��ȯ
	}

	// �޸𸮸� �ٻ�������� �ݳ��Ҷ� ����� �ݳ��ϴ� �Լ�
	static MemoryHeader* DetachHeader(void* ptr)
	{
		// ��� ����, void �����͸� MemoryHeader �����ͷ� ĳ���� �� �� ũ�⸸ŭ ���ֱ�
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

	// �޸𸮸� �� ��������� �������� �ʰ� Pool�� �ݳ�
	void				 Push(MemoryHeader* ptr);
	// �޸𸮰� �ʿ��ؼ� ����Ϸ��� �Ѵٸ� Header�� �����ؼ� ���
	MemoryHeader*		 Pop();
private:
	SLIST_HEADER	_header; // LockFreeStack ����
	// �ټ��� MemoryPool�� ������ ���簡 ����ϰ��ִ� ũ�� 
	int32			_allocSize = 0;
	atomic<int32>	_useCount = 0; // Memory Pool���� ����ϴ�  Memory ����
	atomic<int32> _reserveCount = 0; // MemoryPool�� ����� ���� ī����(������)
};

//
//DECLSPEC_ALIGN(SLIST_ALIGNMENT)
//class MemoryPool
//{
//public:
//	MemoryPool(int32 allocSize);
//	~MemoryPool();
//
//	// �޸𸮸� �� ��������� �������� �ʰ� Pool�� �ݳ�
//	void				 Push(MemoryHeader* ptr);
//	// �޸𸮰� �ʿ��ؼ� ����Ϸ��� �Ѵٸ� Header�� �����ؼ� ���
//	MemoryHeader* Pop();
//private:
//	// �ټ��� MemoryPool�� ������ ���簡 ����ϰ��ִ� ũ�� 
//	int32 _allocSize = 0;
//	atomic<int32> _allocCount = 0; // Memory Pool���� ����ϴ�  Memory ����
//
//	USE_LOCK;
//	queue<MemoryHeader*> _queue;
//
//
//};
