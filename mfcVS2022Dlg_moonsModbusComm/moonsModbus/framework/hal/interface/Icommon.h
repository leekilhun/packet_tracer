#pragma once
//#include "system_def.h"




namespace common
{
	const	errno_t	ERR_ICOMMON_SUCCESS = 0;
	const int LOG_DEFAULT_KEEPING_DAYS = 60;

	struct Attribute_t
	{
		int   ObjNo{};
		int		InstanceNo{};
		int		LogKeepingDays{};
		char	ObjectName[PRJ_NAME_STR_LENGTH_MAX]{};
		char	logFileName[PRJ_FILE_DIR_STR_MAX]{};

		Attribute_t() {};
		Attribute_t(const Attribute_t& self)
		{
			ObjNo = self.ObjNo;
			InstanceNo = self.InstanceNo;
			memcpy_s(ObjectName, PRJ_NAME_STR_LENGTH_MAX, self.ObjectName, strlen(self.ObjectName));
			memcpy_s(logFileName, PRJ_FILE_DIR_STR_MAX, self.logFileName, strlen(self.logFileName));
			LogKeepingDays = self.LogKeepingDays;
		}


		inline errno_t SetData(Attribute_t* data) {
			ObjNo = data->ObjNo;
			InstanceNo = data->InstanceNo;
			memcpy_s(ObjectName, PRJ_NAME_STR_LENGTH_MAX, data->ObjectName, strlen(data->ObjectName));
			memcpy_s(logFileName, PRJ_FILE_DIR_STR_MAX, data->logFileName, strlen(data->logFileName));
			LogKeepingDays = data->LogKeepingDays;
			return ERR_ICOMMON_SUCCESS;
		}

		inline void GetData(Attribute_t* data) const {
			data->ObjNo = ObjNo;
			data->InstanceNo = InstanceNo;
			memcpy_s(data->ObjectName, PRJ_NAME_STR_LENGTH_MAX, ObjectName, strlen(ObjectName));
			memcpy_s(data->logFileName, PRJ_FILE_DIR_STR_MAX, logFileName, strlen(logFileName));
			data->LogKeepingDays = LogKeepingDays;
		}

		inline Attribute_t GetData() const {
			Attribute_t data;
			data.ObjNo = ObjNo;
			data.InstanceNo = InstanceNo;
			memcpy_s(data.ObjectName, PRJ_NAME_STR_LENGTH_MAX, ObjectName, strlen(ObjectName));
			memcpy_s(data.logFileName, PRJ_FILE_DIR_STR_MAX, logFileName, strlen(logFileName));
			data.LogKeepingDays = LogKeepingDays;
			return data;
		}


		inline char* GetObjectName() const {
			return (char*)&ObjectName[0];
		}

		inline int GetInstanceNo() const {
			return InstanceNo;
		}

		inline errno_t SetInstanceNo(int no) {
			InstanceNo = no;
			return ERR_ICOMMON_SUCCESS;
		}

		inline char* GetLogFileName() const {
			return (char*)&logFileName[0];
		}


		inline int GetLogKeepingDays() const {
			return LogKeepingDays;
		}

		inline int GetObjectID() const {
			return ObjNo;
		}

		inline errno_t SetObjectID(int id) {
			ObjNo = id;
			return ERR_ICOMMON_SUCCESS;
		}

		inline errno_t SetObjectName(char* name, int length = PRJ_NAME_STR_LENGTH_MAX) {
			memcpy_s(ObjectName, PRJ_NAME_STR_LENGTH_MAX, name, length);
			return ERR_ICOMMON_SUCCESS;
		}

		inline errno_t SetLogFileName(char* name, int length = PRJ_FILE_DIR_STR_MAX) {

			memcpy_s(logFileName, PRJ_FILE_DIR_STR_MAX, name, length);
			return ERR_ICOMMON_SUCCESS;
		}

	};

}


class Icommon
{
	/****************************************************
	 *	data
	 ****************************************************/
private:

public:

protected:
	/** 속성 데이터*/
	common::Attribute_t m_Attribute;

	/** Log */
	syslog::MSystemLog m_log;

	/****************************************************
	 *	Constructor
	 ****************************************************/
public:
	Icommon() 
	{
	};
	Icommon(common::Attribute_t commonData): m_Attribute(commonData)
	{		
	}

	/** @destructor */
	virtual ~Icommon() {
		/*if (m_plog != nullptr)
			delete m_plog;
		m_plog = nullptr;*/
	}

	/****************************************************
	 *	func
	 ****************************************************/
	 /** Set Common Attribute */
	inline void SetCommonAttribute(common::Attribute_t commonData) {
		m_Attribute = commonData;
	}

	/** Get Common Attribute */
	inline void GetCommonAttribute(common::Attribute_t* pCommonData) {
		if (pCommonData == NULL)
			return;
		pCommonData->SetData(&m_Attribute);
	}

	/** GetObject Name */
	inline char* GetObjectName() const {
		return m_Attribute.GetObjectName();
	}

	/// <summary>
	/// Object ID를 설정한다.
	/// </summary>
	/// <param name="ObjectID">설정할 Object ID</param>
	inline void SetObjectID(int ObjectID) {
		m_Attribute.SetObjectID(ObjectID);
	}

	/// <summary>
	/// Object ID를 읽는다
	/// </summary>
	/// <param name=""></param>
	/// <returns> Object ID</returns>
	inline int GetObjectID(void) const {
		return m_Attribute.GetObjectID();
	}

	/// <summary>
	/// Instance 번호를 설정한다.
	/// </summary>
	/// <param name="InstanceNo">설정할 Instacne 번호</param>
	inline void SetInstanceNo(int InstanceNo) {
		m_Attribute.SetInstanceNo(InstanceNo);
	}

	/// <summary>
	/// Instance 번호를 읽는다.
	/// </summary>
	/// <param name=""></param>
	/// <returns>Instacne 번호</returns>
	inline int GetInstanceNo(void) {
		return m_Attribute.GetInstanceNo();
	}


protected:

	/// <summary>
	/// objectID + 에러코드 조합하여 반환
	/// </summary>
	/// <param name="err_code">object Type + errno 의 6자리</param>
	/// <returns>object id + err_code [xxx][xxxxxx]</returns>
	inline obj_errno generateErrorCode(errcode err_code) {
#ifndef	NO_ERROR_ENCODING
		obj_errno	obj_type_errno = 0;
		/** Error Code가 SUCCESS가 아니면 코드 생성 */
		if ((errno_t)err_code != ERROR_SUCCESS)
		{
			obj_type_errno = (err_code)+(m_Attribute.GetObjectID() * 1000000);
		}	

		return obj_type_errno;
#else
		return err_code;
#endif
	}

};