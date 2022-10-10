// D:\_Github\packet_tracer\mfcVS2022Dlg_packer_tracer\jLTracer\ui\CFormDlg_formMoons.cpp: 구현 파일
//

#include "pch.h"
#include "jLTracer.h"
#include "afxdialogex.h"
#include "CFormDlg_formMoons.h"

#include "jLTracerDlg.h"
#include <array>
#include <vector>

// CformMoons 대화 상자
//static void receiveDataCB(void* obj, void* w_parm, void* l_parm);



IMPLEMENT_DYNAMIC(CformMoons, CDialogEx)

CformMoons::CformMoons(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FORM_MOONS, pParent)
	, m_pSystem(nullptr), m_pSerialport(nullptr), m_pParent(nullptr)
	, m_TimerID(0), m_hWind(), m_pComm(nullptr), m_nodeId(0)
	, m_TrdLock(false)
{
	CjLTracerApp* pApp = (CjLTracerApp*)AfxGetApp();
	m_pSystem = pApp->GetSystem();
	m_pSerialport = m_pSystem->GetSerialCommComponent();
	
}

CformMoons::~CformMoons()
{
}

void CformMoons::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


errno_t CformMoons::Init(CformMoons::cfg_t& cfg)
{
	errno_t ret = ERROR_SUCCESS;
	m_nodeId = cfg.AxisId;
	m_pComm = cfg.pComm;

	return ret;
}


BEGIN_MESSAGE_MAP(CformMoons, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CformMoons::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CformMoons 메시지 처리기


void CformMoons::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CformMoons::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CformMoons::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CformMoons::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		m_TimerID = SetTimer(UPDATE_TIMER_MAIN_TAB_MOONS_DLG, 50, NULL);

		
		m_pParent = (CjLTracerDlg*)GetParent()->GetParent();
		m_hWind = m_pParent->GetSafeHwnd();
		m_pSerialport->AttCallbackFunc(this, receiveDataFunc);

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


void CformMoons::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	update();



	CDialogEx::OnTimer(nIDEvent);
}


BOOL CformMoons::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CformMoons::receiveDataFunc(void* obj, void* w_parm, void* l_parm)
{
	// TODO: 여기에 구현 코드 추가.
	CformMoons* pThis = (CformMoons*)obj;
	if (w_parm == nullptr)
		return;
	int length = *((int*)w_parm);

	
	int index = 0;
	for (index = 0; index < length; index++)
	{
		uint8_t data = *((uint8_t*)l_parm + index);

		pThis->m_Que.Put(data);

	}
}


void CformMoons::update()
{
	// TODO: 여기에 구현 코드 추가.
	
	
	if (m_TrdLock == false)
	{
		if (receivePacket() == true)
		{
			receiveCplt();
		}
	}
	
	

}


bool CformMoons::receivePacket()
{
	// TODO: 여기에 구현 코드 추가.
	constexpr int STATE_WAIT_ID = 0;
	constexpr int STATE_WAIT_FUNCNTION = 1;
	constexpr int STATE_WAIT_LENGTH = 2;
	constexpr int STATE_WAIT_REG_ADDR_L = 3;
	constexpr int STATE_WAIT_REG_ADDR_H = 4;
	constexpr int STATE_WAIT_DATA = 5;
	constexpr int STATE_WAIT_CHECKSUM_L = 6;
	constexpr int STATE_WAIT_CHECKSUM_H = 7;



	bool ret = false;
	uint8_t rx_data = 0x00;
	std::array<uint8_t, MOONS_PACKET_BUFF_LENGTH> arry_data{ 0, };
	int data_length = m_Que.Available() % MOONS_PACKET_BUFF_LENGTH;
	if (data_length)
	{
		m_Que.Read(arry_data.data(), data_length);
	}
	else
	{
		return false;
	}


	rx_packet_t* rx_packet = &m_packet.rx_packet;

	if (util::millis() - m_packet.pre_time >= 100)
	{
		m_packet.state = STATE_WAIT_ID;
		m_packet.index = 0;
		m_packet.data_cnt = 0;
		rx_packet->check_sum = 0xffff;
		memset(&rx_packet->buffer[0], 0x00, MOONS_PACKET_BUFF_LENGTH);
	}
	m_packet.pre_time = util::millis();


	for (int i = 0; i < data_length; i++)
	{
		rx_data = arry_data[i];
		switch (m_packet.state)
		{
		case STATE_WAIT_ID:
			rx_packet->node_id = rx_data;
			m_packet.state = STATE_WAIT_FUNCNTION;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			break;

		case STATE_WAIT_FUNCNTION:
			rx_packet->func_type = rx_data;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;

			switch (static_cast<func_e>(rx_data))
			{
			case func_e::read_HoldingReg:
				__fallthrough;
			case func_e::read_InputReg:
				m_packet.state = STATE_WAIT_LENGTH;
				break;

			case func_e::write_SingleReg:  // skip break;
				__fallthrough;
			case func_e::write_MultiReg:   // skip break;
				__fallthrough;
			default:
				m_packet.state = STATE_WAIT_REG_ADDR_L;
				break;
			}
			break;

		case STATE_WAIT_LENGTH:
			rx_packet->data_length = rx_data;
			m_packet.state = STATE_WAIT_DATA;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			break;

		case STATE_WAIT_REG_ADDR_L:
			rx_packet->reg_addr = rx_data;
			m_packet.state = STATE_WAIT_REG_ADDR_H;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			break;

		case STATE_WAIT_REG_ADDR_H:
			rx_packet->reg_addr |= (rx_data << 8);
			rx_packet->data_length = 2;// 0x06, 0x10 경우 2 byte; 
			m_packet.state = STATE_WAIT_DATA;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			break;

		case STATE_WAIT_DATA:
			if (m_packet.data_cnt++ == 0)
			{
				rx_packet->data = &rx_packet->buffer[m_packet.index];
			}
			if (m_packet.data_cnt >= rx_packet->data_length)
			{
				m_packet.state = STATE_WAIT_CHECKSUM_L;
			}
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			util::crc16_modbus_update(&rx_packet->check_sum, rx_data);
			break;


		case STATE_WAIT_CHECKSUM_L:
			rx_packet->check_sum_recv = rx_data;
			m_packet.state = STATE_WAIT_CHECKSUM_H;
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			break;

		case STATE_WAIT_CHECKSUM_H:
			rx_packet->buffer[m_packet.index++ % MOONS_PACKET_BUFF_LENGTH] = rx_data;
			rx_packet->check_sum_recv |= (rx_data << 8);
			if (rx_packet->check_sum == rx_packet->check_sum_recv)
			{
				ret = true;
			}
			m_packet.state = STATE_WAIT_ID;
			break;
		}

	}


	return ret;
}


