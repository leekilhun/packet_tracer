#include "pch.h"
#include "syslog.h"

#include <vector>
#include <array>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace util;


/* 전역 함수로 구현 */
_que<syslog::dat_t> log_table[static_cast<int>(syslog::level::_max)]{ SYS_LOG_BUFF_LIST_MAX, };

static bool is_init = false;
static mutex _mutex;



void syslog::syslog_Init()
{
  is_init = true;
  //TODO:: 초기화 타임에 뭘 해야하지?
}

void syslog::syslog_Del()
{
  //TODO:: 삭제 기능 ???
}


uint32_t syslog::syslog_Put(syslog::level level, const int obj, const char* file, const char* func, const int line, const char* fmt, ...)
{
  uint32_t ret = 0;
  if (is_init != true) return ret;

  constexpr int ARG_DATE = 0;
  constexpr int ARG_TIME = 1;
  syslog::dat_t data;

  _mutex.lock();
  va_list args;
  va_start(args, fmt);

  int len = vsnprintf(nullptr, 0, fmt, args);
  if (len < 0) {
    va_end(args);
    _mutex.unlock();
    return ret;
  }
  else if (len > 0)
  {
    //obj id
    data.SetObjId((uint32_t)obj);
    //obj id
    data.SetLineNo((uint32_t)line);
    //file
    size_t size = strnlen_s(file, SYS_LOG_DATA_FILE_STR_MAX) + 1;
    char tmp_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
    strcpy_s(tmp_str, (size % SYS_LOG_DATA_FILE_STR_MAX), file);
    char* arg_tbl[SYS_LOG_ARG_TBL_CNT_MAX] = {};
    uint8_t arg_count = trans::SplitArgs(tmp_str, arg_tbl, "\\", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_tbl[arg_count - 1] != 0) {
      data.SetFile(arg_tbl[arg_count - 1]);
    }
    //func
    data.SetFunc(func);
    //date, time
    char datetime[SYS_LOG_DATE_LENGTH_MAX] = { 0, };
    trans::DateFormStr(&datetime[0], SYS_LOG_DATE_LENGTH_MAX);
    arg_count = trans::SplitArgs(&datetime[0], arg_tbl, " ", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_count > 1) {
      if (arg_tbl[ARG_DATE] != 0) {
        data.SetDate(arg_tbl[0]);
      }
      if (arg_tbl[ARG_TIME] != 0) {
        data.SetTime(arg_tbl[1]);
      }
    }

    uint32_t vec_len = (uint32_t)len + SYS_LOG_DATA_DATE_STR_MAX + 1;
    array<char, SYS_LOG_TXT_LENGTH_MAX> buffer = { 0 };
    array<char, SYS_LOG_TXT_LENGTH_MAX> message = { 0 };

    len = vsnprintf(&buffer[0], buffer.size(), fmt, args);
    uint32_t idx = 0;
    for (char &atm : buffer)
    {
      if (atm == '\n' || atm == '\r')
        continue;
      //message.insert(message.begin(), atm);
      message[idx++] = atm;
    }
    //result = strcpy_s(p_data->message, LOG_MANAGE_DATA_MSG_STR_MAX, &buffer[0]);
    data.SetMsg(&message[0]);

    switch (level)
    {
    case syslog::level::err:
      data.SetLevel("[ERR]");
      ret = log_table[static_cast<int>(syslog::level::err)].Put(data);
      break;
    case syslog::level::warning:
      data.SetLevel("[WAR]");
      ret = log_table[static_cast<int>(syslog::level::warning)].Put(data);
      break;
    default: // info
      data.SetLevel("[INF]");
      ret = log_table[static_cast<int>(syslog::level::info)].Put(data);
      break;
    }
  }

  va_end(args);
  _mutex.unlock();

  return ret;
}

syslog::dat_t syslog::syslog_Get(int level)
{
  syslog::dat_t ret;
  switch (level)
  {
  case static_cast<int>(syslog::level::err):
  {
    log_table[static_cast<int>(syslog::level::err)].Get(&ret);
  }
  break;
  case static_cast<int>(syslog::level::warning):
  {
    log_table[static_cast<int>(syslog::level::warning)].Get(&ret);
  }
  break;
  default:
  {
    log_table[static_cast<int>(syslog::level::info)].Get(&ret);
  }
  break;
  }
  return ret;
}

