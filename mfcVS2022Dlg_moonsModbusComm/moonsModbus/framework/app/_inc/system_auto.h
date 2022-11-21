#pragma once
/*

system_auto를 system_op 객체를 assign받아 설비 운영 auto/ manual operating 관련 함수를 제공한다. 
op 상태 정보의 핸들링은 symtem_op 객체를 사용

*/


#define PROCESS_ALARM(obj_errcode) m_pAuto->AlarmAuto(obj_errcode,(__FILE__),__FUNCTION__,(__LINE__))
namespace AP_SYS
{


	class MSystemAuto :public Icommon
	{
		/****************************************************
		 *  data
		 ****************************************************/
	public:
		struct cfg_t
		{
			MSystemOP* pOp;
			std::shared_ptr<IIO> pIo{};
			inline void operator = (const cfg_t* p_cfg) {
				this->pIo = p_cfg->pIo;
			}

		};

	private:
		cfg_t m_cfg;
		util::_que<uint8_t> m_eventQue{ 4096 }; // 이벤트 자료 구조 후 적용
		bool m_TrdLife;
		HANDLE m_hThread;
		DWORD m_TrdId;
		std::mutex m_mutex;

		//MSystemOP::mode_e m_opMode;
		//MSystemOP::status_e m_opStatus;


	 /****************************************************
	  *  Constructor
	  ****************************************************/
	public:
		MSystemAuto(common::Attribute_t& common_data, cfg_t& cfg);
		virtual ~MSystemAuto();

		/****************************************************
		 *	func
		 ****************************************************/
	private:

		void threadStop(void);
		void threadRun(void);
		void threadJob(void);
		static UINT threadFunc(LPVOID pParam);
		static void functionCB(void* obj, void* w_parm, void* l_parm);

		void doRunStep();
		errno_t checkStartRunCondition();
		errno_t checkApplicationSystem();

		bool isEStopSWPressed();
		bool isDoorOpened();
		bool isMotorsReady();
		bool isRobotReady();
	public:
		errno_t StartThread();
		errno_t StopThread();
		errno_t AutoReady();
		errno_t EStopAxis(AXIS_OBJ axis_is = AXIS_OBJ::_max);
		MSystemOP::mode_e GetOPMode() const;
		MSystemOP::status_e GetOPStatus() const;
		void SetOPMode(MSystemOP::mode_e mode);
		void SetOPStatus(MSystemOP::status_e status);

		void AlarmAuto(int obj_id, errcode err_code,
			const char* file,
			const char* func,
			const int line,
			const char* msg);

		void AlarmAuto(obj_errno obj_errcode,
			const char* file,
			const char* func,
			const int line);



	};

}