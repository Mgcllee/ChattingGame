
// ChatServerLogViewer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CChatServerLogViewerApp:
// See ChatServerLogViewer.cpp for the implementation of this class
//

class CChatServerLogViewerApp : public CWinApp
{
public:
	CChatServerLogViewerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CChatServerLogViewerApp theApp;