void CformMoons::receiveCplt()
{
	// TODO: 여기에 구현 코드 추가.
	{//RX
		std::vector<char> msg{ 'R','X' , (char)0 };
		{
			for (uint32_t i = 0; i < m_packet.index; i++)
			{
				msg.push_back(m_packet.rx_packet.buffer[i]);
			}

			int size = (int)msg.size();
			// msg


		}
	}
}


// 데이터 전송
int CformMoons::SendPacket(char** args, uint32_t argc)
{
	// TODO: 여기에 구현 코드 추가.
	int ret = ERROR_SUCCESS;
	std::vector<char*> rev_char(argc);
	std::vector<uint8_t> send_data(argc);
	for (uint32_t i = 0; i < argc; i++)
	{
		rev_char[i] = args[i];
		send_data[i] = (uint8_t)strtol(rev_char[i], nullptr, 16);
	}

	uint8_t idx = 0;
	uint16_t result_crc = 0xFFFF;
	uint16_t ret_crc = (uint16_t)util::crc16_modbus_cal(&send_data[0], (uint32_t)send_data.size());

	send_data.push_back((uint8_t)ret_crc);
	send_data.push_back((uint8_t)(ret_crc >> 8));

	if (this->sendPacketWaitRxResp(&send_data[0], (uint32_t)send_data.size(), 1000) != true)
	{
		ERR_PRINT("fail! moons_motor send packet data");
		ret = -1;
	}


	{ //TX
		std::vector<char> msg{ 'T','X' };
		for (char elm : send_data)
		{
			msg.push_back(elm);
		}

		msg.push_back('\r');
		msg.push_back('\n');
		int size = (int)msg.size();

		for (char elm : msg)
		{
			::PostMessage((HWND)m_hWind, WM_TRACER_MAIN_MESSAGE, 0, (LPARAM)elm);
		}
		/*if (m_cbObj)
		{
			m_func(m_cbObj, &size, &msg[0]);
		}*/
	}

	{//RX
		std::vector<char> msg{ 'R','X' , (char)ret };
		if (ret != ERROR_SUCCESS)
		{
			for (char elm : "wait packet timeout!")
			{
				msg.push_back(elm);
			}

			for (char elm : msg)
			{
				::PostMessage((HWND)m_hWind, WM_TRACER_MAIN_MESSAGE, 0, (LPARAM)elm);
			}
			int size = (int)msg.size();
			/*if (m_cbObj)
			{
				m_func(m_cbObj, &size, &msg[0]);
			}*/
		}
		else
		{
			for (uint32_t i = 0; i < m_packet.index; i++)
			{
				msg.push_back(m_packet.rx_packet.buffer[i]);
			}

			int size = (int)msg.size();
			for (char elm : msg)
			{
				::PostMessage((HWND)m_hWind, WM_TRACER_MAIN_MESSAGE, 0, (LPARAM)elm);
			}
			/*if (m_cbObj)
			{
				m_func(m_cbObj, &size, &msg[0]);
			}*/
		}
	}





	return 0;
}


bool CformMoons::sendPacketWaitRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout)
{
	// TODO: 여기에 구현 코드 추가.
	bool  ret = false;
	uint32_t time_pre;

	m_TrdLock = true;

	m_pComm->SendData((char*)p_data, length);

	time_pre = util::millis();

	while (1)
	{
		if (receivePacket() == true)
		{
			ret = true;
			break;
		}

		if (util::millis() - time_pre >= timeout)
		{
			ERR_PRINT("wait packet timeout!  wait response packet data");
			break;
		}
		Sleep(10);
	}


	m_TrdLock = false;

	return ret;
}
