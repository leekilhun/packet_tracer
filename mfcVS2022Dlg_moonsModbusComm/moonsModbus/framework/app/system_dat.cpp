#include "pch.h"
#include "system_def.h"

#include "system_dat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AP_DATA;
using namespace util;

constexpr int SYS_DATA_RESERVE_CNT = 20;

GConfig_dat::GConfig_dat(char* file_info) :parser::json(file_info, static_cast<uint8_t>(GConfig_dat::addr_e::_max) + SYS_DATA_RESERVE_CNT)/*m_pParser(nullptr)*/
, m_isOpened(false), m_sizeFile (0)
{
  sprintf_s(m_dirFile, PRJ_FILE_DIR_STR_MAX - 1, file_info);
  //m_pParser = new parser::json(m_dirFile, static_cast<uint8_t>(conf_dat::addr_e::_max));
}

GConfig_dat::~GConfig_dat()
{
}

int GConfig_dat::LoadData()
{
  m_sizeFile = parser::json::LoadData();
  if (m_sizeFile > 0)
  {
    m_isOpened = true;
  }
  return m_sizeFile;
}

void GConfig_dat::WriteData(GConfig_dat::addr_e addr, parser::json_t data)
{
  m_mutex.lock();


  m_mutex.unlock();
}

errno_t GConfig_dat::ReadData(GConfig_dat::addr_e addr, void** p_ret)
{
  errno_t ret = ERROR_SUCCESS;

  if (m_isOpened == false)
  {
    return -1;
  }

  parser::json_t config_data = parser::json::ReadData(static_cast<uint32_t>(addr));


  if (config_data.is_array)
  {
    switch (config_data.key_value->type)
    {
    case parser::valueType_e::boolian:
      *(p_ret) = (void*)&config_data.key_value->yesorno;
      break;

    case parser::valueType_e::integer:
      *(p_ret) = (void*)&config_data.key_value->number;
      break;

    case parser::valueType_e::realnumber:
      *(p_ret) = (void*)&config_data.key_value->realnumber;
      break;

    case parser::valueType_e::string:
      *(p_ret) = (void*)&config_data.key_value->str_value;
      break;
    case parser::valueType_e::none:
      __fallthrough;
    default:
      ret = -1;
      break;
    }
  }
  else
  {
    switch (config_data.type)
    {
    case parser::valueType_e::boolian:
      *(p_ret) = (void*)&config_data.yesorno;
      break;
    case parser::valueType_e::integer:
      *(p_ret) = (void*)&config_data.number;
      break;
    case parser::valueType_e::realnumber:
      *(p_ret) = (void*)&config_data.realnumber;
      break;
    case parser::valueType_e::string:
      *(p_ret) = (void*)&config_data.str_value;
      break;
    case parser::valueType_e::none:
      __fallthrough;
    default:
      ret = -1;
      break;
    }
  }


  /*

  switch (addr)
  {
  case GConfig_dat::addr_e::USE_BUZZER:
    __fallthrough;
  case GConfig_dat::addr_e::USE_DOOR:
    __fallthrough;
  case GConfig_dat::addr_e::USE_MES:
    *(p_ret) = (void*)&config_data.key_value->yesorno;
    break;

  case GConfig_dat::addr_e::LANGUAGE_SELECT:
    __fallthrough;
  case GConfig_dat::addr_e::SHIFT_A_TIME:
    __fallthrough;
  case GConfig_dat::addr_e::SHIFT_C_TIME:
    __fallthrough;
  case GConfig_dat::addr_e::RETAY_MAX_CNT:
    __fallthrough;  
  case GConfig_dat::addr_e::MOONS_BAUDRATE:
    __fallthrough;  
  case GConfig_dat::addr_e::ORIENTAL_BAUDRATE:
    __fallthrough;
  case GConfig_dat::addr_e::SICK_BAUDRATE:
    __fallthrough;
  case GConfig_dat::addr_e::FANUC_PORT:
    __fallthrough;
  case GConfig_dat::addr_e::FASTECH_PORT:
    *(p_ret) = (void*)&config_data.key_value->number;
    break;

  case GConfig_dat::addr_e::MACHINE_NO:
    __fallthrough;
  case GConfig_dat::addr_e::PASSWORD:
    __fallthrough;
  case GConfig_dat::addr_e::MACHINE_NAME:
    __fallthrough;
  case GConfig_dat::addr_e::RUN_MODE:
    __fallthrough;
  case GConfig_dat::addr_e::MODEL_DIR:
    __fallthrough;
  case GConfig_dat::addr_e::MODEL_NAME:
    __fallthrough;
  case GConfig_dat::addr_e::MOONS_PORT_NAME:
    __fallthrough;
  case GConfig_dat::addr_e::ORIENTAL_PORT_NAME:
    __fallthrough;
  case GConfig_dat::addr_e::SICK_PORT_NAME:
    __fallthrough;
  case GConfig_dat::addr_e::FANUC_IP:
    __fallthrough;
  case GConfig_dat::addr_e::FANUC_MODE:
    __fallthrough;
  case GConfig_dat::addr_e::FASTECH_IP:
    __fallthrough;
  case GConfig_dat::addr_e::FASTECH_MODE:
    *(p_ret) = (void*)&config_data.key_value->str_value;
    break;

  default:
    ret = -1;
    break;
  }
  */

 return ret;
}

