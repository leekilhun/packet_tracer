#include "pch.h"
#include "hal_def.h"

#include "_inc/netSocket.h"
#include "_inc/SerialComm.h"

#include "_inc/io_fastech.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef USE_HW_IO_FASTECH



ioFastech::ioFastech(ioFastech::cfg_t& cfg) :m_cfg(cfg)
, m_TrdLife(false), m_hThread(), m_TrdId(0), m_receiveData()
{
  m_cfg.p_Comm->AttCallbackFunc(this, receiveDataCB);

  threadRun();
}

ioFastech::~ioFastech()
{
  threadStop();
}

bool ioFastech::receivePacket()
{
  bool ret = false;
  uint8_t rx_data;
  int rx_size = m_Que.GetSize();
  
  enum class state 
  {
    headerAA,
    length,
    syncNo,
    reserved,
    frameType,
    comm_status,
    input_1,
    input_2,
    input_3,
    input_4,
    latch_1,
    latch_2,
    latch_3,
    latch_4,
    _max
  };

  state parsing_state = state::headerAA;

  for (size_t i = 0; i < rx_size; i++)
  {
    m_Que.Get(&rx_data);

    switch (parsing_state)
    {
    case state::headerAA:
      if (rx_data == 0xaa)
      {
        parsing_state = state::length;
      }
      else
      {
        if (rx_data == '\0')
        {
          ret = true;
        }
      }
      break;

    case state::length:
      m_receiveData.length = rx_data;
      parsing_state = state::syncNo;
      break;

    case state::syncNo:
      m_receiveData.syncNo = rx_data;
      parsing_state = state::reserved;
      break;

    case state::reserved:
      m_receiveData.reserved = rx_data;
      parsing_state = state::frameType;
      break;

    case state::frameType:
      m_receiveData.frameType = rx_data;
      parsing_state = state::input_1;
      break;


    case state::comm_status:
      m_receiveData.comm_state = rx_data;
      parsing_state = state::input_1;
      break;

    case state::input_1:
      m_receiveData.input[0] = rx_data;
      parsing_state = state::input_2;
      break;

    case state::input_2:
      m_receiveData.input[1] = rx_data;
      parsing_state = state::input_3;
      break;

    case state::input_3:
      m_receiveData.input[2] = rx_data;
      parsing_state = state::input_4;
      break;

    case state::input_4:
      m_receiveData.input[3] = rx_data;
      parsing_state = state::latch_1;
      break;

    case state::latch_1:
      m_receiveData.latch[0] = rx_data;
      parsing_state = state::latch_2;
      break;

    case state::latch_2:
      m_receiveData.latch[1] = rx_data;
      parsing_state = state::latch_3;
      break;

    case state::latch_3:
      m_receiveData.latch[2] = rx_data;
      parsing_state = state::latch_4;
      break;

    case state::latch_4:
      m_receiveData.latch[3] = rx_data;
      ret = true;
      break;

    default:
      break;
    }


  }



  return ret;
}

void ioFastech::threadStop(void)
{
  m_TrdLife = false;
  while (m_TrdId);
}

void ioFastech::threadRun(void)
{
  m_TrdLife = true;
  // 포트 감시 스레드 생성
  m_hThread = CreateThread(
    (LPSECURITY_ATTRIBUTES)NULL,
    0,
    (LPTHREAD_START_ROUTINE)threadFunc,
    (LPVOID)this,
    0,
    &m_TrdId
  );
}

void ioFastech::threadJob(void)
{
  byte data[5] = { 0xAA, 0x03, 0xE2, 0x00 ,0xC0 };


  if (m_Que.Available())
  {
    receivePacket();
  }
  m_cfg.p_Comm->SendData((char*)data,5);

}

UINT ioFastech::threadFunc(LPVOID pParam)
{
  ioFastech* pThis = (ioFastech*)pParam;

  TRACE("fastech io commucation Thread Start\n\r");

  while (pThis->m_TrdLife)
  {
    pThis->threadJob();

    Sleep(10);
  }

  TRACE("fastech io commucation Thread Terminatet\n\r");
  pThis->m_TrdId = 0;
  pThis->m_hThread = NULL;
  return 0;
}

void ioFastech::receiveDataCB(void* obj, void* w_parm, void* l_parm)
{
  ioFastech* p_this = (ioFastech*)obj;
  if (w_parm == nullptr)
    return;
  int length = *((int*)w_parm);
  int index = 0;
  for (index = 0; index < length; index++)
  {
    uint8_t data = *((uint8_t*)l_parm + index);
    p_this->m_Que.Put(data);
  }
}


#endif