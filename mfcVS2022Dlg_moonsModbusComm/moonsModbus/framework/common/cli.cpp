#include "pch.h"

#include <conio.h>
#include <Windows.h>
#include "cli.h"



// 콘솔 가상 터미널 시퀀스
// https://learn.microsoft.com/ko-kr/windows/console/console-virtual-terminal-sequences

#ifdef PRJ_USE_FUNC_CLI


using namespace cli;




#define ESC "\x1b"
#define CSI "\x1b["



cli_t   cli_node;



static bool cliUpdate(cli_t* p_cli, uint8_t rx_data);
static void cliLineClean(cli_t* p_cli);
static void cliLineAdd(cli_t* p_cli);
static void cliLineChange(cli_t* p_cli, int8_t key_up);
static void cliShowPrompt(cli_t* p_cli);
static void cliToUpper(char* str);
static bool cliRunCmd(cli_t* p_cli);
static bool cliParseArgs(cli_t* p_cli);

static int32_t  cliArgsGetData(uint8_t index);
static float    cliArgsGetFloat(uint8_t index);
static char* cliArgsGetStr(uint8_t index);
static bool     cliArgsIsStr(uint8_t index, char* p_str);


void cliShowList(void* obj, cli_args_t* args);


bool cli::cliInit(void)
{
  cli_node.is_open = false;
  cli_node.is_log = false;
  cli_node.state = CLI_RX_IDLE;

  cli_node.hist_line_i = 0;
  cli_node.hist_line_last = 0;
  cli_node.hist_line_count = 0;
  cli_node.hist_line_new = false;

  cli_node.cmd_args.getData = cliArgsGetData;
  cli_node.cmd_args.getFloat = cliArgsGetFloat;
  cli_node.cmd_args.getStr = cliArgsGetStr;
  cli_node.cmd_args.isStr = cliArgsIsStr;

  cliLineClean(&cli_node);


  cli::cliAdd(nullptr, "help", cliShowList);

  return true;
}

bool cli::cliOpen()
{
  cli_node.is_open = true;

  return cli_node.is_open;
}

bool cli::cliOpenLog()
{
  bool ret = false;

  return ret;
}

void cli::cliArgs(cli_args_t* args, int argc, char* argv[])
{
  args->getData = cliArgsGetData;
  args->getFloat = cliArgsGetFloat;
  args->getStr = cliArgsGetStr;
  args->isStr = cliArgsIsStr;

  if (argc > 0)
  {
    args->argc = argc - 1;
    args->argv = &argv[1];
  }
  else
  {
    args->argc = 0;
    args->argv[0] = NULL;
  }
}

bool cliLogClose(void)
{
  cli_node.is_log = false;
  return true;
}

void cliShowLog(cli_t* p_cli)
{
  if (cli_node.is_log == true)
  {
    cliPrintf( "Cursor  : %d\n", p_cli->line.cursor);
    cliPrintf( "Count   : %d\n", p_cli->line.count);
    cliPrintf( "buf_len : %d\n", p_cli->line.buf_len);
    cliPrintf( "buf     : %s\n", p_cli->line.buf);
    cliPrintf( "line_i  : %d\n", p_cli->hist_line_i);
    cliPrintf( "line_lt : %d\n", p_cli->hist_line_last);
    cliPrintf( "line_c  : %d\n", p_cli->hist_line_count);

    for (int i = 0; i < p_cli->hist_line_count; i++)
    {
      cliPrintf("buf %d   : %s\n", i, p_cli->line_buf[i].buf);
    }
    cliPrintf("\n");
  }
}

void cliShowPrompt(cli_t* p_cli)
{
  cliPrintf( "\n\r");
  cliPrintf( CLI_PROMPT_STR);
}

bool cli::cliMain(void)
{
  if (cli_node.is_open != true)
  {
    return false;
  }

  if (cliAvailable() > 0)
  {
    cliUpdate(&cli_node, cliRead());
  }

  return true;
}

