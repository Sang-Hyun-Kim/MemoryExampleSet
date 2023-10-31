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
	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	// MemoryHeader��  List_Entry�� ��ȯ�ؼ� �־��� ���̴�  �纯Ȱ �Ҷ��� ����ȯ�� �ʿ���
	// ��ӹ��� �����̴� static_cast ��밡��
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)	);

	// ������ ���� �Ҵ�
	// �����ڿ��� ����� �����ϴ� ��ĵ� ����
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT)); // 16����Ʈ ������ ��������� �ϱ� ������ ::malloc ���
		// alligned_malloc ���
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0); // ����׿�, ������ üũ
	}

	_allocCount.fetch_add(1);


	return memory;
}
