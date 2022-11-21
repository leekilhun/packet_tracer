#include "pch.h"
#include "hal_def.h"

#include "SerialComm.h"
#include <mutex>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef USE_HW_SERIAL_COMM



GSerialComm::GSerialComm(common::Attribute_t common_data, GSerialComm::cfg_t& cfg)
  :Icommon(common_data), m_received_data(0), m_cbObj(nullptr) , m_func()
{
  m_Serialcfg = cfg;
  m_TrdLife = false;
  m_IsOpened = false;
  m_hThread = {};
  m_TrdId = 0;

  //SerialComport::cfg_t comm_cfg = { 0, };
  //comm_cfg.SetPortName(cfg->GetPortName());
  //comm_cfg.baud = cfg->baudrate;
  //
  ///*hw serial port를 생성*/
  //m_pSerial = new SerialComport(&comm_cfg);
}

GSerialComm::~GSerialComm()
{
  //if (m_pSerial) delete m_pSerial; m_pSerial = nullptr;
  Close();
  m_TrdLife = false;
  m_IsOpened = false;
  m_hThread = {};
  m_TrdId = 0;
}

//errno_t SerialComm::PortOpen(char* port_name, uint32_t baud)
//{
//  m_Serialcfg.SetPortName(port_name);
//  m_Serialcfg.baudrate = baud;
//  return openPort(m_Serialcfg.GetPortName(), m_Serialcfg.baudrate);;
//}

errno_t GSerialComm::Open()
{
  /*data 수신을 위한 콜백함수 등록*/
  //m_pSerial->AttCallbackFunc(this, cbFunc);
  //m_pSerial->Open();
  return openPort(m_Serialcfg.GetPortName(), m_Serialcfg.baudrate);
}
void GSerialComm::Close()
{
  if (m_Serialcfg.serial_handle == nullptr)
    return;
  m_TrdLife = false;
  while (m_TrdId);
  closePort();

}
void GSerialComm::Recovery()
{
}
bool GSerialComm::IsOpened() const
{
  return m_IsOpened;
}


uint8_t GSerialComm::Read()
{
  return m_received_data;
}

uint32_t GSerialComm::Write(uint8_t* p_data, uint32_t length)
{
  uint32_t ret = 0;
  DWORD dwWrite = 0;

  if (WriteFile(m_Serialcfg.serial_handle, p_data, (DWORD)length, &dwWrite, NULL) == FALSE)
  {
    ret = 0;
  }
  else
  {
    ret = dwWrite;
  }

  return ret;
}
uint32_t GSerialComm::CommPrintf(char* fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = Write((uint8_t*)buf, len);

  va_end(args);


  return ret;
}

void GSerialComm::AttCallbackFunc(void* obj, evt_cb cb)
{
  m_cbObj = obj;
  m_func = cb;
}

int GSerialComm::SendData(char* p_str, uint32_t length)
{
  DWORD dwWrite = 0;
  int ret = 0;
  if (WriteFile(m_Serialcfg.serial_handle, p_str, (DWORD)length, &dwWrite, NULL) == FALSE)
  {
    ret = 0;
  }
  else
  {
    ret = dwWrite;
  }

  return ret;
}


