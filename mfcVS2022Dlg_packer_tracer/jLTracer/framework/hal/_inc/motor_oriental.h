#pragma once

#ifdef USE_HW_MOTOR_ORIENTAL




// ## Library 추가하기
#pragma comment(lib, "lib/oriental/AZ/x64/INA_AZ_DLL.lib")

#include "INA_AZ_DLL.h"

constexpr int DEF_AZ_RET_SUCCESS		  = 1;
constexpr int DEF_AZ_RET_FAIL				  = 0;

constexpr errno_t DEF_AZ_ERR_SUCCESS  = 0;
constexpr errno_t DEF_AZ_ERR_RET_FAIL = -1;
//
#define AZ_LOG_WRITE(fmt, ...)		m_log.syslog_Write(loglevel::info, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)
#define AZ_ERR_WRITE(fmt, ...)		m_log.syslog_Write(loglevel::err, m_Attribute.GetObjectID(), m_Attribute.GetLogFileName(), (__FILE__), __LOGFUNC__, __LINE__, fmt, ##__VA_ARGS__)


class oriental_motor : public IAxis
{
	/****************************************************
	 *	data
	 ****************************************************/
public:
	struct cfg_t
	{
		IComm* pComm{};
		uint8_t AxisId{};
		int			port_no{};								// 시리얼포트
		int			baudrate{};					// Baudrate
		int			comm_delay_ms{};					// Delay Time 초기값 - 35ms
		int			slave_start_id{};					// 
		int			default_velocity{};		// 

	};


	/****************************************************
	 *	Oriental Moter
	 ****************************************************/
	typedef union
	{
		int	iStatus;
		struct
		{
			unsigned	AZ_FLAG_M0_R : 1;			// = 0x00000001;
			unsigned	AZ_FLAG_M1_R : 1;			// = 0x00000002;
			unsigned	AZ_FLAG_M2_R : 1;			// = 0x00000004;
			unsigned	AZ_FLAG_START_R : 1;	// = 0x00000008;
			unsigned	AZ_FLAG_HOME_END : 1;	// = 0x00000010;
			unsigned	AZ_FLAG_READY : 1;		// = 0x00000020;
			unsigned	AZ_FLAG_INFO : 1;			// = 0x00000040;
			unsigned	AZ_FLAG_ALM_A : 1;		// = 0x00000080;
			unsigned	AZ_FLAG_SYS_BSY : 1;	// = 0x00000100;
			unsigned	AZ_FLAG_AREA0 : 1;		// = 0x00000200;
			unsigned	AZ_FLAG_AREA1 : 1;		// = 0x00000400;
			unsigned	AZ_FLAG_AREA2 : 1;		// = 0x00000800;
			unsigned	AZ_FLAG_TIM : 1;			// = 0x00001000;
			unsigned	AZ_FLAG_MOVE : 1;			// = 0x00002000;
			unsigned	AZ_FLAG_IN_POS : 1;		// = 0x00004000;
			unsigned	AZ_FLAG_TLC : 1;			// = 0x00008000;
		};
	} ORIENRAL_AZ_STATUS;

	ORIENRAL_AZ_STATUS m_regAxis;




	/**
	*	Oriental Motor - Parameter
	*/
	INA_Serial	m_Serial;					    // -- 시리얼 통신 구조체 변수
	int					m_SerialIndex;				// -- 시리얼 통신 INDEX
	bool				m_IsConnect;					// -- 시리얼 통신 상태 변수

	int			m_DataNo = 0;		// 

	uint8_t			m_AxisId;
	oriental_motor::cfg_t m_Cfg;

