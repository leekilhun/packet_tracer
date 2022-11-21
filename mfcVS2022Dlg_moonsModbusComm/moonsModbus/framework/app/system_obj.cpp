#include "pch.h"
#include "system_def.h"

#include "system_obj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>
/// 
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////

/*
객체 정보를 추가할 때 uint32_t baseAddr 정렬 수선를 참조

*/
#include <vector>

using namespace AP_SYS;
using namespace AP_OBJ;

static std::vector <ObjDataDef::Info_t> sysObj
{
  { false,ObjType::NONE			    ,NULL			      ,0	,"None"						,""												,NULL},
  { false,ObjType::HL_IO		    ,HL_IO_BASE		      ,0	,"IO"							,"./Log/IO.log"						,NULL},
  { false,ObjType::HL_SERIAL	    ,HL_SERIAL_BASE		  ,static_cast<int>(SERIAL_OBJ::moons_motor)	,"Serial_1"				,"./Log/Serial.log"				,NULL},   
	{ false,ObjType::HL_MOTION_LIB	    ,HL_MOTION_LIB_BASE		  ,static_cast<int>(MOTION_LIB_OBJ::moons_modbus_lib)	,"moons_modbus_lib"				,"./Log/moons_modbus_lib.log"				,NULL},

	{ false,ObjType::HL_MOTION		,HL_AXIS_BASE		  ,static_cast<int>(AXIS_OBJ::moons_modbus)	,"moons_modbus"		,"./Log/Axis.log"					,NULL},

};



//////////////////////////////////////////////////////////////////////////////////////



MSystemObject::MSystemObject() :m_idx(0), m_infoObject(nullptr)/*, m_iNumObjects(0)*/
{
	makeSystemInfo();
}

MSystemObject::~MSystemObject()
{
	if (m_infoObject)
	{
		delete[] m_infoObject;
	}
}

void MSystemObject::makeSystemInfo()
{
	ObjDataDef::Info_t* pSys = sysObj.data();
	m_infoObject = new ObjDataDef[sysObj.size()];
	for (size_t i = 0; i < sysObj.size() ; i++)
	{
		m_infoObject[i].SetData(&sysObj[i]);
	}	
}

ObjDataDef* MSystemObject::getObjectData(int obj_id) const
{
	ObjDataDef* ret = nullptr;
	for (size_t i = 0; i < sysObj.size(); i++)
	{
		if (m_infoObject[i].IsMatchingID(obj_id))
		{
			ret = &m_infoObject[i];
			i = sysObj.size();
		}
	}	
	return ret;
}

common::Attribute_t MSystemObject::GetCommonData(int id)
{
	common::Attribute_t commonData;
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return commonData;

	commonData.SetObjectID(id);
	commonData.SetInstanceNo(p_data->GetObjectInstanceNo());
	commonData.SetObjectName(p_data->GetObjectName());
	commonData.SetLogFileName(p_data->GetLogFileName());
	return commonData;
}

ObjType MSystemObject::GetObjectType(int id) const
{
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return ObjType();
	return p_data->GetObjectType();
}

char* MSystemObject::GetObjectName(int id) const
{
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return (char*)nullptr;
	return p_data->GetObjectName();
}

char* MSystemObject::GetLogFileName(int id) const
{
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return (char*)nullptr;
	return p_data->GetLogFileName();
}

Icommon* MSystemObject::GetSystemObject(int id)
{
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return (Icommon*)nullptr;
	return p_data->GetSystemObject();
}

errno_t MSystemObject::RegisterComponent(int id, Icommon* pComp)
{
	ObjDataDef* p_data = getObjectData(id);
	if (p_data == nullptr)
		return ERR_SYSTEMINFO_INVALID_OBJECTID;


	uint32_t object_id = 0;

	// vector내에 존재 확인
	bool is_exist_obj = false; 
	for (size_t i = 0; i < sysObj.size(); i++)
	{
		object_id = sysObj[i].baseAddr + sysObj[i].instanceNo;
		if (object_id == id)
		{
			sysObj[i].is_reg = true;
			sysObj[i].pObj = pComp;
			i = sysObj.size();
		}
	}	

	p_data->SetSystemObject(pComp);
	return ERR_SYSTEMINFO_SUCCESS;
}
