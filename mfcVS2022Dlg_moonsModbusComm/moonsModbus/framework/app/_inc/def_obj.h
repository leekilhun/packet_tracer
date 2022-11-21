#pragma once




#define  SYSTEM_PERIPH_BASE						0x00000000UL // 증가 시 0x00100000UL~
/*!< peripherals> */

//Hardware Layer 
#define HL_IO_BASE									(SYSTEM_PERIPH_BASE + 0x00000010UL)
#define HL_MOTION_LIB_BASE          (SYSTEM_PERIPH_BASE + 0x00000020UL)
#define HL_SERIAL_BASE							(SYSTEM_PERIPH_BASE + 0x00000100UL)
#define HL_ETHERNET_BASE	   				(SYSTEM_PERIPH_BASE + 0x00000140UL)
#define HL_DIST_SENSOR_BASE 				(SYSTEM_PERIPH_BASE + 0x00000160UL)
#define HL_CYLINDER_BASE						(SYSTEM_PERIPH_BASE + 0x00000200UL)
#define HL_VACUUM_BASE							(SYSTEM_PERIPH_BASE + 0x00000400UL)
#define HL_AXIS_BASE								(SYSTEM_PERIPH_BASE + 0x00000800UL)
//Mechanical Layer 
#define ML_OPPANEL_BASE							(SYSTEM_PERIPH_BASE + 0x00001000UL) // max 16 object 
//#define ML__BASE										(SYSTEM_PERIPH_BASE + 0x00001010UL)
//#define ML__BASE										(SYSTEM_PERIPH_BASE + 0x00001020UL)


//Control Layer 
#define CTR_MANAGE_OPPANEL_BASE					(SYSTEM_PERIPH_BASE + 0x00001100UL)
//#define CTR__BASE										(SYSTEM_PERIPH_BASE + 0x00001110UL)

//Process Layer 
#define TRS_AUTOMANAGER_BASE					(SYSTEM_PERIPH_BASE + 0x00001200UL)
#define TRS_TOWERLAMP_BASE						(SYSTEM_PERIPH_BASE + 0x00001210UL)
namespace AP_OBJ
{


	enum class ObjType
	{
		NONE,
		// Hardware Layer
		HL_IO = 100,
		HL_CYLINDER,			//101
		HL_VACUUM,				//102
		HL_INDUCTION_MOTOR,		//103
		HL_VISION,				//104
		HL_MOTION,				//105
		HL_MOTION_LIB,			//106
		HL_MOTION_BOARD,		//107
		HL_SERIAL,				//108
		HL_ETHERNET,			//109
		HL_DIST_SENSOR,			//110

		// Mechanical Layer - Common
		ML_OP_PANEL = 200,

		// Control Layer - Common
		CL_MANAGE_OP_PANEL = 300,

		// Process Layer - Common
		PL_AUTO_MANAGER = 400,
		PL_TOWER_LAMP = 405,

	};


	struct ObjDataDef
	{
		/****************************************************
		 *	data
		 ****************************************************/
		struct Info_t
		{
			// 생성 후 시스템에 포함되면 true
			bool		 is_reg;
			// 오브젝트 
			ObjType	 type;
			uint32_t baseAddr;
			uint32_t instanceNo;
			char	objName[PRJ_NAME_STR_LENGTH_MAX];
			char	logFileName[PRJ_FILE_DIR_STR_MAX];
			Icommon* pObj;
		};

	private:
		bool			m_is_reg;
		ObjType		m_type;
		uint32_t	m_objID;
		uint32_t	m_baseAddr;
		uint32_t	m_instanceNo;
		char	m_objName[PRJ_NAME_STR_LENGTH_MAX];
		char	m_logFileName[PRJ_FILE_DIR_STR_MAX];

		Icommon* m_pObj;


	public:
		/****************************************************
		 *	Constructor
		 ****************************************************/
		ObjDataDef() :m_is_reg(false)
			, m_type(ObjType::NONE), m_objID(0), m_baseAddr(0)
			, m_instanceNo(0), m_pObj(nullptr)
		{
			Init();
		}