syslog::dat_t syslog::syslog_Pop(uint32_t addr, int level)
{
  syslog::dat_t ret;
  switch (level)
  {
  case static_cast<int>(syslog::level::err):
  {
    log_table[static_cast<int>(syslog::level::err)].Pop(addr, &ret);
  }
  break;
  case static_cast<int>(syslog::level::warning):
  {
    log_table[static_cast<int>(syslog::level::warning)].Pop(addr, &ret);
  }
  break;
  default:
  {
    log_table[static_cast<int>(syslog::level::info)].Pop(addr, &ret);
  }
  break;
  }
  return ret;
}

uint32_t syslog::syslog_Available(int level)
{
  uint32_t ret;
  switch (level)
  {
  case static_cast<int>(syslog::level::err):
  {
    ret = log_table[static_cast<int>(syslog::level::err)].Available();
  }
  break;
  case static_cast<int>(syslog::level::warning):
  {
    ret = log_table[static_cast<int>(syslog::level::warning)].Available();
  }
  break;
  default:
  {
    ret = log_table[static_cast<int>(syslog::level::info)].Available();
  }
  break;
  }
  return ret;
}



uint32_t syslog::syslog_Write(syslog::level level, const int obj, const char* file_info, const char* file, const char* func, const int line, const char* fmt, ...)
{
  uint32_t ret = 0;
  if (is_init != true) return ret;

  syslog::dat_t data;

  constexpr int ARG_DATE = 0;
  constexpr int ARG_TIME = 1;

  char* arg_tbl[SYS_LOG_ARG_TBL_CNT_MAX] = {};
  uint8_t arg_count = 0;
  _mutex.lock();
  va_list args;
  va_start(args, fmt);

  int len = vsnprintf(nullptr, 0, fmt, args);
  if (len < 0) {
    va_end(args);
    _mutex.unlock();
    return ret;
  }
  else if (len > 0)
  {
    //obj id
    data.SetObjId((uint32_t)obj);
    //obj id
    data.SetLineNo((uint32_t)line);
    //file
    size_t size = strnlen_s(file, SYS_LOG_DATA_FILE_STR_MAX) + 1;
    char tmp_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
    strcpy_s(tmp_str, (size % SYS_LOG_DATA_FILE_STR_MAX), file);

    arg_count = trans::SplitArgs(tmp_str, arg_tbl, "\\", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_tbl[arg_count - 1] != 0) {
      data.SetFile(arg_tbl[arg_count - 1]);
    }
    //func
    data.SetFunc(func);
    //date, time
    char datetime[SYS_LOG_DATE_LENGTH_MAX] = { 0, };
    trans::DateFormStr(&datetime[0], SYS_LOG_DATE_LENGTH_MAX,trans::DataTimeFormat_e::YYYYMMDD_HHMMSS_UU);
    arg_count = trans::SplitArgs(&datetime[0], arg_tbl, " ", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_count > 1) {
      if (arg_tbl[ARG_DATE] != 0) {
        data.SetDate(arg_tbl[ARG_DATE]);
      }
      if (arg_tbl[ARG_TIME] != 0) {
        data.SetTime(arg_tbl[ARG_TIME]);
      }
    }

    array<char, SYS_LOG_TXT_LENGTH_MAX> buffer = { 0 };
    array<char, SYS_LOG_TXT_LENGTH_MAX> message = { 0 };

    len = vsnprintf(&buffer[0], buffer.size(), fmt, args);
    uint32_t idx = 0;
    for (char& atm : buffer)
    {
      if (atm == '\n' || atm == '\r')
        continue;
      //message.insert(message.begin(), atm);
      message[idx++] = atm;
    }
    //result = strcpy_s(p_data->message, LOG_MANAGE_DATA_MSG_STR_MAX, &buffer[0]);
    data.SetMsg(&message[0]);

    switch (level)
    {
    case syslog::level::err:
      data.SetLevel("[ERR]");
      ret = log_table[static_cast<int>(syslog::level::err)].Put(data);
      break;
    case syslog::level::warning:
      data.SetLevel("[WAR]");
      ret = log_table[static_cast<int>(syslog::level::warning)].Put(data);
      break;
    default: // info
      data.SetLevel("[INF]");
      ret = log_table[static_cast<int>(syslog::level::info)].Put(data);
      break;
    }
  }

  {
    char directory_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };

    const char* log_file;
    log_file = strrchr(file_info, '/') + 1;
    memcpy(directory_str, file_info, (strlen(file_info) - strlen(log_file)));    

    // folder가 존재하는지 확인 후 없으면 생성
    if (file::isDirExists(directory_str) == false)
    {
      file::MakeDir(directory_str);
    }


    // 파일이 있다면 old 파일을 만들고 
    if (file::isFileExists(file_info))
    {
      //make old file
      FILE* fp_cpy;
      char tmp_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
      const char* str_old = "_old.log";
      //errno_t ret = strcpy_s(tmp_str, (strlen(file_info) -4), file_info);
      errno_t ret = strncpy_s(tmp_str, SYS_LOG_DATA_FILE_STR_MAX, file_info, (strlen(file_info) - 4));
      ret = strcat_s(tmp_str, SYS_LOG_DATA_FILE_STR_MAX, str_old);
      errno_t fret = fopen_s(&fp_cpy, (const char*)&tmp_str[0], "w");
      if (fp_cpy != nullptr)
      {
        fclose(fp_cpy); //파일 포인터 닫기
      }

      // old파일에 복사한다.
      file::FileCopy(file_info, &tmp_str[0]);
    }


    // 로그 내용을 추가한다.
    {
      FILE* fp;
      errno_t fret = fopen_s(&fp, file_info, "a"); //test파일을 w(쓰기) 모드로 열기

      if (fret == ERROR_SUCCESS)
      {
        //[2021-12-13 14:56:46'43] [1004] [1-ERR] Wait4Done() : IO Interlock Checked   (maxis.cpp, 1493)
        fprintf_s(fp, "[%s %s] [%d] %s %s() : %s (%s,  %d)\n"
          , data.date
          , data.time
          , data.obj_id
          , data.level
          , data.func
          , data.message
          , data.file
          , data.line_no
        );
      }

      if (fp != nullptr)
      {
        fclose(fp); //파일 포인터 닫기
      }

    }

  }
  va_end(args);
  _mutex.unlock();

  return ret;
}



