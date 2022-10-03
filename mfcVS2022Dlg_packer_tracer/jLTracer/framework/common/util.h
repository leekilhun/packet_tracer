#pragma once


/*
 * util.h
 *
 * 작성자 : 이길훈
 * 22.08.30
 * - 프로젝트 전역에서 사용될 함수 모음
 * - 06.25 - json 파싱 func
 */


#include <stdint.h>
#include <queue>
#include <chrono>
#include <string>

#include "binary.h"

using namespace std;

/// <summary>
/// data파일에서 데이터를 추출
/// json 타입 적용
/// </summary>
namespace parser
{
  constexpr int _KEY_STR_MAX = PRJ_NAME_STR_LENGTH_MAX;
  constexpr int _NAME_STR_MAX = PRJ_NAME_STR_LENGTH_MAX;
  constexpr int _VALUE_STR_MAX = PRJ_NAME_STR_LENGTH_MAX;
  constexpr int _ARG_CNT_MAX = 64;
  constexpr int _PARSING_LINE_STR_MAX = 64;
  constexpr int _JSON_DATA_MAX = 1024;

  enum class valueType_e
  {
    none,
    string,
    integer,
    realnumber,
    boolian,
    key_value,
    type_max
  };


  struct key_t
  {
    char  key[_KEY_STR_MAX]{};
    valueType_e type = valueType_e::none;       // 토큰 종류
    union {                                     // 두 종류 중 한 종류만 저장할 것이므로 공용체로 만듦
      bool   yesorno;                           // boolian 타입
      char  str_value[_VALUE_STR_MAX]{};        // 문자열 포인터
      int    number;
      double realnumber;                        // 실수형 숫자
    };
    key_t() = default;
  };

  struct json_t
  {
    char  key[_KEY_STR_MAX]{};
    valueType_e type = valueType_e::none;       // 토큰 종류
    union {                                     // 두 종류 중 한 종류만 저장할 것이므로 공용체로 만듦
      bool   yesorno;                           // boolian 타입
      char  str_value[_VALUE_STR_MAX]{}; // 문자열 포인터
      int    number;
      double realnumber;                        // 실수형 숫자
      key_t* key_value;
    };
    bool is_array = false;       // 현재 토큰이 배열인지 표시
  };

  /// <summary>
  /// json타입 데이터 파싱 클래스
  /// </summary>
  class json
  {
    json_t* m_pData;
    uint32_t m_incNo;
    uint32_t m_dataMax;
    char m_dirFile[PRJ_FILE_DIR_STR_MAX]{};


  public:
    json(char* file_info, uint32_t data_max) :m_pData(new json_t[data_max]), m_incNo(0), m_dirFile{}
    {
      sprintf_s(m_dirFile, PRJ_FILE_DIR_STR_MAX - 1, file_info);
      m_dataMax = data_max;
    }

    virtual ~json()
    {
      while (m_incNo > 0)
      {
        if (m_pData[m_incNo].type == valueType_e::key_value)
        {
          delete m_pData[m_incNo].key_value;
        }
        --m_incNo;
      }

      delete[] m_pData;
    }

    /// <summary>
    /// 파싱할 json파일을 로딩
    /// </summary>
    /// <returns>로딩한 파일 사이즈</returns>
    int LoadData();

    /// <summary>
    /// 파싱한 데이터를 출력
    /// </summary>
    void PrintData() {
      string total_table;
      string key_ref;
      bool is_arry_head = false;
      //bool is_key_head = false;
      //int* col_max_wide = new int[max_col_size];
      // 
      // 맨 상단에 열 정보 표시
      total_table = '{';
      total_table += "\r\n";
      total_table += "// Print Data Start";
      total_table += "\r\n";
      for (uint32_t i = 0; i < m_incNo; i++) {
        if (i > 0 && !is_arry_head)
        {
          total_table += ",\r\n";
        }

        if (m_pData[i].is_array)
        {
          if (is_arry_head == false)
          {
            is_arry_head = true;
            total_table += putStr(m_pData[i].key) + ':' + "\r\n" + '[';
          }
          else
          {
            if (!is_contaArry(key_ref, m_pData[i].key))
            {
              //is_arry_head = false;
              total_table += "],\r\n";
              total_table += putStr(m_pData[i].key) + ':' + "\r\n" + '[';
            }
            else
            {
              total_table += ',';
            }
          }

          key_ref = m_pData[i].key;
          if (m_pData[i].type == valueType_e::key_value)
          {
            /* if (is_key_head)
             {
               total_table += ',';
             }
             is_key_head = true;*/

            total_table += '{';
            total_table += putStr(m_pData[i].key_value->key) + ':';
            total_table += putData(m_pData[i].key_value->type, i, true);
            total_table += '}';
          }
          else
          {
            total_table += putData(m_pData[i].type, i);
          }

        }
        else
        {
          if (is_arry_head)
          {
            is_arry_head = false;
            total_table += "],\r\n";
          }

          /*if (is_key_head)
          {
            is_key_head = false;
          }*/

          total_table += putStr(m_pData[i].key) + ':';

          total_table += putData(m_pData[i].type, i);
        }

      }
      if (is_arry_head)
      {
        total_table += "]\r\n";
      }
      total_table += "// Print Data End \r\n";
      total_table += '}';

    }

