#pragma once


/*
 * syslog.h
 *
 * 작성자 : 이길훈
 * 22.09.21
 * 전역 system log 관리를 위한 클래스
 *
 * 수정
 * '22.10.04 날짜 표시 수정 [2022-10-04 13:38:41'596.proj_AsTest] -> [2022-10-04 13:45:51'11]
 */

#include <mutex>


#ifdef PRJ_CFG_USE_CSTRING
#define SYS_LOG_CFG_STRING  CString
#else
#define SYS_LOG_CFG_STRING  std::string
#endif


#define SYS_LOG_SPACE_LEN               1
#define SYS_LOG_BUFF_LIST_MAX					  4096  
#define SYS_LOG_TXT_LENGTH_MAX 				  256
#define SYS_LOG_ARG_TBL_CNT_MAX 				256

#define SYS_LOG_DATA_LEVEL_STR_MAX 		  40
#define SYS_LOG_DATA_DATE_STR_MAX 			40
#define SYS_LOG_DATA_TIME_STR_MAX 			40
#define SYS_LOG_DATA_MSG_STR_MAX				SYS_LOG_TXT_LENGTH_MAX
#define SYS_LOG_DATA_FILE_STR_MAX			  MAX_PATH
#define SYS_LOG_DATA_FUNC_STR_MAX			  40

#define SYS_LOG_DATE_LENGTH_MAX				  40



namespace syslog
{

	using namespace util;

