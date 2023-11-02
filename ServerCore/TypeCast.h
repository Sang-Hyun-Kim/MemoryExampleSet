#pragma once
#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;

};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion


// Ÿ�� ����Ʈ
// Ŭ���� Ÿ���� ��� ����Ʈ ������ �����̳�
// TypeList�� ������ ������ Ÿ�ӿ� ���� �� ����
// List�� Vector�� ��� �����Ͱ� ��Ÿ�ӿ� ����
// ������ Ÿ�ӿ� ���� �ȴٴ� ���� ���� ���� ����ܰ迡���� �ƹ� �δ��� �Ȱɸ��ٴ� ����

#pragma region Length
// TypeList �ȿ� �� ���ø� ������ ���� üũ���ִ� ��� ���
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
	// enum�� ������ Ÿ�ӿ� ������
	// ����ִ� ��� value = 0�̶� ���� ��ȯ
};


template<typename T, typename... U>
struct Length<TypeList<T,U...>>
{
	enum {value = 1+ Length<TypeList<U...>>::value }; // ��� ȣ���� ���� �� ���� ���
	// ���� ������ ȣ�� �غ��� ������Ÿ�ӿ� ���� �����Ǿ ���콺 ������ ���� Ȯ�ΰ��� 
};
#pragma endregion

#pragma region TypeAt
// �����Ͱ� ���������� ��ġ�� ã��
template<typename TL, int32 index>
struct TypeAt;

template<typename Head , typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};
template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail... >, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion 

#pragma region IndexOf
// ���� �ݴ�� Index�� �����ְ� �ش� �ε����� �ش��ϴ� Type�� ��ȯ
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { vaule = 0 };
};

template<typename T>// �ƹ��͵� ��ã��
struct IndexOf<TypeList<>, T>
{
	enum {value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};

#pragma endregion

#pragma region Conversion

template<typename From, typename To>
class Conversion // From Ÿ�� ���� To Type���� ��ȯ
{
private:
	using Small = __int8;
	using Big = __int32;
	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }


public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
		// To �� ��ȯ�� ������ ��� Small�� ��ȯ
		// ��ȯ�� �ȵǴ� ��� Big ��ȯ������ ��(...)���� �ٸ� ������ ��찡 ���� �����
	};

};  // exist�� ��ȯ ���� 0�̳� 1�̳Ŀ� ���� ������ Ÿ�ӿ��� ��ȯ ���� ���θ� Ȯ�� �� �� ����
#pragma endregion

#pragma region TypeCast

template<int32 v> // Ư�� �������� �� ������ ���� ClassType�� �ο�
struct Int2Type
{
	enum { value = v};
}; // ���� for���� ������ �Ϲ� ���� int�� ������ Ÿ�ӿ� ����� ����� ��� �� �� ����
// typename V �� ���� ������ Ŭ������ ��������� ����ü�� ������

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};
	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j+1>());
		// �̸��� ���� �ƿ� Ÿ���� �ٸ��� ������ ���ο� MakeTable �Լ��� �����Ǿ ���� ��
	}

	// ���� ������ ����ü�� ���� ���ڷ� �� int i, j�� ������ ���ο� ���� Ŭ������ �� ���̰�
	// ���� ��ȯ���� �ϳ��� ��ȯ �������� üũ�ϴ� ����� �����

	template<int32 i>
	static void MakeTable(Int2Type<i>(), Int2Type<length>())
	{
		MakeTable(Int2Type<i+1>, Int2Type<0>());
	}
	// i�� ���� ��ȸ�� ������(j�� ������ ���� length���� ���� ���� ���� ���ø� Ư��ȭ

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
		// i�� �ִ밪 ��ȸ�� ������ ��ȸ ���� �ʵ���
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length]; // �迭 Index�� enum�� �Ѱ��༭ ������Ÿ�ӿ���  ���� ��� ������
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

template<typename To,typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;
	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value)) // ��Ī�� TypeId�� �ְ� üũ�� Type�� Ȯ��, 
		return static_cast<To>(ptr);// �����͸��Ѱ��ټ��� ������ �����͸� �Ϲ�Ÿ������ �д� remove_pointer���
	return nullptr; // ��ȯ�� �ȵǴ� ��� nullptr ��ȯ, ����� class�鿡 ���� ����(�ڷ��� ����)_typeId�� �־���ϴ� ����!!
} 

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;
	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value)) // ��Ī�� TypeId�� �ְ� üũ�� Type�� Ȯ��, 
		return static_pointer_cast<To>(ptr);// �����͸��Ѱ��ټ��� ������ �����͸� �Ϲ�Ÿ������ �д� remove_pointer���
	return nullptr; // ��ȯ�� �ȵǴ� ��� nullptr ��ȯ, ����� class�鿡 ���� ����(�ڷ��� ����)_typeId�� �־���ϴ� ����!!
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value); // ��Ī�� TypeId�� �ְ� üũ�� Type�� Ȯ��, 

}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value); // ��Ī�� TypeId�� �ְ� üũ�� Type�� Ȯ��, 

}
#pragma endregion

#define DECLARE_TL using TL = TL; int32 _typeid; // �� Ŭ�������� �ݺ������� �־��� ����
#define INIT_TL(Type) _typeid = IndexOf<TL,Type>::value; // Ÿ���� �־��ָ� TL�� Ÿ���� ã���ش��� �� Value�� _typeid �� �־��ش�.