    void WriteData(uint32_t addr, json_t data) {

    }
    void AddData(uint32_t addr, json_t data) {

    }
    void DelData(uint32_t addr, json_t data) {

    }


    json_t ReadData(uint32_t addr) {
      return m_pData[addr];
    }


    json_t* GetData(uint32_t addr) {
      return &m_pData[addr];
    }

  private:

    void parsing(char* p_data, uint32_t size);
    void parsingArray(char* p_data, uint32_t size);
    bool putData(char** pdata, uint8_t cnt);


    inline bool is_contaArry(const string ref, const string check)
    {
      return (ref.compare(check) == 0) ? true : false;
    }

    /// <summary>
    /// 파싱한 데이터를 출력 형태(string)로 반환 
    /// </summary>
    /// <param name="type">파싱 데이터 종류</param>
    /// <param name="index">인덱스</param>
    /// <param name="is_key_t"></param>
    /// <returns></returns>
    inline string putData(valueType_e type, uint32_t index, bool is_key_t = false)
    {
      string ret_str;
      switch (type)
      {
      case valueType_e::string:
        ret_str = putStr(m_pData[index].str_value);
        if (is_key_t)
        {
          ret_str = putStr(m_pData[index].key_value->str_value);
        }
        break;

      case valueType_e::integer:
        ret_str = putInt(m_pData[index].number);
        if (is_key_t)
        {
          ret_str = putInt(m_pData[index].key_value->number);
        }
        break;

      case valueType_e::boolian:
        ret_str = putBool(m_pData[index].yesorno);
        if (is_key_t)
        {
          ret_str = putBool(m_pData[index].key_value->yesorno);
        }
        break;

      case valueType_e::realnumber:
        ret_str = putReal(m_pData[index].realnumber);
        if (is_key_t)
        {
          ret_str = putReal(m_pData[index].key_value->realnumber);
        }
        break;

      case valueType_e::key_value:
        break;

      default:
        break;
      }
      return ret_str;
    }

    inline string putStr(const string in_str)
    {
      return ('\"' + in_str + '\"');
    }

    inline string putInt(const int in_int)
    {
      return std::to_string(in_int);
    }

    inline string putBool(const bool in_bool)
    {
      return in_bool ? "true" : "false";
    }

    inline string putReal(const double in_real)
    {
      return std::to_string(in_real);
    }



  };



}










/// <summary>
/// 입력 값 확인 
/// </summary>
namespace scope
{
  /// <summary>
  /// 입력 값이 범위 내 유지되도록 함 
  /// </summary>
  /// <param name="amt">입력 값</param>
  /// <param name="low">low limit</param>
  /// <param name="high">high limit</param>
  /// <returns>유지된 값</returns>
  int constrain(int amt, int low, int high);

  /// <summary>
  /// 입력 값이 범위 내 있는지 확인
  /// </summary>
  /// <param name="amt">입력 값</param>
  /// <param name="low">하위 값</param>
  /// <param name="high">상위 값</param>
  /// <returns>true 범위 내, false 범위 외 </returns>
  bool between(int amt, int low, int high);
}


/// <summary>
/// 데이터 버퍼 클래스
/// </summary>
namespace buffer
{
  const int DEF_BUFF_DEFAULT_SIZE = 4096;
  const int DEF_VECTOR_DEFAULT_CAPA = 1;

