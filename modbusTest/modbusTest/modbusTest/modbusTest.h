
// modbusTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmodbusTestApp:
// �йش����ʵ�֣������ modbusTest.cpp
//

class CmodbusTestApp : public CWinApp
{
public:
	CmodbusTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	//void m_com_close(void);
};

extern CmodbusTestApp theApp;