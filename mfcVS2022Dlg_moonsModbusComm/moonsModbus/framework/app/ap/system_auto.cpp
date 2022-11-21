#include "pch.h"

#include "system.h"

#include "system_auto.h"

using namespace AP_SYS;
using namespace AP_OBJ;


MSystemAuto::MSystemAuto(common::Attribute_t& common_data, cfg_t& cfg) : Icommon(common_data), m_cfg(cfg)
, m_eventQue(), m_TrdLife(), m_hThread(), m_TrdId(), m_mutex()
{

}

MSystemAuto::~MSystemAuto()
{
	threadStop();
}

void MSystemAuto::threadStop(void)
{
	if (this) {
		m_TrdLife = false;
		while (m_TrdId);
	}
	
}

void MSystemAuto::threadRun(void)
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

void MSystemAuto::threadJob(void)
{

	switch (GetOPMode())
	{
	case MSystemOP::mode_e::STOP:
	{

	}
	break;
	case MSystemOP::mode_e::READY:
	{

	}
	break;
	case MSystemOP::mode_e::DRY_RUN:
	{

	}
	break;
	case MSystemOP::mode_e::AUTORUN:
	{
		switch (GetOPStatus())
		{
		case MSystemOP::status_e::INIT:
		{

		}
		break;
		case MSystemOP::status_e::STEP_STOP:
		{

		}
		break;
		case MSystemOP::status_e::ERR_STOP:
		{

		}
		break;
		case MSystemOP::status_e::RUN_READY:
		{

		}
		break;
		case MSystemOP::status_e::RUN:
		{

		}
		break;
		default:
			break;
		}//switch (GetOPStatus())

	}
	break;
	default:
		break;
	}//switch (GetOPMode())

	doRunStep();
	m_cfg.pOp->UpdateOpPanelAndTowerLamp();
}

UINT MSystemAuto::threadFunc(LPVOID pParam)
{
	MSystemAuto* pThis = (MSystemAuto*)pParam;
	LTRACE("MSystemAuto Thread Start\n\r");

	while (pThis->m_TrdLife)
	{
		pThis->threadJob();

		Sleep(10);
	}

	LTRACE("MSystemAuto Thread Terminatet\n\r");
	pThis->m_TrdId = 0;
	pThis->m_hThread = NULL;
	return 0;
}

void MSystemAuto::functionCB(void* obj, void* w_parm, void* l_parm)
{
}

void MSystemAuto::doRunStep()
{
}

errno_t MSystemAuto::checkStartRunCondition()
{
	return errno_t();
}

errno_t MSystemAuto::checkApplicationSystem()
{
	return errno_t();
}

errno_t MSystemAuto::AutoReady()
{
	return errno_t();
}

errno_t MSystemAuto::EStopAxis(AXIS_OBJ axis_is)
{
	return errno_t();
}

MSystemOP::mode_e MSystemAuto::GetOPMode() const
{
	return m_cfg.pOp->GetMode()/*m_opMode*/;
}

MSystemOP::status_e MSystemAuto::GetOPStatus() const
{
	return m_cfg.pOp->GetStatus()/*m_opStatus*/;
}

void MSystemAuto::SetOPMode(MSystemOP::mode_e mode)
{
	m_cfg.pOp->SetMode(mode);
}

void MSystemAuto::SetOPStatus(MSystemOP::status_e status)
{
	m_cfg.pOp->SetStatus(status);
}

void MSystemAuto::AlarmAuto(int obj_id, errcode err_code, const char* file, const char* func, const int line, const char* msg)
{
}

void MSystemAuto::AlarmAuto(obj_errno obj_errcode, const char* file, const char* func, const int line)
{
	if (this == nullptr)
		return;

	constexpr std::size_t directory_cnt_max = 50;
	std::vector<char> tmp(strlen(file)+5);
	// strcpy_s(tmp.data(), strlen(file) + 5, file); // 카피 방법 1
	memcpy(tmp.data(), file, tmp.size()); // 카피 방법 2
	std::vector<char*> tmp_p(directory_cnt_max);
	uint8_t cnt = util::trans::SplitArgs(tmp.data(), tmp_p.data(), "\\", (int)directory_cnt_max);
	// 파일 이름 tmp_p[cnt-1];


	size_t func_length = strlen(func);
	char* begin = (char*)func;
	char* end = strchr(begin, '::') + 2;
	size_t stringLength = end - begin;
	func_length = func_length - stringLength + 1; // 종단 \0 포함하여 길이를 계산 
	std::vector<char>v_func_name;
	for (size_t i = 0; i < func_length; i++)
	{
		v_func_name.emplace_back((*end++));
	}
	//
	

	syslog_Write(syslog::level::err, 0, SYSLOGFILE_ERR, file, func, line, "Log 발생 메세지 보냅니다");

	return ;

}

bool MSystemAuto::isEStopSWPressed()
{
	return  m_cfg.pOp->GetPressed(MSystemOP::op_t::sw_e::ESTOP);
}

bool MSystemAuto::isDoorOpened()
{
	return  m_cfg.pOp->GetPressed(MSystemOP::op_t::sw_e::ESTOP);
}

bool MSystemAuto::isMotorsReady()
{
	return false;
}

bool MSystemAuto::isRobotReady()
{
	return false;
}

errno_t MSystemAuto::StartThread()
{
	errno_t ret =  ERROR_SUCCESS;
	if (this == nullptr)
		- 1;
	threadRun();
	return ret;
}

errno_t MSystemAuto::StopThread()
{
	errno_t ret = ERROR_SUCCESS;
	if (this == nullptr)
		- 1;
	threadStop();
	return ret;
}
