#pragma once

#ifdef USE_HW_MOTOR_ORIENTAL




// ## Library �߰��ϱ�
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
		int			port_no{};								// �ø�����Ʈ
		int			baudrate{};					// Baudrate
		int			comm_delay_ms{};					// Delay Time �ʱⰪ - 35ms
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
	INA_Serial	m_Serial;					    // -- �ø��� ��� ����ü ����
	int					m_SerialIndex;				// -- �ø��� ��� INDEX
	bool				m_IsConnect;					// -- �ø��� ��� ���� ����

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
			// �˶�		
			AZ_ERR_WRITE("INA_AZ_INITIALIZE err_ret : %d", az_ret);
			
			AZ_Disconnect();
			return DEF_AZ_ERR_RET_FAIL;
		}
		// �ø��� ��ü ����
		m_Serial = INA_AZ_GET_SERIAL(m_SerialIndex);
		Sleep(100);

		az_ret = INA_AZ_SET_DELAY(m_SerialIndex, m_Cfg.comm_delay_ms);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�		
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
		// �����ִ� �� ��� �ݱ�. ��� �ȴ����� �翬�� ����.
		for (int i = 0; i < m_SerialIndex; i++)
			az_ret = INA_AZ_UNINITIALIZE(i);  // retrun 1 : Success

		az_ret = INA_AZ_UNINITIALIZE(m_SerialIndex);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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
			// �˶�
			AZ_ERR_WRITE("INA_AZ_SET_SERIAL err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}


	/**
	*	Oriental Moter - ����
	*/
	inline errno_t AZ_Move() {
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		az_ret = INA_AZ_SET_FWD(m_SerialIndex, m_Cfg.slave_start_id, 0, TRUE);  // retrun 1 : Success
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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
			// �˶�
			AZ_ERR_WRITE("AZ_Move_For err_ret : %d", az_ret);
			ret = DEF_AZ_ERR_RET_FAIL;
		}

		return ret;
	}

	inline errno_t  AZ_Start(int data_no){	// Data No ���������� ����
		errno_t ret = DEF_AZ_ERR_SUCCESS;
		int az_ret = DEF_AZ_RET_SUCCESS;
		m_DataNo = data_no;

		// Ready Status Ȯ���ϱ�
		// ORIENRAL_AZ_STATUS stAxis;
		m_regAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_SerialIndex, m_Cfg.slave_start_id);

		if (!m_regAxis.AZ_FLAG_READY)
		{
			// �˶� �߰��ϱ�.
			AZ_ERR_WRITE("error motor not ready !");
			return DEF_AZ_ERR_RET_FAIL;
		}

		// Move
		az_ret = INA_AZ_SET_START(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
			// �˶�
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
		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo, get_value);  // ���� ���� Set (0~1000, 1 = 0.1%)

		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, m_DataNo, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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


		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_for, get_value);  // ���� ���� Set (0~1000, 1 = 0.1%)
		
		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_for, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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

		m_regAxis.iStatus = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_back, get_value);  // ���� ���� Set (0~1000, 1 = 0.1%)

		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_SerialIndex, m_Cfg.slave_start_id, data_back, get_value);
		if (az_ret != DEF_AZ_RET_SUCCESS)
		{
			// �˶�
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

		//// **1202 -> msec ������ ����
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
		 // �˶�
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
























	// �߰��ϱ�
	// INA_AZ_SET_DELAY(m_nSerialIndex, nDelay); 


#if 0 // test oriental motor lib 
	//  packet frame 
	//  slave_addr   func code    data     crc-16
	//    8 bit        8 bit    N��8 bit    16 bit
	{
		uint8_t crc_data[6] = { 0x01,0x03,0x18,0x40,0x00,0x06 };
		uint16_t ret_crc = 0;
		ret_crc = util::crc16_modbus_cal(&crc_data[0], 6);


		int az_ret = 0; // retrun 1 : Success , 0 : fail
		constexpr int AZ_RET_SUCCESS = 1;
		constexpr int AZ_RET_FAIL = 0;
		// �ʱ�ȭ
		az_ret = INA_AZ_INITIALIZE(m_nSerialIndex, m_nPort, m_dwBaud);

		// �ø��� ��ü ����
		m_Serial = INA_AZ_GET_SERIAL(m_nSerialIndex);
		Sleep(100);

		az_ret = INA_AZ_SET_DELAY(m_nSerialIndex, nDelay);


		Sleep(100);
		//az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);
		stAxis.iStatus = INA_AZ_GET_CURRENT_DRIVER_OUTPUT_LOW(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);

		//ALARM �ʱ�ȭ �Լ�
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

		// POSITION �ʱ�ȭ �Լ�
		az_ret = INA_AZ_SET_P_PRESET(m_nSerialIndex, m_iSlaveStartID);
		Sleep(100);

		// �̵�
	// AZ_CW_T20 0 - �ӵ�  2000 ��ũ 20% (�� ���� ����)
	// AZ_CCW_T20 1 - �ӵ� -2000 ��ũ 20% 
	// AZ_CW_T50 2 - �ӵ�  2000 ��ũ 50% (�� ���� ����)

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

		//DRIVER INPUT COMMAND �Լ�
		int memory_addr = 1; //Memory Address ��ȣ ( 0 ~ 255 )
		int bit_no = 0;
		az_ret = INA_AZ_SET_START(m_nSerialIndex, m_iSlaveStartID, memory_addr); // START �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_HOME(m_nSerialIndex, m_iSlaveStartID); // HOME �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_STOP(m_nSerialIndex, m_iSlaveStartID); // STOP �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_MS0(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS0 �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_MS1(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS1 �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_MS2(m_nSerialIndex, m_iSlaveStartID, AZ_ON); // MS2 �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_CLEAR(m_nSerialIndex, m_iSlaveStartID); // BIT �Է� ��ȣ �ʱ�ȭ �Լ�

		// servo off
		az_ret = INA_AZ_SET_FREE(m_nSerialIndex, m_iSlaveStartID, AZ_ON);

		az_ret = INA_AZ_SET_FWD(m_nSerialIndex, m_iSlaveStartID, memory_addr, AZ_ON); // FWD �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_RVS(m_nSerialIndex, m_iSlaveStartID, memory_addr, AZ_ON);//	RVS �Է� ��ȣ SET �Լ�



		az_ret = INA_AZ_SET_JOG_P(m_nSerialIndex, m_iSlaveStartID, AZ_ON); //JOG_P �Է� ��ȣ SET �Լ�
		az_ret = INA_AZ_SET_JOG_P(m_nSerialIndex, m_iSlaveStartID, AZ_OFF); // JOG_N �Է� ��ȣ SET �Լ�
		//int  az_state = stAxis.AZ_FLAG_INFO;


		//Return Value -  �Լ� �� MONITOR ��� ������ ��
		az_ret = INA_AZ_GET_CURRENT_ALARM(m_nSerialIndex, m_iSlaveStartID); //���� �˶� ���� GET �Լ� 
		az_ret = INA_AZ_GET_CURRENT_POSITION(m_nSerialIndex, m_iSlaveStartID); //���� ��ġ ���� GET �Լ� 
		az_ret = INA_AZ_GET_CURRENT_VELOCITY(m_nSerialIndex, m_iSlaveStartID); //���� ���� �ӵ� ���� GET�Լ� 
		az_ret = INA_AZ_GET_CURRENT_FEEDBACK_POSITION(m_nSerialIndex, m_iSlaveStartID); //���� FEEDBACK ��ġ GET �Լ� 
		az_ret = INA_AZ_GET_CURRENT_FEEDBACK_VELOCITY(m_nSerialIndex, m_iSlaveStartID); //���� FEEDBACK �ӵ� GET �Լ�



		// DRIVER �Է� ���� �Լ�
		az_ret = INA_AZ_SET_CURRENT_DRIVER_INPUT_HIGH(m_nSerialIndex, m_iSlaveStartID, bit_no, AZ_ON); // Driver �Է� ���� HIGH SET�Լ� 
		az_ret = INA_AZ_GET_CURRENT_DRIVER_INPUT_HIGH(m_nSerialIndex, m_iSlaveStartID);// Driver �Է� ���� HIGH GET�Լ�
		az_ret = INA_AZ_SET_CURRENT_DRIVER_INPUT_LOW(m_nSerialIndex, m_iSlaveStartID, bit_no, AZ_ON); //  Driver �Է� ���� LOW SET�Լ� 
		az_ret = INA_AZ_GET_CURRENT_DRIVER_INPUT_LOW(m_nSerialIndex, m_iSlaveStartID); //  Driver �Է� ���� LOW GET�Լ�


		//OPERATING DATA R/W COMMAND �Լ�
		int set_value = 1;// �Լ� ���� ����
		/*
			1:���� ��ġ ����											2: ��� ��ġ ���� ���� ��ġ ����
			3: ��� ��ġ ���� ���� ��ġ ���� )		7: ���� ���� ��ġ����
			8:Round ���� ��ġ ����								9: Round ��ó ��ġ ����
			10: Round FWD ���� ���� ��ġ ����			11: Round RVS ���� ���� ��ġ ����
			12: Round ���� ����									13: Round ��ó ����
			14: Round FWD ���� ����							15: Round RVS ���� ����
			16: ���� ���� �ӵ� ����								17: ���� ���� ����
			18: ���� ���� (Torque)								20: ���� ��ġ ���� ����
			21: ��� ��ġ ���� ���� ���� ��ġ ���� )
			22: ��� ��ġ ���� ���� ���� ��ġ ����

			*/
		az_ret = INA_AZ_SET_DATA_POSITION_MODE(m_nSerialIndex, m_iSlaveStartID, memory_addr, set_value); //		���� ��� SET �Լ�
		az_ret = INA_AZ_GET_DATA_POSITION_MODE(m_nSerialIndex, m_iSlaveStartID, memory_addr);            // ���� ��� GET �Լ�


		int pos_val = 0;//-2,147,483,648~2,147,483,647step
		az_ret = INA_AZ_SET_DATA_POSITION(m_nSerialIndex, m_iSlaveStartID, memory_addr, pos_val); // ��ġ SET �Լ�
		az_ret = INA_AZ_GET_DATA_POSITION(m_nSerialIndex, m_iSlaveStartID, memory_addr);          // ��ġ GET �Լ�
		int vel_val = 0;//-4,000,000~4,000,000Hz
		az_ret = INA_AZ_SET_DATA_VELOCITY(m_nSerialIndex, m_iSlaveStartID, memory_addr, vel_val); //�ӵ� SET �Լ�
		az_ret = INA_AZ_GET_DATA_VELOCITY(m_nSerialIndex, m_iSlaveStartID, memory_addr);          //�ӵ� GET �Լ�

		int acc_val = 0;//1,000,000,000(1=0.001kHz)
		az_ret = INA_AZ_SET_DATA_ACC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr, acc_val); // ����(�⵿, ����) SET �Լ�
		az_ret = INA_AZ_GET_DATA_ACC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr); //			���� �⵿ ���� ) GET �Լ�
		int dec_val = 0;//1,000,000,000(1=0.001kHz)
		az_ret = INA_AZ_SET_DATA_DEC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr, dec_val); //			���� ���� SET �Լ�
		az_ret = INA_AZ_GET_DATA_DEC_RATE(m_nSerialIndex, m_iSlaveStartID, memory_addr); //			���� ���� ) GET �Լ�


		int current_val = 0;//: 0~1000(1= 0.1%)
		az_ret = INA_AZ_SET_DATA_OPERATING_CURRENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, current_val); //���� ���� SET �Լ�
		az_ret = INA_AZ_GET_DATA_OPERATING_CURRENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); // ���� ���� GET �Լ�
		int delaytime = 0;// 0~65535(1 = 0.001s)
		az_ret = INA_AZ_SET_DATA_DELAY_TIME(m_nSerialIndex, m_iSlaveStartID, memory_addr, delaytime); // ���� ���� ���� SET �Լ�
		az_ret = INA_AZ_GET_DATA_DELAY_TIME(m_nSerialIndex, m_iSlaveStartID, memory_addr); // ���� ���� ���� GET �Լ�

		int link_val = 0; //0: �������� ����, 1 : ���� ����, 2 : �ڵ� ����, 3 : ���� ����
		az_ret = INA_AZ_SET_DATA_LINK(m_nSerialIndex, m_iSlaveStartID, memory_addr, link_val); //	���� SET �Լ�
		az_ret = INA_AZ_GET_DATA_LINK(m_nSerialIndex, m_iSlaveStartID, memory_addr);           //	���� GET �Լ�
		int next_data_no = 0; // -256: STOP, 2 : +2, 1 : (+1), 0~255 : ���� No.
		az_ret = INA_AZ_SET_DATA_NEXT_DATA_NO(m_nSerialIndex, m_iSlaveStartID, memory_addr, next_data_no); //	���� ��� SET �Լ�
		az_ret = INA_AZ_GET_DATA_NEXT_DATA_NO(m_nSerialIndex, m_iSlaveStartID, memory_addr);               //	���� ��� GET �Լ�


		int area_offset = 0; //-2,147,483,648~2,147,483,647step
		az_ret = INA_AZ_SET_DATA_AREA_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr, area_offset); //      OFFSET(AREA) SET �Լ�
		az_ret = INA_AZ_GET_DATA_AREA_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr); // OFFSET(AREA) GET �Լ�
		int area_width = 0; // -1 : ��ȿ , 0~4,194,303 : 1step ������ ����
		az_ret = INA_AZ_SET_DATA_AREA_WIDTH(m_nSerialIndex, m_iSlaveStartID, memory_addr, area_width); //    ��(AREA) SET �Լ�
		az_ret = INA_AZ_GET_DATA_AREA_WIDTH(m_nSerialIndex, m_iSlaveStartID, memory_addr); //    ��(AREA) GET �Լ�



		int loop_cnt = 0; //0: ���� ((-), 2~255: Loop Ƚ�� (loop2{~loop256{)
		az_ret = INA_AZ_SET_DATA_LOOP_COUNT(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_cnt); //  Count(Loop) SET �Լ�
		az_ret = INA_AZ_GET_DATA_LOOP_COUNT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       Count(Loop) GET �Լ�
		int loop_offset = 0;// -4,194,304 ~ 4,194,303step
		az_ret = INA_AZ_SET_DATA_LOOP_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_offset); //       ��ġ Offset(Loop) SET �Լ�
		az_ret = INA_AZ_GET_DATA_LOOP_OFFSET(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       ��ġ Offset(Loop) GET �Լ�
		int  loop_end = 0; //0: ���� ((-) , 1: }L End
		az_ret = INA_AZ_SET_DATA_LOOP_END(m_nSerialIndex, m_iSlaveStartID, memory_addr, loop_end); //       ����(Loop) S ET �Լ�
		az_ret = INA_AZ_GET_DATA_LOOP_END(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       ����(L oop) GET �Լ�


		int low_io_event = 0; // - 1: ���� ((-) , 0~31: ���� I/O Event ��ȣ (0~31)
		az_ret = INA_AZ_SET_DATA_LOW_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, low_io_event); //        �� Event SET �Լ�
		az_ret = INA_AZ_GET_DATA_LOW_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       �� Event GET �Լ�
		int high_io_event = 0; // - 1: ���� ((-) , 0~31: ���� I/O Event ��ȣ (0~31)
		az_ret = INA_AZ_SET_DATA_HIGH_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr, high_io_event); //       �� Event SET �Լ�
		az_ret = INA_AZ_GET_DATA_HIGH_IO_EVENT(m_nSerialIndex, m_iSlaveStartID, memory_addr); //       �� Event GET �Լ�




		//goto end
	az_end:

		Sleep(100);
	}


#endif



};



#endif