bool GConfig_dat::IsOpened() const
{
  return m_isOpened;
}

char* GConfig_dat::GetMoonsPortName()
{
  if (m_isOpened == false)
  {
    return nullptr;
  }

  return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_PORT_NAME)).key_value->str_value;
}


int GConfig_dat::GetMoonsPortNo()
{
    if (m_isOpened == false)
    {
        return 0;
    }
    int ret = 0;
    char* port_name = GetMoonsPortName();
    
    char* begin = port_name +3;
   
    ret = atoi((const char*)begin);

    return ret;
    //return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_PORT_NAME)).key_value->str_value;
}

int GConfig_dat::GetMoonsBaudrate()
{
  if (m_isOpened == false)
  {
    return 0;
  }
  //int ret = 0;
  //parser::json_t config_data = parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::MOONS_BAUDRATE));
  //ret = config_data.key_value->number;
  return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_BAUDRATE)).key_value->number;
}


char* GConfig_dat::GetOrientalPortName()
{
    if (m_isOpened == false)
    {
        return nullptr;
    }
    return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_ORIENTAL_PORT_NAME)).key_value->str_value;
}
int GConfig_dat::GetOrientalPortNo()
{
    if (m_isOpened == false)
    {
        return 0;
    }
    int ret = 0;
    char* port_name = GetOrientalPortName();
    char* begin = port_name + 3;

    ret = atoi((const char*)begin);

    return ret;
    //return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_PORT_NAME)).key_value->str_value;
}

int GConfig_dat::GetOrientalBaudrate()
{
    if (m_isOpened == false)
    {
        return 0;
    }
    return  parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_ORIENTAL_BAUDRATE)).key_value->number;;
}



char* GConfig_dat::GetSickPortName()
{
  if (m_isOpened == false)
  {
    return nullptr;
  }
  return parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_SICK_PORT_NAME)).key_value->str_value;
}

int GConfig_dat::GetSickBaudrate()
{
  if (m_isOpened == false)
  {
    return 0;
  }
  return  parser::json::ReadData(static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_SICK_BAUDRATE)).key_value->number;;
}


void GConfig_dat::GetSerialConfig(SERIAL_OBJ obj, serialCfgDat_t* p_data)
{
    if (m_isOpened)
    {
        uint32_t addr;
        switch (obj)
        {
        case SERIAL_OBJ::dist_sensor:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_SICK_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_SICK_PORT_NAME);
            p_data->port_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_SICK_BAUDRATE);
            p_data->baudrate = parser::json::ReadData(addr).key_value->number;
            break;

        case SERIAL_OBJ::moons_motor:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_PORT_NAME);
            p_data->port_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_MOONS_BAUDRATE);
            p_data->baudrate = parser::json::ReadData(addr).key_value->number;
            break;

        case SERIAL_OBJ::oriental_motor:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_ORIENTAL_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_ORIENTAL_PORT_NAME);
            p_data->port_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::SERIAL_ORIENTAL_BAUDRATE);
            p_data->baudrate = parser::json::ReadData(addr).key_value->number;
            break;

        default:
            break;
        }

    }
}