  template <typename T>
  class _que
  {
    T* m_Buff;
    uint32_t m_Head, m_Tail;
    uint32_t m_Length;

  public:
    _que(uint32_t length = DEF_BUFF_DEFAULT_SIZE) :
      m_Head(0), m_Tail(0), m_Length(length)
      , m_Buff(new T[length]{0,}) {
    }

    virtual ~_que() {
      if (m_Buff != nullptr)
        delete[] m_Buff;
      m_Buff = nullptr;
      //Init();
    }

    inline void Init() {
      m_Head = m_Tail = 0;
    }

    inline bool Write(T* p_data, uint32_t length) {
      bool ret = true;
      uint32_t next_in;
      for (int i = 0; i < (int)length; i++) {
        next_in = (m_Head + 1) % m_Length;
        if (next_in != m_Tail) {
          m_Buff[m_Head] = p_data[i];
          m_Head = next_in;
        }
        else {
          ret = false;
          break;
        }
      }
      return ret;
    }

    inline bool Read(T* p_data, uint32_t length) {
      bool ret = true;
      for (int i = 0; i < (int)length; i++) {
        p_data[i] = m_Buff[m_Tail];
        if (m_Tail != m_Head) {
          m_Tail = (m_Tail + 1) % m_Length;
        }
        else {
          ret = false;
          break;
        }
      }
      return ret;
    }
    int Available() {
      return (m_Length + (m_Head - m_Tail)) % m_Length;
    }


    inline bool Put(T data) {
      bool ret = true;
      uint32_t next_in;
      next_in = (m_Head + 1) % m_Length;
      if (next_in != m_Tail) {
        m_Buff[m_Head] = data;
        m_Head = next_in;
      }
      else
        ret = false;

      return ret;
    }

    inline bool Get(T* p_data) {
      bool ret = true;
      *(p_data) = m_Buff[m_Tail];
      if (m_Tail != m_Head) {
        m_Tail = (m_Tail + 1) % m_Length;
      }
      else
        ret = false;

      return ret;
    }

    inline T Pop(uint32_t addr) {
      if (addr > (m_Length - 1))
        return m_Buff[m_Tail];
      return m_Buff[addr];
    }


    inline void Pop(uint32_t addr, T* p_data) {
      if (addr > (m_Length - 1))
        return;
      *(p_data) = m_Buff[addr];
    }

    inline uint32_t GetSize() {
      return ((m_Head - m_Tail) % m_Length);
    }

    inline void Flush() {
      m_Head = m_Tail = 0;
    }

    inline T operator[](uint32_t addr) {
      return Pop[addr];
    }

  };


  template <typename T>
  class _vector
  {
    T* m_Data;
    uint32_t m_Capacity;
    uint32_t m_Length;

  public:
    _vector(uint32_t capa = 1) :
      m_Data(new T[capa]), m_Capacity(capa), m_Length(0) { }

    virtual ~_vector() {
      if (m_Data != nullptr) {
        delete[] m_Data;
      }
    }

  private:
  public:
    inline void push_back(T data) {
      if (m_Capacity <= m_Length) {
        T* temp = new T[m_Capacity * 2]{};
        for (int i = 0; i < m_Length; i++) {
          temp[i] = data[i % m_Capacity];
        }
        delete[] m_Data;
        m_Data = temp;
        m_Capacity *= 2;
      }
      m_Data[m_Length] = data;
      m_Length++;
    }

    inline T operator[](uint32_t i) { return m_Data[i]; }

    inline void remove(uint32_t x) {
      for (int i = (int)x + 1; i < m_Length; i++) {
        m_Data[i - 1] = m_Data[i];
      }
      m_Length--;
    }

    inline int size() { return (int)m_Length; }

    inline bool insert(T& element, uint32_t addr) {
      if (m_Length < (add - 1))
        return false;
      m_Length++;
      for (int i = (int)m_Length; i > (int)addr; i--) {
        m_Data[i] = m_Data[(i - 1)];
      }
      m_Data[addr] = element;
      return true;
    }

  };

  template <typename T>
  class _stack
  {
    struct Node
    {
      Node* prev;
      T s;
      Node(Node* prev, T s) :prev(prev), s(s) {}
    };
    Node* m_Current;
    Node m_Start;
  public:
    _stack() :m_Start(0, 0) {
      m_Current = &m_Start;
    }

