
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

static void callbackFunc(void* obj, void* w_parm, void* l_parm);

// CjLTracerDlg 대화 상자
#define UPDATE_TIMER_MAIN_DLG  1
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

/*
#define COMM_RS232_BAUD_LIST_MAX	8
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

*/


CjLTracerDlg::CjLTracerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JLTRACER_DIALOG, pParent)
	,m_pSystem(nullptr), m_TimerID(), m_pSerialport(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CjLTracerApp* pApp = (CjLTracerApp*)AfxGetApp();
	m_pSystem = pApp->GetSystem();
	m_pSerialport = m_pSystem->GetSerialCommComponent();
	/*pPrarent가 null임*/
	//m_pSystem = ((CjLTracerApp*)((CWinApp*)pParent))->GetSystem();

	m_pSerialport->AttCallbackFunc(this, callbackFunc);


}

void CjLTracerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RCV_DATA, m_rcvData);
}

BEGIN_MESSAGE_MAP(CjLTracerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OPEN_COMM, &CjLTracerDlg::OnBnClickedBtnOpenComm)
	ON_BN_CLICKED(IDC_BTN_SEND_CMD, &CjLTracerDlg::OnBnClickedBtnSendCmd)
END_MESSAGE_MAP()


// CjLTracerDlg 메시지 처리기

BOOL CjLTracerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//addPortList(((CComboBox*)GetDlgItem(IDC_COMBO_PORT)));
	//addBaudList(((CComboBox*)GetDlgItem(IDC_COMBO_BAUD)));


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
}


void CjLTracerDlg::receiveCplt()
{
}

// 화면 정보를 업데이트 한다
void CjLTracerDlg::update()
{
	// TODO: 여기에 구현 코드 추가.
	if (receivePacket() == true)
	{
		receiveCplt();
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
		index++;

		//szName-레지터스터 항목의 이름  
		//dwType-항목의 타입, 여기에서는 널로 끝나는 문자열  
		//szData-항목값이 저장될 배열  
		//dwSize2-배열의 크기  
		RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
		combobox->AddString(CString(szData));

		memset(szData, 0x00, sizeof(szData));
		memset(szName, 0x00, sizeof(szName));
		dwSize = 100;
		dwSize2 = 20;
	}

	RegCloseKey(hKey);
}

void CjLTracerDlg::addBaudList(CComboBox* combobox)
{

#if 0
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
	combobox->SetCurSel(index - 1);
#endif
}


void CjLTracerDlg::OnBnClickedBtnOpenComm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	m_pSerialport->Open();

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


#if 1
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
