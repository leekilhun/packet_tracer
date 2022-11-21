
// moonsModbusDlg.h: 헤더 파일
//

#pragma once


// CmoonsModbusDlg 대화 상자
class CmoonsModbusDlg : public CDialogEx
{

	AP_SYS::GMoons_motor* m_pMotor;
	AP_SYS::MSystem* m_pSystem;
	UINT_PTR m_TimerID;
	CStatusBar		m_CStatusBar;

	CEdit m_txtIntervalTime;
	CButton m_chkMonitor;
	CEdit m_txtDriveStatus;
	CEdit m_txtAlarmCode;
	CEdit m_txtActualSpeed;
	CEdit m_txtTargetSpeed;
	CEdit m_txtEncoderPos;
	CEdit m_editVelocity;
	CEdit m_editAccel;
	CEdit m_editDecel;
	CEdit m_editAbsPosition;
	CEdit m_editRelPosition;
	CComboBox m_cmbDirection;

// 생성입니다.
public:
	CmoonsModbusDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOONSMODBUS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void update();
public:
	afx_msg void OnBnClickedMoonsBtn5();
};
