#include "pch.h"
#include "Allocator.h"

void* BaseAllocator::Alloc(int32 size)
{
    return  ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
    ::free(ptr);
}
//----------------
// StompAllocator
//----------------

void* StompAllocator::Alloc(int32 size)
{
    // Page 수 계산 계산은 반올림 용 Page_Size만큼의 용량이 들어와도 1로 떨어지게 만듬
    const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    // 오버플로우 해결을 위한 방법=> 데이터를 끝에 배치해서 해결 [     [  ]], 언더플로우는 잘 발생하지 않으니 배제
    const int64 dataOffset = pageCount * PAGE_SIZE - size;
    void* baseAddress =  ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
    // 위치는 너가, pageCount*Page_Size 만큼, 메모리 예약과 동시에 사용,읽고 쓰기가 동시에 가능하도록
    // 만약 READONLY로 설정하고 쓰려고하면 CRASH가 남
    // 다만 메모리 오버플로우 문제는 잡아주지 못하고 있음
    // 할당되는 사이즈 보다 더큰 사이즈로 메모리를 할당 받기 때문이다
}

void StompAllocator::Release(void* ptr)
{
    const int64 address = reinterpret_cast<int64>(ptr);
    const int64 baseAddress = address - (address % PAGE_SIZE);
    ::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
