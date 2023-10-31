#pragma once

// MemoryPool 테스트용 콛
// 싱글 스레드 환경
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
//void InitializeHead(SListHeader* header);// 헤더 초기화
//
//void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack에 데이터를 밀어넣는 작업
//
//SListEntry* PopEntrySList(SListHeader* header);


// 멀티 스레드
// compare and swap 방식으로 변경
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
//void InitializeHead(SListHeader* header);// 헤더 초기화
//
//void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack에 데이터를 밀어넣는 작업
//
//SListEntry* PopEntrySList(SListHeader* header);


// 멀티 스레드 및 ABA 문제를 해결하기 위한 수정

DECLSPEC_ALIGN(16) // 16바이트 정렬
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
			uint64 region;		//64 총 128바이트
		} DUMMYSTRUCTNAME; // 의미 없음
		struct
		{	// 세부적인 비트 지정
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next : 60;
		} HeaderX64;

	};	// 둘중의 하나의 구조를 선택
};

void InitializeHead(SListHeader* header);// 헤더 초기화

void PushEntrySList(SListHeader* header, SListEntry* entry);//Stack에 데이터를 밀어넣는 작업

SListEntry* PopEntrySList(SListHeader* header);