		enum class level {
		info,
		err,
		warning,
		_max
	};
#define LOG_LEVEL_INT(x)         static_cast<int>(syslog::level::x)


#define SYSLOGFILE_INFO   "./Log/system.log"
#define SYSLOGFILE_ERR   "./Log/sysError.log"


#define __LOGFUNC__				__FUNCTION__
#define SYSLOG_INFO(obj,fmt, ...)		syslog_Put(syslog::level::info,obj, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define SYSLOG_ERR(obj,fmt, ...)		syslog_Put(syslog::level::err,obj, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define SYSLOG_WAR(obj,fmt, ...)		syslog_Put(syslog::level::warning,obj, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)

#define LOG_PRINT(fmt, ...)		syslog_Write(syslog::level::info, 0,SYSLOGFILE_INFO, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define ERR_PRINT(fmt, ...)		syslog_Write(syslog::level::err, 0,SYSLOGFILE_ERR, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define LTRACE(fmt, ...)   syslog_Write(syslog::level::info, 0, SYSLOGFILE_INFO, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__);\
                           TRACE(fmt, ##__VA_ARGS__)

#define SYSLOG_AVAILABLE(level)     AvailableLog(JLOG_LEVEL_INT(level))



	struct dat_t
	{
		char level[SYS_LOG_DATA_LEVEL_STR_MAX]{ 0, };
		char date[SYS_LOG_DATA_DATE_STR_MAX]{ 0, };
		char time[SYS_LOG_DATA_TIME_STR_MAX]{ 0, };
		char message[SYS_LOG_DATA_MSG_STR_MAX]{ 0, };
		char file[SYS_LOG_DATA_FILE_STR_MAX]{ 0, };
		char func[SYS_LOG_DATA_FUNC_STR_MAX]{ 0, };
		uint32_t line_no;
		uint32_t obj_id;

		void Init() {
			memset(&level[0], 0x00, SYS_LOG_DATA_LEVEL_STR_MAX);
			memset(&date[0], 0x00, SYS_LOG_DATA_DATE_STR_MAX);
			memset(&time[0], 0x00, SYS_LOG_DATA_TIME_STR_MAX);
			memset(&message[0], 0x00, SYS_LOG_DATA_MSG_STR_MAX);
			memset(&file[0], 0x00, SYS_LOG_DATA_FILE_STR_MAX);
			memset(&func[0], 0x00, SYS_LOG_DATA_FUNC_STR_MAX);
			line_no = 0;
			obj_id = 0;
		}

		int SetLevel(const char* value) {
			char in_value[SYS_LOG_DATA_LEVEL_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_LEVEL_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(level, SYS_LOG_DATA_LEVEL_STR_MAX, value);
		}

		int SetDate(const char* value) {
			char in_value[SYS_LOG_DATA_DATE_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_DATE_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(date, SYS_LOG_DATA_DATE_STR_MAX, value);
		}

		int SetTime(const char* value) {
			char in_value[SYS_LOG_DATA_TIME_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_TIME_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(time, SYS_LOG_DATA_TIME_STR_MAX, value);
		}

		int SetMsg(const char* value) {
			char in_value[SYS_LOG_DATA_MSG_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_MSG_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(message, SYS_LOG_DATA_MSG_STR_MAX, value);
		}

		int SetFile(const char* value) {
			char in_value[SYS_LOG_DATA_FILE_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_FILE_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(file, SYS_LOG_DATA_FILE_STR_MAX, value);
		}

		int SetFunc(const char* value) {
			char in_value[SYS_LOG_DATA_FUNC_STR_MAX] = { 0, };
			size_t len = strnlen_s(value, SYS_LOG_DATA_FUNC_STR_MAX - 1);
			memcpy(&in_value[0], value, len);
			return sprintf_s(func, SYS_LOG_DATA_FUNC_STR_MAX, &in_value[0]);
		}

		int SetObjId(uint32_t id) {
			obj_id = id;
			return 0;
		}

		int SetLineNo(uint32_t _no) {
			line_no = _no;
			return 0;
		}

		void GetLevel(SYS_LOG_CFG_STRING& p_str) {
#ifdef PRJ_CFG_USE_CSTRING 
			p_str = trans::CharToCString(level);
#else
			p_str.assign(level);
#endif
		}
		void GetDate(SYS_LOG_CFG_STRING& p_str) {
#ifdef PRJ_CFG_USE_CSTRING 
			p_str = trans::CharToCString(date);
#else
			p_str.assign(date);
#endif
		}
		void GetTime(SYS_LOG_CFG_STRING& p_str) {
#ifdef PRJ_CFG_USE_CSTRING 
			p_str = trans::CharToCString(time);
#else
			p_str.assign(time);
#endif
		}
		void GetMsg(SYS_LOG_CFG_STRING& p_str) {
#ifdef PRJ_CFG_USE_CSTRING 
			p_str = trans::CharToCString(message);
#else
			p_str.assign(message);
#endif
		}
		void GetFile(SYS_LOG_CFG_STRING& p_str) {
#ifdef PRJ_CFG_USE_CSTRING 
			p_str = trans::CharToCString(file);
#else
			p_str.assign(file);
#endif
		}

		uint32_t GetObjid() {
			return obj_id;
		}

		uint32_t GetLineNo() {
			return line_no;
		}

	};


	/* 전역 함수로 구현 */
	void syslog_Init();
	void syslog_Del();
	uint32_t syslog_Put(
		syslog::level level,
		const int obj,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...);

	syslog::dat_t syslog_Get(int level = static_cast<int>(syslog::level::info));
	syslog::dat_t syslog_Pop(uint32_t addr, int level = static_cast<int>(syslog::level::info));
	uint32_t syslog_Available(int level = static_cast<int>(syslog::level::info));

	uint32_t syslog_Write(
		syslog::level level,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...);




/* 클래스 객체 */
class MSystemLog
{
	bool m_isInit;
	uint32_t m_size;
	//_que<SYS_LOG_CFG_STRING>* m_queList;
	//buffer::_que<syslog::dat_t>* m_logTable;
	mutex m_mutex;

public:
	MSystemLog();
	~MSystemLog();
private:
	void initLog();
public:
	uint32_t syslog_Write(
		syslog::level level,
		const int obj,
		const char* file_info,
		const char* file,
		const char* func,
		const int line,
		const char* fmt, ...);

};




}