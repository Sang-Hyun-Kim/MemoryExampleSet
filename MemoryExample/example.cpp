#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include "RefCountable.h"
// 예제2 : SmartPointer

// RefCounting 커스텀 클래스의 단점
// 1. 이미 만들어진 클래스 대상으로 사용 불가능
// 2. 순환 문제(Cycle)

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

	KnightRef _knight; // &형태의 멤버 변수와 **로 참조의 참조로 설정해야할 
	//생성자가 이렇게 될 경우
	// 순환 문제가 발생할 수 있다.
};


int main()
{
	KnightRef k1(new Knight());
	k1->ReleaseRef();

	k1->_inventory = new Inventory(k1);
	unique_ptr<Knight> k2 = make_unique<Knight>();
	// Unique PTR => 복사가 막혀있음, 단일 취급 
	// unique_ptr<Knight> k3 = k2; // 오류
	unique_ptr<Knight> k3 = ::move(k2); // 복사가 아닌 이동, 소유권 포기하고 넘겨라
	// shared_ptr과 마찬가지로 소멸자에서 삭제해주는 것은 동일하고
	// 복사를 막아주는 것이 주된 기능한 간단한 표준 포인터

	//shared_ptr: 직접 만들어 준 것은 만들어준 sharedptr 관련 클래스를 상속해야하는 단점이있어
	// 이미 만들어진 클래스에 대해서는 사용이 제한되지만  표준 shared_ptr은 가능함
	// 우리가 직접 만들게 되면 메모리를 두 공간을 가지게 됨
	// [Knight]     [RefCountable]
	// 상속 구조의 경우 임의 객체 정보에 RefCountable 정보가 들어가있지 않음
	
	// [T*][RefCountBlocking*] 이 경우에는 한번에 들어감
	// shared_ptr<Knight> spr;

	//[ T | RefCountBlocking* ] make_shared<>()는 공간을 두번에 걸쳐 할당하지 않고 한번에 할당함
	shared_ptr<Knight> spr = make_shared<Knight>(); // new Knight를 대체
	// [T*][RefCountBlocking*]
	shared_ptr<Knight> spr2 = spr;
	//Ref_count_base 객체는 _Uses 와 _Weaks 라는 변수를 가짐

	// weak_ptr
	weak_ptr<Knight> wpr = spr; // shared_ptr 받아서 사용 가능
	bool expired = wpr.expired(); // Weakptr은 존재유무를 확인해야함
	// 또는 다시 shared_ptr로 변환해서 사용
	shared_ptr<Knight> spr2 = wpr.lock();
	// 만약 기본 참조인 spr이 날아간다면?
	// wpr은 물론 sp2까지 nullptr이 됨
	// 즉 위의 변수 _Uses는 shared_ptr로 Ref_count_base(RefCountBlock)를 참조하고 있는 것을 저장한 것이고
	// _Weaks는 WeakPtr로 동일한 Ref_count_base를 참조하는 횟수를 저장함

	// 용도는 이와 같다 _Uses가 0이 된 경우 더 이상 해당 객체가 사용 되지 않으므로
	// [ nullptr | Ref_Count_Base]로 만들지만 _Weaks가 0이 아니므로 뒤의 RefCountBlock은
	// 삭제하지 않고 기다린다.
	// 다른 객체가 weakptr을 이용해서 참조중이라면 계속 유지할 것이다 
	// 즉
	// bool expired = wpr.expired();=> 현재 Knight부분이 nullptr인지 질의 하는 함수이며
	// shared_ptr<Knight> spr2 = wpr.lock();=> shared_ptr을 만드는 함수이다.
	// 다만 만약없는 객체 포인터의 lock을 가져올 경우 빈 shared_ptr을 반환할 것이다.
	// 따라서 두가지 방법으로 원본의 유무를 확인해서 사용한다.
	
}