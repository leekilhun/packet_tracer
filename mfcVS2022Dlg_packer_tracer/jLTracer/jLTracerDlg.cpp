
// jLTracerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "jLTracer.h"
#include "jLTracerDlg.h"
#include "afxdialogex.h"


#include  <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//for status bar .... indicator...  
static UINT BASED_CODE indicators[] =
{
	IDS_INDICATOR_COMM_STATUS,
	IDS_INDICATOR_MOTOR_STATUS,
	IDS_INDICATOR_INFOR
};

static constexpr int STATUSBAR_IDX_COMM = 0;
static constexpr int STATUSBAR_IDX_RESULT = 1;

/*

다이얼로그 창 크기 변경 못하도록 하기 테두리 옵셥을 Resizing -> Thin 하니 리사이즈가 안된다!!


*/

//static void callbackFunc(void* obj, void* w_parm, void* l_parm);


// CjLTracerDlg 대화 상자

#if 0
#define PACKET_BUFF_LENGTH     100

struct modbus_packet_t {
	uint8_t   dev_id{};
	uint8_t   func_type{};
	uint8_t   error{};
	uint16_t reg_addr{};
	uint16_t  length{};
	uint16_t   check_sum{};
	uint16_t   check_sum_recv{};
	uint8_t   buffer[PACKET_BUFF_LENGTH]{};
	uint8_t* data{};
};

struct modbus_t {
	uint8_t   state{};
	uint32_t  pre_time{};
	uint32_t  index{};
	uint32_t  data_cnt{};
	uint8_t   error{};

	modbus_packet_t  rx_packet;
	modbus_packet_t  tx_packet;

};


modbus_t packet;

#endif



CjLTracerDlg::CjLTracerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JLTRACER_DIALOG, pParent)
	,m_pSystem(nullptr), m_TimerID(), m_pSerialport(nullptr)
	,m_ptabMoons(nullptr), m_ptabN4dio(nullptr)
	,m_Baudrate(0),m_PortNo(0),m_NodeId(0)
{	 
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CjLTracerApp* pApp = (CjLTracerApp*)AfxGetApp();
	m_pSystem = pApp->GetSystem();
	m_pSerialport = m_pSystem->GetSerialCommComponent();
	/*pPrarent가 null임*/
	//m_pSystem = ((CjLTracerApp*)((CWinApp*)pParent))->GetSystem();



}

void CjLTracerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TX, m_sendData);
	DDX_Control(pDX, IDC_TAB_BASE, m_tabBase);
	DDX_Control(pDX, IDC_COMBO_NODE_ID, m_SelNodeId);
	DDX_Control(pDX, IDC_COMBO_PORT, m_SelPortNo);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_SelBaudrate);
	DDX_Control(pDX, IDC_BTN_OPEN_COMM, m_btnCommOpenCloase);
	DDX_Control(pDX, IDC_EDIT_VIEW, m_editView);
	DDX_Control(pDX, IDC_LIST_RECEI_PACK, m_listLog);
}

BEGIN_MESSAGE_MAP(CjLTracerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OPEN_COMM, &CjLTracerDlg::OnBnClickedBtnOpenComm)
	ON_BN_CLICKED(IDC_BTN_SEND_CMD, &CjLTracerDlg::OnBnClickedBtnSendCmd)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_BASE, &CjLTracerDlg::OnTcnSelchangeTabBase)
	ON_BN_CLICKED(IDC_BUTTON2, &CjLTracerDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_TRACER_MAIN_MESSAGE, &CjLTracerDlg::OnPostmsg)
END_MESSAGE_MAP()


// CjLTracerDlg 메시지 처리기

