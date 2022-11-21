#pragma once

#define ICOMM_CMD_MAX_DATA_LENGTH       256

class IComm :public virtual Icommon
{
public:
  virtual errno_t Open() = 0;
  virtual void Close() = 0;
  virtual bool IsOpened() const = 0;
  virtual void Recovery() = 0;
  virtual void AttCallbackFunc(void* obj, evt_cb cb) = 0;
  virtual int SendData(char* p_str, uint32_t length) = 0;

};