#pragma once


// 프로젝트 전역
 /****************************************************
  *	prject define
  ****************************************************/

#define PRJ_CFG_USE_CSTRING
#ifdef PRJ_CFG_USE_CSTRING
#define PRJ_CF_USE_STRING CString
#else
#define PRJ_CF_USE_STRING std::string
#endif

  //typedef const int def_t;

#define PRJ_NAME_STR_LENGTH_MAX 			  26
#define PRJ_FILE_DIR_STR_MAX			      260
#define PRJ_DATA_MSG_STR_MAX				    256



typedef void (*evt_cb)(void* obj, void* w_parm, void* l_parm);
typedef void (*init_cb)(void); //void(*cd_increase)(void)
typedef bool (*openExe_cb)(void); //void(*cd_increase)(void)
typedef void (*swTimer_cb)(void* obj); // 콜백 타입 정의

#include "util.h"



