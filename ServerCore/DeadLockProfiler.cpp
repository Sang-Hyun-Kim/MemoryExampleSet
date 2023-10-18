#include "pch.h"
#include "DeadLockProfiler.h"

/*-----------------------
	DeadLockProfiler
------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ID를 찾아 발급
	int32 lockId = 0;

	// 이전 단계에서 이미 Id를 받은 것인가 확인하고 없다면
	// ID를 부여
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); // ID 부여
		_nameToId[name] = lockId; // 등록
		_idToName[lockId] = name; // 등록
	}
	else // 이미 있다면
	{
		lockId = findIt->second; // LockID 추출
	}

	// 잡고있는 락이 있었다면
	// 새로운 간선이 식별된다면 순환구조를 체크해야함
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스라면 데드락 여부를 다시 확인한다.  
		const int32 prevId = _lockStack.top(); // 맨 마지막 락을 확인
		if (lockId != prevId) // 근데 지금 잡으려고하는 락과 다르다면, 지금은 동일한 락을 다시잡아도 막지 않는 예시임
		{
			// 다른 락이라면 히스토리를 참조해서 살펴봐야 함
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end()) // 지금 ID가 처음 발견한 것이라면
			{
				history.insert(lockId); // history에 넣어주고
				CheckCycle(); // 순환구조가 생기는지 점검해야함
			}
			
		}
	}

	// 아니면 넣어주자
	_lockStack.push(lockId);// Lockstack에 넣기
}


void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty()) // Pop하려고하는데 비어있으면 오류
	{
		CRASH("MULTIPLE_UNLOCK");
	}
	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId) // Stack구조인데 id가 탑에있는 것이 아닐경우, 버그예방
	{
		CRASH("INVALID_UNLOCK");
	}


	_lockStack.pop();
}

// 1. 초기화 , 2. 체크, 3. 정리
void DeadLockProfiler::CheckCycle()
{
	//Cycle을 체크할때 사용하는 변수들을 선언 및 초기화
	const int32 lockCount = static_cast<int32>(_nameToId.size()); // 지금까지 발견된 LockCount 개수 확인
	_discoveredOrder = vector<int32>(lockCount, -1); // 초기화, 기본: -1
	// 0부터 정점 발견 수

	_discoveredCount = 0; // 개수 초기화
	_finished = vector<bool>(lockCount, false); // 해당 DFS가 완벽하게 끝났는지 체크하는 변수이고
	// 초기화는 false로

	_parent = vector<int32>(lockCount, -1); // 어떤경로에 의해 발견됐는지 확인하는 변수고
	// -1로 초기화

	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		Dfs(lockId);
	}

	// 연산끝나고 정리
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}


void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return; // 방문이 끝났으면 리턴
	
	// 방문하고 있는 정점의 순번을 매기자
	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회한다.
	// lockhistory를 통해 해당하는 번호의 정점이 간선을 가지고있는지 체크
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end()) // 현재 정점에 해당하는 history를 판단하고
	{	// 뭐가 없다 => 아직까지 해당 lock을 잡은상태에서 다른 락을 잡은 적이 없다.
		_finished[here] = true; // 끝내도됨
		return;
	}


	// 락을 잡은 상태에서 다른 락을 잡은 상황이 있다.
	set<int32>& nextSet = findIt->second;//추출
	//잡은다음에 잡은 목록 추출
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문
		// DFS 다시 호출
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here; // 이전 노드 기록
			Dfs(there); 
			continue; // 다시 반복
		}
		
		// 방문한 적이 있다면 순방향 역방향 체크
		// here가 there 보다 먼저 발견되었다면 there는 here의 후손이다.(순방향 간선)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue; // 문제 상황이 아니니 넘어가자

		// 순방향이 아니고 Dfs(there)가 아직 종료하지않았다면 , there는 here의 선조이다. (역방향간선이다.)
		if (_finished[there] == false)
		{
			// 오류전에 어느사이클이 문제인지 로그를 찍어줘야함
			// 추후 수정을 위해 필요
			printf("%s -> %s\n", _idToName[here], _idToName[there]);
			int now = here; 
			while (true)
			{
				// 부모를 계속 타고 타고 올라가자
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now]; // 순서 역순환
				if (now == there) // now가 there가 되면 끝
					break;

			}
			CRASH("DEADLOCK_DETECTED");
		}

	}

	_finished[here] = true; // 문제 없다.
}
