#pragma once



///** Error Define */
const	errno_t		ERR_SYSTEMINFO_INVALID_OBJECTID = -1;
const	errno_t		ERR_SYSTEMINFO_SUCCESS = 0;



//
//#define SYSTEM_IO  extern ObjDataDef::Info_t systemObj;\
//                          ((Icommon*)(systemObj[2].pObj))\

namespace AP_SYS
{

	using namespace AP_OBJ;

	class MSystemObject
	{

		/****************************************************
		 *	data
		 ****************************************************/
	private:
		uint32_t m_idx;

		/* Number of component objects */
		//uint32_t	m_iNumObjects;

		/* Object */
		ObjDataDef* m_infoObject;

	public:


		/****************************************************
		 *	Constructor
		 ****************************************************/
	public:
		MSystemObject();
		virtual ~MSystemObject();

		/****************************************************
		 *	func
		 ****************************************************/
	private:
		void	makeSystemInfo();
		//int		getInstanceNo(int obj_id) const;
		ObjDataDef* getObjectData(int obj_id) const;

	public:
		common::Attribute_t GetCommonData(int id);
		ObjType	GetObjectType(int id) const;
		char* GetObjectName(int id) const;
		char* GetLogFileName(int id) const;
		Icommon* GetSystemObject(int id);
		errno_t		RegisterComponent(int id, Icommon* pComp);

	};


}