void GConfig_dat::GetNetConfig(NET_OBJ obj, netCfgDat_t* p_data)
{
    if (m_isOpened )
    {
        uint32_t addr;
        switch (obj)
        {
        case NET_OBJ::ups_0:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_0_NET_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_0_NET_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_0_NET_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_0_NET_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::ups_1:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_1_NET_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_1_NET_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_1_NET_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::UPS_1_NET_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fanuc_robot:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::FANUC_NET_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::FANUC_NET_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::FANUC_NET_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::FANUC_NET_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16_CB_0:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_NET_NAME_0);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_NET_IP_0);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_PORT_0);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_MODE_0);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16_CB_1:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_NET_NAME_1);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_NET_IP_1);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_PORT_1);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16_CB_MODE_1);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_o16_CB_2:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_NET_NAME_2);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_NET_IP_2);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_PORT_2);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_MODE_2);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_o16_CB_3:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_NET_NAME_3);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_NET_IP_3);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_PORT_3);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_o16_CB_MODE_3);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;


        case NET_OBJ::fastech_io_i16o16_TC_0:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_NET_NAME_0);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_NET_IP_0);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_PORT_0);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_MODE_0);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16o16_TC_1:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_NET_NAME_1);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_NET_IP_1);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_PORT_1);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_TC_MODE_1);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16o16_SL_0:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_NET_NAME_0);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_NET_IP_0);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_PORT_0);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_MODE_0);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16o16_SL_1:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_NET_NAME_1);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_NET_IP_1);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_PORT_1);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_SL_MODE_1);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_io_i16o16_RT:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_ROBOT_NET_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_ROBOT_NET_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_ROBOT_NET_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_io_i16o16_ROBOT_NET_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_mot_lib_fastech_tapeLoader:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_tapeLoader_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_tapeLoader_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_tapeLoader_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_tapeLoader_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_mot_lib_fastech_old_tape:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_old_tape_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_old_tape_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_old_tape_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_old_tape_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        case NET_OBJ::fastech_mot_lib_fastech_new_tape:
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_new_tape_NAME);
            p_data->name = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_new_tape_IP);
            p_data->ip_str = parser::json::ReadData(addr).key_value->str_value;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_new_tape_PORT);
            p_data->port_no = parser::json::ReadData(addr).key_value->number;
            addr = static_cast<uint32_t>(GConfig_dat::addr_e::fastech_mot_lib_fastech_new_tape_MODE);
            p_data->mode_str = parser::json::ReadData(addr).key_value->str_value;
            break;

        default:
            break;
        }
        
    }


}


bool GConfig_dat::IsUseDoorOpenSensor() 
{
    bool ret = false;
    uint32_t addr = static_cast<uint32_t>(GConfig_dat::addr_e::USE_DOOR);
    ret = parser::json::ReadData(addr).key_value->yesorno;
    return ret;
}


bool GConfig_dat::IsUseBuzzer()
{
    bool ret = false;
    uint32_t addr = static_cast<uint32_t>(GConfig_dat::addr_e::USE_BUZZER);
    ret = parser::json::ReadData(addr).key_value->yesorno;
    return ret;
}


GAxis_dat::GAxis_dat(char* file_info) :parser::json(file_info, (static_cast<uint8_t>(GAxis_dat::addr_e::_max) * AXIS_DATA_ELEMENT_MAX) + +SYS_DATA_RESERVE_CNT)
, m_isOpened(false), m_sizeFile(0)
{
    sprintf_s(m_dirFile, PRJ_FILE_DIR_STR_MAX - 1, file_info);
}

GAxis_dat::~GAxis_dat()
{
}

