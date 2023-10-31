#include "pch.h"
#include "LockFreeStack.h"




// 싱글 스레드 환경
//// [SListEntry(다음 노드를 가르킴)+Data(hp,mp)]
//// [header]
//// 의사코드
//// 스택이 헤더를 기반으로 데이터를 넣고 뺄 수 있게 만듬
//void InitializeHead(SListHeader* header)// 헤더 초기화
//{
//	header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack에 데이터를 밀어넣는 작업
//{
//	// 데이터를 넣으면 헤더를 재연결해줌
//	entry->next = header->next;
//	header->next = entry;
//	// 헤더 next가 가리키던 data를 추출해서 보내기	
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* first = header->next;
//	// 첫번째 데이터추출
//	if (first != nullptr)
//	{
//		header->next = first->next;
//	} // 데이터가 있으면 다음 것을 header로 지정
//	// 데이터가 하나밖에 없으면 하나 제외하고 nullptr 상태로 만들 것임
//
//	return first;
//}

// 멀티 스레드

//void InitializeHead(SListHeader* header)// 헤더 초기화
//{
//	header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack에 데이터를 밀어넣는 작업
//{
//	// 데이터를 넣으면 헤더를 재연결해줌
//	entry->next = header->next;
//	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next)==0)
//	{
//
//	}
//	//	1,3번째 인자의 값이 같으면 2번째 인자의 값이 1번째 인자의 값으로 들어감
//	// 0이 반환되면 실패=> 성공 할 때 까지 반복
//	//header->next = entry;
//	// 헤더 next가 가리키던 data를 추출해서 보내기	
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* expected = header->next;
//	
//	
//	// 첫번째 데이터추출
//	/*
//	if (first != nullptr)
//	{
//		header->next = first->next;
//	}
//	*/ 
//// 데이터가 있으면 다음 것을 header로 지정
//	// 데이터가 하나밖에 없으면 하나 제외하고 nullptr 상태로 만들 것임
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

// ABA 문제 개선
void InitializeHead(SListHeader* header)// 헤더 초기화
{
	header->alignment = 0;
	header->region = 0;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)//Stack에 데이터를 밀어넣는 작업
{
	SListHeader expected = {};
	SListHeader desired = {};

	// 항상 16바이트 정렬을 하고 정렬이 되어있다면 최하위 비트 4개는 0000이 되어있을 꺼임
	// 16 정렬로 떨어지는 데이터는 마지막 4개비트가 반드시 0000 이므로
	// 60바이트 데이터에 비트쉬프트 연산을 통해 넣어줄 수 있게 됨
	desired.HeaderX64.next = (((uint64)entry) >> 4);

	while (true)
	{ 
		// 해당 작업사이에 데이터가 변경될 수 있으며 depth와 sequence를 통해 카운트 해줄 것이다.
		entry->next = (SListEntry*)((uint64)expected.HeaderX64.next << 4); // 4비트 복원
		
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
		// countting을 통해 고유숫자를 발급받아 해당 숫자를 비교해서
		// 값이 그대로라면 연산종료
		// 값이 바뀌었다면 다시 순회하며 바뀐값을 통해 진행함
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
		// Use_After_Free 문제 해결필요
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1; // pop을 통해 데이터를 하나 줄여줌
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1; // sequence는 연산 자체를 카운팅 하는 역할
	
		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}