BOOL CjLTracerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	// dialog 하단의 status bar 정보를 나타낸다.
	{    
		//resource string table

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

  addNodeId(&m_SelNodeId);
  addPortList(&m_SelPortNo);
  addBaudList(&m_SelBaudrate);
	//addNodeId(((CComboBox*)GetDlgItem(IDC_COMBO_NODE_ID)));
  //addPortList(((CComboBox*)GetDlgItem(IDC_COMBO_PORT)));
  //addBaudList(((CComboBox*)GetDlgItem(IDC_COMBO_BAUD)));
	
	int idx = 0;
	m_tabBase.DeleteAllItems();
	CImageList img_list;
	img_list.Create(IDB_BITMAP_TABMAIN, 16, 5, RGB(255, 255, 255));

	//CBitmap bm;
	//bm.LoadBitmap(IDB_BITMAP2);
	//img_list.Add(&bm, RGB(255, 255, 255));

	m_tabBase.SetImageList(&img_list);
	img_list.Detach();


	m_tabBase.InsertItem(idx++, L"Moons Motor", 0);
	m_tabBase.InsertItem(idx++, L"NDIOG08 ", 0);

	CRect rec_item;
	m_tabBase.GetWindowRect(&rec_item);

	m_tabBase.GetItemRect(0, &rec_item);
	m_ptabMoons = new CformMoons;
	m_ptabMoons->Create(IDD_FORM_MOONS, &m_tabBase);
	m_ptabMoons->GetWindowRect(&rec_item);
	m_ptabMoons->MoveWindow(5, 25, rec_item.Width(), rec_item.Height());
	m_ptabMoons->ShowWindow(SW_SHOW);

	m_ptabN4dio = new CformN4dio;
	m_ptabN4dio->Create(IDD_FORM_N4DIO, &m_tabBase);
	m_ptabN4dio->GetWindowRect(&rec_item);
	m_ptabN4dio->MoveWindow(5, 25, rec_item.Width(), rec_item.Height());
	m_ptabN4dio->ShowWindow(SW_HIDE);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CjLTracerDlg::OnPaint()
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
HCURSOR CjLTracerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CjLTracerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();


	CDialogEx::OnTimer(nIDEvent);
}


void CjLTracerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_MAIN_DLG, 50, NULL);
		m_ptabMoons->ShowWindow(SW_SHOW);
		m_ptabN4dio->ShowWindow(SW_HIDE);
		CformMoons::cfg_t cfg;
		m_NodeId = (int)m_SelNodeId.GetItemData(m_SelNodeId.GetCurSel());//(int)GetDlgItemInt(IDC_COMBO_NODE_ID);

		cfg.AxisId = m_NodeId;
		cfg.pComm = m_pSerialport;
		m_ptabMoons->Init(cfg);
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


void CjLTracerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ptabMoons)
	{
		delete m_ptabMoons;
	}
	m_ptabMoons = nullptr;

	if (m_ptabN4dio)
	{
		delete m_ptabN4dio;
	}
	m_ptabN4dio = nullptr;

}



// 화면 정보를 업데이트 한다
void CjLTracerDlg::update()
{
	// TODO: 여기에 구현 코드 추가.
	
	if (parsingRx())
	{

	}




	/* Set Status Bar information */
	{
		m_NodeId = (int)m_SelNodeId.GetItemData(m_SelNodeId.GetCurSel());//(int)GetDlgItemInt(IDC_COMBO_NODE_ID);
		//m_PortNo = ((CComboBox*)GetDlgItem(IDC_COMBO_PORT))->GetItemData(((CComboBox*)GetDlgItem(IDC_COMBO_PORT))->GetCurSel());
		m_PortNo = (int)m_SelPortNo.GetItemData(m_SelPortNo.GetCurSel());
		m_Baudrate = (int)m_SelBaudrate.GetItemData(m_SelBaudrate.GetCurSel());//(int)GetDlgItemInt(IDC_COMBO_BAUD);

		CString str, port_name, baudrate;
		int length;
		length = m_SelPortNo.GetLBTextLen(m_SelPortNo.GetCurSel());
		m_SelPortNo.GetLBText(m_SelPortNo.GetCurSel(), port_name.GetBuffer(length));
		port_name.ReleaseBuffer();
		m_Port = port_name;
		length = m_SelBaudrate.GetLBTextLen(m_SelBaudrate.GetCurSel());
		m_SelBaudrate.GetLBText(m_SelBaudrate.GetCurSel(), baudrate.GetBuffer(length));
		baudrate.ReleaseBuffer();
		//m_SelPortNo.GetDlgItemText(m_SelPortNo.GetCurSel(), port_name);
		//m_SelBaudrate.GetDlgItemText(m_SelBaudrate.GetCurSel(), baudrate);

		if (m_pSerialport->IsOpened())
		{
			//str = L"Serial Comm Opened";
			str.Format(L"%s baudrate %s Opened", port_name.GetBuffer(), baudrate.GetBuffer());
			port_name.ReleaseBuffer();
			baudrate.ReleaseBuffer();
			m_btnCommOpenCloase.SetWindowText(L"Opened");
		}
		else
		{
			str = L"Serial Comm Closed ";
			m_btnCommOpenCloase.SetWindowText(L"Closed");
		}

		m_CStatusBar.SetPaneText(STATUSBAR_IDX_COMM, str);

	}
	
}