		ObjDataDef(const ObjDataDef& self)
		{
			m_is_reg = self.m_is_reg;
			m_type = self.m_type;
			m_objID = self.m_objID;
			m_baseAddr = self.m_baseAddr;
			m_instanceNo = self.m_instanceNo;
			memcpy_s(m_objName, PRJ_NAME_STR_LENGTH_MAX, self.m_objName, PRJ_NAME_STR_LENGTH_MAX);
			memcpy_s(m_logFileName, PRJ_FILE_DIR_STR_MAX, self.m_logFileName, PRJ_FILE_DIR_STR_MAX);
			m_pObj = self.m_pObj;
		}
		virtual ~ObjDataDef() { Init(); }

		/****************************************************
		 *	func
		 ****************************************************/
	public:
		inline void Init() {
			m_is_reg = false;
			m_type = ObjType::NONE;
			m_objID = 0;
			m_baseAddr = 0;
			m_instanceNo = 0;
			memset(m_objName, 0x00, PRJ_NAME_STR_LENGTH_MAX);
			memset(m_logFileName, 0x00, PRJ_FILE_DIR_STR_MAX);
			m_pObj = nullptr;


		};

		inline errno_t SetData(Info_t* data) {
			m_type = data->type;

			m_baseAddr = data->baseAddr;
			m_instanceNo = data->instanceNo;
			m_objID = m_baseAddr + m_instanceNo;
			memcpy_s(m_objName, PRJ_NAME_STR_LENGTH_MAX, data->objName, strlen(data->objName));
			memcpy_s(m_logFileName, PRJ_NAME_STR_LENGTH_MAX, data->logFileName, strlen(data->logFileName));
			//m_is_reg = data->is_reg;

			//m_pObj				= data.m_pObj;
			return 0;
		}

		inline errno_t SetData(ObjDataDef& data) {
			//m_is_reg = data->is_reg;
			m_type = data.m_type;

			m_baseAddr = data.m_baseAddr;
			m_instanceNo = data.m_instanceNo;
			m_objID = m_baseAddr + m_instanceNo;
			memcpy_s(m_objName, PRJ_NAME_STR_LENGTH_MAX, data.m_objName, strlen(data.m_objName));
			memcpy_s(m_logFileName, PRJ_NAME_STR_LENGTH_MAX, data.m_logFileName, strlen(data.m_logFileName));

			//m_pObj				= data.m_pObj;
			return 0;
		}

		inline void GetData(ObjDataDef* data) const {
			data->m_type = m_type;
			data->m_baseAddr = m_baseAddr;
			data->m_instanceNo = m_instanceNo;
			memcpy_s(data->m_objName, PRJ_NAME_STR_LENGTH_MAX, m_objName, strlen(m_objName));
			memcpy_s(data->m_logFileName, PRJ_NAME_STR_LENGTH_MAX, m_logFileName, strlen(m_logFileName));
			data->m_pObj = m_pObj;
			data->m_is_reg = m_is_reg;
		}

		inline int GetObjectID() const {
			return m_objID;
		}

		inline ObjType GetObjectType() const {
			return m_type;
		}

		inline char* GetObjectName() const {
			return (char*)&m_objName[0];
		}

		inline char* GetLogFileName() const {
			return (char*)&m_logFileName[0];
		}

		inline int GetObjectInstanceNo() const {
			return m_instanceNo;
		}

		inline Icommon* GetSystemObject() const {
			return m_pObj;
		}

		inline void SetSystemObject(Icommon* pComp) {
			m_is_reg = true;
			m_pObj = pComp;
		}

		inline bool IsMatchingID(int ojb_id) {
			if (ojb_id == m_objID)
				return true;
			return false;
		}

		inline uint32_t GetBaseAddr() const {
			return m_baseAddr;
		}
	};

}