bool GAxis_dat::IsOpened() const
{
    return m_isOpened;
}

int GAxis_dat::LoadData()
{
    m_sizeFile = parser::json::LoadData();
    if (m_sizeFile > 0)
    {
        for (size_t i = 0; i < static_cast<size_t>(GAxis_dat::addr_e::_max); i++)
        {
            assignData(static_cast<GAxis_dat::addr_e>(i));
        }
        
        m_isOpened = true;
    }
    return m_sizeFile;
}

uint32_t GAxis_dat::GetDataCnt() const
{
    return static_cast<uint32_t>(GAxis_dat::addr_e::_max);
}


void GAxis_dat::assignData(addr_e addr)
{
    // TODO: 여기에 구현 코드 추가.

    uint32_t data_count = json::GetDataCnt();

    uint32_t i = 0 + (static_cast<uint32_t>(addr) * axis_dat[static_cast<uint8_t>(addr)].DataLength());
    {
        axis_dat[static_cast<uint8_t>(addr)].SetName(json::ReadData(i).key);
        axis_dat[static_cast<uint8_t>(addr)].CWSWLimit = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].CCWSWLimit = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].HomeFastVelocity = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].HomeSlowVelocity = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].HomeAccelerate = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].HomeOffset = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].JogSlow = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].JogFast = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].RunAccelerate = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].RunVelocity = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].SlowRunVelocity = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].LimitTime = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].OriginLimitTime = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].StabilityTime = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].Tolerance = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].PassOffset = json::ReadData(i++).key_value->realnumber;
        axis_dat[static_cast<uint8_t>(addr)].StopDecelerate = json::ReadData(i).key_value->realnumber;
    }

}


GAxis_dat::dat_t* GAxis_dat::GetData(GAxis_dat::addr_e addr)
{
    return &axis_dat[static_cast<uint8_t>(addr)];
}









#if 0

void GAxis_dat::WriteData(GAxis_dat::addr_e addr, GAxis_dat::dat_t data)
{
    cmd_pos_dat[static_cast<uint8_t>(addr)] = data;
    //TODO:: write mcu eeprom
}


GAxis_dat::dat_t GAxis_dat::ReadData(GAxis_dat::addr_e addr)
{
    return cmd_pos_dat[static_cast<uint8_t>(addr)];
}

void GAxis_dat::LoadRomData()
{
    uint32_t rcv_data[2] = { 0, };
    for (uint8_t i = 0; i < static_cast<uint8_t>(GAxis_dat::addr_e::_max); i++) {
        //TODO:: read mcu eeprom
        cmd_pos_dat[i].cmd_pos = (int)rcv_data[0];
        cmd_pos_dat[i].cmd_vel = rcv_data[1];
    }
}

bool GAxis_dat::ClearRomData()
{
    uint8_t data[8] = { 0, };
    bool ret = false;
    for (uint8_t i = 0; i < static_cast<uint8_t>(GAxis_dat::addr_e::_max); i++) {
        //TODO:: write mcu eeprom
    }
    return ret;
}



#endif


GCylinder_dat::GCylinder_dat(char* file_info) :parser::json(file_info, (static_cast<uint8_t>(GCylinder_dat::addr_e::_max)* CYLINDER_DATA_ELEMENT_MAX)+SYS_DATA_RESERVE_CNT)
, m_isOpened(false), m_sizeFile(0)
{
}

GCylinder_dat::~GCylinder_dat()
{
}


bool GCylinder_dat::IsOpened() const
{
    return m_isOpened;
}

int GCylinder_dat::LoadData()
{
    m_sizeFile = parser::json::LoadData();
    if (m_sizeFile > 0)
    {
        for (size_t i = 0; i < static_cast<size_t>(GCylinder_dat::addr_e::_max); i++)
        {
            assignData(static_cast<GCylinder_dat::addr_e>(i));
        }

        m_isOpened = true;
    }
    return m_sizeFile;
}

uint32_t GCylinder_dat::GetDataCnt() const
{
    return  static_cast<uint32_t>(GCylinder_dat::addr_e::_max);
}

