#include "pch.h"
#include "hal_def.h"
#include "flag.h"
#include "motor_fastech.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef USE_HW_MOTOR_FASTECH


using namespace std;
using namespace GFastech;

const errno_t JL_MOTOR_ERR_SUCCESS = 0;


#define FM_ERR_01    (uint8_t)0
#define FM_ERR_02    (uint8_t)1
#define FM_STATE_01  (uint8_t)2
#define FM_STATE_02  (uint8_t)3

/* fastech motor axis status*/
#define REG_ERR_ALL               0
#define REG_ERR_HW_PLUS_LIMIT     1
#define REG_ERR_HW_MINUS_LIMIT    2
#define REG_ERR_SW_PLUS_LIMIT     3
#define REG_ERR_SW_MINUS_LIMIT    4
#define REG_ERR_RESERVED          5
#define REG_ERR_POS_CNT_OVER      6
#define REG_ERR_SERVO_ALARM       7

#define REG_ERR_OVER_CURRENT      0
#define REG_ERR_OVER_SPEED        1
#define REG_ERR_STEP_OUT          2
#define REG_ERR_OVER_LOAD         3
#define REG_ERR_OVER_HEAT         4
#define REG_ERR_BACK_EMF          5
#define REG_ERR_MOTOR_POWER       6
#define REG_ERR_INPOSITION        7

#define REG_EMG_STOP              0
#define REG_SLOW_STOP             1
#define REG_ORG_RETURNING         2
#define REG_INPOSIOTION           3
#define REG_SERVO_ON              4
#define REG_ALARM_RESET           5
#define REG_PT_STOPPED            6
#define REG_ORIGIN_SENSOR         7

#define REG_Z_PLUSE               0
#define REG_ORG_RETURN_OK         1
#define REG_MOTION_DIR            2
#define REG_MOTIONING             3
#define REG_MOTION_PAUSE          4
#define REG_MOTION_ACCEL          5
#define REG_MOTION_DECEL          6
#define REG_MOTION_CONSTANT       7


#define CMD_STX1                    0xAA
#define CMD_STX2                    0xCC
#define CMD_ETX1                    0xAA
#define CMD_ETX2                    0xEE
#define STUFFING_BYTE               0xAA


#define FASTECH_MOTOR_TO_PC_DATA        0
#define FASTECH_PC_TO_MOTOR_DATA        1

#define FASTECH_CMD_GET_SLAVE_INFO          0x01
#define FASTECH_CMD_GET_MOTOR_INFO          0x05
#define FASTECH_CMD_SAVE_ROM_ALL            0x10
#define FASTECH_CMD_GET_ROM_PARAM           0x11
#define FASTECH_CMD_SET_RAM_PARAM           0x12
#define FASTECH_CMD_GET_RAM_PARAM           0x13
#define FASTECH_CMD_SET_IO_OUT_REG          0x20
#define FASTECH_CMD_SET_IO_IN_REG           0x21
#define FASTECH_CMD_GET_IO_IN_REG           0x22
#define FASTECH_CMD_GET_IO_OUT_REG          0x23
#define FASTECH_CMD_SET_ASSN_IO_MAP         0x24
#define FASTECH_CMD_GET_ASSN_IO_MAP         0x25
#define FASTECH_CMD_READ_IO_MAP             0x26
#define FASTECH_CMD_TRG_OUT_RUN_A           0x27
#define FASTECH_CMD_TRG_OUT_STATUS          0x28
#define FASTECH_CMD_STEP_ENABLE             0x2A
#define FASTECH_CMD_ALARM_RESET             0x2C
#define FASTECH_CMD_GET_ALARM_INFO          0x2E
#define FASTECH_CMD_MOVE_STOP               0x31
#define FASTECH_CMD_MOVE_EMG_STOP           0x32
#define FASTECH_CMD_MOVE_ORG_SINGLE_AXIS    0x33
#define FASTECH_CMD_MOVE_ABS_SINGLE_AXIS    0x34
#define FASTECH_CMD_MOVE_REL_SINGLE_AXIS    0x35
#define FASTECH_CMD_MOVE_TO_LIMIT           0x36
#define FASTECH_CMD_MOVE_VELOCITY           0x37
#define FASTECH_CMD_MOVE_ABS_OVERRIDE       0x38
#define FASTECH_CMD_MOVE_REL_OVERRIDE       0x39
#define FASTECH_CMD_MOVE_VELOCITY_OVERRIDE  0x3A
#define FASTECH_CMD_MOVE_ALL_STOP           0x3B
#define FASTECH_CMD_MOVE_ALL_EMG_STOP       0x3C
#define FASTECH_CMD_MOVE_ALL_ORG_STOP       0x3D
#define FASTECH_CMD_MOVE_ALL_ABS_POS        0x3E
#define FASTECH_CMD_MOVE_ALL_REL_POS        0x3F
#define FASTECH_CMD_GET_AXIS_STATUS         0x40
#define FASTECH_CMD_GET_IO_AXIS_STATUS      0x41
#define FASTECH_CMD_GET_MOTION_STATUS       0x42
#define FASTECH_CMD_CLEAR_POSITION          0x56

