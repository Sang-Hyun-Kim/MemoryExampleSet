#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.


//��Ŀ���Ͽ� �������ְų�
//���� ������� ����//��ó���� �ܰ迡�� ��������
#ifdef _DEBUG
#pragma comment(lib,"Debug\\ServerCore.lib")
#else
#pragma comment(lib,"Release\\ServerCore.lib")
#endif
// ���⿡ �̸� �������Ϸ��� ��� �߰�
#include "CorePch.h"