    virtual ~_stack() {
      while (m_Current != &m_Start) {
        Node* prev = m_Current;
        m_Current = m_Current->prev;
        delete prev;
      }
    }

  private:
  public:
    inline void push(T s) {
      Node* n = new Node(m_Current, s);
      m_Current = n;
    }

    inline T pop() {
      T ret = {};
      if (m_Current == &m_Start) return ret;
      ret = m_Current->s;
      Node* prev = m_Current;
      m_Current = m_Current->prev;
      delete prev;
      return ret;
    }

    inline T peek() { return m_Current->s; }

    inline bool is_empty() {
      if (m_Current == &m_Start) return true;
      return false;
    }
  };

}


/// <summary>
/// data 변경
/// </summary>
namespace conv
{
  int DwToInt(uint8_t* bytes);

  uint32_t DwToUint(uint8_t* bytes);
}


#include<io.h>
#include<direct.h>
#include<list>

/// <summary>
/// 파일 생성 및 읽고 쓰기 등
/// </summary>
namespace file
{
  const errno_t FILE_ERR_SUCCESS = 0;
  const errno_t FILE_ERR_NO_FILE_IN_DIR = -1;

  /// <summary>
  /// 현재 디렉토리 정보를 버퍼에 가져온다
  /// </summary>
  /// <param name="p_buff">버퍼 주소</param>
  /// <param name="len">버퍼 사이즈</param>
  void  CurrDir(char* p_buff, int len);

  /// <summary>
  /// 디렉토리를 생성
  /// </summary>
  /// <param name="dir_name">디렉토리 정보</param>
  /// <returns></returns>
  int  MakeDir(const char* dir_name);

  /// <summary>
  /// 디렉토리를 삭제
  /// </summary>
  /// <param name="dir_name">디렉토리 정보</param>
  /// <returns></returns>
  int  DeleteDir(const char* dir_name);

  /// <summary>
  /// 네임 타입이 파일 형태인지 확인
  /// </summary>
  /// <param name="fd">_finddata_t</param>
  /// <returns>파일 true</returns>
  bool  isFile(_finddata_t fd);

  /// <summary>
  /// 네임 타입이 폴더인지 확인
  /// </summary>
  /// <param name="fd">_finddata_t</param>
  /// <returns>폴더 true</returns>
  bool  isDir(_finddata_t fd);

  /// <summary>
  /// 존재하는 폴더인지 확인
  /// </summary>
  /// <param name="dir">존재하면 true</param>
  /// <returns></returns>
  bool  isDirExists(const char* dir);

  /// <summary>
  /// 존재하는 파일인지 확인
  /// </summary>
  /// <param name="file">존재하면 true</param>
  /// <returns></returns>
  bool  isFileExists(const char* file);


  /// <summary>
  /// 하위 디렉토리 포함 파일 수를 리턴한다.
  /// </summary>
  /// <param name="dir_str"></param>
  /// <returns></returns>
  int  SearchDir(const char* dir_str);


  /// <summary>
  /// 파일 갯수를 반환
  /// </summary>
  /// <param name="dir_str">확인 경로</param>
  /// <param name="inc_dir">하위 폴더 포함 여부</param>
  /// <returns>파일 갯수</returns>
  int  GetFileCount(const char* dir_str, bool inc_dir = false);


  /// <summary>
  /// 최하위 디렉토리 인지 확인
  /// </summary>
  /// <param name="dir_str"></param>
  /// <returns></returns>
  bool  isLeafDir(const char* dir_str);

  /// <summary>
  /// 경로 폴더 내 파일을 삭제
  /// </summary>
  /// <param name="dir_str">삭제 경로</param>
  /// <param name="inc_dir">하위 폴더 포함 여부</param>
  /// <returns>삭제 파일 수</returns>
  int  DeleteFilesInDir(const char* dir_str, bool inc_dir = false);




  // copy files in directory
  // 1. src dir 파일이 있지 확인/ src 디렉토리 없다면
  // 2. dst dir 없다면 생성하고
  // 3. src file과 동일한 dst file이 있으면 비교해 같으면 생략
  // 4. 하위 디렉토리 동일하게 처리