GFastech_motor::GFastech_motor(common::Attribute_t common_data, cfg_t cfg):
  Icommon(common_data), m_AxisId(0), m_sysTimerId(0), m_Cfg(cfg)
  , /*m_pSysTimer(nullptr), */ m_motorParam(), m_pComm(nullptr)
{
 /* m_pSysTimer = m_Cfg.pSysTimer;
  sysTimer::obj_t st_data = {};
  st_data.cnt_ms = 100;
  st_data.obj = this;
  st_data.func_cb = timerFunc;

  m_sysTimerId = m_pSysTimer->Set(&st_data);*/

  Initialize();
}

GFastech_motor::~GFastech_motor()
{
}

void GFastech_motor::receiveData(void* obj, void* w_parm, void* l_parm)
{
  GFastech_motor* p_this = (GFastech_motor * )obj;
  //uint8_t data = (uint8_t)l_parm;
}

void GFastech_motor::timerFunc(void* obj)
{
}

errno_t GFastech_motor::update_AxisState()
{
  errno_t ret = 0;

  GFastech_motor::cmd_t cmd = (GFastech_motor::cmd_t)m_Packet.rx_packet.type;
  comm_t rcv_data = m_Packet;
  byte* p_data = rcv_data.rx_packet.data;
  switch (cmd)
  {
  case GFastech_motor::cmd_t::GET_SLAVE_INFO:
    break;
  case GFastech_motor::cmd_t::GET_MOTOR_INFO:
    break;
  case GFastech_motor::cmd_t::SAVE_ROM_ALL:
    break;
  case GFastech_motor::cmd_t::GET_ROM_PARAM:
    break;
  case GFastech_motor::cmd_t::SET_RAM_PARAM:
    break;
  case GFastech_motor::cmd_t::GET_RAM_PARAM:
    break;
  case GFastech_motor::cmd_t::SET_IO_OUT_REG:
    break;
  case GFastech_motor::cmd_t::SET_IO_IN_REG:
    break;
  case GFastech_motor::cmd_t::GET_IO_IN_REG:
    break;
  case GFastech_motor::cmd_t::GET_IO_OUT_REG:
    break;
  case GFastech_motor::cmd_t::SET_ASSN_IO_MAP:
    break;
  case GFastech_motor::cmd_t::GET_ASSN_IO_MAP:
    break;
  case GFastech_motor::cmd_t::READ_IO_MAP:
    break;
  case GFastech_motor::cmd_t::TRG_OUT_RUN_A:
    break;
  case GFastech_motor::cmd_t::TRG_OUT_STATUS:
    break;
  case GFastech_motor::cmd_t::SERVO_ENABLE:
    break;
  case GFastech_motor::cmd_t::ALARAM_RESET:
    break;
  case GFastech_motor::cmd_t::GET_ALARM_INFO:
    break;
  case GFastech_motor::cmd_t::MOVE_STOP:
    break;
  case GFastech_motor::cmd_t::MOVE_EMG_STOP:
    break;
  case GFastech_motor::cmd_t::MOVE_ORG_SINGLE_AXIS:
    break;
  case GFastech_motor::cmd_t::MOVE_ABS_SINGLE_AXIS:
    break;
  case GFastech_motor::cmd_t::MOVE_REL_SINGLE_AXIS:
    break;
  case GFastech_motor::cmd_t::MOVE_TO_LIMIT:
    break;
  case GFastech_motor::cmd_t::MOVE_VELOCITY:
    break;
  case GFastech_motor::cmd_t::MOVE_ABS_OVERRIDE:
    break;
  case GFastech_motor::cmd_t::MOVE_REL_OVERRIDE:
    break;
  case GFastech_motor::cmd_t::MOVE_VELOCITY_OVERRIDE:
    break;
  case GFastech_motor::cmd_t::MOVE_ALL_STOP:
    break;
  case GFastech_motor::cmd_t::MOVE_ALL_EMG_STOP:
    break;
  case GFastech_motor::cmd_t::MOVE_ALL_ORG_STOP:
    break;
  case GFastech_motor::cmd_t::MOVE_ALL_ABS_POS:
    break;
  case GFastech_motor::cmd_t::MOVE_ALL_REL_POS:
    break;
  case GFastech_motor::cmd_t::GET_AXIS_STATUS:
    break;
  case GFastech_motor::cmd_t::GET_AXIS_IO_STATUS:
    break;
  case GFastech_motor::cmd_t::GET_MOTION_STATUS:
  {
    enum class step_ {
      cmd_pos, act_pos, pos_err, act_vel, axis_reg
    };
    //int step = 0;
    step_ step = step_::cmd_pos;
    uint8_t loc = 0;
    while (loc < rcv_data.rx_packet.length)
    {
      switch (step)
      {
      case step_::cmd_pos:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        m_AxisState.cmd_pos = util::DwToInt(&tmp[0]);
        step = step_::act_pos;
      }
      break;
      case step_::act_pos:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        m_AxisState.act_pos = util::DwToInt(&tmp[0]);
        step = step_::pos_err;
      }
      break;
      case step_::pos_err:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        m_AxisState.pos_err = util::DwToInt(&tmp[0]);
        step = step_::act_vel;
      }
      break;
      case step_::act_vel:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        m_AxisState.act_vel = util::DwToInt(&tmp[0]);
        step = step_::axis_reg;
      }
      break;
      case step_::axis_reg:
      {
        for (uint8_t i = 0; i < 4; i++)
        {
          m_AxisState.axis_status[(uint8_t)i] = p_data[loc++];
        }
      }
      break;
      }
    }
    m_AxisState.is_motor_on = m_AxisState.axis_status[FM_STATE_01][REG_BIT(REG_SERVO_ON)];
    ret = 1;

  }
  break;
  case GFastech_motor::cmd_t::GET_ALL_STATUS:
  {
    enum class step_ {
      io_in, io_out, axis_reg, cmd_pos, act_pos, pos_err, act_vel, pt_item
    };
    //int step = 0;
    step_ step = step_::io_in;
    uint8_t loc = 0;
    while (loc < rcv_data.rx_packet.length)
    {
      switch (step)
      {
      case step_::io_in:
      {
        for (uint8_t i = 0; i < m_AxisState.io_in.GetSize(); i++)
        {
          m_AxisState.io_in[(uint8_t)i] = p_data[loc++];
        }
        step = step_::io_out;
      }
      break;
      case step_::io_out:
      {
        for (uint8_t i = 0; i < m_AxisState.io_out.GetSize(); i++)
        {
          m_AxisState.io_out[(uint8_t)i] = p_data[loc++];
        }
        step = step_::axis_reg;
      }
      break;
      case step_::axis_reg:
      {
        for (uint8_t i = 0; i < 4; i++)
        {
          m_AxisState.axis_status[(uint8_t)i] = p_data[loc++];
        }
        step = step_::cmd_pos;
      }
      break;
      case step_::cmd_pos:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        step = step_::act_pos;
        m_AxisState.cmd_pos = util::DwToInt(&tmp[0]);
      }
      break;
      case step_::act_pos:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        step = step_::pos_err;
        m_AxisState.act_pos = util::DwToInt(&tmp[0]);
      }
      break;
      case step_::pos_err:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        step = step_::act_vel;
        m_AxisState.pos_err = util::DwToInt(&tmp[0]);
      }
      break;
      case step_::act_vel:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        step = step_::pt_item;
        m_AxisState.act_vel = util::DwToInt(&tmp[0]);
      }
      break;
      case step_::pt_item:
      {
        byte tmp[4] = { 0, };
        for (uint8_t i = 0; i < 4; i++)
        {
          tmp[i] = p_data[loc++];
        }
        m_AxisState.pt_item = util::DwToInt(&tmp[0]);
      }
      break;
      }
    }

    m_AxisState.is_motor_on = m_AxisState.axis_status[FM_STATE_01][REG_BIT(REG_SERVO_ON)];
    ret = 1;
  }
  break;
  case GFastech_motor::cmd_t::CLEAR_POSITION:
    break;
  }

  //OutputOn(17);

  return ret;
}


