#pragma once
#include "afxdialogex.h"


// CformMoons 대화 상자
constexpr int MOONS_PACKET_BUFF_LENGTH = 100;
constexpr int MOONS_MULTI_WRITE_DATA_LENGTH = 10;

class CjLTracerDlg;

class CformMoons : public CDialogEx
{
  /****************************************************
   *	data
   ****************************************************/
  enum class reg_e {
    alarm_code = 0x0000,
    cmd_Opcode = 0x007c,
  };


  enum class Opcode_e {
    alarm_reset = 0xBA,
    start_jogging = 0x96,
    stop_jogging = 0xD8,
    encoder_function = 0xD6,
    encoder_position = 0x98,
    feed_to_double_sensor = 0x69,
    follow_encoder = 0xCC,
    feed_to_length = 0x66,
    feed_to_sensor_with_mask_distance = 0x6A,
    feedand_set_output = 0x68,
    feed_to_position = 0x67,
    feed_to_sensor = 0x6B,
    feed_to_sensor_with_safety_distance = 0x6C,
    jog_disable = 0xA3,
    jog_enable = 0xA2,
    motor_disable = 0x9E,
    motor_enable = 0x9F,
    seek_home = 0x6E,
    set_position = 0xA5,
    filter_input = 0xC0,
    filter_select_inputs = 0xD3,
    step_filter_freq = 0x06,
    analog_deadband = 0xD2,
    alarm_reset_input = 0x46,
    alarm_output = 0x47,
    analog_scaling = 0xD1,
    define_limits = 0x42,
    set_output = 0x8B,
    wait_for_input = 0x70,
    queue_load_and_execute = 0x78,
    wait_time = 0x6F,
    stop_move_kill_buffer = 0xE1,
    stop_move_kill_Buffer_normal_decel = 0xE2,

  };


  /*
  // SCL Command Encoding Table

  Function                              SCL  Opcode   Param1               Param2             Param3         Param4        Param5
  Alarm Reset                           AX    0xBA
  Start Jogging                         CJ    0x96
  Stop Jogging                          SJ    0xD8
  Encoder Function                      EF    0xD6       0,1,2 or 6
  Encoder Position                      EP    0x98       Position
  Feed to Double Sensor                 FD    0x69       I/O Point 1        Condition 1     I/O Point 2     Condition 2
  Follow Encoder                        FE    0xCC       I/O Point          Condition
  Feed to Length                        FL    0x66
  Feed to Sensor with Mask Distance     FM    0x6A       I/O Point          Condition
  Feed and Set Output                   FO    0x68       I/O Point          Condition
  Feed to Position                      FP    0x67
  Feed to Sensor                        FS    0x6B       I/O Point          Condition
  Feed to Sensor with Safety Distance   FY    0x6C       I/O Point          Condition
  Jog Disable                           JD    0xA3
  Jog Enable                            JE    0xA2
  Motor Disable                         MD    0x9E
  Motor Enable                          ME    0x9F
  Seek Home                             SH    0x6E       I/O Point          Condition
  Set Position                          SP    0xA5       Position
  Filter Input                          FI    0xC0       I/O Point          Filter Time
  Filter Select Inputs                  FX    0xD3
  Step Filter Freq                      SF    0x06       Freq
  Analog Deadband                       AD    0xD2       0.001 V
  Alarm Reset Input                     AI    0x46       Function(‘1’..’3’) I/O Point
  Alarm Output                          AO    0x47       Function(‘1’..’3’) I/O Point
  Analog Scaling                        AS    0xD1
  Define Limits                         DL    0x42       1..3
  Set Output                            SO    0x8B       I/O Point          Condition
  Wait for Input                        WI    0x70
  Queue Load & Execute                  QX    0x78       1..12
  Wait Time                             WT    0x6F       0.01 sec
  Stop Move, Kill Buffer                SK    0xE1
  Stop Move, Kill Buffer, Normal Decel  SKD   0xE2

*/








  enum class func_e {
    read_HoldingReg = 0x03,
    read_InputReg = 0x04,
    write_SingleReg = 0x06,
    write_MultiReg = 0x10

  };


