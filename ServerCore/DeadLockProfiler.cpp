#include "pch.h"
#include "DeadLockProfiler.h"

/*-----------------------
	DeadLockProfiler
------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ID�� ã�� �߱�
	int32 lockId = 0;

	// ���� �ܰ迡�� �̹� Id�� ���� ���ΰ� Ȯ���ϰ� ���ٸ�
	// ID�� �ο�
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); // ID �ο�
		_nameToId[name] = lockId; // ���
		_idToName[lockId] = name; // ���
	}
	else // �̹� �ִٸ�
	{
		lockId = findIt->second; // LockID ����
	}

	// ����ִ� ���� �־��ٸ�
	// ���ο� ������ �ĺ��ȴٸ� ��ȯ������ üũ�ؾ���
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽���� ����� ���θ� �ٽ� Ȯ���Ѵ�.  
		const int32 prevId = _lockStack.top(); // �� ������ ���� Ȯ��
		if (lockId != prevId) // �ٵ� ���� ���������ϴ� ���� �ٸ��ٸ�, ������ ������ ���� �ٽ���Ƶ� ���� �ʴ� ������
		{
			// �ٸ� ���̶�� �����丮�� �����ؼ� ������� ��
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end()) // ���� ID�� ó�� �߰��� ���̶��
			{
				history.insert(lockId); // history�� �־��ְ�
				CheckCycle(); // ��ȯ������ ������� �����ؾ���
			}
			
		}
	}

	// �ƴϸ� �־�����
	_lockStack.push(lockId);// Lockstack�� �ֱ�
}


void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty()) // Pop�Ϸ����ϴµ� ��������� ����
	{
		CRASH("MULTIPLE_UNLOCK");
	}
	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId) // Stack�����ε� id�� ž���ִ� ���� �ƴҰ��, ���׿���
	{
		CRASH("INVALID_UNLOCK");
	}


	_lockStack.pop();
}

// 1. �ʱ�ȭ , 2. üũ, 3. ����
void DeadLockProfiler::CheckCycle()
{
	//Cycle�� üũ�Ҷ� ����ϴ� �������� ���� �� �ʱ�ȭ
	const int32 lockCount = static_cast<int32>(_nameToId.size()); // ���ݱ��� �߰ߵ� LockCount ���� Ȯ��
	_discoveredOrder = vector<int32>(lockCount, -1); // �ʱ�ȭ, �⺻: -1
	// 0���� ���� �߰� ��

	_discoveredCount = 0; // ���� �ʱ�ȭ
	_finished = vector<bool>(lockCount, false); // �ش� DFS�� �Ϻ��ϰ� �������� üũ�ϴ� �����̰�
	// �ʱ�ȭ�� false��

	_parent = vector<int32>(lockCount, -1); // ���ο� ���� �߰ߵƴ��� Ȯ���ϴ� ������
	// -1�� �ʱ�ȭ

	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		Dfs(lockId);
	}

	// ���곡���� ����
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}


void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return; // �湮�� �������� ����
	
	// �湮�ϰ� �ִ� ������ ������ �ű���
	_discoveredOrder[here] = _discoveredCount++;

	// ��� ������ ������ ��ȸ�Ѵ�.
	// lockhistory�� ���� �ش��ϴ� ��ȣ�� ������ ������ �������ִ��� üũ
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end()) // ���� ������ �ش��ϴ� history�� �Ǵ��ϰ�
	{	// ���� ���� => �������� �ش� lock�� �������¿��� �ٸ� ���� ���� ���� ����.
		_finished[here] = true; // ��������
		return;
	}


	// ���� ���� ���¿��� �ٸ� ���� ���� ��Ȳ�� �ִ�.
	set<int32>& nextSet = findIt->second;//����
	//���������� ���� ��� ����
	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ���ٸ� �湮
		// DFS �ٽ� ȣ��
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here; // ���� ��� ���
			Dfs(there); 
			continue; // �ٽ� �ݺ�
		}
		
		// �湮�� ���� �ִٸ� ������ ������ üũ
		// here�� there ���� ���� �߰ߵǾ��ٸ� there�� here�� �ļ��̴�.(������ ����)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue; // ���� ��Ȳ�� �ƴϴ� �Ѿ��

		// �������� �ƴϰ� Dfs(there)�� ���� ���������ʾҴٸ� , there�� here�� �����̴�. (�����Ⱓ���̴�.)
		if (_finished[there] == false)
		{
			// �������� �������Ŭ�� �������� �α׸� ��������
			// ���� ������ ���� �ʿ�
			printf("%s -> %s\n", _idToName[here], _idToName[there]);
			int now = here; 
			while (true)
			{
				// �θ� ��� Ÿ�� Ÿ�� �ö���
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now]; // ���� ����ȯ
				if (now == there) // now�� there�� �Ǹ� ��
					break;

			}
			CRASH("DEADLOCK_DETECTED");
		}

	}

	_finished[here] = true; // ���� ����.
}