uint32_t GFastech_motor::SendCommand(cmd_t cmd, uint8_t* p_data, uint8_t length)
{
 // if (m_Cfg.pComm->IsConnected() == false)
 //   return 0;

  byte packet[FASTECH_CMD_MAX_DATA_LENGTH] = { 0 };
  uint16_t crc = 0xffff;
  int index = 0;
  packet[index++] = CMD_STX1;
  packet[index++] = CMD_STX2;

  packet[index] = m_Cfg.AxisId; // id
  util::crc16_update(&crc, packet[index++]);

  packet[index] = m_Packet.sync_no++; // sync_no
  util::crc16_update(&crc, packet[index++]);

  packet[index] = static_cast<byte>(cmd); // cmd
  util::crc16_update(&crc, packet[index++]);

  uint32_t ret_data = 0;
  if (length > 0)
  {
    for (size_t i = 0; i < length; i++)
    {
      packet[index] = p_data[i]; // data
      util::crc16_update(&crc, packet[index++]);
      ret_data = p_data[i] << (i * 8);
    }
  }

  packet[index++] = crc & 0xff;
  packet[index++] = (crc >> 8) & 0xff;

  packet[index++] = CMD_ETX1;
  packet[index++] = CMD_ETX2;

  m_Packet.resp_time = util::millis();
  m_Packet.wait_resp = true;

 // uint32_t ret = (uint32_t)m_Cfg.pComm->SendCmd((uint8_t)cmd, packet, index);


  return 0;
}