  /// <summary>
  ///  src 디렉토리를 dest 디렉토리롤 복사 
  /// </summary>
  /// <param name="dst_dir">복사할 위치</param>
  /// <param name="src_dir">복사될 위치</param>
  /// <param name="over_write">덮어쓰기 여부</param>
  /// <param name="inc_dir">하위 폴더 적용 여부</param>
  /// <returns></returns>
  errno_t  CopyFilesInDir_binary(const char* dst_dir, const char* src_dir, bool over_write = false, bool inc_dir = false);

}


/// <summary>
/// 시간 측정
/// </summary>
namespace timer
{
  class _us
  {
    chrono::system_clock::time_point start_time;
    bool is_start;
  public:
    _us() :start_time({}), is_start(false) {
    }

    virtual ~_us() {
    }

  private:
  public:
    inline void Start() {
      start_time = chrono::system_clock::now();
      is_start = true;
    }

    inline chrono::system_clock::time_point End() {
      return chrono::system_clock::now();
    }

    inline void Stop() {
      is_start = false;
    }

    inline bool IsStarted() {
      return is_start;
    }

    inline uint64_t Elaps(bool is_reset = false) {
      if (!is_start) //not
        return 0;
      chrono::microseconds micro = chrono::duration_cast<chrono::microseconds>(End() - start_time);
      if (is_reset)
        Start();
      return micro.count();
    }

    inline bool LessThan(uint32_t us) {
      if (Elaps() < us)
      {
        Start();
        return true;
      }
      else
        return false;
    }

    inline bool MoreThan(uint32_t us) {
      if (Elaps() > us)
      {
        Start();
        return true;
      }
      else
        return false;
    }
  };



  class _ms
  {
    chrono::system_clock::time_point start_time;
    bool is_start;
  public:
    _ms() :start_time({}), is_start(false) {
    }

    virtual ~_ms() {
    }

  private:
  public:
    inline void Start() {
      start_time = chrono::system_clock::now();
      is_start = true;
    }

    inline chrono::system_clock::time_point End() {
      return chrono::system_clock::now();
    }

    inline void Stop() {
      is_start = false;
    }

    inline bool IsStarted() {
      return is_start;
    }

    inline uint64_t Elaps(bool is_reset = false) {
      if (!is_start) //not
        return 0;
      chrono::milliseconds mill = chrono::duration_cast<chrono::milliseconds>(End() - start_time);
      if (is_reset)
        Start();
      return mill.count();
    }

    /// <summary>
    /// 미만 확인
    /// </summary>
    /// <param name="ms"></param>
    /// <returns>초과 false</returns>
    inline bool LessThan(uint32_t ms) {
      if (Elaps() < ms)
      {
        Start();
        return true;
      }
      else
        return false;
    }

    /// <summary>
    /// 초과 확인
    /// </summary>
    /// <param name="ms"></param>
    /// <returns>초과  true</returns>
    inline bool MoreThan(uint32_t ms) {
      if (Elaps() > ms)
      {
        Start();
        return true;
      }
      else
        return false;
    }
  };
}


/// <summary>
/// 전역으로 상시적 사용 함수 
/// </summary>
namespace util
{


  const uint16_t MODBUS_CRC16Table[] =
  {  0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601, 0X06C0, 0X0780, 
     0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 
     0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801, 
     0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40, 
     0XDD01, 0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 
     0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040, 0XF001, 0X30C0, 
     0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240, 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 
     0X35C0, 0X3480, 0XF441, 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41, 
     0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981, 
     0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 
     0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640, 0X2200, 
     0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141, 
     0X6300, 0XA3C1, 0XA281, 0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 
     0XA441, 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01, 0X6AC0, 
     0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 
     0X7BC0, 0X7A80, 0XBA41, 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40, 
     0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381, 
     0X7340, 0XB101, 0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 
     0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440, 0X9C01, 
     0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40, 0X5A00, 0X9AC1, 0X9B81, 0X5B40, 
     0X9901, 0X59C0, 0X5880, 0X9841, 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 
     0X4A40, 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1, 
     0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 
     0X81C1, 0X8081, 0X4040 
  };



  inline int crc16_modbus_cal(uint8_t* p_data, uint16_t length) {
    uint8_t nTemp;

    uint16_t ret_crc = 0xFFFF;

    while (length--)
    {
      nTemp = *p_data++ ^ ret_crc;
      ret_crc >>= 8;
      ret_crc ^= MODBUS_CRC16Table[nTemp];
    }

    return ret_crc;
  }