void CjLTracerDlg::addPortList(CComboBox* combobox)
{
	if (combobox == NULL) return;

	//HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM 에  시리얼포트 번호들이 등록되어 있음.  
	//레지스터 읽어오면 됨. 코드는 아래 링크에서 가져옴  
	HKEY hKey;

	//오픈할 레지스터 키에 대한 기본키 이름  
	//오픈할 레지스터 서브키 이름  
	//레지스터키에 대한 핸들  
	RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

	TCHAR szData[20] = { 0, }, szName[100] = { 0, };
	DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
	combobox->ResetContent();


	//LPCTSTR sData, sName;

	//hKey - 레지스터키 핸들  
	//index - 값을 가져올 인덱스.. 다수의 값이 있을 경우 필요  
	//szName - 항목값이 저장될 배열  
	//dwSize - 배열의 크기  
	while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL))
	{
		//szName-레지터스터 항목의 이름  
		//dwType-항목의 타입, 여기에서는 널로 끝나는 문자열  
		//szData-항목값이 저장될 배열  
		//dwSize2-배열의 크기  
		RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
		CString str_port = CString(szData);
		combobox->AddString(str_port);
		str_port.Replace(L"COM", L"");
		DWORD data = _ttoi(str_port);
		combobox->SetItemData(index, data);
		memset(szData, 0x00, sizeof(szData));
		memset(szName, 0x00, sizeof(szName));
		dwSize = 100;
		dwSize2 = 20;

		index++;
	}

	combobox->SetCurSel(0);

	RegCloseKey(hKey);
}

void CjLTracerDlg::addBaudList(CComboBox* combobox)
{

	constexpr int COMM_RS232_BAUD_LIST_MAX = 8;
	struct sRs232_baudrate_tbl { DWORD baudrate_data; LPCTSTR label; }
	RS_232_BAUDRATE_DATA[COMM_RS232_BAUD_LIST_MAX] =
	{
		{CBR_9600,		_T("9600")},
		{CBR_14400,		_T("14400")},
		{CBR_19200,		_T("19200")},
		{CBR_38400,		_T("38400")},
		{CBR_56000,		_T("56000")},
		{CBR_115200,	_T("115200")},
		{CBR_128000,	_T("128000")},
		{CBR_256000,	_T("256000")},
};

	if (combobox == NULL) return;
	combobox->ResetContent();

	int index = 0;
	while (RS_232_BAUDRATE_DATA)
	{
		if (index >= COMM_RS232_BAUD_LIST_MAX) break;
		combobox->AddString(RS_232_BAUDRATE_DATA[index].label);
		combobox->SetItemData(index, RS_232_BAUDRATE_DATA[index].baudrate_data);
		index++;
	}
	combobox->SetCurSel(0);

}