using namespace syslog;

MSystemLog::MSystemLog() :m_isInit(false), m_size(0)
{
  m_isInit = true;
}


MSystemLog::~MSystemLog()
{
  
}

void MSystemLog::initLog()
{
  //TODO:: 초기화 기능은 뭘 해야하지?
}


uint32_t MSystemLog::syslog_Write(syslog::level level, const int obj, const char* file_info, const char* file, const char* func, const int line, const char* fmt, ...)
{
  uint32_t ret = 0;
  if (is_init != true) return ret;

  syslog::dat_t data;

  constexpr int ARG_DATE = 0;
  constexpr int ARG_TIME = 1;

  char* arg_tbl[SYS_LOG_ARG_TBL_CNT_MAX] = {};
  uint8_t arg_count = 0;
  m_mutex.lock();
  va_list args;
  va_start(args, fmt);

  int len = vsnprintf(nullptr, 0, fmt, args);
  if (len < 0) {
    va_end(args);
    m_mutex.unlock();
    return ret;
  }
  else if (len > 0)
  {
    //obj id
    data.SetObjId((uint32_t)obj);
    //obj id
    data.SetLineNo((uint32_t)line);
    //file
    size_t size = strnlen_s(file, SYS_LOG_DATA_FILE_STR_MAX) + 1;
    char tmp_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
    strcpy_s(tmp_str, (size % SYS_LOG_DATA_FILE_STR_MAX), file);

    arg_count = trans::SplitArgs(tmp_str, arg_tbl, "\\", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_tbl[arg_count - 1] != 0) {
      data.SetFile(arg_tbl[arg_count - 1]);
    }
    //func
    data.SetFunc(func);
    //date, time
    char datetime[SYS_LOG_DATE_LENGTH_MAX] = { 0, };
    trans::DateFormStr(&datetime[0], SYS_LOG_DATE_LENGTH_MAX, trans::DataTimeFormat_e::YYYYMMDD_HHMMSS_UU);
    arg_count = trans::SplitArgs(&datetime[0], arg_tbl, " ", SYS_LOG_ARG_TBL_CNT_MAX);
    if (arg_count > 1) {
      if (arg_tbl[ARG_DATE] != 0) {
        data.SetDate(arg_tbl[0]);
      }
      if (arg_tbl[ARG_TIME] != 0) {
        data.SetTime(arg_tbl[1]);
      }
    }

    uint32_t vec_len = (uint32_t)len + SYS_LOG_DATA_DATE_STR_MAX + 1;    

#if 1
    array<char, SYS_LOG_TXT_LENGTH_MAX> buffer = { 0 };
    array<char, SYS_LOG_TXT_LENGTH_MAX> message = { 0 };
#else
    vector<char> buffer(vec_len);
    vector<char> message(vec_len);
#endif

    len = vsnprintf(&buffer[0], buffer.size(), fmt, args);
    uint32_t idx = 0;
    for (char &atm : buffer)
    {
      if (atm == '\n' || atm == '\r')
        continue;
      //message.insert(message.begin(), atm);
      message[idx++] = atm;
    }

#if 0
    
    if (find(buffer.begin(), buffer.end(), '\n') != buffer.end())
    { 
      // vector내에 '\n'이 존재함
      int index  = find(buffer.begin(), buffer.end(), '\n') - buffer.begin(); // index 확인
      buffer.erase(buffer.begin() + index);
    }
    if (find(buffer.begin(), buffer.end(), '\r') != buffer.end())
    {
      // vector내에 '\r'이 존재함
      int index = find(buffer.begin(), buffer.end(), '\r') - buffer.begin(); // index 확인
      buffer.erase(buffer.begin() + index);
    }

#endif


    //result = strcpy_s(p_data->message, LOG_MANAGE_DATA_MSG_STR_MAX, &buffer[0]);
    data.SetMsg(&message[0]);

    switch (level)
    {
    case syslog::level::err:
      data.SetLevel("[ERR]");
      break;
    case syslog::level::warning:
      data.SetLevel("[WAR]");
      break;
    default: // info
      data.SetLevel("[INF]");
      break;
    }
  }

  {
    char directory_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };

    /*
    const char* log_file;
    log_file = strrchr(file_info, '/') + 1;
    memcpy(directory_str, file_info, (strlen(file_info) - strlen(log_file)));
    */

    // folder가 존재하는지 확인 후 없으면 생성
    if (file::isDirExists(directory_str) == false)
    {
      file::MakeDir(directory_str);
    }


    // 파일이 있다면 old 파일을 만들고 
    if (file::isFileExists(file_info))
    {
      //make old file
      FILE* fp_cpy;
      char tmp_str[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
      const char* str_old = "_old.log";
      //errno_t ret = strcpy_s(tmp_str, (strlen(file_info) -4), file_info);
      errno_t ret = strncpy_s(tmp_str, SYS_LOG_DATA_FILE_STR_MAX, file_info, (strlen(file_info) - 4));
      ret = strcat_s(tmp_str, SYS_LOG_DATA_FILE_STR_MAX, str_old);
      errno_t fret = fopen_s(&fp_cpy, (const char*)&tmp_str[0], "w");
      if (fp_cpy != nullptr)
      {
        fclose(fp_cpy); //파일 포인터 닫기
      }

      // old파일에 복사한다.
      file::FileCopy(file_info, &tmp_str[0]);
    }


    // 로그 내용을 추가한다.
    {
      FILE* fp;
      errno_t fret = fopen_s(&fp, file_info, "a"); //test파일을 w(쓰기) 모드로 열기

      if (fret == ERROR_SUCCESS)
      {
        //[2021-12-13 14:56:46.43] [1004] [1-ERR] Wait4Done() : IO Interlock Checked   (maxis.cpp, 1493)
        fprintf_s(fp, "[%s %s] [%d] %s %s() : %s (%s,  %d)\n"
          , data.date
          , data.time
          , data.obj_id
          , data.level
          , data.func
          , data.message
          , data.file
          , data.line_no
        );
      }

      if (fp != nullptr)
      {
        fclose(fp); //파일 포인터 닫기
      }

    }

  }
  va_end(args);
  m_mutex.unlock();

  return ret;
}

