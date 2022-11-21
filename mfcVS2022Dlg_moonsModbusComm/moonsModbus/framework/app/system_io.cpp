#include "pch.h"
#include "system_def.h"

#include "system_dat.h"

#include "system_io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace AP_SYS;
using namespace LIB_FASTECH;

MSystemIO::MSystemIO(common::Attribute_t common_data)
	: Icommon(common_data), m_fastNetIO(), m_TrdLife(), m_IsOpened(), m_TrdId(), m_hThread()
{

}

MSystemIO::~MSystemIO()
{
	threadStop(); 
	for (GFastech::GFastech_motorE_lib*& elm : m_fastNetIO)
	{
		delete elm;
		elm = nullptr;
	}
	/*for (GFastech_IO*& elm : m_fastNetIO)
	{
		delete elm;
		elm = nullptr;
	}*/
	//m_fastNetIO.reset();
}

errno_t MSystemIO::StartThread()
{
	threadRun();

	return errno_t();
}

int MSystemIO::AddDevice(MSystemIO::cfg_t& cfg)
{
	int ret = -1;

	switch (cfg.data.deviceType)
	{
	case MSystemIO::device_e::fastech_netio_in_16:
	{
		//GFastech_motorE_lib
		AP_DATA::GConfig_dat::netCfgDat_t data{};
		GFastech::GFastech_motorE_lib::cfg_t net_cfg{};
		cfg.p_cfgDat->GetNetConfig(cfg.data.net_id, &data);
		
		net_cfg.SetIpStr((const char*)(data.ip_str));
		net_cfg.SetNetMode(data.mode_str);
		//net_cfg.SetIpStr("192.168.0.4");
		//net_cfg.SetNetMode("TCP_CLIENT");

		GFastech::GFastech_motorE_lib::ioCfg_t conf;
		conf.device_type = GFastech::GFastech_motorE_lib::type_e::I16;
		conf.startAddrIn = cfg.data.startAddrIn;//IO::in::OP_SW_Estop;
		conf.lenghtIn = cfg.data.lenghtIn;
		conf.startAddrOut = 0;// cfg.data.startAddrOut;//IO::out::OP_SW_Lamp_start;
		conf.lenghtOut = 0;//cfg.data.lenghtOut;
		GFastech::GFastech_motorE_lib* pObj = new GFastech::GFastech_motorE_lib(net_cfg, conf);
		m_fastNetIO.emplace_back(pObj);

		device_t set_data{};
		set_data = cfg.data;
		set_data.objIo = (IIO*)pObj;
		m_vDevice.emplace_back(set_data);

		ret = pObj->Initialize();


	}
	break;
	case MSystemIO::device_e::fastech_netio_out_16:
	{

		//GFastech_motorE_lib
		AP_DATA::GConfig_dat::netCfgDat_t data{};
		GFastech::GFastech_motorE_lib::cfg_t net_cfg{};
		cfg.p_cfgDat->GetNetConfig(cfg.data.net_id, &data);

		net_cfg.SetIpStr((const char*)(data.ip_str));
		net_cfg.SetNetMode(data.mode_str);
		//net_cfg.SetIpStr("192.168.0.4");
		//net_cfg.SetNetMode("TCP_CLIENT");

		GFastech::GFastech_motorE_lib::ioCfg_t conf;
		conf.device_type = GFastech::GFastech_motorE_lib::type_e::O16;
		conf.startAddrIn = 0;//cfg.data.startAddrIn;
		conf.lenghtIn = 0;// cfg.data.lenghtIn;
		conf.startAddrOut = cfg.data.startAddrOut; 
		conf.lenghtOut = cfg.data.lenghtOut;
		GFastech::GFastech_motorE_lib* pObj = new GFastech::GFastech_motorE_lib(net_cfg, conf);
		m_fastNetIO.emplace_back(pObj);

		device_t set_data{};
		set_data = cfg.data;
		set_data.objIo = (IIO*)pObj;
		m_vDevice.emplace_back(set_data);

		ret = pObj->Initialize();


	}
	break;
	case MSystemIO::device_e::fastech_netio_i16o16:
	{

		//GFastech_motorE_lib
		AP_DATA::GConfig_dat::netCfgDat_t data{};
		GFastech::GFastech_motorE_lib::cfg_t net_cfg{};
		cfg.p_cfgDat->GetNetConfig(cfg.data.net_id, &data);

		net_cfg.SetIpStr((const char*)(data.ip_str));
		net_cfg.SetNetMode(data.mode_str);
		//net_cfg.SetIpStr("192.168.0.4");
		//net_cfg.SetNetMode("TCP_CLIENT");

		GFastech::GFastech_motorE_lib::ioCfg_t conf;
		conf.device_type = GFastech::GFastech_motorE_lib::type_e::I16O16;
		conf.startAddrIn = cfg.data.startAddrIn;
		conf.lenghtIn = cfg.data.lenghtIn;
		conf.startAddrOut = cfg.data.startAddrOut;
		conf.lenghtOut = cfg.data.lenghtOut;
		GFastech::GFastech_motorE_lib* pObj = new GFastech::GFastech_motorE_lib(net_cfg, conf);
		m_fastNetIO.emplace_back(pObj);

		device_t set_data{};
		set_data = cfg.data;
		set_data.objIo = (IIO*)pObj;
		m_vDevice.emplace_back(set_data);

		ret = pObj->Initialize();


	}
	break;

	default:
		break;
	}
	return ret;
}

