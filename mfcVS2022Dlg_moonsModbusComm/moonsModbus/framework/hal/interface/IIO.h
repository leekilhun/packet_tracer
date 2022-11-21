#pragma once

class IIO :public virtual Icommon
{
public:

	virtual bool IsOn(uint32_t addr) = 0;
	virtual bool IsOff(uint32_t addr) = 0;
	virtual errno_t OutputOn(uint32_t addr) = 0;
	virtual errno_t OutputOff(uint32_t addr) = 0;
	virtual errno_t OutputToggle(uint32_t addr) = 0;

};