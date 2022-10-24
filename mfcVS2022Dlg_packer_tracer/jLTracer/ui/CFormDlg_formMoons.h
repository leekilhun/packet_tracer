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
    move_accel = 0x001B,
    move_decel = 0x001C,
    move_velocity = 0x001D,
    move_distance = 0x001F,
    jog_accel = 0x002E,
    jog_decel = 0x002F,
    jog_velocity = 0x0030,
    max_velocity = 0x0031,

    set_dest_distance = 0x001e,
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


  /*
  
  Hex Value BLu SV STAC6 ST STM
  0x0001     Position Limit
  0x0002     CCW Limit
  0x0004     CW Limit
  0x0008     Over Temp
  0x0010     Excess Regen* Internal Voltage Excess Regen Internal Voltage Internal Voltage
  0x0020     Over Voltage
  0x0040     Under Voltage* Under Voltage Under Voltage Under Voltage Under Voltage
  0x0080     Over Current
  0x0100     Bad Hall Sensor Open Motor Winding
  0x0200     Bad Encoder (not used)
  0x0400     Comm Error
  0x0800     Bad Flash
  0x1000     Wizard Failed No Move
  0x2000     Current Foldback Motor Resistance Out of Range (not used) (not used)
  0x4000     Blank Q Segment
  0x8000     No Move (not used) 
  
  */
  union ALARM_STATUS
  {
    uint16_t	al_status;
    struct
    {
      unsigned	Position_Error : 1;			// = 0x00000001;
      unsigned	CCW_Limit : 1;			      // = 0x00000002;
      unsigned	CW_Limit : 1;			  // = 0x00000004;
      unsigned	Over_Temp : 1;	      // = 0x00000008;
      unsigned	Internal_Voltage : 1;	            // = 0x00000010;
      unsigned	Over_Voltage : 1;		        // = 0x00000020;
      unsigned	Under_Voltage: 1;			      // = 0x00000040;
      unsigned	Over_Current : 1;	// = 0x00000080;
      unsigned	Open_Winding : 1;	            // = 0x00000100;
      unsigned	Bad_Encoder : 1;		  // = 0x00000200;
      unsigned	Comm_Error : 1;		          // = 0x00000400;
      unsigned	Bad_Flash : 1;		        // = 0x00000800;
      unsigned	No_Move: 1;// = 0x00001000;
      unsigned	Current_Foldback: 1;			  // = 0x00002000;
      unsigned	Blank_Q_Segment : 1;		      // = 0x00004000;
      unsigned	NV_Memory_Double_err : 1;			    // = 0x00008000;
    };
  };
  /*
  
  Hex Value  Status Code bit definition
  0x0001     Motor Enabled (Motor Disabled if this bit = 0)
  0x0002     Sampling (for Quick Tuner)
  0x0004     Drive Fault (check Alarm Code)
  0x0008     In Position (motor is in position)
  0x0010     Moving (motor is moving)
  0x0020     Jogging (currently in jog mode)
  0x0040     Stopping (in the process of stopping from a stop command)
  0x0080     Waiting for an input (executing WI command)
  0x0100     Saving (parameter data is being saved)
  0x0200     Alarm present (check Alarm Code)
  0x0400     Homing (executing an SH command)
  0x0800     Wait Time (executing a WT command)
  0x1000     Q Program is running
  0x2000     Initializing (happens at power up)
  0x4000     not used
  0x8000     not used
  0x  
  */
  union DRIVE_STATUS
  {
    uint16_t	sc_status;
    struct
    {
      unsigned	Motor_Enabled : 1;			// = 0x00000001;
      unsigned	Sampling : 1;			      // = 0x00000002;
      unsigned	Drive_Fault : 1;			  // = 0x00000004;
      unsigned	In_Position : 1;	      // = 0x00000008;
      unsigned	Moving : 1;	            // = 0x00000010;
      unsigned	Jogging : 1;		        // = 0x00000020;
      unsigned	Stopping : 1;			      // = 0x00000040;
      unsigned	Waiting_for_input : 1;	// = 0x00000080;
      unsigned	Saving : 1;	            // = 0x00000100;
      unsigned	Alarm_present : 1;		  // = 0x00000200;
      unsigned	Homing : 1;		          // = 0x00000400;
      unsigned	WaitTime : 1;		        // = 0x00000800;
      unsigned	QProgram_is_running : 1;// = 0x00001000;
      unsigned	Initializing : 1;			  // = 0x00002000;
      unsigned	not_used0 : 1;		      // = 0x00004000;
      unsigned	not_used1 : 1;			    // = 0x00008000;
    };
  } ;

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
  DRIVE_STATUS m_mtStatus;
  ALARM_STATUS m_mtAlarmCode;
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
	afx_msg void OnBnClickedMoonsBtnJogCw();



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
  int SendData(char* p_data, uint32_t length);
  bool sendPacketWaitRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout);
  afx_msg void OnBnClickedMoonsBtn1();
  afx_msg void OnBnClickedMoonsBtn2();
  afx_msg void OnBnClickedMoonsBtn3();
  afx_msg void OnBnClickedMoonsBtnJogCcw();
  afx_msg void OnBnClickedMoonsBtn4();
  afx_msg void OnBnClickedMoonsBtn5();
private:
    CEdit m_txtIntervalTime;
    CButton m_chkMonitor;
    CEdit m_txtDriveStatus;
    CEdit m_txtAlarmCode;
    CEdit m_txtActualSpeed;
    CEdit m_txtTargetSpeed;
    CEdit m_txtEncoderPos;
    CEdit m_editVelocity;
    CEdit m_editAccel;
    CEdit m_editDecel;
    CEdit m_editAbsPosition;
    CEdit m_editRelPosition;
    CComboBox m_cmbDirection;
public:
    afx_msg void OnBnClickedMoonsBtnPosmodeAbsmoveStart();
    afx_msg void OnBnClickedMoonsBtnPosmodeAbsmoveStop();
    afx_msg void OnBnClickedMoonsBtnPosmodeRelmoveStart();
    afx_msg void OnBnClickedMoonsBtnPosmodeRelmoveStop();
private:
  void updateMotorStatus();
  void motorStop();
public:
  afx_msg void OnBnClickedMoonsBtn6();
private:
  int setPosition(int position);
  void startJogging();
  int setJogAccel(uint16_t accel);
  int setJogDecel(uint16_t decel);
  int setMaxVelocity(uint16_t velocity);
  int setAccel(uint16_t accel);
  int setDecel(uint16_t decel);
};
