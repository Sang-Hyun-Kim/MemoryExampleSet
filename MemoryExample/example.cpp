#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include "RefCountable.h"
// ����2 : SmartPointer

// RefCounting Ŀ���� Ŭ������ ����
// 1. �̹� ������� Ŭ���� ������� ��� �Ұ���
// 2. ��ȯ ����(Cycle)

using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	void SetTarget(KnightRef target)
	{
		_target = target;
	}

	KnightRef _target = nullptr;
	InventoryRef _inventory = nullptr;
};
class Inventory : public RefCountable
{
public:

	Inventory(KnightRef knight) : _knight(knight)
	{

	}

	KnightRef _knight; // &������ ��� ������ **�� ������ ������ �����ؾ��� 
	//�����ڰ� �̷��� �� ���
	// ��ȯ ������ �߻��� �� �ִ�.
};


int main()
{
	KnightRef k1(new Knight());
	k1->ReleaseRef();

	k1->_inventory = new Inventory(k1);
	unique_ptr<Knight> k2 = make_unique<Knight>();
	// Unique PTR => ���簡 ��������, ���� ��� 
	// unique_ptr<Knight> k3 = k2; // ����
	unique_ptr<Knight> k3 = ::move(k2); // ���簡 �ƴ� �̵�, ������ �����ϰ� �Ѱܶ�
	// shared_ptr�� ���������� �Ҹ��ڿ��� �������ִ� ���� �����ϰ�
	// ���縦 �����ִ� ���� �ֵ� ����� ������ ǥ�� ������

	//shared_ptr: ���� ����� �� ���� ������� sharedptr ���� Ŭ������ ����ؾ��ϴ� �������־�
	// �̹� ������� Ŭ������ ���ؼ��� ����� ���ѵ�����  ǥ�� shared_ptr�� ������
	// �츮�� ���� ����� �Ǹ� �޸𸮸� �� ������ ������ ��
	// [Knight]     [RefCountable]
	// ��� ������ ��� ���� ��ü ������ RefCountable ������ ������ ����
	
	// [T*][RefCountBlocking*] �� ��쿡�� �ѹ��� ��
	// shared_ptr<Knight> spr;

	//[ T | RefCountBlocking* ] make_shared<>()�� ������ �ι��� ���� �Ҵ����� �ʰ� �ѹ��� �Ҵ���
	shared_ptr<Knight> spr = make_shared<Knight>(); // new Knight�� ��ü
	// [T*][RefCountBlocking*]
	shared_ptr<Knight> spr2 = spr;
	//Ref_count_base ��ü�� _Uses �� _Weaks ��� ������ ����

	// weak_ptr
	weak_ptr<Knight> wpr = spr; // shared_ptr �޾Ƽ� ��� ����
	bool expired = wpr.expired(); // Weakptr�� ���������� Ȯ���ؾ���
	// �Ǵ� �ٽ� shared_ptr�� ��ȯ�ؼ� ���
	shared_ptr<Knight> spr2 = wpr.lock();
	// ���� �⺻ ������ spr�� ���ư��ٸ�?
	// wpr�� ���� sp2���� nullptr�� ��
	// �� ���� ���� _Uses�� shared_ptr�� Ref_count_base(RefCountBlock)�� �����ϰ� �ִ� ���� ������ ���̰�
	// _Weaks�� WeakPtr�� ������ Ref_count_base�� �����ϴ� Ƚ���� ������

	// �뵵�� �̿� ���� _Uses�� 0�� �� ��� �� �̻� �ش� ��ü�� ��� ���� �����Ƿ�
	// [ nullptr | Ref_Count_Base]�� �������� _Weaks�� 0�� �ƴϹǷ� ���� RefCountBlock��
	// �������� �ʰ� ��ٸ���.
	// �ٸ� ��ü�� weakptr�� �̿��ؼ� �������̶�� ��� ������ ���̴� 
	// ��
	// bool expired = wpr.expired();=> ���� Knight�κ��� nullptr���� ���� �ϴ� �Լ��̸�
	// shared_ptr<Knight> spr2 = wpr.lock();=> shared_ptr�� ����� �Լ��̴�.
	// �ٸ� ������� ��ü �������� lock�� ������ ��� �� shared_ptr�� ��ȯ�� ���̴�.
	// ���� �ΰ��� ������� ������ ������ Ȯ���ؼ� ����Ѵ�.
	
}