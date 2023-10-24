#pragma once

#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));
	// ��������� ����� �޸𸮰� �Ҵ��� ���ִµ� ��ü ������/�Ҹ��ڸ� ȣ������ �ʴ� ������ �߻���

	// �޸𸮰� �Ҵ�� ���¿��� �� �޸� �����ٰ� ��ü�� �����ڸ� ȣ���ϴ� ������
	// Placement new��� �����̶� ��
	// �̷��� �� ���ڸ� �ִ� ������ ������ �����ؾ���
	// C++11�������� �����ڸ� ����� ����ŭ ���� ���� �������������
	// variable template ����� �ѹ��� ���� ����

	new(memory)Type(forward<Args>(args)...);

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// �Ҹ��� ȣ��
	obj->~Type(); // ��ü�� ������� �Ҹ��� ȣ��
	Xrelease(obj);
}