#pragma once

#define IAXIS_DIR_CW				(int)1
#define IAXIS_DIR_CCW				(int)0

//motion을 위한 기본 parameter





class IAxis :public virtual Icommon
{

public:

	struct motion_param_t {
		uint32_t jog_speed{}; //단위 mmsec
		uint32_t jog_accel{}; //단위 mmsec/ms 목표 속도에 도달할 시간ms
		uint32_t jog_decel{}; //단위  mmsec/ms 

		uint32_t move_velocity{}; //단위 mmsec  
		uint32_t move_accel{}; //단위 mmsec/ms  목표 속도에 도달할 시간ms 
		uint32_t move_decel{}; //단위 mmsec/ms

		uint32_t limit_velocity{}; //한계 속도 mmsec [max_rps * one_turn_move_mm]

		double max_rps{}; // 초당 최대 회전수 (3'000 RPM ==  50rps)
		uint32_t turn_pulse_count{}; // 한 바퀴 발생 엔코더 카운터
		double one_turn_move_mm{}; // 한 바퀴 구동 시 이동되는 실제 거리 mm  (cmd_pos/turn_per_move)*turn_per_pulse

		//double motor_scale_mm_pulse_cnt{}; // 1mm 이동에 필요한 pulse count
		//double mm_per_sec{}; //초당 이동 mm 거리

		/*
		(pulse / mm)(cmd_vel * (1/60))*turn_per_pulse
		
		move(vel = 1000, acc = 100, dec = 100) 가감속 100ms에 목표 속도 도달, 
		                                       목표 속도는 초당 1000mm 가능한 rps 요구
											   a [목표거리mm/one_turn_move_mm]
											   한계 500mmsec 제한

											   목표 펄스cnt b 산출은 [a * turn_pulse_count] 로 산출 한다  


		모터만 보면
		속도 rpm으로 통일 (분당 회전 수)
		가감속 rpm/s 초당

		 1회전 카운트 수이고 초당
		 EX) 엔코더 or 홀센서 : 1회전 펄스수가 300개이고 1초당 회전시 입력 펄스수가 200개 일때 RPM 계산 : 200 / 300 = 0.66 (초당) RPM = 0.66 X 60 (분) = 39.6 RPM (분당) 거리 계산 : 원형 바

		 기본속도 10 rps 

		*/

		void Init() {
			constexpr double default_rps = 10.0;
			jog_speed = (uint32_t)(one_turn_move_mm * default_rps);
			jog_accel = 100;
			jog_decel = 100;

			move_velocity = (uint32_t)(one_turn_move_mm * default_rps * 3.0);
			move_accel = 100;
			move_decel = 100;

			limit_velocity = (uint32_t)(one_turn_move_mm * max_rps);
		}


		inline void operator = (const motion_param_t* p_cfg) {
			this->jog_speed = p_cfg->jog_speed;
			this->jog_accel = p_cfg->jog_accel;
			this->jog_decel = p_cfg->jog_decel;
			this->move_velocity = p_cfg->move_velocity;
			this->move_accel = p_cfg->move_accel;
			this->move_decel = p_cfg->move_decel;
			this->limit_velocity = p_cfg->limit_velocity;

			this->turn_pulse_count = p_cfg->turn_pulse_count;
			this->max_rps = p_cfg->max_rps;
			this->one_turn_move_mm = p_cfg->one_turn_move_mm;
		}


	};


#if 0
	struct origin_param_t {
		int orgSpeed{};
		int orgSearchSpeed{};
		int orgAccDecTime{};
		int orgMethod{};	// Origin Method = 2 is 'Limit Origin' in the Ezi-SERVOII Plus-E model.
		int orgDir{};
		int orgOffset{};
		int orgPositionSet{};
		int orgTorqueRatio{};

		origin_param_t() {
			orgSpeed = 50000;
			orgSearchSpeed = 1000;
			orgAccDecTime = 50;
			orgMethod = 2;	// Origin Method =
			orgDir = IAXIS_DIR_CW;
			orgOffset = 0;
			orgPositionSet = 0;
			orgTorqueRatio = 50;
		}

		inline void operator = (const origin_param_t* p_cfg) {
			this->orgSpeed = p_cfg->orgSpeed;
			this->orgSearchSpeed = p_cfg->orgSearchSpeed;
			this->orgAccDecTime = p_cfg->orgAccDecTime;
			this->orgMethod = p_cfg->orgMethod;
			this->orgDir = p_cfg->orgDir;
			this->orgOffset = p_cfg->orgOffset;
			this->orgPositionSet = p_cfg->orgPositionSet;
			this->orgTorqueRatio = p_cfg->orgTorqueRatio;
		}

	};

#endif


	virtual errno_t Initialize() = 0;
	virtual errno_t MotorOnOff(bool on_off) = 0;
	virtual bool  IsMotorOn() = 0;
	virtual errno_t WaitDone(uint32_t timeout) = 0;
	virtual bool IsAxisDone() = 0;
	virtual errno_t  ClearState() = 0;
	virtual uint32_t  GetMotorState() = 0;
	virtual errno_t SetMotionData(motion_param_t &param) = 0;

	virtual errno_t OriginAxis() = 0;
	virtual errno_t ClearPosition() = 0;

	virtual errno_t Move(int cmd_pos, uint32_t cmd_vel = 0, uint32_t acc = 100, uint32_t decel = 100) = 0;
	//축을 정지한다. (한개의 축에 대한 정지)
	virtual errno_t Stop() = 0;
	//축을 등속이동에 대해 정지한다.
	virtual errno_t VStop() = 0;
	// 축을 비상정지한다.
	virtual errno_t EStop() = 0;

	virtual errno_t JogStop() = 0;

	virtual errno_t JogMove(uint32_t cmd_vel = 0, bool is_cw = true) = 0;
	//virtual errno_t JogMove(uint32_t jog_pos, uint32_t cmd_vel = 1000, bool is_cw = true) = 0;

	virtual errno_t GetActualPosition(long& p_position) = 0;
	virtual errno_t GetCommandPosition(long& p_position) = 0;

};






/*

motor_scale : 1 mm 이동에 필요한 pulse수
( 1 turn pulse )/ (1 바퀴 이동 거리 mm)    예)  1 turn 20'000펄스  1바퀴 이동 거리 10mm  면 20'000/10mm   1mm 이동에 필요한 펄스는 2'000펄스

turn_scale : 한바퀴 구동에 이동되는 거리 mm

motor 속도 (m/sec)  이동거리/sec
case 1.  1초에 1m 이동 속도를 내려면 motor_scale(2'000) * 1'000 / 1초로  1초에 2'000'000펄스가 나오는 속도가 필요.
case 2.  turn_scale * (50 rps) 1초에 최대 속도


rpm 3'000 기준  :  한 바퀴 펄스 20'000   초당 50 바퀴
param.max_velocity = case 1. (모터 이동거리 [50바퀴 * 20'000펄스] / motor scale(펄스))
					 case 2.  최대 속도 1초에 이동하는 거리 ((50바퀴 * 20'000펄스 ) / motor_scale)/sec

param.jog_speed = 5000; //pps
param.jog_accel = 100; //msec
param.jog_decel = 100; //msec
param.move_velocity = 10000; //pulse
param.move_accel = 100;//msec
param.move_decel = 100;//msec


*/