  inline void crc16_modbus_update(uint16_t* crc_in, uint8_t data) {
    constexpr uint16_t poly = 0xA001;
    uint16_t crc = *crc_in;
    uint8_t i;
    /* Exclusive-OR the byte with the CRC */
    crc ^= data; //*(pDataBuffer + iByte);

    /* Loop through all 8 data bits */
    for (i = 0; i <= 7; i++) {
      /* If the LSB is 1, shift the CRC and XOR the polynomial mask with the CRC */
      // Note - the bit test is performed before the rotation, so can't move the << here
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= poly;
      }
      else {
        // Just rotate it
        crc >>= 1;
      }
    }
    *crc_in = crc;
  }




  
  inline void crc16_update(uint16_t* crc_in, uint8_t data) {
    constexpr uint16_t poly = 0xA001;
    uint16_t crc = *crc_in;
    uint8_t i;
    /* Exclusive-OR the byte with the CRC */
    crc ^= data; //*(pDataBuffer + iByte);

    /* Loop through all 8 data bits */
    for (i = 0; i <= 7; i++) {
      /* If the LSB is 1, shift the CRC and XOR the polynomial mask with the CRC */
      // Note - the bit test is performed before the rotation, so can't move the << here
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= poly;
      }
      else {
        // Just rotate it
        crc >>= 1;
      }
    }
    *crc_in = crc;
  }

  /// <summary>
  /// 프로세스 시간 측정 (ms)
  /// </summary>
  /// <param name=""></param>
  /// <returns></returns>
  inline uint32_t millis(void) {
    double ret;
    LARGE_INTEGER freq, counter;

    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);

    ret = ((double)counter.QuadPart / (double)freq.QuadPart) * 1000.0;

    return (uint32_t)ret;
  }

  /// <summary>
  /// 프로세스 정밀 시간 측정
  /// </summary>
  /// <param name=""></param>
  /// <returns></returns>
  inline uint32_t micros(void) {
    LARGE_INTEGER freq, counter;
    double ret;

    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);

    ret = ((double)counter.QuadPart / (double)freq.QuadPart) * 1000000.0;

    return (uint32_t)ret;
  }

  /// <summary>
  /// 프로세스 지연
  /// </summary>
  /// <param name="ms">ms</param>
  inline void delay(uint32_t ms) {
    uint32_t pre_time;

    pre_time = millis();
    while (1) {
      if (millis() - pre_time >= ms)
        break;
    }
  }

}


/// <summary>
/// 문자열 타입 변경 외
/// </summary>
namespace trans
{
  /// <summary>
  /// 문자열을 delim_char 기준 배열로 분리
  /// </summary>
  /// <param name="arg_str">문자열</param>
  /// <param name="argv">반환할 문자 배열 변수</param>
  /// <param name="delim_chars">구분 char</param>
  /// <param name="max">최대 값</param>
  /// <returns>반환 배열 갯수</returns>
  uint8_t SplitArgs(char* arg_str, char** argv, char* delim_chars = "/ -:,", int max = 4096);

  /// <summary>
  /// 현재 date time 문자열 반환
  /// </summary>
  /// <param name="p_str">반환 버퍼 주소</param>
  /// <param name="max_len">버퍼 최대 갯수</param>
  void NowStr(char* p_str, int max_len);


  enum class DataTimeFormat_e
  {
    YYYYMMDD_HHMMSSUU_space,
    YYYYMMDD_HHMMSS_space,
    YYYYMMDD_HHMMSS_square_brackets,

  };

  /// <summary>
  /// Data타임 포맷 반환
  /// </summary>
  /// <param name="p_str">반환 버퍼 주소</param>
  /// <param name="length">버퍼 최대 갯수</param>
  /// <param name="type">포맷 타입</param>
  void DateFormStr(char* p_str, int length, DataTimeFormat_e type = DataTimeFormat_e::YYYYMMDD_HHMMSSUU_space);

  /// <summary>
  /// 문자열 왼쪽에서 count+ '\0' 만큼 반환 한다
  /// </summary>
  /// <returns>문자열 주소</returns>
  void Left(const char* pdata, char* pdest, int size, int count);

