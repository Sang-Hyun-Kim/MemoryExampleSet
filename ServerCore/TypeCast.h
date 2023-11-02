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


// 타입 리스트
// 클래스 타입을 담는 리스트 형태의 컨테이너
// TypeList의 변수는 컴파일 타임에 결정 될 것임
// List나 Vector의 경우 데이터가 런타임에 결정
// 컴파일 타임에 결정 된다는 것은 빌드 이후 실행단계에서는 아무 부담이 안걸린다는 것임

#pragma region Length
// TypeList 안에 들어간 템플릿 변수의 수를 체크해주는 기능 담당
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
	// enum도 컴파일 타임에 결정됨
	// 비어있느 경우 value = 0이란 값을 반환
};


template<typename T, typename... U>
struct Length<TypeList<T,U...>>
{
	enum {value = 1+ Length<TypeList<U...>>::value }; // 재귀 호출을 통한 총 길이 계산
	// 실제 변수를 호출 해보면 컴파일타임에 값이 결정되어서 마우스 오버로 값을 확인가능 
};
#pragma endregion

#pragma region TypeAt
// 데이터가 많아졌을때 위치를 찾기
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
// 위와 반대로 Index를 던져주고 해당 인덱스에 해당하는 Type을 반환
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { vaule = 0 };
};

template<typename T>// 아무것도 못찾음
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
class Conversion // From 타입 에서 To Type으로 변환
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
		// To 로 변환이 가능한 경우 Small로 반환
		// 변환이 안되는 경우 Big 반환형으로 들어감(...)으로 다른 예외의 경우가 전부 실행됨
	};

};  // exist의 반환 값이 0이냐 1이냐에 따라 컴파일 타임에서 변환 가능 여부를 확인 할 수 있음
#pragma endregion

#pragma region TypeCast

template<int32 v> // 특정 정수에게 그 정수에 대한 ClassType을 부여
struct Int2Type
{
	enum { value = v};
}; // 이중 for문을 돌릴때 일반 변수 int는 컴파일 타임에 선언될 수없어서 사용 할 수 없음
// typename V 에 따라 각각의 클래스가 만들어지는 구조체를 선언함

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
		// 이름만 같지 아예 타입이 다르기 때문에 새로운 MakeTable 함수가 생성되어서 실행 됨
	}

	// 위의 선언한 구조체를 통해 인자로 들어간 int i, j는 각각이 새로운 숫자 클래스가 될 것이고
	// 이증 순환으로 하나씩 변환 가능한지 체크하는 기능을 담당함

	template<int32 i>
	static void MakeTable(Int2Type<i>(), Int2Type<length>())
	{
		MakeTable(Int2Type<i+1>, Int2Type<0>());
	}
	// i에 대한 순회가 끝나면(j에 마지막 값인 length값이 들어갔을 때에 대한 템플릿 특수화

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{
		// i의 최대값 순회가 끝나면 순회 하지 않도록
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length]; // 배열 Index로 enum을 넘겨줘서 컴파일타임에서  변수 사용 가능함
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

template<typename To,typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;
	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value)) // 서칭할 TypeId를 넣고 체크할 Type를 확인, 
		return static_cast<To>(ptr);// 포인터를넘겨줄수도 있으니 포인터를 일반타입으로 읽는 remove_pointer사용
	return nullptr; // 변환이 안되는 경우 nullptr 반환, 사용할 class들에 임의 변수(자료형 무관)_typeId가 있어야하니 주의!!
} 

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;
	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value)) // 서칭할 TypeId를 넣고 체크할 Type를 확인, 
		return static_pointer_cast<To>(ptr);// 포인터를넘겨줄수도 있으니 포인터를 일반타입으로 읽는 remove_pointer사용
	return nullptr; // 변환이 안되는 경우 nullptr 반환, 사용할 class들에 임의 변수(자료형 무관)_typeId가 있어야하니 주의!!
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value); // 서칭할 TypeId를 넣고 체크할 Type를 확인, 

}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value); // 서칭할 TypeId를 넣고 체크할 Type를 확인, 

}
#pragma endregion

#define DECLARE_TL using TL = TL; int32 _typeid; // 각 클래스마다 반복적으로 넣어줄 내용
#define INIT_TL(Type) _typeid = IndexOf<TL,Type>::value; // 타입을 넣어주면 TL의 타입을 찾아준다음 그 Value를 _typeid 에 넣어준다.