bool MSystemIO::IsOn(uint32_t addr)
{
	MSystemIO::obj_t io_obj{};
	bool ret = true;
	uint32_t org_addr{};
	if (addr > (IO_ADDR_BASE_OUTPUT - 1))
	{
		if (getOutObject(addr, io_obj))
		{
			org_addr = addr - m_vDevice[io_obj.index].startAddrOut;
		}
		else
			ret = false;
	}
	else
	{
		if (getInObject(addr, io_obj))
		{
			org_addr = addr - m_vDevice[io_obj.index].startAddrIn;
		}
		else
			ret = false;
	}
	if (ret)
	{
		ret = io_obj.p_Io->IsOn(addr);

	}
	return ret;

}

bool MSystemIO::IsOff(uint32_t addr)
{
	bool ret = true;
	ret = !(IsOn(addr));
	return ret;
}

errno_t MSystemIO::OutputOn(uint32_t addr)
{
	bool ret = true;
	std::lock_guard <std::mutex>lock(m_mutex);
	MSystemIO::obj_t io_obj{};
	if (getOutObject(addr, io_obj))
	{
		uint32_t org_addr = addr - m_vDevice[io_obj.index].startAddrOut;
	}
	else
		ret = false;
	if (ret)
	{
		io_obj.p_Io->OutputOn(addr);
	}
	return ret;
}

errno_t MSystemIO::OutputOff(uint32_t addr)
{
	bool ret = true;
	std::lock_guard <std::mutex>lock(m_mutex);
	MSystemIO::obj_t io_obj{};
	if (getOutObject(addr, io_obj))
	{
		uint32_t org_addr = addr - m_vDevice[io_obj.index].startAddrOut;
	}
	else
		ret = false;
	if (ret)
	{
		io_obj.p_Io->OutputOff(addr);
	}
	return ret;
}

errno_t MSystemIO::OutputToggle(uint32_t addr)
{
	bool ret = true;
	std::lock_guard <std::mutex>lock(m_mutex);
	MSystemIO::obj_t io_obj{};
	if (getOutObject(addr, io_obj))
	{
		uint32_t org_addr = addr - m_vDevice[io_obj.index].startAddrOut;
	}
	else
		ret = false;
	if (ret)
	{
		io_obj.p_Io->OutputToggle(addr);
	}
	return ret;
}

uint32_t MSystemIO::GetResponseTime(MSystemIO::device_e device) const
{	
	return 0;//m_fastNetIO[static_cast<uint32_t>(device)]->GetResponseTime();
}

void MSystemIO::threadStop(void)
{
	m_TrdLife = false;
	while (m_TrdId);
}

void MSystemIO::threadRun(void)
{
	m_TrdLife = true;
	// 포트 감시 스레드 생성
	m_hThread = CreateThread(
		(LPSECURITY_ATTRIBUTES)NULL,
		0,
		(LPTHREAD_START_ROUTINE)threadFunc,
		(LPVOID)this,
		0,
		&m_TrdId
	);
}

void MSystemIO::threadJob(void)
{
	for (size_t i = 0; i < m_fastNetIO.size(); i++)
	{
		if (m_fastNetIO[i]->IsConnected())
		{
			m_fastNetIO[i]->GetOutput();
			m_fastNetIO[i]->GetInput();
		}		
	}		

}

UINT MSystemIO::threadFunc(LPVOID pParam)
{
	MSystemIO* pThis = (MSystemIO*)pParam;

	TRACE("MSystemIO Thread Start\n\r");

	while (pThis->m_TrdLife)
	{
		pThis->threadJob();

		Sleep(10);
	}

	TRACE("MSystemIO Thread Terminatet\n\r");
	pThis->m_TrdId = 0;
	pThis->m_hThread = NULL;
	return 0;
}

bool MSystemIO::getInObject(uint32_t addr, obj_t& obj)
{
	// TODO: 여기에 구현 코드 추가.
	//uint32_t ret_Addr = addr - IO_ADDR_BASE_INPUT;
	size_t elm_idx = 0;
	uint32_t high = 0, low = 0;
	bool is_fined = false;
	for (size_t i = 0; i < m_vDevice.size(); i++)
	{
		low = m_vDevice[i].startAddrIn;
		high = m_vDevice[i].startAddrIn + (m_vDevice[i].lenghtIn - 1);
		elm_idx = i;
		if (util::between(addr, low, high))
		{
			i = m_vDevice.size();
			is_fined = true;
		}
	}
	if (is_fined == false)
	{
		return false;
	}
	MSystemIO::obj_t ret_obj{};
	ret_obj.p_Io = m_vDevice[elm_idx].objIo;
	ret_obj.index = (uint32_t)elm_idx;
	obj = ret_obj;
	return true;
}

bool MSystemIO::getOutObject(uint32_t addr, obj_t& obj)
{
	// TODO: 여기에 구현 코드 추가.
	size_t elm_idx = 0;
	uint32_t high = 0, low = 0;
	bool is_fined = false;
	for (size_t i = 0; i < m_vDevice.size(); i++)
	{
		low = m_vDevice[i].startAddrOut;
		high = m_vDevice[i].startAddrOut + (m_vDevice[i].lenghtOut - 1);
		elm_idx = i;
		if (util::between(addr, low, high))
		{		
			i = m_vDevice.size();
			is_fined = true;
		}
	}
	if (is_fined == false)
	{
		return false;
	}
	MSystemIO::obj_t ret_obj{};
	ret_obj.p_Io = m_vDevice[elm_idx].objIo;
	ret_obj.index = (uint32_t)elm_idx;
	obj = ret_obj;
	return true;
}
