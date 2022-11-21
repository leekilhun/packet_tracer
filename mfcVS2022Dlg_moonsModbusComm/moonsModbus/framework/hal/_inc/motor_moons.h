#pragma once

#include "motor_moons_reg.h"

#ifdef USE_HW_MOTOR_MOONS_LIB
#else
#ifdef USE_HW_COMM_MOONS
#include "motor_moons_comm.h"
#endif
#endif
namespace GMoons
{
#ifdef USE_HW_MOTOR_MOONS_LIB
  using namespace LIB_MOONS;
#endif
  using namespace std;

  using moons_param_t = IAxis::motion_param_t;

  class GMoons_motor :public Icommon
  {

    /****************************************************
     *	data
     ****************************************************/
  public:

    struct origin_param_t
    {
      uint8_t home_x_no{};
      int find_home_dir{}; // 1 CW, -1 CCW
      double find_home_vel{};//rps
      char home_x_level{}; //'L' Low, 'H' High, 

      origin_param_t()
      {
        home_x_no = 5; // home sensor x5
        home_x_level = 'L';
        find_home_dir = 1;
        find_home_vel = 2.0;//rps
      }

      void Init() {
        home_x_no = 5; // home sens
        home_x_level = 'L';
        find_home_dir = 1;
        find_home_vel = 2.0;//rps
      }


      inline void operator = (const origin_param_t* p_cfg) {
        this->home_x_no = p_cfg->home_x_no;
        this->find_home_dir = p_cfg->find_home_dir;
        this->find_home_vel = p_cfg->find_home_vel;
        this->home_x_level = p_cfg->home_x_level;
      }

    };


    struct cfg_t
    {
      uint8_t AxisId{};
      origin_param_t originParam{};
      moons_param_t motion_param{};

#ifdef MOTOR_MOONS_USE_COMM_LIB
      shared_ptr<GMoons_motor_lib> pLib{};
#else
#ifdef USE_HW_COMM_MOONS
      GMoons_comm* pComm{};
#endif
#endif

      inline void operator = (const cfg_t* p_cfg) {
        this->AxisId = p_cfg->AxisId;
        this->originParam = p_cfg->originParam;
        this->motion_param = p_cfg->motion_param;
#ifdef MOTOR_MOONS_USE_COMM_LIB
#else
#ifdef USE_HW_COMM_MOONS
        this->pComm = p_cfg->pComm;
#endif
#endif
      }

      cfg_t()
      {
        AxisId = 0;
        originParam.Init();
      }

      void Init() {
        AxisId = 0;
        originParam.Init();
      }
    };

  private:

    /****************************************************
     *
     ****************************************************/
    cfg_t m_cfg;
    //bool m_IsMotorOn;
    bool m_waitResp;
    
    bool m_IsConnect;
    MOONS_SS_ALARM_STATUS m_mtAlarmCode;
    MOONS_SS_DRIVE_STATUS m_mtStatus;
    int m_motorId;
    IAxis::motion_param_t m_motionParam;
    origin_param_t m_originParam;
    std::vector<uint8_t> m_vBuffer;
    /****************************************************
       *	Constructor
     ****************************************************/
  public:
    GMoons_motor(common::Attribute_t& common_data, cfg_t& cfg)
      : Icommon(common_data), m_cfg(cfg), /*m_IsMotorOn(false),*/ m_mtAlarmCode(), m_mtStatus()
      , m_motionParam(cfg.motion_param), m_originParam(cfg.originParam)
      , m_motorId(cfg.AxisId), m_IsConnect(false), m_waitResp(false), m_vBuffer()
    {

      constexpr double default_rps = 10.0;
      m_motionParam.jog_speed = (uint32_t)default_rps;/*(uint32_t)(m_motionParam.turn_pulse_count * default_rps)*/ //rps
      m_motionParam.jog_accel = 100; //rps/s
      m_motionParam.jog_decel = 100; //rps/s

      m_motionParam.move_velocity = (uint32_t)(default_rps * 2.0); /*(uint32_t)(m_motionParam.turn_pulse_count * default_rps * 2.0); */ //rps
      m_motionParam.move_accel = 100;//rps/s
      m_motionParam.move_decel = 100;//rps/s

      m_motionParam.limit_velocity = (uint32_t)m_motionParam.max_rps;

      m_cfg.pComm->AttCallbackFunc(this, receiveDataFunc);

#ifdef PRJ_USE_FUNC_CLI
      cli::cliAdd(this, "moons", cliMoonsTest);
#endif

    }
    virtual ~GMoons_motor() {
#ifdef USE_HW_MOTOR_MOONS_LIB
      m_cfg.pLib.reset();
#else
#ifdef USE_HW_COMM_MOONS
#endif
#endif			
    }



