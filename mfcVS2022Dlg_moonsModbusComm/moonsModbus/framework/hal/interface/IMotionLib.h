
#ifndef IMOTIONLIB_H
#define IMOTIONLIB_H

#include "Icommon.h"

//#include "DefMotionStruct.h"

  /** This class is defined Motion component interface class of Motion libraries as a abstract class.
   * 이 Class는 Motion Library를 위한 Class에 대한 Interface Class로 정의한다.
   * 
   * @interface
   */
class IMotionLib : public virtual Icommon
{

public:
	virtual ~IMotionLib() {};

	virtual  errno_t Initialize() = 0;


	/// <summary>
	/// 축의 동작 완료될 때까지 대기한다.
	/// 이동 및 속도 이동 완료 확인 또는 더불어 In - Position 여부까지 확인한다.
	/// </summary>
	/// <param name="axis_id">축 ID </param>
	/// <returns>0=SUCCESS , 그외 에러</returns>
	virtual errno_t WaitDone(short axis_id, unsigned short timeout) = 0;

	virtual errno_t MotorOnOff(short axis_id, bool on_off) = 0;

	virtual bool IsMotorOn(short axis_id) = 0;

	/// <summary>
	/// 축의 동작 완료 여부를 Return
	/// </summary>
	/// <param name="axis_id">축 ID </param>
	/// <returns>0=SUCCESS , 그외 에러</returns>
	virtual bool IsAxisDone(short axis_id) = 0;

	/// <summary>
	/// 축 현재 상태를 읽는다.
	/// 상태는 bit 조합으로 구성되어 있다.
	/// </summary>
	/// <param name="axis_id">축 ID</param>
	/// <param name="p_state">상태 반환 변수</param>
	/// <returns>0=SUCCESS , 그외 에러</returns>
	virtual errno_t GetAxisStatus(short axis_id, DWORD& p_state) = 0;


	/// <summary>
	/// 목표	(Command)위치값과 실제(Actual) 위치값을 0 으로
	/// </summary>
	/// <param name="axis_id">축 ID</param>
	/// <returns>0=SUCCESS , 그외 에러</returns>
	virtual errno_t ClearPosition(short axis_id) = 0;

	virtual errno_t AlarmReset(short axis_id) = 0;

	virtual errno_t GetAlarmType(short axis_id, DWORD& p_state) = 0;

	virtual errno_t GetCommandPosition(short axis_id, long& p_position) = 0;

	virtual errno_t GetActualPosition(short axis_id, long& p_position) = 0;
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="axis_id"></param>
	/// <param name="velocity"></param>
	/// <param name="is_cw"></param>
	/// <returns></returns>
	virtual errno_t JogMove(short axis_id, DWORD velocity, bool is_cw) = 0;

	virtual errno_t JogStop(short axis_id) = 0;

	virtual errno_t MoveStop(short axis_id) = 0;

	virtual errno_t Move(short axis_id, long position, DWORD velocity, bool wait_done = false) = 0;

	virtual errno_t OriginAxis(short axis_id) = 0;
	

};

#endif //IMOTIONLIB_H

// End of File IMotionLib.h