uint32_t cli::cliAvailable(void)
{
  uint32_t ret = 0;
  if (_kbhit())
  {
    ret = 1;
  }

  return ret;
}

uint8_t cli::cliRead(void)
{
  return _getch();
}

bool cliUpdate(cli_t* p_cli, uint8_t rx_data)
{
  bool ret = false;
  uint8_t tx_buf[8];
  cli_line_t* line;

  line = &p_cli->line;

  //cliPrintf("rx 0x%X\n", rx_data);

  if (p_cli->state == CLI_RX_IDLE)
  {
    switch (rx_data)
    {
      // 엔터
      //
    case CLI_KEY_ENTER:
      if (line->count > 0)
      {
        cliLineAdd(p_cli);
        cliRunCmd(p_cli);
      }

      line->count = 0;
      line->cursor = 0;
      line->buf[0] = 0;
      cliShowPrompt(p_cli);
      break;


    case CLI_KEY_ESC:
      p_cli->state = CLI_RX_SP1;
      break;

    case 0x00:
      p_cli->state = CLI_RX_SP2;
      break;

      // DEL
      //
    case CLI_KEY_DEL:
      if (line->cursor < line->count)
      {
        uint8_t mov_len;

        mov_len = line->count - line->cursor;
        for (int i = 1; i < mov_len; i++)
        {
          line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
        }

        line->count--;
        line->buf[line->count] = 0;

        cliPrintf("\x1B[1P");
      }
      break;


      // 백스페이스
      //
    case CLI_KEY_BACK:
      if (line->count > 0 && line->cursor > 0)
      {
        if (line->cursor == line->count)
        {
          line->count--;
          line->buf[line->count] = 0;
        }

        if (line->cursor < line->count)
        {
          uint8_t mov_len;

          mov_len = line->count - line->cursor;

          for (int i = 0; i < mov_len; i++)
          {
            line->buf[line->cursor + i - 1] = line->buf[line->cursor + i];
          }

          line->count--;
          line->buf[line->count] = 0;
        }
      }

      if (line->cursor > 0)
      {
        line->cursor--;
        cliPrintf("\b \b\x1B[1P");
        //printf(ESC "[1D"); // '\x1b[31m'은 n이 31인 ESC [ <n> m<> ESC [ <n> D
        //printf(CSI "K"); // clear the line
      }
      break;


    default:
      if ((line->count + 1) < line->buf_len)
      {
        if (line->cursor == line->count)
        {
          clitWrite(&rx_data, 1);

          line->buf[line->cursor] = rx_data;
          line->count++;
          line->cursor++;
          line->buf[line->count] = 0;
        }
        if (line->cursor < line->count)
        {
          uint8_t mov_len;

          mov_len = line->count - line->cursor;
          for (int i = 0; i < mov_len; i++)
          {
            line->buf[line->count - i] = line->buf[line->count - i - 1];
          }
          line->buf[line->cursor] = rx_data;
          line->count++;
          line->cursor++;
          line->buf[line->count] = 0;

          cliPrintf("\x1B[4h%c\x1B[4l", rx_data);
        }
      }
      break;
    }
  }

  switch (p_cli->state)
  {
  case CLI_RX_SP1:
    p_cli->state = CLI_RX_SP2;
    break;

  case CLI_RX_SP2:
    p_cli->state = CLI_RX_SP3;
    break;

  case CLI_RX_SP3:
    p_cli->state = CLI_RX_IDLE;

    if (rx_data == CLI_KEY_LEFT)
    {
      if (line->cursor > 0)
      {
        line->cursor--;
        tx_buf[0] = 0x1B;
        tx_buf[1] = 0x5B;
        tx_buf[2] = rx_data;
        clitWrite(tx_buf, 3);
      }
    }

    if (rx_data == CLI_KEY_RIGHT)
    {
      if (line->cursor < line->count)
      {
        line->cursor++;

        tx_buf[0] = 0x1B;
        tx_buf[1] = 0x5B;
        tx_buf[2] = rx_data;
        clitWrite(tx_buf, 3);
      }
    }

    if (rx_data == CLI_KEY_UP)
    {
      cliLineChange(p_cli, true);
      cliPrintf((char*)p_cli->line.buf);
    }

    if (rx_data == CLI_KEY_DOWN)
    {
      cliLineChange(p_cli, false);
      cliPrintf((char*)p_cli->line.buf);
    }

    if (rx_data == CLI_KEY_HOME)
    {
      cliPrintf("\x1B[%dD", line->cursor);
      line->cursor = 0;

      p_cli->state = CLI_RX_SP4;
    }

    if (rx_data == CLI_KEY_END)
    {
      uint16_t mov_len;

      if (line->cursor < line->count)
      {
        mov_len = line->count - line->cursor;
        cliPrintf("\x1B[%dC", mov_len);
      }
      if (line->cursor > line->count)
      {
        mov_len = line->cursor - line->count;
        cliPrintf("\x1B[%dD", mov_len);
      }
      line->cursor = line->count;
      p_cli->state = CLI_RX_SP4;
    }
    break;

  case CLI_RX_SP4:
    p_cli->state = CLI_RX_IDLE;
    break;
  }



  cliShowLog(p_cli);

  return ret;
}