   /****************************************************
    *	수신 call back
    ****************************************************/
    static inline void receiveDataFunc(void* obj, void* w_parm, void* l_parm) {
      GMoons_motor* pThis = (GMoons_motor*)obj;
      if (w_parm == nullptr)
        return;
      int length = *((int*)w_parm);

      
      /*
      for (size_t i = 0; i < length; i++)
      {        
        pThis->m_vBuffer.emplace_back(((uint8_t*)l_parm)[i]);
      }
      */

#if 1
      int index = 0;
      for (index = 0; index < length; index++)
      {
        uint8_t data = *((uint8_t*)l_parm + index);
        //pThis->m_Que.Put(data);
        pThis->m_vBuffer.emplace_back(data);

      }
#endif


      //pThis->m_vBuffer.reserve((const size_t)length);

      //memcpy(pThis->m_vBuffer.data(), (uint8_t*)l_parm, length);

      pThis->m_waitResp = false;
    }



    /****************************************************
     *	func
     ****************************************************/


    inline uint32_t cal_mms_to_rps_Velocity(uint32_t vel) {
      double target_rps = (double)vel / m_motionParam.one_turn_move_mm;
      if (target_rps > m_motionParam.max_rps)
        target_rps = m_motionParam.max_rps;
      return (uint32_t)target_rps;
    }
#ifdef USE_HW_COMM_MOONS
    inline bool IsMotorOn() {
      getMotorStatus();
      return false; /*m_cfg.pLib->IsMotorOn((short)m_motorId);*/
    }



    inline errno_t MotorOnOff(bool on_off) {
      getMotorStatus();
      return 0;/*m_cfg.pLib->MotorOnOff((short)m_motorId, on_off);*/
    }




    private:
      inline void getMotorStatus() {
        DWORD data = 0;
        if (m_cfg.pComm->IsOpened() == false)
          return;
        using func_e = moons_modbus_func_e;
        using reg_e = moons_SS_reg_e;

        constexpr int array_max = 6;
        uint8_t  func = static_cast<uint8_t>(func_e::read_HoldingReg);
        uint16_t regist_no = static_cast<uint16_t>(reg_e::Alarm_Code_AL_f);
        uint16_t data_cnt = 4;
        std::array<uint8_t, array_max> send_data =
        {
          m_cfg.AxisId,
          func,
          (uint8_t)(regist_no >> 8),
          (uint8_t)regist_no ,
          (uint8_t)(data_cnt >> 8),
          (uint8_t)data_cnt
        };

        constexpr uint32_t timeout =1000;
        uint32_t pass_ms = util::millis();
        m_waitResp = true;
        errno_t result = m_cfg.pComm->SendCmdRxResp(send_data.data(), (uint32_t)send_data.size(), timeout);
        if (result == ERROR_SUCCESS)
        {
          bool ok_recv = true;
          uint32_t pre_ms = util::millis();
          while (m_waitResp)
          {
            if (util::millis() - pre_ms > timeout)
            {
              ok_recv = false;
              m_waitResp = false;
            }
          }
          if (ok_recv)
          {
            uint32_t check_ms = util::millis()- pass_ms;
            uint8_t size = (uint8_t)m_vBuffer.size();
            if (size == 8)
            {
              m_mtStatus.sc_status = (uint16_t)(m_vBuffer[2] << 8) | (uint16_t)(m_vBuffer[3] << 0);
              m_mtAlarmCode.al_status = (uint16_t)(m_vBuffer[0] << 8) | (uint16_t)(m_vBuffer[1] << 0);
            }
            
            m_vBuffer.clear();
          }
        }





      }




#endif


#ifdef USE_HW_MOTOR_MOONS_LIB

    inline void getMotorStatus() {
      DWORD data = 0;
      if (m_cfg.pLib->GetAxisStatus((short)m_motorId, data) == ERROR_SUCCESS)
      {
        m_motorState.sc_status = (uint16_t)data;
      }
    }



    inline errno_t setMotionParam(IAxis::motion_param_t& param) {
      m_motionParam = param;
      return (m_cfg.pLib->SetMototionParam((BYTE)m_motorId, param) == TRUE) ? ERROR_SUCCESS : -1;
    }

#else
#ifdef USE_HW_COMM_MOONS
#endif
#endif


