#pragma once

// 사용할 Define을 모아서 저장하는 헤더

#define OUT // 출력값에 따라 정해진다는 표시를 위한 정의

/*--------
    Lock
--------*/

#define USE_MANY_LOCKS(count)   Lock _locks[count];//락을 정의해줌
#define USE_LOCK                USE_MANY_LOCKS(1);//하나만 사용
#define READ_LOCK_IDX(idx)      ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK               READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)     WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK              WRITE_LOCK_IDX(0)

// 몇번째락 index를 사용할 것이냐+ 클래스 내부에서 진행되는 이름이기때문에
//typeid를 통해서 해당 클래스의 이름을 얻어오는 코드로 작성, 컴파일러가 런타임에 해당하는 객체의 이름을 얻어옴

// CRASH를 인위적으로 내고싶을때 사용
// CRASH를 내고싶어서 의도적으로 코드를 꼬아도 (ex:3/0 ) 컴파일리가 알아서 잡아버릴 수 도있음
// 여기서 cause에는 어떤 오류가 나는건지 그냥 입력을 받는 부분(CRASH 내부에서 사용되는 변수가 아님)
// crash를 입력하는 부분에서 nullptr을 넣을
// 그대로 nullptr을 유지하라는 뜻

#define CRASH(cause)                            \
{                                               \
    uint32* crash = nullptr;              \
    __analysis_assume(crash != nullptr);        \
    *crash = 0xDEADBEEF;                        \
}

// 값이 true인지 체크하고 조건부로 CRASH 내고싶은 매크로 함수
// 매크로함수 작성시 한줄씩 {} 자동 완성을 없애고 만들자..

#define ASSERT_CRASH(expr)      \
{                               \
    if(!(expr))                 \
    {                           \
        CRASH("ASSERT_CRASH");  \
        __analysis_assume(expr);\
    }                           \
}