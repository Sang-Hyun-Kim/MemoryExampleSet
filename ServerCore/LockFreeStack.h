#pragma once

// MemoryPool �׽�Ʈ�� ��
// �̱� ������ ȯ��
//template<typename T>
//struct Node
//{
//	T data;
//	Node* node;
//};
//
//struct SListEntry
//{
//	SListEntry* next;
//
//};
//struct SListHeader
//{
//	SListEntry* next = nullptr;
//};
//
//void InitializeHead(SListHeader* header);// ��� �ʱ�ȭ
//
//void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack�� �����͸� �о�ִ� �۾�
//
//SListEntry* PopEntrySList(SListHeader* header);


// ��Ƽ ������
// compare and swap ������� ����
//struct SListEntry
//{
//	SListEntry* next;
//
//};
//struct SListHeader
//{
//	SListEntry* next = nullptr;
//};
//
//void InitializeHead(SListHeader* header);// ��� �ʱ�ȭ
//
//void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack�� �����͸� �о�ִ� �۾�
//
//SListEntry* PopEntrySList(SListHeader* header);


// ��Ƽ ������ �� ABA ������ �ذ��ϱ� ���� ����

DECLSPEC_ALIGN(16) // 16����Ʈ ����
struct SListEntry
{
	SListEntry* next;

};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	union
	{
		struct
		{
			uint64 alignment; //64
			uint64 region;		//64 �� 128����Ʈ
		} DUMMYSTRUCTNAME; // �ǹ� ����
		struct
		{	// �������� ��Ʈ ����
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next : 60;
		} HeaderX64;

	};	// ������ �ϳ��� ������ ����
};

void InitializeHead(SListHeader* header);// ��� �ʱ�ȭ

void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack�� �����͸� �о�ִ� �۾�

SListEntry* PopEntrySList(SListHeader* header);