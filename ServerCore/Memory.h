#pragma once

#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));
	// 여기까지만 만들면 메모리가 할당은 해주는데 객체 생성자/소멸자를 호출하지 않는 문제가 발생함

	// 메모리가 할당된 상태에서 그 메모리 위에다가 객체의 생성자를 호출하는 문법이
	// Placement new라는 문법이라 함
	// 이러면 또 인자를 넣는 생성자 구현을 고려해야함
	// C++11이전에는 생성자를 경우의 수만큼 전부 따로 구현해줬었으나
	// variable template 기능을 한번에 구현 가능

	new(memory)Type(forward<Args>(args)...);

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// 소멸자 호출
	obj->~Type(); // 객체를 대상으로 소멸자 호출
	Xrelease(obj);
}