  private:
#ifdef PRJ_USE_FUNC_CLI
    static void cliMoonsTest(void* obj, cli::cli_args_t* args)
    {
      GMoons_motor* pThis = (GMoons_motor*)obj;
      bool ret = true;
      if (args->argc == 3)
      {
        if (args->isStr(0, "write_dio_out"))
        {

        }
        else if (args->isStr(0, "write_rio_out"))
        {

        }
        else
        {
          ret = false;
        }
      }
      else	if (args->argc == 2)
      {
        int value = (int)args->getData(1);
        if (args->isStr(0, "jog"))
        {

        }
        else if (args->isStr(0, "motion"))
        {


        }
        else
        {
          ret = false;
        }
      }
      else if (args->argc == 1)
      {
        errno_t result = 0;
        if (args->isStr(0, "motor_off"))
        {
          result = pThis->MotorOnOff(false);
          if (result == ERROR_SUCCESS)
          {
            cli::cliPrintf("motor_off success\n");
          }
          else
          {
            cli::cliPrintf("motor_off faile\n");
          }
        }
        else if (args->isStr(0, "motor_on"))
        {
          result = pThis->MotorOnOff(true);
          if (result == ERROR_SUCCESS)
          {
            cli::cliPrintf("motor_on success\n");
          }
          else
          {
            cli::cliPrintf("motor_on faile\n");
          }
        }
        else if (args->isStr(0, "alarm_reset"))
        {

        }


        else if (args->isStr(0, "move"))
        {


        }
        else if (args->isStr(0, "init"))
        {
          pThis->Initialize();
        }
        else
        {
          ret = false;
        }
      }
      else
      {
        ret = false;
      }


      if (ret == false)
      {
        cli::cliPrintf("moons motor_on\n");
        cli::cliPrintf("moons motor_off\n");
        cli::cliPrintf("moons alarm_reset\n");
        cli::cliPrintf("moons init\n");
        cli::cliPrintf("moons move\n");


      }
    }
#endif




  public:



#ifdef USE_HW_MOTOR_MOONS_LIB
    inline errno_t GetAlarmType(DWORD& p_state) {
      errno_t ret = -1;
      ret = m_cfg.pLib->GetAlarmType((short)m_motorId, p_state);
      if (ret == ERROR_SUCCESS)
      {
        m_alarmState.al_status = (uint16_t)p_state;
      }
      return ret;
    }



    inline errno_t GetCommandPosition(long& p_position) {
      return m_cfg.pLib->GetCommandPosition((short)m_motorId, p_position);
    }

    inline errno_t GetActualPosition(long& p_position) {
      return  m_cfg.pLib->GetActualPosition((short)m_motorId, p_position);
    }

    /* IAxis  함수*/

    inline errno_t Initialize() {
      errno_t ret = ERROR_SUCCESS;
      if (m_cfg.pLib->Initialize() == ERROR_SUCCESS)
      {
        m_IsConnect = true;
      }
      return ret;
    }



    inline errno_t MotorOnOff(bool on_off) {
      getMotorStatus();
      return m_cfg.pLib->MotorOnOff((short)m_motorId, on_off);
    }

    inline bool IsMotorOn() {
      getMotorStatus();
      return m_cfg.pLib->IsMotorOn((short)m_motorId);
    }

    inline errno_t WaitDone(uint32_t timeout) {
      return m_cfg.pLib->WaitDone((short)m_motorId, (unsigned short)timeout);
    }

    inline bool IsAxisDone() {
      return m_cfg.pLib->IsAxisDone((short)m_motorId);
    }

    inline errno_t  ClearState() {
      return m_cfg.pLib->AlarmReset((short)m_motorId);
    }
    /*inline errno_t AlarmReset() {
      return m_cfg.pLib->AlarmReset((short)m_motorId);
    }*/

    inline uint32_t GetMotorState() {
      errno_t ret = -1;
      DWORD data = 0;
      ret = m_cfg.pLib->GetAxisStatus((short)m_motorId, data);
      if (ret == ERROR_SUCCESS)
      {
        m_motorState.sc_status = (uint16_t)data;
      }
      return ret;
    }

    inline errno_t SetMotionData(motion_param_t& param) {
      return setMotionParam(param);
    }

    inline errno_t OriginAxis() {
      return m_cfg.pLib->OriginAxis((short)m_motorId);
    }


    inline errno_t ClearPosition() {
      return m_cfg.pLib->ClearPosition((short)m_motorId);
    }

    inline errno_t Move(int cmd_pos, uint32_t cmd_vel = 0, uint32_t acc = 100, uint32_t decel = 100) {
      uint32_t velocity_rps = m_motionParam.move_velocity;
      if (cmd_vel > 0)
        velocity_rps = cal_mms_to_rps_Velocity(cmd_vel);
      /*
      m_motionParam.move_accel = acc;
      m_motionParam.move_decel = decel;
      */
      return m_cfg.pLib->Move((short)m_motorId, cmd_pos, (DWORD)velocity_rps);
    }


    inline errno_t Stop() {
      return  m_cfg.pLib->MoveStop((short)m_motorId);
    }

    inline errno_t VStop() {
      return  m_cfg.pLib->MoveStop((short)m_motorId);
    }

    inline errno_t EStop() {
      return  m_cfg.pLib->MoveStop((short)m_motorId);
    }


    inline errno_t JogStop() {
      return  m_cfg.pLib->JogStop((short)m_motorId);
    }


    inline errno_t JogMove(uint32_t cmd_vel = 0, bool is_cw = true) {
      uint32_t velocity_rps = m_motionParam.jog_speed;
      if (cmd_vel > 0)
        velocity_rps = cal_mms_to_rps_Velocity(cmd_vel);
      return  m_cfg.pLib->JogMove((short)m_motorId, (DWORD)velocity_rps, is_cw);
    }



#else
#ifdef USE_HW_COMM_MOONS
#endif
#endif






  };

}

