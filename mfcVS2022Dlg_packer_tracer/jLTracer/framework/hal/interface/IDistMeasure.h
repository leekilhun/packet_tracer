#pragma once

class IDistMeasure :public virtual Icommon
{
public:

  virtual errno_t Open() = 0;
  virtual void Close() = 0;
  virtual bool IsOpened() const = 0;
  virtual void Recovery() = 0;
  virtual double GetDist() = 0;

};