	/****************************************************
	 *	Constructor
	 ****************************************************/
public:
	inline oriental_motor(common::Attribute_t &common_data, cfg_t &cfg)
		:Icommon(common_data), m_AxisId(0), m_Cfg(cfg)
    ,	m_Serial(), m_IsConnect(), m_SerialIndex(), m_regAxis()
	{

		AZ_LOG_WRITE("Initialize oriental_motor[%d]", m_Attribute.GetInstanceNo());

		errno_t ret_init = Initialize();
		if (ret_init != DEF_AZ_ERR_SUCCESS)
		{
			//m_plog->syslog_Write()
			AZ_ERR_WRITE("oriental_motor[%d] FAIL : Initialize ", m_Attribute.GetInstanceNo());
			TRACE("FAIL : Oriental Motor Initialize \n");
		}

#ifdef _DEBUG
		cliAdd(this, "oriental", cliOrientalTest);
#endif


	}
	virtual ~oriental_motor()
	{
		AZ_Disconnect();
	}


	/****************************************************
   *	func
   ****************************************************/
private:

#ifdef _DEBUG

	inline static void oriental_motor::cliOrientalTest(void* obj, cli_args_t* args)
	{
		oriental_motor* pThis = (oriental_motor*)obj;

		bool ret = true;
		if (args->argc == 3)
		{
			if (args->isStr(0, "write_dio_out"))
			{

			}
			else if (args->isStr(0, "write_rio_out"))
			{

			}
			else
			{
				ret = false;
			}
		}
		else	if (args->argc == 2)
		{
			int value = (int)args->getData(1);
			if (args->isStr(0, "jog"))
			{
				errno_t result = pThis->JogPitchMove(value);
				if (result == DEF_AZ_ERR_SUCCESS)
				{
					cliPrintf("JogPitchMove [%d] success\n", value);
				}
				else
				{
					cliPrintf("JogPitchMove faile\n");
				}
			}
			else if (args->isStr(0, "motion"))
			{
				errno_t result = pThis->AZ_Start(value);
				if (result == DEF_AZ_ERR_SUCCESS)
				{
					cliPrintf("AZ_Start [%d] success\n", value);
				}
				else
				{
					cliPrintf("AZ_Start faile\n");
				}
				
			}
			else
			{
				ret = false;
			}
		}
		else if (args->argc == 1)
		{
			errno_t result = 0;
			if (args->isStr(0, "motor_off"))
			{
				result = pThis->AZ_FreeOn();
				if (result == DEF_AZ_ERR_SUCCESS)
				{
					cliPrintf("motor_off success\n");
				}
				else
				{
					cliPrintf("motor_off faile\n");
				}
				/*while (cliKeepLoop())
				{

				}*/
			}
			else if (args->isStr(0, "motor_on"))
			{
				result = pThis->AZ_FreeOff();
				if (result == DEF_AZ_ERR_SUCCESS)
				{
					cliPrintf("motor_on success\n");
				}
				else
				{
					cliPrintf("motor_on faile\n");
				}
			}
			else if (args->isStr(0, "alarm_reset"))
			{
				result = pThis->AZ_Alarm_Reset();
				if (result == DEF_AZ_ERR_SUCCESS)
				{
					cliPrintf("alarm_reset success\n");
				}
				else
				{
					cliPrintf("alarm_reset faile\n");
				}
			}
			else if (args->isStr(0, "get_alarm"))
			{
				result = pThis->AZ_Get_Current_Alarm();				
				cliPrintf("get_alarm return value : %d \n", result);
				
			}
			else if (args->isStr(0, "get_flag"))
			{
				cliPrintf("get_flag return value : %d \n", pThis->GetFlagStatus());
			}
			else if (args->isStr(0, "move"))
			{
				result = pThis->Move();
				cliPrintf("move ret - %d \n", result);

			}
			else if (args->isStr(0, "init"))
			{
				pThis->Initialize();
			}
			else
			{
				ret = false;
			}
		}
		else
		{
			ret = false;
		}


		if (ret == false)
		{
			cliPrintf("oriental motor_on\n");
			cliPrintf("oriental motor_off\n");
			cliPrintf("oriental alarm_reset\n");
			cliPrintf("oriental get_alarm\n");
			cliPrintf("oriental init\n");
			cliPrintf("oriental move\n");
			cliPrintf("oriental get_flag\n");

			cliPrintf("oriental jog value[1~1000]\n");
			cliPrintf("oriental motion no[1~5]\n");

		}

	}


#endif

public:


