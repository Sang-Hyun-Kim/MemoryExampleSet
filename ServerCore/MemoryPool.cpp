#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		::free(header);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0;

	//Pool�� �޸� �ݳ�
	_queue.push(ptr);
	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	// Queue�� �޸� Ȯ��
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		//Pool�� ������ �ִ��� üũ
		if (_queue.empty() == false)
		{
			// �ϳ� ������
			header = _queue.front();
			_queue.pop();
		}
	
	}
	// ������ ���� �Ҵ�
	// �����ڿ��� ����� �����ϴ� ��ĵ� ����
	if (header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0); // ����׿�, ������ üũ
	}

	_allocCount.fetch_add(1);


	return header;
}