  /// <summary>
  /// 특정 char를 변경
  /// </summary>
  /// <param name="src">소스 문자열</param>
  /// <param name="find">변경할 char</param>
  /// <param name="upcase">대문자로 변경할지 여부</param>
  /// <param name="replace">변경할 char</param>
  void Replace(char* src, char find, bool upcase = false, char replace = ' ');

#ifdef _AFX

  /// <summary>
  /// char → CString
  /// </summary>
  /// <param name="str"></param>
  /// <returns></returns>
  CString CharToCString(char* str);

  CString TcharToCString(TCHAR* str);


#endif

  /// <summary>
  /// Char 문자열을 Tchar문자열로 변환
  /// </summary>
  /// <param name="str">Char 문자열</param>
  /// <param name="ret_str">Tchar문자열로 반환주소</param>
  /// <returns>문자 수 </returns>
  int CharToLPTSTR(LPSTR str, LPTSTR ret_str, int length);

  /// <summary>
  /// TCHAR 문자열 → Char 문자열
  /// </summary>
  /// <param name="str"></param>
  /// <param name="ret_str"></param>
  /// <returns></returns>
  int TcharToLPSTR(LPTSTR str, LPSTR ret_str, int length);

}


namespace compare
{
  bool IsEqualStr(char* str_rf, char* str_cp, bool is_ignore = false);

}



/// <summary>
/// 비트, 바이트 단위 플래그 데이터 형
/// </summary>
namespace flag
{
  const int DEF_FLAG_BANK_MAX = 255;
  enum class bit_e {
    b0,
    b1,
    b2,
    b3,
    b4,
    b5,
    b6,
    b7,
    _max
  };
#define REG_BIT(x)          static_cast<flag::bit_e>(x)

  class fdata {

    struct _flag
    {
      bool m_bit[static_cast<uint8_t>(bit_e::_max)]{ 0 };
      explicit _flag(const _flag& ref) :m_bit() {}
      _flag& operator=(const _flag& ref) { return *this; }
    public:
      explicit _flag() :m_bit() {};
      virtual ~_flag() {};

      inline bool& operator[](const bit_e bit_no) {
        return m_bit[static_cast<int>(bit_no)];
      }

      inline void operator = (const uint8_t in_data) {
        uint8_t data = 0;
        for (uint8_t i = 0; i < 8; i++) {
          data = in_data;
          data = data >> i;
          data = 0xfe | data;
          data = ~(data);
          m_bit[i] = !data; //!(~(0xfe | (data >> i)));
        }
      }

      inline uint8_t get() {
        uint8_t data;
        data = (int)m_bit[0] | ((int)m_bit[1] * 2) | ((int)m_bit[2] * 4) | ((int)m_bit[3] * 8)
          | ((int)m_bit[4] * 16) | ((int)m_bit[5] * 32) | ((int)m_bit[6] * 64 | ((int)m_bit[7] * 128));
        return data;
      }

      inline uint8_t on_bit(const bit_e bit_no) {
        m_bit[static_cast<int>(bit_no)] = true;
        return get();
      }

      inline uint8_t off_bit(const bit_e bit_no) {
        m_bit[static_cast<int>(bit_no)] = false;
        return get();
      }
    };

  public:
    _flag m_byte[DEF_FLAG_BANK_MAX];
    uint8_t m_size;

    explicit fdata(const fdata& ref) :m_byte(), m_size() {}
    fdata& operator=(const fdata ref) { return *this; }
  public:
    fdata(uint8_t size = DEF_FLAG_BANK_MAX) :m_byte(), m_size(size) { };
    virtual ~fdata() {};

  public:
    inline void ClearAll() {
      for (uint8_t i = 0; i < m_size; i++) {
        Clear(i);
      }
    }

    inline void Clear(uint8_t bank) {
      m_byte[bank] = 0;
    }

    inline uint8_t SetOn(uint8_t bank, bit_e bit_no) {
      m_byte[bank][bit_no] = true;
      return m_byte[bank].get();
    }
    inline uint8_t SetOff(uint8_t bank, bit_e bit_no) {
      m_byte[bank][bit_no] = false;
      return m_byte[bank].get();
    }

    inline bool Get(uint8_t bank, bit_e bit) {
      return m_byte[bank][bit];
    }

    inline uint8_t GetSize() const {
      return m_size;
    }

    inline _flag& operator[](uint8_t bank) {
      return m_byte[bank];
    }

    inline uint8_t operator[](int bit) {
      return m_byte[bit].get();
    }

  };
}