void cliLineClean(cli_t* p_cli)
{
  p_cli->line.count = 0;
  p_cli->line.cursor = 0;
  p_cli->line.buf_len = CLI_LINE_BUF_MAX - 1;
  p_cli->line.buf[0] = 0;
}

void cliLineAdd(cli_t* p_cli)
{

  p_cli->line_buf[p_cli->hist_line_last] = p_cli->line;

  if (p_cli->hist_line_count < CLI_LINE_HIS_MAX)
  {
    p_cli->hist_line_count++;
  }

  p_cli->hist_line_i = p_cli->hist_line_last;
  p_cli->hist_line_last = (p_cli->hist_line_last + 1) % CLI_LINE_HIS_MAX;
  p_cli->hist_line_new = true;
}

void cliLineChange(cli_t* p_cli, int8_t key_up)
{
  uint8_t change_i;


  if (p_cli->hist_line_count == 0)
  {
    return;
  }


  if (p_cli->line.cursor > 0)
  {
    cliPrintf("\x1B[%dD", p_cli->line.cursor);
  }
  if (p_cli->line.count > 0)
  {
    cliPrintf("\x1B[%dP", p_cli->line.count);
  }


  if ((bool)key_up == true)
  {
    if (p_cli->hist_line_new == true)
    {
      p_cli->hist_line_i = p_cli->hist_line_last;
    }
    p_cli->hist_line_i = (p_cli->hist_line_i + p_cli->hist_line_count - 1) % p_cli->hist_line_count;
    change_i = p_cli->hist_line_i;
  }
  else
  {
    p_cli->hist_line_i = (p_cli->hist_line_i + 1) % p_cli->hist_line_count;
    change_i = p_cli->hist_line_i;
  }

  p_cli->line = p_cli->line_buf[change_i];
  p_cli->line.cursor = p_cli->line.count;

  p_cli->hist_line_new = false;
}

bool cliRunCmd(cli_t* p_cli)
{
  bool ret = false;


  if (cliParseArgs(p_cli) == true)
  {
    cliPrintf("\r\n");

    cliToUpper(p_cli->argv[0]);

    for (int i = 0; i < p_cli->cmd_count; i++)
    {
      if (strcmp(p_cli->argv[0], p_cli->cmd_list[i].cmd_str) == 0)
      {
        p_cli->cmd_args.argc = p_cli->argc - 1;
        p_cli->cmd_args.argv = &p_cli->argv[1];
        p_cli->cmd_list[i].cmd_func(p_cli->cmd_list[i].object, &p_cli->cmd_args);
        break;
      }
    }
  }

  return ret;
}

