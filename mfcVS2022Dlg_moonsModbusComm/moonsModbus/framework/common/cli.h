#pragma once



/*
 * cli.h
 *
 *  Created on: sep 27, 2022
 *      Author: 이길훈
 *
 *  CPP 클래스 함수 테스를 할 수 있도록 변경함
 *  mfc 실행 프롬프트에서 기능이 제대로 안됨 (back, 저장된 것들)
 * 
 * 22.10.11  : cli namespce 적용 
 */


#ifdef PRJ_USE_FUNC_CLI

#define CLI_CMD_LIST_MAX      PRJ_CLI_CMD_LIST_MAX
#define CLI_CMD_NAME_MAX      PRJ_CLI_CMD_NAME_MAX

#define CLI_LINE_HIS_MAX      PRJ_CLI_LINE_HIS_MAX
#define CLI_LINE_BUF_MAX      PRJ_CLI_LINE_BUF_MAX

namespace cli
{



#define CLI_KEY_BACK              0x08
#define CLI_KEY_DEL               0x7F
#define CLI_KEY_ENTER             0x0D
#define CLI_KEY_ESC               0x1B
#define CLI_KEY_LEFT              0x4B
#define CLI_KEY_RIGHT             0x4D
#define CLI_KEY_UP                0x48
#define CLI_KEY_DOWN              0x50
#define CLI_KEY_HOME              0x31
#define CLI_KEY_END               0x34

#define CLI_PROMPT_STR            "cli# "

#define CLI_ARGS_MAX              32
#define CLI_PRINT_BUF_MAX         256



  typedef struct cliArgsType
  {
    uint16_t   argc;
    char** argv;

    int32_t(*getData)(uint8_t index);
    float    (*getFloat)(uint8_t index);
    char* (*getStr)(uint8_t index);
    bool     (*isStr)(uint8_t index, char* p_str);
    void* obj;
  } cli_args_t;


  enum cliKeyType
 {
    CLI_RX_IDLE,
    CLI_RX_SP1,
    CLI_RX_SP2,
    CLI_RX_SP3,
    CLI_RX_SP4,
  };


  typedef struct cliCmdType 
  {
    void* object;
    char   cmd_str[CLI_CMD_NAME_MAX];
    void (*cmd_func)(void*, cli_args_t*);
  } cli_cmd_t;


  typedef struct cliLineType
  {
    uint8_t buf[CLI_LINE_BUF_MAX];
    uint8_t buf_len;
    uint8_t cursor;
    uint8_t count;
  } cli_line_t;


  typedef struct cliType
  {
    bool     is_open;
    bool     is_log;
    uint8_t  state;
    char     print_buffer[CLI_PRINT_BUF_MAX];
    uint16_t  argc;
    char* argv[CLI_ARGS_MAX];


    bool        hist_line_new;
    int8_t      hist_line_i;
    uint8_t     hist_line_last;
    uint8_t     hist_line_count;

    cli_line_t  line_buf[CLI_LINE_HIS_MAX];
    cli_line_t  line;

    uint16_t    cmd_count;
    cli_cmd_t   cmd_list[CLI_CMD_LIST_MAX];
    cli_args_t  cmd_args;
  } cli_t;











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
}


  

#endif


