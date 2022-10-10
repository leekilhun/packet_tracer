#pragma once



class IAxis :public virtual Icommon
{
public:
	virtual errno_t Initialize() = 0;
	virtual errno_t MotorOnOff(bool on_off) = 0;
	virtual bool  IsMotorOn() const = 0;
	virtual void AttCallbackFunc(void* obj, evt_cb cb) = 0;
#if 0

	virtual errno_t WaitDone(uint8_t mode) = 0;
	virtual bool IsAxisDone() = 0;
	virtual errno_t  ClearState() = 0;
	virtual uint32_t  GetMotorState() = 0;

	virtual int Move(int cmd_pos, uint32_t cmd_vel = 100, uint32_t acc = 100, uint32_t decel = 100) = 0;
	//���� �����Ѵ�. (�Ѱ��� �࿡ ���� ����)
	virtual int Stop() = 0;
	//���� ����̵��� ���� �����Ѵ�.
	virtual int VStop() = 0;
	// ���� ��������Ѵ�.
	virtual int EStop() = 0;

	virtual int JogMove(uint32_t cmd_vel = 1000, bool is_cw = true) = 0;
	virtual int JogMove(uint32_t jog_pos, uint32_t cmd_vel = 1000, bool is_cw = true) = 0;
#endif

};