void GFastech_motor::LoadingParamterData()
{
}

errno_t GFastech_motor::Initialize()
{
  errno_t ret = JL_MOTOR_ERR_SUCCESS;
 // if (m_Cfg.pComm->IsConnected() == false)
 //   ret = m_Cfg.pComm->OpenPort();
  if (ret == JL_MOTOR_ERR_SUCCESS) 
  {
 //   m_Cfg.pComm->AttCallbackFunc(this, receiveData);
    m_AxisState.id = m_Cfg.AxisId;
    m_AxisState.is_motor_on = true;
    m_Packet.is_init = true;
    m_Packet.state = FM_COMM_ST(wait_stx);

    //m_pSysTimer->Start(m_sysTimerId);
  }
  return ret;
}

errno_t GFastech_motor::MotorOnOff(bool on_off)
{
  uint8_t data = (uint8_t)on_off;
  SendCommand(cmd_t::SERVO_ENABLE, &data, 1);

  return errno_t();
}

bool GFastech_motor::IsMotorOn()
{
  m_AxisState.is_motor_on = m_AxisState.axis_status[FM_STATE_01][REG_BIT(REG_SERVO_ON)];
  return  m_AxisState.is_motor_on;
}

errno_t GFastech_motor::WaitDone(uint8_t mode)
{
  return errno_t();
}

bool GFastech_motor::IsAxisDone()
{
  return false;
}

errno_t GFastech_motor::ClearState()
{
  return errno_t();
}


uint32_t GFastech_motor::GetAllState()
{
  //if (m_Cfg.pComm->IsConnected() == false)
  //  return 0;

  return SendCommand(cmd_t::GET_ALL_STATUS, 0, 0);
}


uint32_t GFastech_motor::GetMotorState()
{
  //if (m_Cfg.pComm->IsConnected() == false)
  //  return 0;

  return SendCommand(cmd_t::GET_MOTION_STATUS, 0, 0);
}

int GFastech_motor::Move(int cmd_pos, uint32_t cmd_vel, uint32_t acc, uint32_t decel)
{
  return 0;
}

int GFastech_motor::Stop()
{
  return 0;
}

int GFastech_motor::VStop()
{
  return 0;
}

int GFastech_motor::EStop()
{
  return 0;
}

int GFastech_motor::JogMove(uint32_t cmd_vel, bool is_cw)
{
  return 0;
}

int GFastech_motor::JogMove(uint32_t jog_pos, uint32_t cmd_vel, bool is_cw)
{
  return 0;
}

bool GFastech_motor::IsOn(uint32_t addr)
{
  return false;
}

bool GFastech_motor::IsOff(uint32_t addr)
{
  return false;
}

errno_t GFastech_motor::OutputOn(uint32_t addr)
{
  return errno_t();
}

errno_t GFastech_motor::OutputOff(uint32_t addr)
{
  return errno_t();
}

errno_t GFastech_motor::OutputToggle(uint32_t addr)
{
  return errno_t();
}


#endif