  //union MOONS_STATUS
  //{
  //  int	iStatus;
  //  struct
  //  {
  //    unsigned	AZ_FLAG_M0_R : 1;			// = 0x00000001;
  //    unsigned	AZ_FLAG_M1_R : 1;			// = 0x00000002;
  //    unsigned	AZ_FLAG_M2_R : 1;			// = 0x00000004;
  //    unsigned	AZ_FLAG_START_R : 1;	// = 0x00000008;
  //    unsigned	AZ_FLAG_HOME_END : 1;	// = 0x00000010;
  //    unsigned	AZ_FLAG_READY : 1;		// = 0x00000020;
  //    unsigned	AZ_FLAG_INFO : 1;			// = 0x00000040;
  //    unsigned	AZ_FLAG_ALM_A : 1;		// = 0x00000080;
  //    unsigned	AZ_FLAG_SYS_BSY : 1;	// = 0x00000100;
  //    unsigned	AZ_FLAG_AREA0 : 1;		// = 0x00000200;
  //    unsigned	AZ_FLAG_AREA1 : 1;		// = 0x00000400;
  //    unsigned	AZ_FLAG_AREA2 : 1;		// = 0x00000800;
  //    unsigned	AZ_FLAG_TIM : 1;			// = 0x00001000;
  //    unsigned	AZ_FLAG_MOVE : 1;			// = 0x00002000;
  //    unsigned	AZ_FLAG_IN_POS : 1;		// = 0x00004000;
  //    unsigned	AZ_FLAG_TLC : 1;			// = 0x00008000;
  //  };
  //} ;

  //ORIENRAL_AZ_STATUS m_regAxis;



  struct tx_packet_t
  {
    uint8_t   node_id{};
    uint8_t   func_type{};
    uint16_t  reg_addr{};
    uint16_t  reg_length{};
    uint16_t  content{};  // case moons motor
    uint8_t   content_cnt{};

    uint16_t   check_sum{};
    uint16_t   check_sum_recv{};
    uint8_t   buffer[MOONS_PACKET_BUFF_LENGTH]{};
  };

  struct rx_packet_t
  {
    uint8_t   node_id{};
    uint8_t   func_type{};
    uint16_t  reg_addr{};
    uint16_t  reg_length{};
    uint8_t   data_length{};
    uint8_t* data{};
    uint16_t  check_sum{};
    uint16_t  check_sum_recv{};
    uint8_t   buffer[MOONS_PACKET_BUFF_LENGTH]{};
  };


  struct modbus_t {
    uint8_t   state{};
    uint32_t  pre_time{};
    uint32_t  index{};
    uint32_t  data_cnt{};
    uint8_t   error{};

    tx_packet_t  tx_packet;
    rx_packet_t  rx_packet;
  };



public:
  struct cfg_t
  {
    IComm* pComm{};
    uint8_t AxisId{};

  };

private:
  UINT_PTR m_TimerID;
  buffer::_que<uint8_t> m_Que{ 4096 };
  modbus_t m_packet;


  CjLTracerDlg* m_pParent;
  HWND m_hWind;
  apSystem* m_pSystem;
  SerialComm* m_pSerialport;
  IComm* m_pComm;
  uint8_t m_nodeId;
  bool m_TrdLock;
  /*
  uint8_t m_AxisId;
  bool m_IsMotorOn;
  buffer::_que<uint8_t> m_Que;
  modbus_t m_packet;

  void* m_cbObj;
  evt_cb m_func;

  bool m_TrdLock;
  bool m_TrdLife;
  HANDLE m_hThread;
  DWORD m_TrdId;*/




	DECLARE_DYNAMIC(CformMoons)

public:
	CformMoons(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CformMoons();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM_MOONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();



  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
  static void receiveDataFunc(void* obj, void* w_parm, void* l_parm);
  void update();
  bool receivePacket();
  void receiveCplt();
public:
  // 데이터 전송
  errno_t Init(CformMoons::cfg_t& cfg);
  int SendPacket(char** args, uint32_t argc);
  bool sendPacketWaitRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout);
};
