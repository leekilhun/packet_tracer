#pragma once

/*

maker - Sick
model - OD2-P250W
comm  - RS-422 통신



1. General information
  1) Data format         
     -> 8n1
  2) Communication request and change
     • Command format ASCII data embedded into STX and ETX, e.g. <STX>xxx<ETX>
     • Request of parameter setting with 1 command (parameter)
     • Change setting of parameter with 2 commands seperated by space (parameter and setting), e.g. <STX>AVG FAST<ETX>.
       For continual data output values are separated by CR e.g. xxx<CR>xxx<CR>xxx
  3) Baud rate 
     -> 9.6 kBaud (Default

2. Command Description
  1) START_MEASURE 
     Start continual measurement output
  2) STOP_MEASURE 
     Stop continual measurement output
  3) MEASURE 
     Read out measurement once
  4) START_Q2 
     Start continual Q2 output
  5) STOP_Q2 
     Stop continual Q2 output
  6) Q2 
     Read out status of Q2
  7) Q2_HI
     Read out setting of Q2 Hi
  8) Q2_LO 
     Read out setting of Q2 Lo
  9) Q2_HI 60.000 
     Set Q2 Hi for example to "60 mm"
  10) Q2_LO 40.000 
     Set Q2 Lo for example to "40 mm"
  11) Q2 DEFAULT 
     Set Q2 to default (Health output)
  12) AVG 
     Read out setting of the speed (Averaging)
  13) AVG FAST 
     Set Avg to "fast"
  14) AVG MEDIUM 
     Set Avg to "medium"
  15) AVG SLOW 
     Set Avg to "slow"
  16) MF 
     Read out setting of MF (multifunctional input)
  17) MF SR OFF 
     Set function of MF to "Laser off"
  18) MF SH 
     Set function of MF to "Trigger"
  19) MF TEACH 
     Set function of MF to "External Teach"
  20) ALARM 
     Read out setting for alarm
  21) ALARM CLAMP 
     Set behavior during alarm to give out "maximum value" (Clamp)
  22) ALARM HOLD 
     Set behavior during alarm to hold last "good measurement value" (Hold)
  23) RESET
     Reset all settings to default settings
  24) ON 
     Activate MF
  25) OFF 
     Deactivate MF
  26) BIT_RATE 
     Read setting for baud rate
  27) BIT_RATE 9.6k 
     Set baud rate to "9.6 kBaud“ (Default), baud rate can be set to:
     19.2 kBaud, 38.4 kBaud, 57.6 kBaud, 76.8 kBaud, 115.2 kBaud,
     128.0 kBaud, 230.4 kBaud, 256.0 kBaud, 312.5 kBaud, 460.8 kBaud, 625.0 kBaud or 1250.0 kBaud


3. Responses 
  ? 
   -> Failure
  > 
   -> Confirm
  40.000 
   -> Measurement value 40.000. The returned value represents real distance to object in mm. 
    When no measurement is possible a value higher than end of measuring range is given
  xxx 
   -> Setting e.g. SH, LSR_OFF etc.


The following parameters are reset to factory settings.
1. Start of measuring range => 4mA (0V) [analog output model only].
2. End of measuring range => 20mA (10V) [analog output model only].
3. Q1 => Complete meaasuring range [except communication model].
4. Q2 => Health output. / 5. Avg => Medium. / 6. MF => Laser off.
7. Alarm => Clamp. / 8. Baud rate => 9600bps [Communication model only].
9. Sampling rate => 500us (250mm or longer type 750us).



*/


#ifdef USE_HW_DIST_SENSOR_SICK


constexpr static int DEF_SICK_COMM_STX                = 0x02; //ascll <STX>
constexpr static int DEF_SICK_COMM_ETX                = 0x03; //ascll <ETX>
constexpr static int DEF_SICK_COMM_CR                 = 0x0D; //ascll Carriage return

constexpr static int DEF_SICK_COMM_RET_OK             = 0x3F; // '?' confirm
constexpr static int DEF_SICK_COMM_RET_FAIL           = 0x5E; // '^' fail
constexpr static int DEF_SICK_COMM_RET_SPACE          = 0x20; // ' ' seperator


