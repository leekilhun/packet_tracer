
// jLTracerDlg.h: 헤더 파일
//

#pragma once
#include "CFormDlg_formMoons.h"
#include "CFormDlg_formN4dio.h"



// CjLTracerDlg 대화 상자
class CjLTracerDlg : public CDialogEx
{
	apSystem* m_pSystem;

	SerialComm* m_pSerialport;
	CformMoons* m_ptabMoons;
	CformN4dio* m_ptabN4dio;
	CStatusBar		m_CStatusBar;

	CString m_Port;

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
	//bool receivePacket();
	//void receiveCplt();
	void update();
	void addPortList(CComboBox* combobox);
	void addBaudList(CComboBox* combobox);

public:
	afx_msg void OnBnClickedBtnOpenComm();
	afx_msg void OnBnClickedBtnSendCmd();
	// 수신된 데이터가 표시된다
	CEdit m_sendData;
	// tap control base
	CTabCtrl m_tabBase;
	afx_msg void OnTcnSelchangeTabBase(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CComboBox m_SelNodeId;
	// serial port no
	CComboBox m_SelPortNo;
	// serial comm baudrate
	CComboBox m_SelBaudrate;
private:
	void addNodeId(CComboBox* combobox);
	// 노드 번호
	int m_NodeId;
	// 시리얼 포트 번호
	int m_PortNo;
	// serial baudrate
	int m_Baudrate;
public:
	// 통신 오픈 또는 클로즈
	CButton m_btnCommOpenCloase;
private:
	bool parsingRx();
	// 수신 데이터 뷰
	CEdit m_editView;
	// 송신시 로그
	CListBox m_listLog;
public:
	afx_msg void OnBnClickedButton2();
protected:
	afx_msg LRESULT OnPostmsg(WPARAM wParam, LPARAM lParam);
};
