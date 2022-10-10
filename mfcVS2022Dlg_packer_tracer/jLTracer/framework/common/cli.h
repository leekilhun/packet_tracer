#pragma once



/*
 * cli.h
 *
 *  Created on: sep 27, 2022
 *      Author: 이길훈
 *
 *  CPP 클래스 함수 테스를 할 수 있도록 변경함
 *  mfc 실행 프롬프트에서 기능이 제대로 안됨 (back, 저장된 것들)
 */


#ifdef PRJ_USE_FUNC_CLI

#define CLI_CMD_LIST_MAX      PRJ_CLI_CMD_LIST_MAX
#define CLI_CMD_NAME_MAX      PRJ_CLI_CMD_NAME_MAX

#define CLI_LINE_HIS_MAX      PRJ_CLI_LINE_HIS_MAX
#define CLI_LINE_BUF_MAX      PRJ_CLI_LINE_BUF_MAX




  typedef struct
  {
    uint16_t   argc;
    char** argv;

    int32_t(*getData)(uint8_t index);
    float    (*getFloat)(uint8_t index);
    char* (*getStr)(uint8_t index);
    bool     (*isStr)(uint8_t index, char* p_str);
    void* obj;
  } cli_args_t;


  bool cliInit(void);
  bool cliOpen();
  bool cliOpenLog();
  void cliArgs(cli_args_t* args, int argc, char* argv[]);
  bool cliMain(void);
  void cliPrintf(const char* fmt, ...);
  bool cliAdd(void* obj, const char* cmd_str, void (*p_func)(void*, cli_args_t*));
  bool cliKeepLoop(void);

  uint32_t cliAvailable(void);
  uint8_t  cliRead(void);
  uint32_t clitWrite(uint8_t* p_data, uint32_t length);

#endif