constexpr static char* DEF_SICK_CMD_START_MEASURE     = "START_MEASURE";

constexpr static char* DEF_SICK_CMD_STOP_MEASURE      = "STOP_MEASURE";
constexpr static char* DEF_SICK_CMD_MEASURE           = "MEASURE";
constexpr static char* DEF_SICK_CMD_START_Q2          = "START_Q2";
constexpr static char* DEF_SICK_CMD_STOP_Q2           = "STOP_Q2";
constexpr static char* DEF_SICK_CMD_Q2                = "Q2";
constexpr static char* DEF_SICK_CMD_Q2_HI             = "Q2_HI";
constexpr static char* DEF_SICK_CMD_Q2_LO             = "Q2_LO";
constexpr static char* DEF_SICK_CMD_Q2_DEFAULT        = "Q2 DEFAULT";

constexpr static char* DEF_SICK_CMD_AVG               = "AVG";
constexpr static char* DEF_SICK_CMD_AVG_FAST          = "AVG FAST";
constexpr static char* DEF_SICK_CMD_AVG_MEDIUM        = "AVG MEDIUM";
constexpr static char* DEF_SICK_CMD_AVG_SLOW          = "AVG SLOW";
constexpr static char* DEF_SICK_CMD_MF                = "MF";
constexpr static char* DEF_SICK_CMD_MF_SR_OFF         = "MF SR OFF";
constexpr static char* DEF_SICK_CMD_MF_SH             = "MF SH";
constexpr static char* DEF_SICK_CMD_MF_TEACH          = "MF TEACH";
constexpr static char* DEF_SICK_CMD_ALARM             = "ALARM";
constexpr static char* DEF_SICK_CMD_ALARM_CLAMP       = "ALARM CLAMP";
constexpr static char* DEF_SICK_CMD_ALARM_HOLD        = "ALARM HOLD";
constexpr static char* DEF_SICK_CMD_RESET             = "RESET";
constexpr static char* DEF_SICK_CMD_ON                = "ON";
constexpr static char* DEF_SICK_CMD_OFF               = "OFF";
constexpr static char* DEF_SICK_CMD_BIT_RATE          = "BIT_RATE";


#define SICK_COMM_BUFF_MAX	4096
#define SICK_PACKET_BUFF_MAX	64

class GDistSensor_sick:IDistMeasure
{
  /****************************************************
   *	data
   ****************************************************/
public:
  struct cfg_t
  {
    IComm* p_Comm{};
  };

  enum class error_code
  {
    Success
  };


  struct packet_t
  {
    uint8_t   state{};
    uint8_t   index{};
    uint8_t   data_len{};

    uint8_t   buffer[SICK_PACKET_BUFF_MAX]{};
    uint8_t*  data{};

    packet_t() {

    }

    void init() {
      state = 0;
      index = 0;
      data_len = 0;

      memset(buffer, 0x00, SICK_PACKET_BUFF_MAX);
      data = nullptr;
    }

  };

private:
  cfg_t  m_cfg;
  float m_lastValue;
  packet_t m_packetData;
  uint32_t m_receiveTime;

  bool m_IsReceiveContinue;
  util::_que<uint8_t> m_Que;
  util::_que<double> m_QmesureValue{100};


  bool m_TrdLife;
  HANDLE m_hThread;
  DWORD m_TrdId;

  /****************************************************
   *	Constructor
   ****************************************************/
public:
  GDistSensor_sick(common::Attribute_t common_data, GDistSensor_sick::cfg_t& cfg);

  virtual ~GDistSensor_sick();

  /****************************************************
   *	func
   ****************************************************/
private:
  bool receiveMeausrData();
  bool receiveRespone();
  
  bool receiveContinueData();

  void threadStop(void);
  void threadRun(void);
  void threadJob(void);
  static UINT threadFunc(LPVOID pParam);
  static void receiveDataCB(void* obj, void* w_parm, void* l_parm);

public:
  errno_t Open();
  void Close();
  bool IsOpened() const;
  void Recovery();
  double GetDist();



  double CmdMeasureDist();
  bool CmdStartMeasure();
  bool CmdStopMeasure();

};



#endif