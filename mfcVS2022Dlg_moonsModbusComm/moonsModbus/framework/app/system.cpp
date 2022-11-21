#include "pch.h"

#include "system.h"

//#include "commonUI.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AP_SYS;
using namespace AP_OBJ;


MSystem::MSystem() :m_ProjectDir()
, m_pSerialComm(), m_pObjinfo(nullptr), m_pMoonsMotor(nullptr)
, m_pMoons_comm(nullptr)
{

  init();
}

MSystem::~MSystem()
{
  terminate();
  init();
}

void MSystem::init()
{
  m_pObjinfo = nullptr;

  for (IComm*& elm : m_pSerialComm)
  {
    elm = nullptr;
  }

  m_pMoons_comm = nullptr;
  m_pMoonsMotor = nullptr;

}

void MSystem::terminate()
{
  stopThreads();
  destroy();
}

void MSystem::destroy()
{


  for (IComm*& elm : m_pSerialComm)
  {
    delete elm;
    elm = nullptr;
  }

  if (m_pMoonsMotor)
  {
    m_pMoonsMotor.reset();
  }
  m_pMoonsMotor = nullptr;


  if (m_pMoons_comm)
  {
    m_pMoons_comm.reset();
  }
  m_pMoons_comm = nullptr;


  if (m_pObjinfo) { delete m_pObjinfo; } m_pObjinfo = nullptr;


}

void MSystem::startThreads()
{
}

void MSystem::stopThreads()
{
}

errno_t MSystem::_createSerialCommComponents()
{
  errno_t ret = ERROR_SUCCESS;

  {
    GSerialComm::cfg_t cfg = {};
    int serial_idx = 0;
    int obj_id = 0;
    cfg.SetPortName("COM3");
    cfg.port_no = 3;
    cfg.baudrate = 115200;
    serial_idx = static_cast<int>(SERIAL_OBJ::moons_motor);
    obj_id = HL_SERIAL_BASE + serial_idx;
    m_pSerialComm[serial_idx] = new GSerialComm(m_pObjinfo->GetCommonData(obj_id), cfg);

    if (m_pObjinfo->RegisterComponent(obj_id, (Icommon*)m_pSerialComm[serial_idx]) != ERROR_SUCCESS)
    {
      ERR_PRINT("RegisterComponent Result Fail!(HL_SERIAL_BASE, m_pSerialComm[%d])\n", serial_idx);
    }
    else
    {
  
    }
  }
  return ret;
}


errno_t MSystem::_createBasicHWComponents()
{
  errno_t ret = ERROR_SUCCESS;
  // 모터 라이브러리 객체 생성
  //moons 
  {
    GMoons_comm::cfg_t lib_cfg{};
    int obj_id = 0;
    int serial_idx = 0;
    lib_cfg.base_node_id = 0;
    lib_cfg.pComm = GetSerialCommComponent(SERIAL_OBJ::moons_motor);

    obj_id = HL_MOTION_LIB_BASE;
    m_pMoons_comm = shared_ptr<GMoons_comm>(new GMoons_comm(m_pObjinfo->GetCommonData(obj_id), lib_cfg));
    Icommon* p_obj = nullptr;
    p_obj = static_cast<Icommon*>(m_pMoonsMotor.get());

    if (m_pObjinfo->RegisterComponent(obj_id, p_obj) != ERROR_SUCCESS)
    {
      ERR_PRINT("RegisterComponent Result Fail!(HL_MOTION_LIB_BASE, m_pMoons_comm)\n");
    }
    else
    {

    }
  }
  return ret;
}

errno_t MSystem::_createMotionComponets()
{
  errno_t ret = ERROR_SUCCESS;

  GMoons_motor::cfg_t cfg = {};
  cfg.AxisId = 1;
  cfg.pComm = GetMoonsCommModbusComponent();


  int obj_id = 0;
  obj_id = HL_AXIS_BASE;

  m_pMoonsMotor = shared_ptr<GMoons_motor>(new GMoons_motor(m_pObjinfo->GetCommonData(obj_id), cfg));
  Icommon* p_obj = nullptr;
  p_obj = static_cast<Icommon*>(m_pMoonsMotor.get());
  if (m_pObjinfo->RegisterComponent(obj_id, p_obj) != ERROR_SUCCESS)
  {
    ERR_PRINT("RegisterComponent Result Fail!(MOTION_AXIS_OBJ %d, m_pMoonsMotor)\n", obj_id);
  }
  else
  {
    //	m_pMoonsMotor->Open();
  }



  return ret;
}

errno_t MSystem::_createEngineComponets()
{
  errno_t ret = ERROR_SUCCESS;
  return ret;
}

errno_t MSystem::_createApComponts()
{
  errno_t ret = ERROR_SUCCESS;
  return ret;
}

errno_t MSystem::Initialize()
{
  errno_t ret = ERROR_SUCCESS;


  m_pObjinfo = new MSystemObject();


  _createSerialCommComponents();
  _createBasicHWComponents();
  _createMotionComponets();
  _createEngineComponets();

  return ret;
}


IComm* MSystem::GetSerialCommComponent(SERIAL_OBJ id) const
{
  return m_pSerialComm[static_cast<int>(id)];
}

GMoons_comm* AP_SYS::MSystem::GetMoonsCommModbusComponent() const
{
  return m_pMoons_comm.get();
}

GMoons_motor* AP_SYS::MSystem::GetMoonsMotorComponent() const
{
    return m_pMoonsMotor.get();
}