void CjLTracerDlg::addNodeId(CComboBox* combobox)
{
	// TODO: 여기에 구현 코드 추가.
  if (combobox == NULL) return;
  combobox->ResetContent();

	for (int i = 0; i <= 32; i++)
	{
		CString str;
		str.Format(L"%d", i);
		combobox->AddString(str);
		combobox->SetItemData(i, i);
	}

	combobox->SetCurSel(1);

}

void CjLTracerDlg::OnBnClickedBtnOpenComm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		
	if (m_pSerialport->IsOpened())
	{
		m_pSerialport->Close();
	}
	else
	{
		std::array<char, 10> ret_char;
		trans::TcharToLPSTR(m_Port.GetBuffer(), ret_char.data(), (int)ret_char.size());
		
		m_pSerialport->PortOpen(ret_char.data(), m_Baudrate);
	}

}


void CjLTracerDlg::OnBnClickedBtnSendCmd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//std::array<uint8_t, 8> get_input{ 0x01, 0x03, 0x00, 0x81, 0x00, 0x08, 0x14, 0x24 };//01 03 00 81 00 08 14 24




	std::array<uint8_t, 8> send_data{ 0x00, };
	uint8_t idx = 0;
	uint16_t result_crc = 0xFFFF;

#if 0 // case 1

	packet.tx_packet.dev_id = 0x01;
	send_data[idx++] = packet.tx_packet.dev_id;
	packet.tx_packet.func_type = 0x03;
	send_data[idx++] = packet.tx_packet.func_type;
	packet.tx_packet.reg_addr = 0x00;
	send_data[idx++] = 0x00;
	packet.tx_packet.reg_addr |= (0x81 << 8);
	send_data[idx++] = 0x81;
	packet.tx_packet.length = 0x00;
	send_data[idx++] = 0x00; 
	packet.tx_packet.length |= (0x08 << 8);
	send_data[idx++] = 0x08;
	uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(send_data.data(), idx);
	send_data[idx++] = (uint8_t)ret_crc;
	send_data[idx++] = (uint8_t)(ret_crc>>8);
#endif


#if 0
	packet.tx_packet.dev_id = 0x01;
	packet.tx_packet.func_type = 0x03;
	packet.tx_packet.reg_addr = 0x0081;
	packet.tx_packet.length = 0x0008;

	uint8_t data = 0;
	data = packet.tx_packet.dev_id;
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	data = packet.tx_packet.func_type;
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	data = (uint8_t)(packet.tx_packet.reg_addr >> 8);
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	data = (uint8_t)packet.tx_packet.reg_addr;
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	data = (uint8_t)(packet.tx_packet.length >> 8);
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	data = (uint8_t)packet.tx_packet.length;
	send_data[idx++] = data;
	util::crc16_modbus_update(&result_crc, data);

	send_data[idx++] = (uint8_t)result_crc;
	send_data[idx++] = (uint8_t)(result_crc >> 8);

#endif

#if 0

	int ret_crc = util::crc16_modbus_cal(test_crc.data(), (uint16_t)test_crc.size());

	/*uint16_t result_crc = 0xFFFF;
	for (uint8_t data : test_crc)
	{
		util::crc16_modbus_update(&result_crc, data);
	}*/
	//uint16_t result_crc = 0xFFFF;
	//for (uint8_t data : test_crc)
	//{
	//	util::crc16_update(&result_crc, data);
	//}

#endif 

	int cplt_size = m_pSerialport->SendData((char*)send_data.data(), (uint32_t)send_data.size());
	if (cplt_size == 0)
	{
		AfxMessageBox(L"fail! send data");
	}


 }




#if 0



void CjLTracerDlg::receiveCplt()
{
}








void callbackFunc(void* obj, void* w_parm, void* l_parm)
{
	CjLTracerDlg* pThis = (CjLTracerDlg*)obj;
	if (w_parm == nullptr)
		return;
	int length = *((int*)w_parm);
	int index = 0;
	for (index = 0; index < length; index++)
	{
		uint8_t data = *((uint8_t*)l_parm + index);
		//pThis->m_rcvData.SetWindowText(L"dfd");
		pThis->m_Que.Put(data);

	}
}




