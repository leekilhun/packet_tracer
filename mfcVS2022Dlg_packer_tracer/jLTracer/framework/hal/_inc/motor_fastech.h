#pragma once
#include "motor_fastech_reg.h"
#include "motor_fastech_comm.h"


#ifdef USE_HW_MOTOR_FASTECH

/*
   h/w     vendor
             | comm_if   -------  serial
             | register     |---  ethernet

   hw의 adstraction interface 포함하며
   컨트롤러와 interface할 포트 클래스 (시리얼 또는 이더넷)
   멤버 요소로 가지 있다.
   통신으로 수신된 정보를 파싱하는 클래스를 포함

*/



#define FASTECH_CMD_MAX_DATA_LENGTH  248
#define FASTECH_IO_ADDR_INPUT_MAX      9
#define FASTECH_IO_ADDR_OUTPUT_MAX     9


class fastech_motor :public IAxis, virtual public IIO
{
  /****************************************************
   *	data
   ****************************************************/
  enum class in_def {
    limit_plus, limit_minus,
    origin, clear_pos,
    pt_a0, pt_a1, pt_a2, pt_a3, pt_a4,
    pt_a5_user, pt_a6_user, pt_a7_user,
    pt_start, stop,
    jog_plus, jog_minus,
    alarm_reset, servo_on, pause, origin_search, teaching, e_stop,
    jpt_in_0, jpt_in_1, jpt_in_2, jpt_start,
    user_in_0, user_in_1, user_in_2, user_in_3, user_in_4,
    user_in_5, user_in_6, user_in_7, user_in_8,
    _max
  };

  enum class out_def {
    compare_out, inposition, alarm, moving, acc_dec, ack, end, alarm_blink,
    org_search_ok, servo_ready, reserved, brake,
    pt_out0, pt_out1, pt_out2,
    user_out_0, user_out_1, user_out_2, user_out_3, user_out_4,
    user_out_5, user_out_6, user_out_7, user_out_8,
    _max
  };


  enum class err_type {
    error_all,
    hw_plus_limit,
    hw_minus_limit,
    sw_plus_limit,
    sw_minus_limit,
    reserved_05,
    pos_cnt_over,
    servo_alarm,

    over_current,
    over_speed,
    step_out,
    over_load,
    over_heat,
    back_EMF,
    motor_power,
    inposition,
    _max
  };
#define FM_ERR(x)          static_cast<uint8_t>(jL_fastechMotor::err_type::x)

  enum class cmd_t
  {
    GET_SLAVE_INFO = 0x01,
    GET_MOTOR_INFO = 0x05,
    SAVE_ROM_ALL = 0x10,
    GET_ROM_PARAM = 0x11,
    SET_RAM_PARAM = 0x12,
    GET_RAM_PARAM = 0x13,
    SET_IO_OUT_REG = 0x20,
    SET_IO_IN_REG = 0x21,
    GET_IO_IN_REG = 0x22,
    GET_IO_OUT_REG = 0x23,
    SET_ASSN_IO_MAP = 0x24,
    GET_ASSN_IO_MAP = 0x25,
    READ_IO_MAP = 0x26,
    TRG_OUT_RUN_A = 0x27,
    TRG_OUT_STATUS = 0x28,
    SERVO_ENABLE = 0x2A,
    ALARAM_RESET = 0x2C,
    GET_ALARM_INFO = 0x2E,
    MOVE_STOP = 0x31,
    MOVE_EMG_STOP = 0x32,
    MOVE_ORG_SINGLE_AXIS = 0x33,
    MOVE_ABS_SINGLE_AXIS = 0x34,
    MOVE_REL_SINGLE_AXIS = 0x35,
    MOVE_TO_LIMIT = 0x36,
    MOVE_VELOCITY = 0x37,
    MOVE_ABS_OVERRIDE = 0x38,
    MOVE_REL_OVERRIDE = 0x39,
    MOVE_VELOCITY_OVERRIDE = 0x3A,
    MOVE_ALL_STOP = 0x3B,
    MOVE_ALL_EMG_STOP = 0x3C,
    MOVE_ALL_ORG_STOP = 0x3D,
    MOVE_ALL_ABS_POS = 0x3E,
    MOVE_ALL_REL_POS = 0x3F,
    GET_AXIS_STATUS = 0x40,
    GET_AXIS_IO_STATUS = 0x41,
    GET_MOTION_STATUS = 0x42,
    GET_ALL_STATUS = 0x43,
    CLEAR_POSITION = 0x56,
  };
#define FM_COMM_CMD(x) static_cast<byte>(fastech_motor::cmd_t::x)