void GCylinder_dat::assignData(addr_e addr)
{
    uint32_t data_count = json::GetDataCnt();

    uint32_t i = 0 + (static_cast<uint32_t>(addr) * cyl_act_dat[static_cast<uint8_t>(addr)].DataLength());
    {
        cyl_act_dat[static_cast<uint8_t>(addr)].SetName(json::ReadData(i).key);
        cyl_act_dat[static_cast<uint8_t>(addr)].SetCylinderType(json::ReadData(i++).key_value->str_value);
        cyl_act_dat[static_cast<uint8_t>(addr)].SetSolenoidType(json::ReadData(i++).key_value->str_value);
        cyl_act_dat[static_cast<uint8_t>(addr)].MovingTime = json::ReadData(i++).key_value->realnumber;
        cyl_act_dat[static_cast<uint8_t>(addr)].SettlingTime1 = json::ReadData(i++).key_value->realnumber;
        cyl_act_dat[static_cast<uint8_t>(addr)].SettlingTime1 = json::ReadData(i++).key_value->realnumber;
    }
}

GCylinder_dat::dat_t* GCylinder_dat::GetData(GCylinder_dat::addr_e addr)
{
    return &cyl_act_dat[static_cast<uint8_t>(addr)];
}

#if 0
void GCylinder_dat::WriteData(GCylinder_dat::addr_e addr, GCylinder_dat::dat_t data)
{
    cyl_act_dat[static_cast<uint8_t>(addr)] = data;
    uint16_t rcv_data[2] = { data.timeout, data.on_settling };
    //TODO:: write mcu eeprom
}



GCylinder_dat::dat_t GCylinder_dat::ReadData(GCylinder_dat::addr_e addr)
{
    return cyl_act_dat[static_cast<uint8_t>(addr)];
}

void GCylinder_dat::LoadRomData()
{
    uint16_t rcv_data[2] = { 0, };
    for (uint8_t i = 0; i < static_cast<uint8_t>(GCylinder_dat::addr_e::_max); i++) {
        //TODO:: read mcu eeprom
        cyl_act_dat[i].timeout = rcv_data[0];
        cyl_act_dat[i].on_settling = rcv_data[1];
    }
}

bool GCylinder_dat::ClearRomData()
{
    uint8_t data[APDAT_CYL_ACT_DATA_LENGTH] = { 0, };
    bool ret = false;
    for (uint8_t i = 0; i < static_cast<uint8_t>(GCylinder_dat::addr_e::_max); i++) {
        //TODO:: write mcu eeprom
    }
    return ret;
}

#endif



GVacuum_dat::GVacuum_dat(char* file_info) :parser::json(file_info, (static_cast<uint8_t>(GCylinder_dat::addr_e::_max)* CYLINDER_DATA_ELEMENT_MAX)+SYS_DATA_RESERVE_CNT)
, m_isOpened(false), m_sizeFile(0)
{
}

GVacuum_dat::~GVacuum_dat()
{
}


bool GVacuum_dat::IsOpened() const
{
    return m_isOpened;
}

int GVacuum_dat::LoadData()
{
    m_sizeFile = parser::json::LoadData();
    if (m_sizeFile > 0)
    {
        for (size_t i = 0; i < static_cast<size_t>(GVacuum_dat::addr_e::_max); i++)
        {
            assignData(static_cast<GVacuum_dat::addr_e>(i));
        }

        m_isOpened = true;
    }
    return m_sizeFile;
}


uint32_t GVacuum_dat::GetDataCnt() const
{
    return  static_cast<uint32_t>(GVacuum_dat::addr_e::_max);
}


