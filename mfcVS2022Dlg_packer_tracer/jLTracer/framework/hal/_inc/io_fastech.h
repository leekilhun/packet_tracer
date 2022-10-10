#pragma once


#ifdef USE_HW_IO_FASTECH

#define NET_IO_1616_REG_OUT_1       B00000000000000010000000000000000 
#define NET_IO_1616_REG_OUT_2       B00000000000000100000000000000000 
#define NET_IO_1616_REG_OUT_3       B00000000000001000000000000000000 
#define NET_IO_1616_REG_OUT_4       B00000000000010000000000000000000 
#define NET_IO_1616_REG_OUT_5       B00000000000100000000000000000000 
#define NET_IO_1616_REG_OUT_6       B00000000001000000000000000000000 
#define NET_IO_1616_REG_OUT_7       B00000000010000000000000000000000 
#define NET_IO_1616_REG_OUT_8       B00000000100000000000000000000000 
                                    
#define NET_IO_1616_REG_OUT_9       B00000001000000000000000000000000 
#define NET_IO_1616_REG_OUT_10      B00000010000000000000000000000000 
#define NET_IO_1616_REG_OUT_11      B00000100000000000000000000000000 
#define NET_IO_1616_REG_OUT_12      B00001000000000000000000000000000 
#define NET_IO_1616_REG_OUT_13      B00010000000000000000000000000000 
#define NET_IO_1616_REG_OUT_14      B00100000000000000000000000000000 
#define NET_IO_1616_REG_OUT_15      B01000000000000000000000000000000 
#define NET_IO_1616_REG_OUT_16      B10000000000000000000000000000000 




#define NET_IO_FUNC_REG_GET_OUT     0xC5
#define NET_IO_FUNC_REG_SET_OUT     0xC6

#define NET_IO_FUNC_REG_GET_IN      0xC0

#define COMM_BUFF_MAX  4096

constexpr static int DEF_INPUT_BYTE_LENGTH = 4;

class ioFastech:public IIO
{
 /****************************************************
  *	data
  ****************************************************/
public:
  enum class mode_e 
  {
    RS_485,
    ETHERNET,
  };

  enum class err_e
  {
    SUCCESS,
    TIMEOUT = 1,
  };


  struct cfg_t
  {
    ioFastech::mode_e mode{};
    IComm* p_Comm{};
    
    //netSocket::cfg_t net_cfg{};
    //SerialComm::cfg_t serial_cfg{};
    
    //void* p_cfg{ new void* () };
    
    inline void operator = (const cfg_t* p_cfg) {
      this->mode = p_cfg->mode;
      this->p_Comm = p_cfg->p_Comm;
      //this->serial_cfg = p_cfg->serial_cfg;
    }

    ~cfg_t() {
      /*if (p_cfg)
      {
        delete p_cfg;
      }
      p_cfg = nullptr;*/
    }
  };


  struct netIO_receiveData
  {
    uint8_t length{};
    uint8_t syncNo{};
    uint8_t reserved{};
    uint8_t frameType{};
    uint8_t comm_state{};
    uint8_t input[4]{};
    uint8_t latch[4]{};

  };


private:

  cfg_t m_cfg;
  netIO_receiveData m_receiveData;

  buffer::_que<uint8_t> m_Que{ COMM_BUFF_MAX };
  bool m_TrdLife;
  HANDLE m_hThread;
  DWORD m_TrdId;



  /****************************************************
   *	Constructor
   ****************************************************/
  public:
    ioFastech(ioFastech::cfg_t& cfg);
    virtual ~ioFastech();

  /****************************************************
   *	func
   ****************************************************/
private:
  bool receivePacket();
  void threadStop(void);
  void threadRun(void);
  void threadJob(void);
  static UINT threadFunc(LPVOID pParam);
  static void receiveDataCB(void* obj, void* w_parm, void* l_parm);

public:
  bool IsOn(uint32_t addr) {
    return false;
  }
  bool IsOff(uint32_t addr) {
    return false;
  }
  errno_t OutputOn(uint32_t addr) {
    return errno_t();
  }
  errno_t OutputOff(uint32_t addr) {
    return errno_t();
  }
  errno_t OutputToggle(uint32_t addr) {
    return errno_t();
  }



};

#endif
