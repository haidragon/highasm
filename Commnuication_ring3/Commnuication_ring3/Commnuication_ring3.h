
// Commnuication_ring3.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCommnuication_ring3App: 
// �йش����ʵ�֣������ Commnuication_ring3.cpp
//

class CCommnuication_ring3App : public CWinApp
{
public:
	CCommnuication_ring3App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCommnuication_ring3App theApp;