bool CjLTracerDlg::receivePacket()
{
	// TODO: 여기에 구현 코드 추가.
	constexpr int STATE_WAIT_ID					= 0;
	constexpr int STATE_WAIT_FUNCNTION	= 1;
	constexpr int STATE_WAIT_LENGTH			= 2;
	constexpr int STATE_WAIT_DATA				= 3;
	constexpr int STATE_WAIT_CHECKSUM_L = 4;
	constexpr int STATE_WAIT_CHECKSUM_H = 5;



	bool ret = false;
  uint8_t rx_data = 0x00;
  std::array<uint8_t, PACKET_BUFF_LENGTH> arry_data{ 0, };
  int data_length = m_Que.Available();
  if (data_length)
  {
		m_Que.Read(arry_data.data(), data_length);
	}
	else
	{
		return false;
	}

	modbus_packet_t* rx_packet = &packet.rx_packet;
	if (util::millis() - packet.pre_time >= 100)
	{
		packet.state = STATE_WAIT_ID;
		packet.index = 0;
		packet.data_cnt = 0;
		rx_packet->check_sum = 0xffff;
		memset(&rx_packet->buffer[0], 0x00, PACKET_BUFF_LENGTH);
	}
	packet.pre_time = util::millis();

	

	for (int i = 0; i < data_length; i++)
	{
		rx_data = arry_data[i];

		switch (packet.state)
		{
		case STATE_WAIT_ID:
			rx_packet->dev_id = rx_data;
			packet.state = STATE_WAIT_FUNCNTION;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[packet.index++] = rx_data;
			break;

		case STATE_WAIT_FUNCNTION:
			rx_packet->func_type = rx_data;
			packet.state = STATE_WAIT_LENGTH;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[packet.index++] = rx_data;
			break;

		case STATE_WAIT_LENGTH:
			rx_packet->length = rx_data;
			packet.state = STATE_WAIT_DATA;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[packet.index++] = rx_data;
			break;

		case STATE_WAIT_DATA:
			if (packet.data_cnt++ == 0)
			{
				rx_packet->data = &rx_packet->buffer[STATE_WAIT_DATA];
			}
			if (packet.data_cnt == rx_packet->length)
			{
				packet.state = STATE_WAIT_CHECKSUM_L;
			}
			rx_packet->buffer[packet.index++] = rx_data;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			break;


		case STATE_WAIT_CHECKSUM_L:
			rx_packet->check_sum_recv = rx_data;
			packet.state = STATE_WAIT_CHECKSUM_H;
			rx_packet->buffer[packet.index++] = rx_data;
			break;

		case STATE_WAIT_CHECKSUM_H:
			rx_packet->buffer[packet.index++] = rx_data;
			rx_packet->check_sum_recv |= (rx_data << 8);
			if (rx_packet->check_sum == rx_packet->check_sum_recv)
			{
				ret = true;
			}
			packet.state = STATE_WAIT_ID;
			break;
		}

	}


	



	return ret;
}


#endif

void CjLTracerDlg::OnTcnSelchangeTabBase(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int curr_tab = this->m_tabBase.GetCurSel();
	if (curr_tab == 0)
	{
		m_ptabMoons->ShowWindow(SW_SHOW);
		m_ptabN4dio->ShowWindow(SW_HIDE);
		CformMoons::cfg_t cfg;
		cfg.AxisId = m_NodeId;
		cfg.pComm = m_pSerialport;
		m_ptabMoons->Init(cfg);

		//
	}
	else if (curr_tab == 1)
	{
		m_ptabMoons->ShowWindow(SW_HIDE);
		m_ptabN4dio->ShowWindow(SW_SHOW);
	}

}


