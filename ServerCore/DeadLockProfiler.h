#pragma once
#include <stack>
#include <map>
#include <vector>
/*-----------------------
	DeadLockProfiler
------------------------*/

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle(); // 역간선 및 순환 구조 체크

private:
	void Dfs(int32 here);

private:
	unordered_map<const char*, int32> _nameToId; // 스레드 이름으로 받지만 정수형으로 처리하기 위해 저장
	unordered_map<int32, const char*> _idToName;// 반환시 다시 이름으로
	stack<int32> _lockStack; // lock을 스택으로 저장하고 추적
	map<int32, set<int32>> _lockHistory; // 어떤 lock이 몇번째 락을 잡았는지 간선정보를 저장

	Mutex _lock;

private:
	vector<int32> _discoveredOrder;// 노드가 발견된 순서를 기록하는 배열
	// 사이클을 체크할때마다 초기화됨
	int32 _discoveredCount = 0;// 노드가 발견된 순서
	vector<bool> _finished;// DFS(i)번째 가 종료되었는지 확인
	vector<int32> _parent; // 발견된 부모노드 추적
};

