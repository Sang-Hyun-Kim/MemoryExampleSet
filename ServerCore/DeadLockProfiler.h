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
	void CheckCycle(); // ������ �� ��ȯ ���� üũ

private:
	void Dfs(int32 here);

private:
	unordered_map<const char*, int32> _nameToId; // ������ �̸����� ������ ���������� ó���ϱ� ���� ����
	unordered_map<int32, const char*> _idToName;// ��ȯ�� �ٽ� �̸�����
	stack<int32> _lockStack; // lock�� �������� �����ϰ� ����
	map<int32, set<int32>> _lockHistory; // � lock�� ���° ���� ��Ҵ��� ���������� ����

	Mutex _lock;

private:
	vector<int32> _discoveredOrder;// ��尡 �߰ߵ� ������ ����ϴ� �迭
	// ����Ŭ�� üũ�Ҷ����� �ʱ�ȭ��
	int32 _discoveredCount = 0;// ��尡 �߰ߵ� ����
	vector<bool> _finished;// DFS(i)��° �� ����Ǿ����� Ȯ��
	vector<int32> _parent; // �߰ߵ� �θ��� ����
};