	inline errno_t Initialize() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = 0; // retrun 1 : Success , 0 : fail

		ret = AZ_Connect();
		if (ret != DEF_AZ_ERR_SUCCESS){
			AZ_ERR_WRITE("oriental AZ_Connect err_ret : %d", ret);
		}
		else
		{
			Sleep(100);
			ret = AZ_Stop();
			Sleep(100);
			ret = AZ_Alarm_Reset();

			ret = AZ_P_PReset();

			Sleep(100);
		}	

		return  ret;
	}




	/**
	*	Oriental Moter - Status
	*/
	inline bool IsHome_AZ() const {
		return m_regAxis.AZ_FLAG_HOME_END;
	}
	inline bool IsReady_AZ() const {
		return m_regAxis.AZ_FLAG_READY;
	}
	inline bool IsInfo_AZ() const {
		return m_regAxis.AZ_FLAG_INFO;
	}
	inline bool IsAlarm_AZ() const {
		return m_regAxis.AZ_FLAG_ALM_A;
	}
	inline bool IsBusy_AZ() const {
		return m_regAxis.AZ_FLAG_SYS_BSY;
	}



	/**
	*	Oriental Moter - Connecting
	*/
	inline errno_t AZ_Connect()	{
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		if (m_IsConnect)
			return ret;		

		int az_ret = INA_AZ_INITIALIZE(m_SerialIndex, m_Cfg.port_no, m_Cfg.baudrate);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람		
			AZ_ERR_WRITE("INA_AZ_INITIALIZE err_ret : %d", az_ret);
			
			AZ_Disconnect();
			return DEF_AZ_ERR_RET_FAIL;
		}
		// 시리얼 객체 리턴
		m_Serial = INA_AZ_GET_SERIAL(m_SerialIndex);
		Sleep(100);

		az_ret = INA_AZ_SET_DELAY(m_SerialIndex, m_Cfg.comm_delay_ms);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람		
			AZ_ERR_WRITE("INA_AZ_SET_DELAY err_ret : %d", az_ret);
			AZ_Disconnect();
			return DEF_AZ_ERR_RET_FAIL;
		}
		m_IsConnect = true;

		return ret;
	}
	inline errno_t AZ_Disconnect() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = 0;
		// 열려있는 것 모두 닫기. 모두 안닫으면 재연결 실패.
		for (int i = 0; i < m_SerialIndex; i++)
			az_ret = INA_AZ_UNINITIALIZE(i);  // retrun 1 : Success

		az_ret = INA_AZ_UNINITIALIZE(m_SerialIndex);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_UNINITIALIZE err_ret : %d", az_ret);
			ret =  DEF_AZ_ERR_RET_FAIL;
		}
		m_IsConnect = false;
		return ret;
	}
	inline errno_t AZ_Set_Serial() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;

		az_ret = INA_AZ_SET_SERIAL(m_SerialIndex, m_Serial);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_SERIAL err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}


	/**
	*	Oriental Moter - 동작
	*/
	inline errno_t AZ_Move() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		az_ret = INA_AZ_SET_FWD(m_SerialIndex, m_Cfg.slave_start_id, 0, TRUE);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_Move err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}
	inline errno_t AZ_Move_For(BOOL bOnOff) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		az_ret = INA_AZ_SET_FWD(m_SerialIndex, m_Cfg.slave_start_id, 0, bOnOff);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_Move_For err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}

	inline errno_t  AZ_Start(int data_no){	// Data No 설정값으로 동작
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		m_DataNo = data_no;

		// Ready Status 확인하기
		// ORIENRAL_AZ_STATUS stAxis;
		m_regAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_SerialIndex, m_Cfg.slave_start_id);

		if (!m_regAxis.AZ_FLAG_READY)
		{
			// 알람 추가하기.
			AZ_ERR_WRITE("error motor not ready !");
			return DEF_AZ_ERR_RET_FAIL;
		}

		// Move
		az_ret = INA_AZ_SET_START(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_START err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		Sleep(200);

		return ret;

	}
	inline errno_t AZ_Home() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;

		return ret;
	}


	inline errno_t AZ_Stop() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;

		az_ret = INA_AZ_SET_STOP(m_SerialIndex, m_Cfg.slave_start_id);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_STOP err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		m_regAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_SerialIndex, m_Cfg.slave_start_id);

		return ret;
	}



	inline errno_t AZ_FreeOn() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		const int motor_on = 1;

		az_ret = INA_AZ_SET_FREE(m_SerialIndex, m_Cfg.slave_start_id, motor_on);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_FreeOn err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}
	inline errno_t AZ_FreeOff(){
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		const int motor_off = 0;

		az_ret = INA_AZ_SET_FREE(m_SerialIndex, m_Cfg.slave_start_id, motor_off);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_FreeOff err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}


	/**
	*	Oriental Moter - Alarm, Reset, ... etc
	*/

	inline errno_t AZ_Alarm_Reset() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		const int motor_off = 0;

		az_ret = INA_AZ_SET_ALARM_RESET(m_SerialIndex, m_Cfg.slave_start_id);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_ALARM_RESET err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}

	inline errno_t AZ_Get_Current_Alarm() {
		int az_ret = DEF_AZ_RET_SUCCESS;
		az_ret = INA_AZ_GET_CURRENT_ALARM(m_SerialIndex, m_Cfg.slave_start_id);
		
	  AZ_ERR_WRITE("INA_AZ_GET_CURRENT_ALARM return value : %d", az_ret);
		return (errno_t)az_ret;
	}

	inline errno_t AZ_P_PReset() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		const int motor_off = 0;

		az_ret = INA_AZ_SET_P_PRESET(m_SerialIndex, m_Cfg.slave_start_id);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_P_PRESET err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}
	


	/**
*	Oriental Moter - Setting
*/
	inline errno_t AZ_SetOperatingCurrent(int value) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		int get_value = value;
		if (get_value > 1000)
			get_value = 1000;
		else if (get_value < 1)
			get_value = 1;

		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id,m_DataNo, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_DATA_OPERATING_CURRENT err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}

	inline errno_t AZ_GetOperatingCurrent(int data_no){
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		m_DataNo = data_no;

		az_ret = INA_AZ_GET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_GET_DATA_OPERATING_CURRENT err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}

	inline errno_t AZ_GetDataVelocity(int data_no) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		m_DataNo = data_no;

		az_ret = INA_AZ_GET_DATA_VELOCITY(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_GET_DATA_VELOCITY err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}

	inline errno_t AZ_GetCurrentVelocity() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;

		az_ret = INA_AZ_GET_CURRENT_VELOCITY(m_SerialIndex, m_Cfg.slave_start_id);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_GET_CURRENT_VELOCITY err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}


	inline errno_t AZ_Set_Velocity(int value = 1000, int data_no = -1) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		int get_value = value;

		if (data_no == 0 || data_no == 1)
			m_DataNo = data_no;

		az_ret = INA_AZ_SET_DATA_VELOCITY(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("INA_AZ_SET_DATA_VELOCITY err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}


	inline errno_t AZ_Set_Torque(int value) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		int get_value = value;		
		//m_regAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_SerialIndex, m_Cfg.slave_start_id);
		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo, get_value);  // 운전 전류 Set (0~1000, 1 = 0.1%)

		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_Set_Torque err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}

	inline errno_t AZ_Set_Torque_For(int value) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		int get_value = value;
		const int data_for = 0;


		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_for, get_value);  // 운전 전류 Set (0~1000, 1 = 0.1%)
		
		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_for, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_Set_Torque_For err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}

	inline errno_t AZ_Set_Torque_Back(int value) {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		int get_value = value;
		const int data_back = 1;

		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_back, get_value);  // 운전 전류 Set (0~1000, 1 = 0.1%)

		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_back, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// 알람
			AZ_ERR_WRITE("AZ_Set_Torque_Back err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}
		else
		{
			Sleep(200);
		}

		return ret;
	}



 inline errno_t JogPitchMove(int lPosition/*, DWORD dwVelocity, short siAccel, short siDecel, */)
	{
	 errno_t ret = DEF_AZ_ERR_SUCCESS;
	 int az_ret = DEF_AZ_RET_SUCCESS;

	 DWORD dwVelocity = 1000;
	 short siAccel = 200;
	 short siDecel = 200;

		//// **1202 -> msec 단위로 수정
		//siAccel = (short)siAccel / 4;
		//siDecel = (short)siDecel / 4;

		//// **050112
		//if (siAccel <= 0) siAccel = 1;
		//if (siDecel <= 0) siDecel = 1;


		az_ret = INA_AZ_SET_JOG_OPERATING_SPEED(m_SerialIndex, m_Cfg.slave_start_id, dwVelocity);
		az_ret = INA_AZ_SET_JOG_TRAVEL_AMOUNT(m_SerialIndex, m_Cfg.slave_start_id, abs(lPosition));

		if (lPosition > 0)
		{
			if ((az_ret = INA_AZ_SET_JOG_P(m_SerialIndex, m_Cfg.slave_start_id, TRUE)) != DEF_AZ_RET_SUCCESS)
			{
				AZ_ERR_WRITE("error : INA_AZ_SET_JOG_P");
				ret = DEF_AZ_ERR_RET_FAIL;
			}
		}
		else
		{
			if ((az_ret = INA_AZ_SET_JOG_N(m_SerialIndex, m_Cfg.slave_start_id, TRUE)) != DEF_AZ_RET_SUCCESS)
			{
				AZ_ERR_WRITE("error : INA_AZ_SET_JOG_N");
				ret = DEF_AZ_ERR_RET_FAIL;
			}
		}

		return ret;
	}



 inline errno_t Move()
 {
	 errno_t ret = DEF_AZ_ERR_SUCCESS;
	 int az_ret = DEF_AZ_RET_SUCCESS;

	 az_ret = INA_AZ_SET_FWD(m_SerialIndex, m_Cfg.slave_start_id, 0, TRUE);
	 if (az_ret != DEF_AZ_RET_SUCCESS)
	 {
		 // 알람
		 AZ_ERR_WRITE("Move err_ret : %d", az_ret);
		 ret = DEF_AZ_ERR_RET_FAIL;
	 }
	 else
	 {
		 Sleep(200);
	 }


	 return ret;
 }

 inline int GetFlagStatus()
 {	 
	 return (m_regAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_SerialIndex, m_Cfg.slave_start_id));
 }
























	// 추가하기
	// INA_AZ_SET_DELAY(m_nSerialIndex, nDelay); 


