#pragma once

#include <array>
#include <vector>
#include <string>
#include <thread>
#include <algorithm>

// ������Ʈ ����
 /****************************************************
  *	prject define
  ****************************************************/

//#define PROJECT_MAIN_HEADER          "moonsModbus.h"
//#define PROJECT_MAIN_DLG_HEAdER      "moonsModbusDlg.h"


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

//#define PRJ_USE_FUNC_CLI
#ifdef PRJ_USE_FUNC_CLI
  #define PRJ_CLI_CMD_LIST_MAX 16 
  #define PRJ_CLI_CMD_NAME_MAX 16 
  
  #define PRJ_CLI_LINE_HIS_MAX 8
  #define PRJ_CLI_LINE_BUF_MAX 64
#endif

//6�ڸ��� �����Ǹ� object Type Code [xxx]  + ���ǵ� error no [xxx]
typedef errno_t errcode;
//9�ڸ��� �����Ǹ� Object ID [xxx] errcode [xxx+xxx] ������ �ڵ�
//objid[xxx]+ type[xxx] + error no[xxx]
typedef int obj_errno;


typedef void (*evt_cb)(void* obj, void* w_parm, void* l_parm);
typedef void (*init_cb)(void); //void(*cd_increase)(void)
typedef bool (*openExe_cb)(void); //void(*cd_increase)(void)
typedef void (*swTimer_cb)(void* obj); // �ݹ� Ÿ�� ����




