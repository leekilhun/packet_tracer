
// moonsModbusDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "moonsModbus.h"
#include "moonsModbusDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmoonsModbusDlg 대화 상자
//for status bar .... indicator...  
static UINT BASED_CODE indicators[] =
{
	IDS_INDICATOR_COMM_STATUS,
	IDS_INDICATOR_MOTOR_STATUS,
	IDS_INDICATOR_INFOR
};



CmoonsModbusDlg::CmoonsModbusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOONSMODBUS_DIALOG, pParent)
	, m_pMotor(nullptr), m_pSystem(nullptr), m_TimerID{}
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pSystem = theApp.GetAppSystem();
	m_pMotor = m_pSystem->GetMoonsMotorComponent();
}

void CmoonsModbusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MOONS_TXT_INTERVAL, m_txtIntervalTime);
	DDX_Control(pDX, IDC_MOONS_CHK_MONITOR, m_chkMonitor);
	DDX_Control(pDX, IDC_MOONS_TXT_DRIVESTATUS, m_txtDriveStatus);
	DDX_Control(pDX, IDC_MOONS_TXT_ALARMCODE, m_txtAlarmCode);
	DDX_Control(pDX, IDC_MOONS_TXT_ACTUALSPEED, m_txtActualSpeed);
	DDX_Control(pDX, IDC_MOONS_TXT_TARGETSPEED, m_txtTargetSpeed);
	DDX_Control(pDX, IDC_MOONS_TXT_ENCODERPOSITION, m_txtEncoderPos);
	DDX_Control(pDX, IDC_MOONS_EDIT_POSMODE_VELOCITY, m_editVelocity);
	DDX_Control(pDX, IDC_MOONS_EDIT_POSMODE_ACCEL, m_editAccel);
	DDX_Control(pDX, IDC_MOONS_EDIT_POSMODE_DECEL, m_editDecel);
	DDX_Control(pDX, IDC_MOONS_EDIT_POSMODE_ABSMOVE_POS, m_editAbsPosition);
	DDX_Control(pDX, IDC_MOONS_EDIT_POSMODE_RELMOVE_POS, m_editRelPosition);
	DDX_Control(pDX, IDC_MOONS_CMB_POSMOVE_DIR, m_cmbDirection);
}

BEGIN_MESSAGE_MAP(CmoonsModbusDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MOONS_BTN_5, &CmoonsModbusDlg::OnBnClickedMoonsBtn5)
END_MESSAGE_MAP()


// CmoonsModbusDlg 메시지 처리기

BOOL CmoonsModbusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

		// dialog 하단의 status bar 정보를 나타낸다.
	{
		//resource string table
		static constexpr int STATUSBAR_IDX_COMM = 0;
		static constexpr int STATUSBAR_IDX_RESULT = 1;
		static constexpr int STATUSBAR_IDX_VER = 2;

		CRect rect;
		CString cStr;

		m_CStatusBar.Create(this);  //create status bar...  
		m_CStatusBar.SetIndicators(indicators, 3);  //set indicator...  


		//setting status bar....  
		this->GetClientRect(&rect);  //get client rect...  
		m_CStatusBar.SetPaneInfo(STATUSBAR_IDX_COMM, IDS_INDICATOR_COMM_STATUS, SBPS_NORMAL, rect.Width() / 3);
		m_CStatusBar.SetPaneInfo(STATUSBAR_IDX_RESULT, IDS_INDICATOR_MOTOR_STATUS, SBPS_NORMAL, rect.Width() / 3);
		m_CStatusBar.SetPaneInfo(STATUSBAR_IDX_VER, IDS_INDICATOR_INFOR, SBPS_NORMAL, rect.Width() / 3);
		m_CStatusBar.MoveWindow(rect.left, rect.bottom - 25, rect.Width(), 25);

		cStr.Format(_T("Status bar Communication"));
		m_CStatusBar.SetPaneText(STATUSBAR_IDX_COMM, cStr);
		cStr.Format(_T("Status bar Result"));
		m_CStatusBar.SetPaneText(STATUSBAR_IDX_RESULT, cStr);
		cStr.Format(_T("Status bar version"));
		m_CStatusBar.SetPaneText(STATUSBAR_IDX_VER, cStr);

	}
	{
		m_txtIntervalTime.SetWindowText(L"100");
		m_editVelocity.SetWindowText(L"10");
		m_editAccel.SetWindowText(L"100");
		m_editDecel.SetWindowText(L"100");
		m_editAbsPosition.SetWindowText(L"20000");
		m_editRelPosition.SetWindowText(L"20000");
		m_cmbDirection.InsertString(0, L"CW");
		m_cmbDirection.InsertString(1, L"CCW");
		m_cmbDirection.SetCurSel(0);
	}




	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CmoonsModbusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CmoonsModbusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmoonsModbusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();


	CDialogEx::OnTimer(nIDEvent);
}


void CmoonsModbusDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_MAIN_DLG, 50, NULL);

	}
	else
	{
		if (m_TimerID != 0)
		{
			KillTimer(m_TimerID);
			m_TimerID = 0;
		}
	}
}


HBRUSH CmoonsModbusDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL CmoonsModbusDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			break;
		case VK_RETURN:
		{
			//if (pMsg->hwnd == GetDlgItem(IDC_EDIT_TX)->m_hWnd)
			//{
			//	// 전송
			//	OnBnClickedButton2();
			//}
			return TRUE;
		}
		break;
		default:
			break;
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_MOONS_BTN_04)->m_hWnd
			|| pMsg->hwnd == GetDlgItem(IDC_MOONS_BTN_05)->m_hWnd)
		{
			//motorStop();
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


void CmoonsModbusDlg::update()
{
	// TODO: 여기에 구현 코드 추가.
	{
		//status bar
		 
	}
}


void CmoonsModbusDlg::OnBnClickedMoonsBtn5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pMotor->IsMotorOn();
}