  enum class state_t
  {
    wait_stx,
    wait_id,
    wait_sync,
    wait_type,
    wiat_response,
    wait_data,
    skip_stuffing,
    wait_checksum,
    wait_etx,
  };
#define FM_COMM_ST(x) static_cast<uint8_t>(fastech_motor::state_t::x)

  struct packet_t
  {
    uint8_t	  id{};
    uint8_t   rec_sync_no{};
    uint8_t   type{};
    uint8_t   response{};
    uint16_t  length{};
    uint32_t  check_sum{};
    uint16_t  check_sum_recv{};
    uint8_t   buffer[FASTECH_CMD_MAX_DATA_LENGTH + 12]{};
    uint8_t* data;
  };

  struct comm_t {
    bool      is_init{};
    bool			wait_next{};
    bool      wait_resp{};
    uint32_t  resp_time{};
    uint32_t  baud{};
    uint8_t   state{};
    uint8_t   sync_no{};
    uint16_t  packet_size{};
    uint32_t  index{};
    uint8_t   error{};
    packet_t  rx_packet{};
  };

public:
  struct motor_state_t
  {
    uint8_t	  id{};
    uint8_t   sync_no{};
    uint8_t   is_motor_on{};
    uint16_t  alarm{};
    uint16_t  encoder{};
    flag::fdata   axis_status;
    flag::fdata    io_in{ 5 };  // 모터 인풋 레지스터는 5바이트 
    flag::fdata    io_out{ 4 }; // 모터 아우풋 레지스터는 4바이트 
    int       act_pos{};
    int       cmd_pos{};
    uint32_t  act_vel{};
    int       pos_err{};
    uint32_t  pt_item{};
  };

  uint8_t m_AxisId;
  motor_state_t m_AxisState;


  struct cfg_t
  {
    IComm* pComm{};
    uint8_t AxisId{};
    sysTimer* pSysTimer{};
  };



private:
  comm_t m_Packet;
  int m_sysTimerId;
  sysTimer* m_pSysTimer;
  cfg_t m_Cfg;
  fm_parameter m_motorParam;
  IComm* m_pComm;


  /*
  fm_parameter m_motorParam;
  _Scheduler* m_jobScheduler;
  timer::_ms m_timer;
  uint32_t m_responseTime;
  bool m_useISR;
  */

  /****************************************************
   *	Constructor
   ****************************************************/
public:
  fastech_motor(common::Attribute_t common_data, cfg_t cfg);
  ~fastech_motor();


  /****************************************************
   *	func
   ****************************************************/
private:
  static void receiveData(void* obj, void* w_parm, void* l_parm);
  static void timerFunc(void* obj);
  errno_t update_AxisState(/*uint8_t* p_data, uint16_t length, uint8_t index*/);

public:
  /*  함수*/
  uint32_t  GetAllState();
  uint32_t SendCommand(cmd_t cmd, uint8_t* p_data, uint8_t length);
  void LoadingParamterData();



  /* IAxis  함수*/
  errno_t Initialize();
  errno_t MotorOnOff(bool on_off);
  bool IsMotorOn();
  errno_t WaitDone(uint8_t mode);
  bool  IsAxisDone();
  errno_t  ClearState();
  uint32_t  GetMotorState();

  int Move(int cmd_pos, uint32_t cmd_vel = 100, uint32_t acc = 100, uint32_t decel = 100);
  //축을 정지한다. (한개의 축에 대한 정지)
  int Stop();
  //축을 등속이동에 대해 정지한다.
  int VStop();
  // 축을 비상정지한다.
  int EStop();

  int JogMove(uint32_t cmd_vel = 1000, bool is_cw = true);
  int JogMove(uint32_t jog_pos, uint32_t cmd_vel = 1000, bool is_cw = true);






  /* IIO  함수*/
  bool IsOn(uint32_t addr);
  bool IsOff(uint32_t addr);
  errno_t OutputOn(uint32_t addr);
  errno_t OutputOff(uint32_t addr);
  errno_t OutputToggle(uint32_t addr);


};

#endif