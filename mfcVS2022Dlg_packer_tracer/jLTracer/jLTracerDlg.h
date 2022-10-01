
// jLTracerDlg.h: 헤더 파일
//

#pragma once


// CjLTracerDlg 대화 상자
class CjLTracerDlg : public CDialogEx
{
	apSystem* m_pSystem;
	SerialComm* m_pSerialport;


	UINT_PTR m_TimerID;

public:
	buffer::_que<uint8_t> m_Que{ 4096 };

// 생성입니다.
public:
	CjLTracerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JLTRACER_DIALOG };
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
	afx_msg void OnDestroy();
private:
	// 화면 정보를 업데이트 한다
	void update();
	void addPortList(CComboBox* combobox);
	void addBaudList(CComboBox* combobox);

public:
	afx_msg void OnBnClickedBtnOpenComm();
	afx_msg void OnBnClickedBtnSendCmd();
	// 수신된 데이터가 표시된다
	CEdit m_rcvData;
};