void GVacuum_dat::assignData(addr_e addr)
{
    uint32_t data_count = json::GetDataCnt();

    uint32_t i = 0 + (static_cast<uint32_t>(addr) * vac_act_dat[static_cast<uint8_t>(addr)].DataLength());
    {
        vac_act_dat[static_cast<uint8_t>(addr)].SetName(json::ReadData(i).key);
        vac_act_dat[static_cast<uint8_t>(addr)].SetVacuumType(json::ReadData(i++).key_value->str_value);
        vac_act_dat[static_cast<uint8_t>(addr)].SetSolenoidType(json::ReadData(i++).key_value->str_value);
        vac_act_dat[static_cast<uint8_t>(addr)].TurningTime = json::ReadData(i++).key_value->realnumber;
        vac_act_dat[static_cast<uint8_t>(addr)].OnSettlingTime = json::ReadData(i++).key_value->realnumber;
        vac_act_dat[static_cast<uint8_t>(addr)].OffSettlingTime = json::ReadData(i++).key_value->realnumber;
    }
}

GVacuum_dat::dat_t* GVacuum_dat::GetData(GVacuum_dat::addr_e addr)
{
    return &vac_act_dat[static_cast<uint8_t>(addr)];
}





GErrDescript_dat::GErrDescript_dat(char* file_info) :parser::json(file_info, 4096)
{
}

GErrDescript_dat::~GErrDescript_dat()
{
}













#if 0

void GVacuum_dat::WriteData(GVacuum_dat::addr_e addr, GVacuum_dat::dat_t data)
{
    vac_act_dat[static_cast<uint8_t>(addr)] = data;
    uint16_t rcv_data[2] = { data.timeout, data.on_settling };
    //TODO:: write mcu eeprom
}



GVacuum_dat::dat_t GVacuum_dat::ReadData(GVacuum_dat::addr_e addr)
{
    return vac_act_dat[static_cast<uint8_t>(addr)];
}

void GVacuum_dat::LoadRomData()
{
    uint16_t rcv_data[2] = { 0, };
    for (uint8_t i = 0; i < static_cast<uint8_t>(GVacuum_dat::addr_e::_max); i++) {
        //TODO:: read mcu eeprom
        vac_act_dat[i].timeout = rcv_data[0];
        vac_act_dat[i].on_settling = rcv_data[1];
    }
}

bool GVacuum_dat::ClearRomData()
{
    uint8_t data[APDAT_VAC_ACT_DATA_LENGTH] = { 0, };
    bool ret = false;
    for (uint8_t i = 0; i < static_cast<uint8_t>(GVacuum_dat::addr_e::_max); i++) {
        //TODO:: write mcu eeprom
    }
    return ret;
}

#endif

#define APDAT_START_ADDRESS               0x0000

#define APDAT_START_ADDR                  APDAT_START_ADDRESS
#define APDAT_LENGTH                      4
#define APDAT_ADDR(x)                     APDAT_START_ADDR + ((x) * APDAT_LENGTH)
#define APDAT_CNT_MAX                     8
#define APDAT_END_ADDR                    APDAT_START_ADDR + (APDAT_LENGTH * APDAT_CNT_MAX)

#define APDAT_AXIS_POS_START_ADDR         APDAT_END_ADDR
#define APDAT_AXIS_POS_DATA_LENGTH        8
#define APDAT_AXIS_POS_DATA_ADDR(x)       APDAT_AXIS_POS_START_ADDR + ((x) * APDAT_AXIS_POS_DATA_LENGTH)
#define APDAT_AXIS_POS_DATA_CNT_MAX       8
#define APDAT_AXIS_POS_END_ADDR           APDAT_AXIS_POS_START_ADDR + (APDAT_AXIS_POS_DATA_LENGTH * APDAT_AXIS_POS_DATA_CNT_MAX)

#define APDAT_CYL_ACT_START_ADDR          APDAT_AXIS_POS_END_ADDR
#define APDAT_CYL_ACT_DATA_LENGTH         4
#define APDAT_CYL_ACT_DATA_ADDR(x)        APDAT_CYL_ACT_START_ADDR + ((x) * APDAT_CYL_ACT_DATA_LENGTH)
#define APDAT_CYL_ACT_DATA_CNT_MAX        8
#define APDAT_CYL_ACT_END_ADDR            APDAT_CYL_ACT_START_ADDR + (APDAT_CYL_ACT_DATA_LENGTH * APDAT_CYL_ACT_DATA_CNT_MAX)

