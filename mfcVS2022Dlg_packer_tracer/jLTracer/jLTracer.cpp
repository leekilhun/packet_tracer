
// jLTracer.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
//#include "framework.h"
#include "jLTracer.h"
#include "jLTracerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/****************************************************
 전역 함수
 ****************************************************/
bool g_TrdLife = false;
DWORD g_TrdId = 0;
HANDLE g_hThread = nullptr;


UINT threadFunc(LPVOID pParam)
{
	LTRACE("app thread start\n\r");
	while (g_TrdLife)
	{
		cliMain();
		Sleep(5);
	}

	LTRACE("app thread stop\n\r");
	g_TrdId = 0;
	g_hThread = nullptr;
	return 0;
}

void app_threadStop(void)
{
	g_TrdLife = false;
	while (g_TrdId);

}

void app_threadRun(void)
{
	g_TrdLife = true;
	// 포트 감시 스레드 생성
	g_hThread = CreateThread(
		(LPSECURITY_ATTRIBUTES)NULL,
		0,
		(LPTHREAD_START_ROUTINE)threadFunc,
		(LPVOID)nullptr,
		0,
		&g_TrdId
	);

}

/*****************************************************/

// CjLTracerApp

BEGIN_MESSAGE_MAP(CjLTracerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CjLTracerApp 생성


	

CjLTracerApp::CjLTracerApp(): m_pSystem(new apSystem())
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CjLTracerApp 개체입니다.

CjLTracerApp theApp;


// CjLTracerApp 초기화

BOOL CjLTracerApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	app_threadRun();

	syslog_Init();

	m_pSystem->Initialize();

	CjLTracerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}





	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}



int CjLTracerApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	app_threadStop();

	if (m_pSystem)
	{
		delete m_pSystem;
	}
	m_pSystem = nullptr;
	return CWinApp::ExitInstance();
}
