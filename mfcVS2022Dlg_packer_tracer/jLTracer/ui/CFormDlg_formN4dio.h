#pragma once
#include "afxdialogex.h"


// CformN4dio 대화 상자

class CformN4dio : public CDialogEx
{
	DECLARE_DYNAMIC(CformN4dio)

public:
	CformN4dio(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CformN4dio();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM_N4DIO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