bool cliParseArgs(cli_t* p_cli)
{
  bool ret = false;
  char* tok = nullptr;
  char* next_ptr = nullptr;
  uint16_t argc = 0;
  static const char* delim = " \f\n\r\t\v";
  char* cmdline;
  char** argv;

  p_cli->argc = 0;

  cmdline = (char*)p_cli->line.buf;
  argv = p_cli->argv;

  argv[argc] = NULL;

  for (tok = strtok_s(cmdline, delim, &next_ptr); tok; tok = strtok_s(NULL, delim, &next_ptr))
  {
    argv[argc++] = tok;
  }

  p_cli->argc = argc;

  if (argc > 0)
  {
    ret = true;
  }

  return ret;
}

void cli::cliPrintf(const char* fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  int32_t len;
  cli_t* p_cli = &cli_node;


  len = vsnprintf(p_cli->print_buffer, 256, fmt, arg);
  va_end(arg);

  clitWrite((uint8_t*)p_cli->print_buffer, len);
}

void cliToUpper(char* str)
{
  uint16_t i;
  uint8_t  str_ch;

  for (i = 0; i < CLI_CMD_NAME_MAX; i++)
  {
    str_ch = str[i];

    if (str_ch == 0)
    {
      break;
    }

    if ((str_ch >= 'a') && (str_ch <= 'z'))
    {
      str_ch = str_ch - 'a' + 'A';
    }
    str[i] = str_ch;
  }

  if (i == CLI_CMD_NAME_MAX)
  {
    str[i - 1] = 0;
  }
}

int32_t cliArgsGetData(uint8_t index)
{
  int32_t ret = 0;
  cli_t* p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  ret = (int32_t)strtoul((const char*)p_cli->cmd_args.argv[index], (char**)NULL, (int)0);

  return ret;
}

float cliArgsGetFloat(uint8_t index)
{
  float ret = 0.0;
  cli_t* p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  ret = (float)strtof((const char*)p_cli->cmd_args.argv[index], (char**)NULL);

  return ret;
}

char* cliArgsGetStr(uint8_t index)
{
  char* ret = NULL;
  cli_t* p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  ret = p_cli->cmd_args.argv[index];

  return ret;
}

bool cliArgsIsStr(uint8_t index, char* p_str)
{
  bool ret = false;
  cli_t* p_cli = &cli_node;


  if (index >= p_cli->cmd_args.argc)
  {
    return 0;
  }

  if (strcmp(p_str, p_cli->cmd_args.argv[index]) == 0)
  {
    ret = true;
  }

  return ret;
}

bool cli::cliKeepLoop(void)
{
  cli_t* p_cli = &cli_node;


  if (cliAvailable() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool cli::cliAdd(void* obj, const char* cmd_str, void (*p_func)(void*, cli_args_t*))
{
  bool ret = true;
  cli_t* p_cli = &cli_node;
  uint16_t index;

  if (p_cli->cmd_count >= CLI_CMD_LIST_MAX)
  {
    return false;
  }

  index = p_cli->cmd_count;

  strcpy_s(p_cli->cmd_list[index].cmd_str, cmd_str);
  p_cli->cmd_list[index].cmd_func = p_func;
  p_cli->cmd_list[index].object = obj;
  cliToUpper(p_cli->cmd_list[index].cmd_str);

  p_cli->cmd_count++;

  return ret;
}

void cliShowList(void* obj, cli_args_t* args)
{
  cli_t* p_cli = &cli_node;


  cliPrintf("\r\n");
  cliPrintf("---------- cmd list ---------\r\n");

  for (int i = 0; i < p_cli->cmd_count; i++)
  {
    cliPrintf(p_cli->cmd_list[i].cmd_str);
    cliPrintf("\r\n");
  }

  cliPrintf("-----------------------------\r\n");
}





uint32_t cli::clitWrite(uint8_t* p_data, uint32_t length)
{
  uint32_t ret = 0;
  for (uint32_t i = 0; i < length; i++)
  {
    putc(p_data[i], stdout);
  }
  ret = length;
  return ret;
}



#endif