#define APDAT_VAC_ACT_START_ADDR          APDAT_CYL_ACT_END_ADDR
#define APDAT_VAC_ACT_DATA_LENGTH         4
#define APDAT_VAC_ACT_DATA_ADDR(x)        APDAT_VAC_ACT_START_ADDR + ((x) * APDAT_VAC_ACT_DATA_LENGTH)
#define APDAT_VAC_ACT_DATA_CNT_MAX        8
#define APDAT_VAC_ACT_END_ADDR            APDAT_VAC_ACT_START_ADDR + (APDAT_VAC_ACT_DATA_LENGTH * APDAT_VAC_ACT_DATA_CNT_MAX)

#define APDAT_SEQ_START_ADDR              APDAT_VAC_ACT_END_ADDR
#define APDAT_SEQ_DATA_LENGTH             4
#define APDAT_SEQ_DATA_ADDR(x)            APDAT_SEQ_START_ADDR + ((x) * APDAT_SEQ_DATA_LENGTH)
#define APDAT_SEQ_DATA_CNT_MAX            8
#define APDAT_SEQ_END_ADDR                APDAT_SEQ_START_ADDR + (APDAT_SEQ_DATA_LENGTH * APDAT_SEQ_DATA_CNT_MAX)


#define APDAT_LOG_START_ADDR              APDAT_SEQ_END_ADDR
#define APDAT_LOG_DATA_LENGTH             64
#define APDAT_LOG_DATA_ADDR(x)            APDAT_LOG_START_ADDR + ((x) * APDAT_LOG_DATA_LENGTH)
#define APDAT_LOG_DATA_CNT_MAX            32
#define APDAT_LOG_END_ADDR                APDAT_LOG_START_ADDR + (APDAT_LOG_DATA_LENGTH * APDAT_LOG_DATA_CNT_MAX)



void GCtrlBoard_dat::WriteData(GCtrlBoard_dat::addr_e addr, GCtrlBoard_dat::dat_t data)
{
  apcfg_dat[static_cast<uint8_t>(addr)] = data;
  uint16_t rcv_data[2] = { data.parm1, data.parm2 };
  //TODO:: write mcu eeprom
}

GCtrlBoard_dat::dat_t GCtrlBoard_dat::ReadData(GCtrlBoard_dat::addr_e addr)
{
  return apcfg_dat[static_cast<uint8_t>(addr)];
}

GCtrlBoard_dat::dat_t* GCtrlBoard_dat::GetData(GCtrlBoard_dat::addr_e addr)
{
  return &apcfg_dat[static_cast<uint8_t>(addr)];
}

void GCtrlBoard_dat::LoadRomData()
{
  uint16_t rcv_data[2] = { 0, };
  for (uint8_t i = 0; i < static_cast<uint8_t>(GCtrlBoard_dat::addr_e::_max); i++) {
    //TODO:: read mcu eeprom
    apcfg_dat[i].parm1 = rcv_data[0];
    apcfg_dat[i].parm2 = rcv_data[1];
  }
}

bool GCtrlBoard_dat::ClearRomData()
{
  uint8_t data[APDAT_LENGTH] = { 0, };
  bool ret = false;
  for (uint8_t i = 0; i < static_cast<uint8_t>(GCtrlBoard_dat::addr_e::_max); i++) {
    //TODO:: write mcu eeprom
  }
  return ret;
}

void GCtrlBoard_dat::Write_MotCfg(GCtrlBoard_dat::dat_t data)
{
  apcfg_dat[static_cast<uint8_t>(GCtrlBoard_dat::addr_e::ap_mot_cfg)] = data;
  uint16_t rcv_data[2] = { data.parm1, data.parm2 };
  //TODO::write mcu eeprom
}

GCtrlBoard_dat::dat_t GCtrlBoard_dat::Read_MotCfg()
{
  return apcfg_dat[static_cast<uint8_t>(GCtrlBoard_dat::addr_e::ap_mot_cfg)];
}