BOOL CjLTracerDlg::PreTranslateMessage(MSG* pMsg)
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
			if (pMsg->hwnd == GetDlgItem(IDC_EDIT_TX)->m_hWnd)
			{
				// 전송
				OnBnClickedButton2();
			}
			return true;
		}
		break;
		default:
			break;
		}
	}

	

	return CDialogEx::PreTranslateMessage(pMsg);
}




bool CjLTracerDlg::parsingRx()
{
	// TODO: 여기에 구현 코드 추가.
	bool ret = false;

	constexpr uint8_t state_wait_head = 0;
	constexpr uint8_t state_wait_tx = 1;
	constexpr uint8_t state_wait_rx = 2;
	constexpr uint8_t state_wait_data = 3;

	constexpr uint8_t step_wait_id = 0;
	constexpr uint8_t step_wait_func = 1;
	constexpr uint8_t step_wait_length = 2;
	constexpr uint8_t step_wait_reg_l = 3;
	constexpr uint8_t step_wait_reg_2 = 4;
	constexpr uint8_t step_wait_data = 5;

	constexpr uint8_t func_read = 0x03;
	constexpr uint8_t func_write = 0x06;

	constexpr int register_base = 40001;


	int length = m_Que.Available();
	uint8_t data = 0;
	uint8_t state = 0;
	struct receive_t
	{
		uint8_t step = 0;
		uint8_t length = 0;
		uint32_t value = 0;
		CString h_str;
		std::vector <uint8_t> data;
		std::vector<CString> str;
	};


	receive_t rec_data;
	CString str;
	std::vector<CString> contents;

	bool is_fail = true;
	bool is_rx = false;
	std::array<char, 40> datetime{ 0, };
	while (m_Que.Available())
	{
		m_Que.Get(&data);
		switch (state)
		{
		case state_wait_head:
			is_rx = false;
			if (data == 'T')
			{
				state = state_wait_tx;
			}
			else if (data == 'R')
			{
				state = state_wait_rx;
			}

			break;

		case state_wait_tx:
			if (data == 'X')
			{
				//char datetime[40] = { 0, };
				is_fail = false;
				trans::DateFormStr(datetime.data(), (int)datetime.size(), trans::DataTimeFormat_e::YYYYMMDD_HHMMSS_UU);
				str = trans::CharToCString(datetime.data());
				str.AppendFormat(L" [PC -→> DRIVER] PACKET : ");
				state = state_wait_data;
			}
			break;
		case state_wait_rx:
			if (data == 'X')
			{
				is_rx = true;
				m_Que.Get(&data);
				if (data == ERROR_SUCCESS)
				{
					is_fail = false;
				}
				//char datetime[40] = { 0, };
				trans::DateFormStr(datetime.data(), (int)datetime.size(), trans::DataTimeFormat_e::YYYYMMDD_HHMMSS_UU);
				str = trans::CharToCString(datetime.data());
				str.AppendFormat(L" [PC <←- DRIVER] PACKET : ");
				state = state_wait_data;
				rec_data.step = 0;
			}
			break;
		case state_wait_data:
		{
			uint8_t pre_data = data;
			if (data == '\r')
			{
				m_Que.Get(&data);
				if (data == '\n')
				{
					if (m_listLog.GetCount() == 10)
					{
						m_listLog.ResetContent();
					}
					m_listLog.AddString(str);
					state = state_wait_head;
					is_fail = true;
				}
				else
				{
					str.AppendFormat(L"%02X ", pre_data);
					str.AppendFormat(L"%02X ", data);
				}
			}
			else
			{
				if (is_fail)
				{
					str.AppendFormat(L"%c", data);
					ret = true;
				}
				else
				{
					str.AppendFormat(L"%02X ", data);

					/* 수신 데이터를 표시해 준다*/
					if (is_rx)
					{
						CString content;
						bool is_read_func = false;
						switch (rec_data.step)
						{
						case step_wait_id:
							content.Format(L"%02X ", data);
							contents.push_back(L"node id    : 0x" + content + L"\r\n");
							rec_data.step = step_wait_func;
							break;

						case step_wait_func:
							content.Format(L"%02X ", data);
							contents.push_back(L"func code : 0x" + content + L"\r\n");
							is_read_func = data < func_write;
							if (is_read_func)
							{
								rec_data.step = step_wait_length;
							}
							else
							{
								rec_data.length = 2;
								rec_data.step = step_wait_reg_l;
							}

							break;

						case step_wait_length:
							content.Format(L"%d (reg cnt %d) ", data, (data / 2));
							contents.push_back(L"length      : " + content + L" \r\n");
							rec_data.length = data;
							rec_data.step = step_wait_data;
							rec_data.data.clear();
							rec_data.value = 0;
							break;

						case step_wait_reg_l:
							rec_data.data.push_back(data);
							rec_data.step = step_wait_reg_2;
							break;

						case step_wait_reg_2:
						{
							rec_data.data.push_back(data);
							uint16_t value = 0;
							value = (uint16_t)rec_data.data[0] << 0;
							value |= (uint16_t)rec_data.data[1] << 8;
							rec_data.h_str.Format(L"%04X ", value);
							content.Format(L"%d [0x%s]", (value), rec_data.h_str.GetBuffer());
							contents.push_back(L"register     : " + content + L"\r\n");
							rec_data.step = step_wait_data;
							rec_data.data.clear();
							rec_data.value = 0;
							rec_data.h_str.Empty();
						}
						break;

						case step_wait_data:  // 데이터 기본 데이터 크기 short (16비트) 
							rec_data.data.push_back(data);
							if ((rec_data.data.size() % 2) == 0)//짝수번째 데이터
							{
								uint16_t value = 0;
								value = (uint16_t)rec_data.data[rec_data.data.size() - 1] << 0;
								value |= (uint16_t)rec_data.data[rec_data.data.size() - 2] << 8;
								rec_data.h_str.AppendFormat(L"     %06d [0x%04X]\r\n", value, value);
							}


							//rec_data.h_str.AppendFormat(L"%02X", data);
							rec_data.value |= (uint32_t)data << (8 * (rec_data.data.size() - 1));
							if (rec_data.data.size() == rec_data.length)
							{
								rec_data.step = step_wait_id;
								//content.Format(L"%d [0x%s]", (rec_data.value), rec_data.h_str.GetBuffer());
								contents.push_back(L"data         : \r\n" + rec_data.h_str);
								CString add_str;
								for (CString elm : contents)
								{
									add_str.Append(elm);
								}
								m_editView.SetWindowText(add_str);
								ret = true;
							}
							//rec_data.step = state_wait_reg_2;
							break;


						default:
							break;
						}
					} // is_rx

					/* 수신 데이터 표시*/
				}

			}
		}
		break;

		default:
			break;
		}
	}



	if (ret/*length > 0*/)
	{

		if (m_listLog.GetCount() == 10)
		{
			m_listLog.ResetContent();
		}
		m_listLog.AddString(str);

		//m_editTX.SetWindowText(str);
	}

	
	
	
	return ret;
}


void CjLTracerDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	constexpr int array_max = 100;
	constexpr int packet_max = 20;
	CString str;
	m_sendData.GetWindowText(str);
	std::array<char, array_max> c_str;
	std::array<char*, packet_max> p_args;
	trans::TcharToLPSTR(str.GetBuffer(), &c_str[0], str.GetLength());
	uint8_t cnt = trans::SplitArgs(&c_str[0], &p_args[0], " ", packet_max);
	
	m_ptabMoons->SendPacket(&p_args[0], cnt);
}


afx_msg LRESULT CjLTracerDlg::OnPostmsg(WPARAM wParam, LPARAM lParam)
{
	int i = 0;

	int length = (int)wParam;
  uint8_t data = (uint8_t)lParam;
	m_Que.Put(data);

	return 0;
}
