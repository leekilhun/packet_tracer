#include "pch.h"

# include "system.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

apSystem::apSystem() :m_ProjectDir()
, m_pSerialComm()
{

  init();
}

apSystem::~apSystem()
{
  terminate();
  init();
}

void apSystem::init()
{
  //m_pObjinfo = nullptr;
  //m_sysTimer = nullptr;
  //m_pSysLog = nullptr;
  //m_pCfgDat = nullptr;
  //m_pApDat = nullptr;
  //m_pAxisDat = nullptr;
  //m_pCylDat = nullptr;
  //m_pVacDat = nullptr;
  //m_pSeqDat = nullptr;
  //m_pSerialComm = nullptr;
  
  //m_pIo = nullptr;
  //m_pMotorComm = nullptr;
  //m_pMotor[0] = nullptr;
  //m_pMotor[1] = nullptr;
  //m_pMotor[2] = nullptr;

}

void apSystem::terminate()
{
  stopThreads();
  destroy();
}

void apSystem::destroy()
{
 
  //if (m_sysTimer) { delete m_sysTimer; } m_sysTimer = nullptr;

  //if (m_pSysLog) { delete m_pSysLog; } m_pSysLog = nullptr;

  //if (m_pCfgDat) { delete m_pCfgDat; }  m_pCfgDat = nullptr;
  //if (m_pApDat) { delete m_pApDat; } m_pApDat = nullptr;
  //if (m_pAxisDat) { delete m_pAxisDat; } m_pAxisDat = nullptr;
  //if (m_pCylDat) { delete m_pCylDat; } m_pCylDat = nullptr;
  //if (m_pVacDat) { delete m_pVacDat; } m_pVacDat = nullptr;
  //if (m_pSeqDat) { delete m_pSeqDat; } m_pSeqDat = nullptr;



  if (m_pSerialComm) 
  {
    delete m_pSerialComm; 
  }
  m_pSerialComm = nullptr;


}

void apSystem::startThreads()
{
}

void apSystem::stopThreads()
{
}

void apSystem::getMcuData()
{
}

errno_t apSystem::_createSerialCommComponents()
{
  errno_t ret = ERROR_SUCCESS;
  SerialComm::cfg_t cfg = {};
  cfg.SetPortName("COM24");
  cfg.port_no = 1;
  cfg.baudrate = 115200;

  
  m_pSerialComm = new SerialComm(cfg);


  return ret;
}

errno_t apSystem::_createEthernetCommComponents()
{
  errno_t ret = ERROR_SUCCESS;


  return ret;
}

errno_t apSystem::_createBasicHWComponents()
{
  errno_t ret = ERROR_SUCCESS;



  return ret;
}


errno_t apSystem::Initialize(openExe_cb cd_increase)
{
  errno_t ret = ERROR_SUCCESS;



  _createSerialCommComponents();
  _createEthernetCommComponents();

  _createBasicHWComponents();

  return ret;
}



SerialComm* apSystem::GetSerialCommComponent() const
{
  return m_pSerialComm;
}

#if 0

conf_dat* apSystem::GetCfgDat()
{
  return nullptr;
}

ap_dat* apSystem::GetApDat()
{
  return nullptr;
}

axis_dat* apSystem::GetAxisDat()
{
  return nullptr;
}

cyl_dat* apSystem::GetCylDat()
{
  return nullptr;
}

vac_dat* apSystem::GetVacDat()
{
  return nullptr;
}

seq_dat* apSystem::GetSeqDat()
{
  return nullptr;
}


#endif