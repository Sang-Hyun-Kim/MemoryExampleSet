#include "pch.h"
#include "LockFreeStack.h"




// �̱� ������ ȯ��
//// [SListEntry(���� ��带 ����Ŵ)+Data(hp,mp)]
//// [header]
//// �ǻ��ڵ�
//// ������ ����� ������� �����͸� �ְ� �� �� �ְ� ����
//void InitializeHead(SListHeader* header)// ��� �ʱ�ȭ
//{
//	header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack�� �����͸� �о�ִ� �۾�
//{
//	// �����͸� ������ ����� �翬������
//	entry->next = header->next;
//	header->next = entry;
//	// ��� next�� ����Ű�� data�� �����ؼ� ������	
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* first = header->next;
//	// ù��° ����������
//	if (first != nullptr)
//	{
//		header->next = first->next;
//	} // �����Ͱ� ������ ���� ���� header�� ����
//	// �����Ͱ� �ϳ��ۿ� ������ �ϳ� �����ϰ� nullptr ���·� ���� ����
//
//	return first;
//}

// ��Ƽ ������

//void InitializeHead(SListHeader* header)// ��� �ʱ�ȭ
//{
//	header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack�� �����͸� �о�ִ� �۾�
//{
//	// �����͸� ������ ����� �翬������
//	entry->next = header->next;
//	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next)==0)
//	{
//
//	}
//	//	1,3��° ������ ���� ������ 2��° ������ ���� 1��° ������ ������ ��
//	// 0�� ��ȯ�Ǹ� ����=> ���� �� �� ���� �ݺ�
//	//header->next = entry;
//	// ��� next�� ����Ű�� data�� �����ؼ� ������	
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* expected = header->next;
//	
//	
//	// ù��° ����������
//	/*
//	if (first != nullptr)
//	{
//		header->next = first->next;
//	}
//	*/ 
//// �����Ͱ� ������ ���� ���� header�� ����
//	// �����Ͱ� �ϳ��ۿ� ������ �ϳ� �����ϰ� nullptr ���·� ���� ����
//
//	// ABAProblem
//	// 
//	while (expected && ::InterlockedExchange64((int64*)&header->next, (int64)expected->next, (int64)expected)==0)
//	{
//
//	}
//
//	return expected;
//}

// ABA ���� ����
void InitializeHead(SListHeader* header)// ��� �ʱ�ȭ
{
	header->alignment = 0;
	header->region = 0;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack�� �����͸� �о�ִ� �۾�
{
	SListHeader expected = {};
	SListHeader desired = {};

	// �׻� 16����Ʈ ������ �ϰ� ������ �Ǿ��ִٸ� ������ ��Ʈ 4���� 0000�� �Ǿ����� ����
	// 16 ���ķ� �������� �����ʹ� ������ 4����Ʈ�� �ݵ�� 0000 �̹Ƿ�
	// 60����Ʈ �����Ϳ� ��Ʈ����Ʈ ������ ���� �־��� �� �ְ� ��
	desired.HeaderX64.next = (((uint64)entry) >> 4);

	while (true)
	{ 
		// �ش� �۾����̿� �����Ͱ� ����� �� ������ depth�� sequence�� ���� ī��Ʈ ���� ���̴�.
		entry->next = (SListEntry*)((uint64)expected.HeaderX64.next << 4); // 4��Ʈ ����
		
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
		// countting�� ���� �������ڸ� �߱޹޾� �ش� ���ڸ� ���ؼ�
		// ���� �״�ζ�� ��������
		// ���� �ٲ���ٸ� �ٽ� ��ȸ�ϸ� �ٲﰪ�� ���� ������
	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *header;

		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		if (entry == nullptr)
		{
			break;
		}
		// Use_After_Free ���� �ذ��ʿ�
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1; // pop�� ���� �����͸� �ϳ� �ٿ���
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1; // sequence�� ���� ��ü�� ī���� �ϴ� ����
	
		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}