#if 0 // test oriental motor lib 
	//  packet frame 
	//  slave_addr   func code    data     crc-16
	//    8 bit        8 bit    N×8 bit    16 bit
	{
		uint8_t crc_data[6] = { 0x01,0x03,0x18,0x40,0x00,0x06 };
		uint16_t ret_crc = 0;
		ret_crc = util::crc16_modbus_cal(&crc_data[0], 6);


		int az_ret = 0; // retrun 1 : Success , 0 : fail
		constexpr int AZ_RET_SUCCESS = 1;
		constexpr int AZ_RET_FAIL = 0;
		// 초기화
		az_ret = INA_AZ_INITIALIZE(m_nSerialIndex, m_nPort, m_dwBaud);

		// 시리얼 객체 리턴
		m_Serial = INA_AZ_GET_SERIAL(m_nSerialIndex);
		Sleep(100);

		az_ret = INA_AZ_SET_DELAY(m_nSerialIndex, nDelay);


		Sleep(100);
		//az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);
		stAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);

		//ALARM 초기화 함수
		az_ret = INA_AZ_SET_ALARM_RESET(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);





		// test code1
		// SafeMoveReel_AZ_Wind

		constexpr int AZ_CW_T20 = 0;
		constexpr int AZ_CCW_T20 = 1;
		constexpr int AZ_CW_T50 = 2;
		constexpr int AZ_CW_T40 = 3;
		constexpr int AZ_CW_T30 = 4;

		bool is_ready = stAxis.AZ_FLAG_READY;

		if (is_ready == false)
		{
			AfxMessageBox(L"motor not ready!");
			goto az_end;
		}

		// POSITION 초기화 함수
		az_ret = INA_AZ_SET_P_PRESET(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);

		// 이동
	// AZ_CW_T20 0 - 속도  2000 토크 20% (릴 감는 방향)
	// AZ_CCW_T20 1 - 속도 -2000 토크 20% 
	// AZ_CW_T50 2 - 속도  2000 토크 50% (릴 감는 방향)

		az_ret = INA_AZ_SET_START(m_nSerialIndex, m_iSlaveStartID, AZ_CW_T20);
		if (az_ret != AZ_RET_SUCCESS)
		{
			AfxMessageBox(L"motor return fail!");
			goto az_end;
		}

		Sleep(1000);
		az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID);
		if (az_ret != AZ_RET_SUCCESS)
		{
			AfxMessageBox(L"motor return fail!");
			goto az_end;
		}


		az_ret = INA_AZ_SET_START(m_nSerialIndex, m_iSlaveStartID, AZ_CCW_T20);
		if (az_ret != AZ_RET_SUCCESS)
		{
			AfxMessageBox(L"motor return fail!");
			goto az_end;
		}

		Sleep(1000);
		az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID);
		if (az_ret != AZ_RET_SUCCESS)
		{
			AfxMessageBox(L"motor return fail!");
			goto az_end;
		}



		goto az_end;

		constexpr int AZ_OFF = 0;
		constexpr int AZ_ON = 1;

		//DRIVER INPUT COMMAND 함수
		int memory_addr = 1; //Memory Address 번호 ( 0 ~ 255 )
		int bit_no = 0;
		az_ret = INA_AZ_SET_START(m_nSerialIndex, m_iSlaveStartID, memory_addr); // START 입력 신호 SET 함수
		az_ret = INA_AZ_SET_HOME(m_nSerialIndex, m_iSlaveStartID); // HOME 입력 신호 SET 함수
		az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID); // STOP 입력 신호 SET 함수
		az_ret = INA_AZ_SET_MS0(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS0 입력 신호 SET 함수
		az_ret = INA_AZ_SET_MS1(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS1 입력 신호 SET 함수
		az_ret = INA_AZ_SET_MS2(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS2 입력 신호 SET 함수
		az_ret = INA_AZ_SET_CLEAR(m_nSerialIndex, m_iSlaveStartID); // BIT 입력 신호 초기화 함수

		// servo off
		az_ret = INA_AZ_SET_FREE(m_nSerialIndex, m_iSlaveStartID, AZ_ON);

		az_ret = INA_AZ_SET_FWD(m_nSerialIndex, m_iSlaveStartID, memory_addr, AZ_ON); // FWD 입력 신호 SET 함수
		az_ret = INA_AZ_SET_RVS(m_nSerialIndex, m_iSlaveStartID, memory_addr, AZ_ON);//	RVS 입력 신호 SET 함수



		az_ret = INA_AZ_SET_JOG_P(m_nSerialIndex, m_iSlaveStartID, AZ_ON); //JOG_P 입력 신호 SET 함수
		az_ret = INA_AZ_SET_JOG_P(m_nSerialIndex, m_iSlaveStartID, AZ_OFF); // JOG_N 입력 신호 SET 함수
		//int  az_state = stAxis.AZ_FLAG_INFO;


		//Return Value -  함수 각 MONITOR 기능 데이터 값
		az_ret = INA_AZ_GET_CURRENT_ALARM(m_nSerialIndex, m_iSlaveStartID); //현재 알람 상태 GET 함수 
		az_ret = INA_AZ_GET_CURRENT_POSITION(m_nSerialIndex, m_iSlaveStartID); //현재 위치 상태 GET 함수 
		az_ret = INA_AZ_GET_CURRENT_VELOCITY(m_nSerialIndex, m_iSlaveStartID); //현재 운전 속도 상태 GET함수 
		az_ret = INA_AZ_GET_CURRENT_FEEDBACK_POSITION(m_nSerialIndex, m_iSlaveStartID); //현재 FEEDBACK 위치 GET 함수 
		az_ret = INA_AZ_GET_CURRENT_FEEDBACK_VELOCITY(m_nSerialIndex, m_iSlaveStartID); //현재 FEEDBACK 속도 GET 함수



		// DRIVER 입력 지령 함수
		az_ret = INA_AZ_SET_CURRENT_DRIVER_INPUT_HIGH(m_nSerialIndex, m_iSlaveStartID, bit_no, AZ_ON); // Driver 입력 지령 HIGH SET함수 
		az_ret = INA_AZ_GET_CURRENT_DRIVER_INPUT_HIGH(m_nSerialIndex, m_iSlaveStartID);// Driver 입력 지령 HIGH GET함수
		az_ret = INA_AZ_SET_CURRENT_DRIVER_INPUT_LOW(m_nSerialIndex, m_iSlaveStartID, bit_no, AZ_ON); //  Driver 입력 지령 LOW SET함수 
		az_ret = INA_AZ_GET_CURRENT_DRIVER_INPUT_LOW(m_nSerialIndex, m_iSlaveStartID); //  Driver 입력 지령 LOW GET함수


		//OPERATING DATA R/W COMMAND 함수
		int set_value = 1;// 함수 설정 범위
		/*
			1:절대 위치 결정											2: 상대 위치 결정 지령 위치 기준
			3: 상대 위치 결정 검출 위치 기준 )		7: 연속 운전 위치제어
			8:Round 절대 위치 결정								9: Round 근처 위치 결정
			10: Round FWD 방향 절대 위치 결정			11: Round RVS 방향 절대 위치 결정
			12: Round 절대 누름									13: Round 근처 누름
			14: Round FWD 방향 누름							15: Round RVS 방향 누름
			16: 연속 운전 속도 제어								17: 연속 운전 누름
			18: 연속 운전 (Torque)								20: 절대 위치 결정 누름
			21: 상대 위치 결정 누름 지령 위치 기준 )
			22: 상대 위치 결정 누름 검출 위치 기준

			*/
		az_ret = INA_AZ_SET_DATA_POSITION_MODE(m_nSerialIndex, m_iSlaveStartID, memory_addr, set_value); //		운전 방식 SET 함수
		az_ret = INA_AZ_GET_DATA_POSITION_MODE(m_nSerialIndex, m_iSlaveStartID, memory_addr);            // 운전 방식 GET 함수


		int pos_val = 0;//-2,147,483,648~2,147,483,647step
		az_ret = INA_AZ_SET_DATA_POSITION(m_nSerialIndex, m_iSlaveStartID, memory_addr, pos_val); // 위치 SET 함수
		az_ret = INA_AZ_GET_DATA_POSITION(m_nSerialIndex, m_iSlaveStartID, memory_addr);          // 위치 GET 함수
		int vel_val = 0;//-4,000,000~4,000,000Hz
		az_ret = INA_AZ_SET_DATA_VELOCITY(m_nSerialIndex, m_iSlaveStartID, memory_addr, vel_val); //속도 SET 함수
		az_ret = INA_AZ_GET_DATA_VELOCITY(m_nSerialIndex, m_iSlaveStartID, memory_addr);          //속도 GET 함수

		int acc_val = 0;//1,000,000,000(1=0.001kHz)
		az_ret = INA_AZ_SET_DATA_ACC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr, acc_val); // 가속(기동, 변속) SET 함수
		az_ret = INA_AZ_GET_DATA_ACC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr); //			가속 기동 변속 ) GET 함수
		int dec_val = 0;//1,000,000,000(1=0.001kHz)
		az_ret = INA_AZ_SET_DATA_DEC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr, dec_val); //			감속 정지 SET 함수
		az_ret = INA_AZ_GET_DATA_DEC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr); //			감속 정지 ) GET 함수


		int current_val = 0;//: 0~1000(1= 0.1%)
		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, current_val); //운전 전류 SET 함수
		az_ret = INA_AZ_GET_DATA_OPERATING_CURRENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); // 운전 전류 GET 함수
		int delaytime = 0;// 0~65535(1 = 0.001s)
		az_ret = INA_AZ_SET_DATA_DELAY_TIME(m_nSerialIndex, m_iSlaveStartID, memory_addr, delaytime); // 운전 종료 지연 SET 함수
		az_ret = INA_AZ_GET_DATA_DELAY_TIME(m_nSerialIndex, m_iSlaveStartID, memory_addr); // 운전 종료 지연 GET 함수

		int link_val = 0; //0: 결합하지 않음, 1 : 수동 순차, 2 : 자동 순차, 3 : 형상 접속
		az_ret = INA_AZ_SET_DATA_LINK(m_nSerialIndex, m_iSlaveStartID, memory_addr, link_val); //	결합 SET 함수
		az_ret = INA_AZ_GET_DATA_LINK(m_nSerialIndex, m_iSlaveStartID, memory_addr);           //	결합 GET 함수
		int next_data_no = 0; // -256: STOP, 2 : +2, 1 : (+1), 0~255 : 운전 No.
		az_ret = INA_AZ_SET_DATA_NEXT_DATA_NO(m_nSerialIndex, m_iSlaveStartID, memory_addr, next_data_no); //	결합 대상 SET 함수
		az_ret = INA_AZ_GET_DATA_NEXT_DATA_NO(m_nSerialIndex, m_iSlaveStartID, memory_addr);               //	결합 대상 GET 함수


		int area_offset = 0; //-2,147,483,648~2,147,483,647step
		az_ret = INA_AZ_SET_DATA_AREA_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr, area_offset); //      OFFSET(AREA) SET 함수
		az_ret = INA_AZ_GET_DATA_AREA_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr); // OFFSET(AREA) GET 함수
		int area_width = 0; // -1 : 무효 , 0~4,194,303 : 1step 단위로 설정
		az_ret = INA_AZ_SET_DATA_AREA_WIDTH(m_nSerialIndex, m_iSlaveStartID, memory_addr, area_width); //    폭(AREA) SET 함수
		az_ret = INA_AZ_GET_DATA_AREA_WIDTH(m_nSerialIndex, m_iSlaveStartID, memory_addr); //    폭(AREA) GET 함수



		int loop_cnt = 0; //0: 없음 ((-), 2~255: Loop 횟수 (loop2{~loop256{)
		az_ret = INA_AZ_SET_DATA_LOOP_COUNT(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_cnt); //  Count(Loop) SET 함수
		az_ret = INA_AZ_GET_DATA_LOOP_COUNT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       Count(Loop) GET 함수
		int loop_offset = 0;// -4,194,304 ~ 4,194,303step
		az_ret = INA_AZ_SET_DATA_LOOP_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_offset); //       위치 Offset(Loop) SET 함수
		az_ret = INA_AZ_GET_DATA_LOOP_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       위치 Offset(Loop) GET 함수
		int  loop_end = 0; //0: 없음 ((-) , 1: }L End
		az_ret = INA_AZ_SET_DATA_LOOP_END(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_end); //       종료(Loop) S ET 함수
		az_ret = INA_AZ_GET_DATA_LOOP_END(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       종료(L oop) GET 함수


		int low_io_event = 0; // - 1: 없음 ((-) , 0~31: 운전 I/O Event 번호 (0~31)
		az_ret = INA_AZ_SET_DATA_LOW_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, low_io_event); //        약 Event SET 함수
		az_ret = INA_AZ_GET_DATA_LOW_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       약 Event GET 함수
		int high_io_event = 0; // - 1: 없음 ((-) , 0~31: 운전 I/O Event 번호 (0~31)
		az_ret = INA_AZ_SET_DATA_HIGH_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, high_io_event); //       강 Event SET 함수
		az_ret = INA_AZ_GET_DATA_HIGH_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       강 Event GET 함수




		//goto end
	az_end:

		Sleep(100);
	}


#endif



};



#endif