errno_t GSerialComm::openPort(char* port_name, uint32_t baud)
{
  errno_t err_code = 0;

  DCB dcb;
  COMMTIMEOUTS timeouts;
  DWORD dwError;

  char name[MAX_PATH] = { 0, };
  sprintf_s(name, MAX_PATH, "//./%s", port_name);

  m_Serialcfg.serial_handle = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (m_Serialcfg.serial_handle == INVALID_HANDLE_VALUE)
  {
    TRACE("Error opening serial port!\n");
    return static_cast<errno_t>(error_code::emptyPort);
  }

  while (1)
  {
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(m_Serialcfg.serial_handle, &dcb) == FALSE)
    {
      TRACE("Error GetCommState\n");
      err_code = static_cast<errno_t>(error_code::GetCommState);
      break;
    }

    // Set baudrate
    dcb.BaudRate = (DWORD)baud;
    dcb.ByteSize = 8;                    // Data bit = 8bit
    dcb.Parity = NOPARITY;             // No parity
    dcb.StopBits = ONESTOPBIT;           // Stop bit = 1
    dcb.fParity = NOPARITY;             // No Parity check
    dcb.fBinary = 1;                    // Binary mode
    dcb.fNull = 0;                    // Get Null byte
    dcb.fAbortOnError = 0;
    dcb.fErrorChar = 0;
    // Not using XOn/XOff
    dcb.fOutX = 0;
    dcb.fInX = 0;
    // Not using H/W flow control
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fOutxCtsFlow = 0;

    if (SetCommState(m_Serialcfg.serial_handle, &dcb) == FALSE)
    {
      DWORD dwError = GetLastError();
      err_code = static_cast<errno_t>(error_code::SerCommState);

      TRACE("SetCommState err: %d\n", (int)dwError);
      break;
    }

    if (SetCommMask(m_Serialcfg.serial_handle, 0) == FALSE) // Not using Comm event
    {
      err_code = static_cast<errno_t>(error_code::SetCommMask);
      break;
    }
    if (SetupComm(m_Serialcfg.serial_handle, SERIAL_COMM_BUFF_MAX, SERIAL_COMM_BUFF_MAX) == FALSE) // Buffer size (Rx,Tx)
    {
      err_code = static_cast<errno_t>(error_code::SetupComm);
      break;
    }
    if (PurgeComm(m_Serialcfg.serial_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
    {
      err_code = static_cast<errno_t>(error_code::PurgeComm);
      break;
    }
    if (ClearCommError(m_Serialcfg.serial_handle, &dwError, NULL) == FALSE)
    {
      err_code = static_cast<errno_t>(error_code::ClearCommError);
      break;
    }

    if (GetCommTimeouts(m_Serialcfg.serial_handle, &timeouts) == FALSE)
    {
      err_code = static_cast<errno_t>(error_code::GetCommTimeouts);
      break;
    }
    // Timeout (Not using timeout)
    // Immediatly return
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1; // must not be zero.
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(m_Serialcfg.serial_handle, &timeouts) == FALSE)
    {
      err_code = static_cast<errno_t>(error_code::SetCommTimeouts);
      break;
    }
    EscapeCommFunction(m_Serialcfg.serial_handle, SETRTS);
    EscapeCommFunction(m_Serialcfg.serial_handle, SETDTR);
    break;
  }

  if (err_code != 0)
  {
    CloseHandle(m_Serialcfg.serial_handle);
  }
  else
  {
    SetCommMask(m_Serialcfg.serial_handle, EV_RXCHAR);
    threadRun();
    m_IsOpened = true;
  }

  return err_code;
}

bool GSerialComm::closePort()
{
  bool ret = false;
  ret = CloseHandle(m_Serialcfg.serial_handle);
  m_IsOpened = false;
  return ret;
}

void GSerialComm::threadStop(void)
{
  Close();
}

void GSerialComm::threadRun(void)
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


uint32_t GSerialComm::readData()
{
  uint32_t ret = 0;
  int32_t length = 0;
  DWORD dwRead = 0;
  uint8_t data;

  if (ReadFile(m_Serialcfg.serial_handle, &data, (DWORD)1, &dwRead, NULL) == TRUE)
  {
    if (dwRead != 1)
    {
      length = 0;
    }
    else
    {
      length = 1;
      m_received_data = data;
      if (m_cbObj)
      {
        m_func(m_cbObj, &length, &m_received_data);
      }
    }
  }
  ret = length;

  return ret;
}

errno_t GSerialComm::PortOpen(char* port_name, uint32_t baud)
{
  m_Serialcfg.SetPortName(port_name);
  m_Serialcfg.baudrate = baud;

  return openPort(m_Serialcfg.GetPortName(), m_Serialcfg.baudrate);
}



void GSerialComm::threadJob(void)
{

  int32_t length = 0;
  DWORD dwRead = 0;
  uint8_t data;

  if (ReadFile(m_Serialcfg.serial_handle, &data, (DWORD)1, &dwRead, NULL) == TRUE)
  {
    if (dwRead != 1)
    {
      length = 0;
    }
    else
    {
      length = 1;
      m_received_data = data;
      if (m_cbObj)
      {
        m_func(m_cbObj, &length, &m_received_data);
      }
      else
      {
        m_Que.Put(data);
      }
    }
  }

}




/****************************************************
 *	thread
 ****************************************************/
UINT GSerialComm::threadFunc(LPVOID pParam)
{
  GSerialComm* pThis = (GSerialComm*)pParam;

  TRACE("CommWatch Thread Start\n\r");

  while (pThis->m_TrdLife)
  {
    pThis->threadJob();
    util::delay(10);
  }

  TRACE("CommWatch Thread Terminatet\n\r");
  pThis->m_TrdId = 0;
  pThis->m_hThread = NULL;
  return 0;
}

void GSerialComm::cbFunc(void* obj, void* w_parm, void* l_parm)
{
  GSerialComm* pThis = (GSerialComm*)obj;

}


#endif