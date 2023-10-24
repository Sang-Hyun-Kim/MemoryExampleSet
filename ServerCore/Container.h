#pragma once
#include "Types.h"
#include "Allocator.h"
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
using namespace std;

// allocator 타이핑을 편하게 하기위한 using
template<typename Type>
using XVector = vector<Type, StlAllocator<Type>>;

template<typename Type>
using XList = list<Type, StlAllocator<Type>>;

template<typename Key, typename Type, typename Pred = less<Key>>
using XMap = map < Key, Type, Pred, StlAllocator<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using XSet = set<Key, Pred, StlAllocator<Key>>;

template<typename Type>
using XDeque = deque<Type, StlAllocator<Type>>;

template<typename Type, typename Container = XDeque<Type>>
using XQueue = queue<Type, Container>;

template<typename Type, typename Container = XDeque<Type>>
using XStack = stack<Type, Container>;

template<typename Type, typename Container = XVector<Type>, typename Pred = less<typename Container::value_type>>
using XPriority_Queue = priority_queue<Type, Container, Pred>;

using XString = basic_string<char, char_traits<char>, StlAllocator<char>>;
using XWString = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using XHashMap = unordered_map<Key, Type, Hasher, KeyEq, StlAllocator<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using XHashSet = unordered_set<Key, Hasher, KeyEq, StlAllocator<Key>>;