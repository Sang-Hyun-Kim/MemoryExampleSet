#pragma once
extern thread_local uint32 LThreadId; // 스레드 번호를 부여
// thread id는 표준에도 부여하지만 생성순서대로 부여한다음 유지하기 위해 직접 생성,관리 할것