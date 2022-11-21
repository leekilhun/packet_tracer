#pragma once


constexpr errno_t  ERR_SUCCESS = 0;
constexpr errno_t  ERR_FAIL = -1;
using namespace AP_OBJ;

/************************************************************************/
/* IO Module								                            */
/************************************************************************/
// IO Error(Component Code : 100)
constexpr errcode ERR_IO_MODULE_OPEN				= (static_cast<int>(ObjType::HL_IO) * 1000) + 1;
constexpr errcode ERR_IO_OUTPUT_OUT_OF_RANGE		= (static_cast<int>(ObjType::HL_IO) * 1000) + 2;
constexpr errcode ERR_IO_NO_MATCH					= (static_cast<int>(ObjType::HL_IO) * 1000) + 3;
constexpr errcode ERR_IO_MODULE_IRQ_SET				= (static_cast<int>(ObjType::HL_IO) * 1000) + 4;
constexpr errcode ERR_IO_MODULE_NO_RESPONSE			= (static_cast<int>(ObjType::HL_IO) * 1000) + 5;
constexpr errcode ERR_IO_INVALID_ADDRESS_NUMVER		= (static_cast<int>(ObjType::HL_IO) * 1000) + 6;

/************************************************************************/
/* Cyliner Module								                        */
/************************************************************************/
// Cylinder Error(Component Code : 101)
constexpr errcode ERR_CYLINDER_TIMEOUT_MOTION		= (static_cast<int>(ObjType::HL_CYLINDER) * 1000) + 1;	// Cylinder 동작시간을 초과하였습니다.
constexpr errcode ERR_CYLINDER_INVALID_POSITION		= (static_cast<int>(ObjType::HL_CYLINDER) * 1000) + 2;	// ERR_CYLINDER_INVALID_POS
constexpr errcode ERR_CYLINDER_FAIL_LOADING_PARAM	= (static_cast<int>(ObjType::HL_CYLINDER) * 1000) + 3;	// ERR_CYLINDER_FAIL_IN_LOADING_PARAM
constexpr errcode ERR_CYLINDER_FAIL_SAVING_PARAM	= (static_cast<int>(ObjType::HL_CYLINDER) * 1000) + 4;	// ERR_CYLINDER_FAIL_IN_SAVING_PARAM




/************************************************************************/
/* Vacuum Module								                        */
/************************************************************************/
// Vacuum Module(Component Code : 102)
constexpr errcode ERR_VACUUM_MISS_MATCH_IN_OUT_SIGNAL	= (static_cast<int>(ObjType::HL_VACUUM) * 1000) + 0;
constexpr errcode ERR_VACUUM_TIMEOUT					= (static_cast<int>(ObjType::HL_VACUUM) * 1000) + 1;
constexpr errcode ERR_VACUUM_FAIL_IN_LOADING_PARAM		= (static_cast<int>(ObjType::HL_VACUUM) * 1000) + 3;
constexpr errcode ERR_VACUUM_FAIL_IN_SAVING_PARAM		= (static_cast<int>(ObjType::HL_VACUUM) * 1000) + 4;





/************************************************************************/
/* Motion Axis Module			   		                                */
/************************************************************************/
// Motion Axis Error(Component Code : 105)
constexpr errcode ERR_MAXIS_AMP_FAULT = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 8;
constexpr errcode ERR_MAXIS_TIMEOUT_MOVE_TIME = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 21;	// Motion Time Out Error, 모터가 정해진 시간동안 동작하지 않습니다.
constexpr errcode ERR_MAXIS_NO_EXIST_MOTION_LIBRARY_OBJECT = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 24;	// ERR_MAXES_NO_EXIST_MOTION_LIBRARY_OBJECT
constexpr errcode ERR_MAXIS_INVALID_POINTER = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 28;	// ERR_MAXES_INVALID_POINTER
constexpr errcode ERR_MAXIS_INVALID_AXIS_ID = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 34;	// ERR_MAXES_INVALID_AXIS_ID
constexpr errcode ERR_MAXIS_HOME_SWITCH_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 36;	// ERR_MAXES_HOME_SWITCH_AXIS_SOURCE
constexpr errcode ERR_MAXIS_POSITIVE_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 37;	// ERR_MAXES_POSITIVE_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_NEGATIVE_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 38;	// ERR_MAXES_NEGATIVE_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_AMP_FAULT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 39;	// ERR_MAXES_AMP_FAULT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_ACCELERATE_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 40;	// ERR_MAXES_ACCELERATE_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_VELOCITY_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 41;	// ERR_MAXES_VELOCITY_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_X_NEGATIVE_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 42;	// ERR_MAXES_X_NEGATIVE_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_X_POSITIVE_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 43;	// ERR_MAXES_X_POSITIVE_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_ERROR_LIMIT_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 44;	// ERR_MAXES_ERROR_LIMIT_AXIS_SOURCE
constexpr errcode ERR_MAXIS_PC_COMMAND_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 45;	// ERR_MAXES_PC_COMMAND_AXIS_SOURCE
constexpr errcode ERR_MAXIS_OUT_OF_FRAMES_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 46;	// ERR_MAXES_OUT_OF_FRAMES_AXIS_SOURCE
constexpr errcode ERR_MAXIS_AMP_POWER_ON_OFF_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 47;	// ERR_MAXES_AMP_POWER_ON_OFF_AXIS_SOURCE
constexpr errcode ERR_MAXIS_RUN_STOP_COMMAND_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 48;	// ERR_MAXES_RUN_STOP_COMMAND_AXIS_SOURCE
constexpr errcode ERR_MAXIS_COLLISION_STATE_AXIS_SOURCE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 49;	// ERR_MAXES_COLLISION_STATE_AXIS_SOURCE
constexpr errcode ERR_MAXIS_NOT_RETURNED_ORIGIN = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 50;	// 원점 복귀가 수행되지 않았습니다.
constexpr errcode ERR_MAXIS_ORIGIN_RETURNED_FAILURE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 51;	// 원점복귀 실패.
constexpr errcode ERR_MAXIS_ORIGIN_RETURNED_TIMEOUT = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 53;	// 원점복귀 제한 시간을 초과하였습니다.
constexpr errcode ERR_MAXIS_INVALID_MOVE_TYPE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 54;	// 유효한 Move Type이 아닙니다.
constexpr errcode ERR_MAXIS_ESTOP = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 58;	// E-Stop Event가 발생하였습니다.
constexpr errcode ERR_MAXIS_OVER_SW_POSITIVE_LIMIT = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 60;	// ERR_MAXES_OVER_SW_POSITIVE_LIMIT
constexpr errcode ERR_MAXIS_OVER_SW_NAGATIVE_LIMIT = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 61;	// ERR_MAXES_OVER_SW_NAGATIVE_LIMIT
constexpr errcode ERR_MAXIS_ORIGIN_STOP_BY_USER = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 62;	// ERR_MAXES_ORIGIN_STOP_BY_USER
constexpr errcode ERR_MAXIS_NO_RESPONSE_COOLMUSCLE = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 63;	// Cool Muscle Motor로부터 응답이 없습니다.
constexpr errcode ERR_MAXIS_INTERLOCK_CHECKED = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 66;	// ERR_MAXES_INTERLOCK_CHECKED
constexpr errcode ERR_MAXIS_NOT_IN_POSITION = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 67;	// Motor가 Inposition아 아닙니다.
constexpr errcode ERR_MAXIS_FAIL_LOAD_PARAM = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 118;	// ERR_MAXES_PARAMETER_LOAD_FAIL;
constexpr errcode ERR_MAXIS_FAIL_SAVE_PARAM = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 119;	// ERR_MAXES_PARAMETER_SAVE_FAIL
constexpr errcode ERR_MAXIS_INPUT_STOP_BUTTON = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 200;	// 정지 버튼이 감지되었습니다.
constexpr errcode ERR_MAXIS_SAFETY_SENSOR_INTERLOCK = (static_cast<int>(ObjType::HL_MOTION) * 1000) + 201;	// Safety Fence Interlock Checked

/************************************************************************/
/* Motion Lib Module			   		                                */
/************************************************************************/
// Motion Lib Error(Component Code : 106)
constexpr errcode ERR_MLIB_INVALID_STOP_TYPE = (static_cast<int>(ObjType::HL_MOTION_LIB) * 1000) + 77;	// ERR_MLIB_INVALID_STOP_TYPE
constexpr errcode ERR_MLIB_INVALID_POINTER = (static_cast<int>(ObjType::HL_MOTION_LIB) * 1000) + 90;	// ERR_MLIB_INVALID_POINTER
constexpr errcode ERR_MLIB_INVALID_AXIS_ID = (static_cast<int>(ObjType::HL_MOTION_LIB) * 1000) + 109;	// ERR_MLIB_INVALID_AXIS_ID

/************************************************************************/
/* Serial Communication Module								            */
/************************************************************************/
// Serial Module(Component Code : 108)
constexpr errcode ERR_SERAIL_TIMEOUT = (static_cast<int>(ObjType::HL_SERIAL) * 1000) + 0;


/************************************************************************/
/* Net Socket Communication Module								            */
/************************************************************************/
// Net Socket Module(Component Code : 109)
constexpr errcode ERR_ETHERNET_TIMEOUT = (static_cast<int>(ObjType::HL_ETHERNET) * 1000) + 0;


/************************************************************************/
/* Distance Sensor Module								            */
/************************************************************************/
// Distance Senso Module(Component Code : 110)
constexpr errcode ERR_DIST_SENSOR_TIMEOUT = (static_cast<int>(ObjType::HL_DIST_SENSOR) * 1000) + 0;







/************************************************************************/
/* OP PANEL Module			   		                                    */
/************************************************************************/
// OP PANEL(Component Code : 200)
constexpr errcode ERR_OPPANEL_TIMEOUT = (static_cast<int>(ObjType::ML_OP_PANEL) * 1000) + 000;






/************************************************************************/
/* autoManager Module			   		                                */
/************************************************************************/
// autoManager (Component Code : 400)
constexpr errcode ERR_AUTO_MANAGER_TIMEOUT = (static_cast<int>(ObjType::PL_AUTO_MANAGER) * 1000) + 000;



#if 0



// Laser Sensor Error(Component Code : 111)
const int ERR_LASER_COMPORT_OPEN_FAIL = 111000;
const int ERR_LASER_RESPONSE_TIMEOUT = 111001;




// SAFETY PLC(Component Code : 205)
const int	ERR_SAFETY_205000 = 205000;

/************************************************************************/
/* input conveyor - 210							                        */
/************************************************************************/
const int ERR_INPUTCONVEYOR_STOPPER_UP_TIMEOVER = 210001;
const int ERR_INPUTCONVEYOR_STOPPER_DOWN_TIMEOVER = 210002;
const int ERR_INPUTCONVEYOR_SHUTTLEHEAD_UP_TIMEOVER = 210003;
const int ERR_INPUTCONVEYOR_SHUTTLEHEAD_DOWN_TIMEOVER = 210004;
const int ERR_INPUTCONVEYOR_SHUTTLE_LEFT_TIMEOVER = 210005;
const int ERR_INPUTCONVEYOR_SHUTTLE_RIGHT_TIMEOVER = 210006;

const int ERR_INPUTCONVEYOR_INTERLOCK_1 = 210071;

const int ERR_INPUTCONVEYOR_NO_DEFINE = 210991;

/************************************************************************/
/* inverse conveyor - 220						                        */
/************************************************************************/
const int ERR_INVERSECONVEYOR_SHUTTLEHEAD_UP_TIMEOVER = 220001;
const int ERR_INVERSECONVEYOR_SHUTTLEHEAD_DOWN_TIMEOVER = 220002;
const int ERR_INVERSECONVEYOR_SHUTTLE_LEFT_TIMEOVER = 220003;
const int ERR_INVERSECONVEYOR_SHUTTLE_RIGHT_TIMEOVER = 220004;

const int ERR_INVERSECONVEYOR_GRIP_UP_TIMEOVER = 220005;
const int ERR_INVERSECONVEYOR_GRIP_DOWN_TIMEOVER = 220006;
const int ERR_INVERSECONVEYOR_GRIP_TURN_TIMEOVER = 220007;
const int ERR_INVERSECONVEYOR_GRIP_RETURN_TIMEOVER = 220008;
const int ERR_INVERSECONVEYOR_LEFT_TIMEOVER = 220009;
const int ERR_INVERSECONVEYOR_RIGHT_TIMEOVER = 220010;


const int ERR_INVERSECONVEYOR_INTERLOCK_1 = 220071;

const int ERR_INVERSECONVEYOR_NO_DEFINE = 220991;
/************************************************************************/
/* fail conveyor - 230							                        */
/************************************************************************/
const int ERR_FAILCONVEYOR_CYL_DOWN_TIMEOVER = 230001;
const int ERR_FAILCONVEYOR_CYL1_UP_TIMEOVER = 230002;
const int ERR_FAILCONVEYOR_CYL2_UP_TIMEOVER = 230003;
const int ERR_FAILCONVEYOR_CYL3_FORWARD_TIMEOVER = 230004;
const int ERR_FAILCONVEYOR_CYL3_BACKWARD_TIMEOVER = 230005;

const int ERR_FAILCONVEYOR_INTERLOCK_1 = 230071;
const int ERR_FAILCONVEYOR_NO_DEFINE = 230991;


/************************************************************************/
/* robot unit - 240									                    */
/************************************************************************/

/************************************************************************/
/* robot interface - 245							                    */
/************************************************************************/

/************************************************************************/
/* testloader  - 250								                    */
/************************************************************************/

/************************************************************************/
/* tester interface  - 260							                    */
/************************************************************************/

/************************************************************************/
/* vision interface  - 270							                    */
/************************************************************************/
const int ERR_VISIONINTERFACE_NOT_CONNECTED = 270001;
const int ERR_VISIONINTERFACE_SENDING_FAIL = 270002;
/************************************************************************/
/* AGV interface  - 280								                    */
/************************************************************************/

/************************************************************************/
/* Label Printer - 290						                            */
/************************************************************************/

/************************************************************************/
/* MES interface  - 294								                    */
/************************************************************************/
const int	ERR_MES_INTERFACE_294000 = 294000;

/************************************************************************/
/* BCR Printer - 298						                            */
/************************************************************************/
const int ERR_BCR_COMPORT_OPEN_FAIL = 298000;





/************************************************************************/
/* OBJ_CL_MANAGE_OP_PANEL - 380							                */
/************************************************************************/
// OBJ_CL_MANAGE_OP_PANEL - 380
const int ERR_MNGOPPANEL_INVALID_SET_OPPANEL_STATE = 380001;	// Unit Setting이 잘못되었습니다.
const int ERR_MNGOPPANEL_DOOR_OPEN = 380002;	// 문이 열린 상태 입니다.
const int ERR_MNGOPPANEL_EMERGENCY = 380003;	// 비상정지 스위치가 눌러졌습니다.
const int ERR_MNGOPPANEL_AIR_ERROR = 380005;	// Air 에러 입니다.



/************************************************************************/
/* OBJ_PL_AUTO_MANAGER - 400				                            */
/************************************************************************/
// OBJ_PL_AUTO_MANAGER - 400
const int ERR_TRS_AUTO_MANAGER_ESTOP_CHECK = 400001;	// 비상정지 스위치가 눌러져 있어 자동운전을 시작할 수 없습니다.
const int ERR_TRS_AUTO_MANAGER_SYSTEM_DOOROPEN = 400002;	// Door가 열려있습니다.
const int ERR_TRS_AUTO_MANAGER_SYSTEM_MCOPEN = 400005; // MC 가 연결되지 않았습니다.
const int ERR_TRS_AUTO_MANAGER_SYSTEM_FRONT_LIGHTCURTAIN = 400006; // Front Light Curtain 이 감지되었습니다.
const int ERR_TRS_AUTO_MANAGER_SYSTEM_REAR_LIGHTCURTAIN = 400007; // Rear Light Curtain 이 감지되었습니다.
const int ERR_TRS_AUTO_MANAGER_SYSTEM_TRAYLOADER_NOT_INPOS = 400008; // Rear Light Curtain 이 감지되었습니다.

const int ERR_TRS_AUTO_MANAGER_NOT_INITIALIZED_TRANSFER = 400010;	// TrsTransfer이 초기회 되지 않았습니다. 자동운전 할 수 없습니다.
const int ERR_TRS_AUTO_MANAGER_NOT_INITIALIZED_Tester = 400011;
const int ERR_TRS_AUTO_MANAGER_NOT_INITIALIZED_ORIGIN = 400012;
const int ERR_TRS_AUTO_MANAGER_NOT_INITIALIZED_SERVOON = 400013;

const int ERR_TRS_AUTO_MANAGER_SYSTEM_MANUAL_BY_ESTOP = 400030;
const int ERR_TRS_AUTO_MANAGE_AIR_ERROR = 400033;

const int ERR_TRS_AUTO_MANAGER_STATUS_RUN = 400050;
const int ERR_TRS_AUTO_MANAGER_STATUS_STOP = 400051;
const int ERR_TRS_AUTO_MANAGER_STATUS_ERROR = 400052;


/************************************************************************/
/* OBJ_PL_Loader : 405						                        */
/************************************************************************/
// OBJ_PL_TOWERLAMP : 405
const int	ERR_TRS_TOWERLAMP_405000 = 405000;


/************************************************************************/
/* OBJ_PL_INPUTCONVEYOR : 410						                        */
/************************************************************************/
// OBJ_PL_INPUTCONVEYOR : 410
const int ERR_TRS_INPUTCONVEYOR_UNKNOWN_SET_DETECTED = 410001;

const int ERR_TRS_INPUTCONVEYOR_STOPPER_UP_TIMEOVER = 410002;
const int ERR_TRS_INPUTCONVEYOR_STOPPER_DOWN_TIMEOVER = 410003;
const int ERR_TRS_INPUTCONVEYOR_SHUTTLEHEAD_UP_TIMEOVER = 410004;
const int ERR_TRS_INPUTCONVEYOR_SHUTTLEHEAD_DOWN_TIMEOVER = 410005;
const int ERR_TRS_INPUTCONVEYOR_SHUTTLE_LEFT_TIMEOVER = 410006;
const int ERR_TRS_INPUTCONVEYOR_SHUTTLE_RIGHT_TIMEOVER = 410007;

const int ERR_TRS_INPUTCONVEYOR_MOTOR_ON_TIMEOVER = 410008;
const int ERR_TRS_INPUTCONVEYOR_MOTOR_OFF_TIMEOVER = 410009;

const int ERR_TRS_INPUTCONVEYOR_VAC_ON_TIMEOVER = 410010;
const int ERR_TRS_INPUTCONVEYOR_VAC_OFF_TIMEOVER = 410011;

const int ERR_TRS_INPUTCONVEYOR_BUFF_1_UP_TIMEOVER = 410012;
const int ERR_TRS_INPUTCONVEYOR_BUFF_2_UP_TIMEOVER = 410013;
const int ERR_TRS_INPUTCONVEYOR_BUFF_1_DOWN_TIMEOVER = 410014;
const int ERR_TRS_INPUTCONVEYOR_BUFF_2_DOWN_TIMEOVER = 410015;

const int ERR_TRS_INPUTCONVEYOR_CYL_INITIALIZE_TIMEOVER = 410030;
const int ERR_TRS_INPUTCONVEYOR_CYLINDER_ERROR = 410031;
const int ERR_TRS_INPUTCONVEYOR_VACUUM_ERROR = 410032;
const int ERR_TRS_INPUTCONVEYOR_MOTOR_ERROR = 410033;
const int ERR_TRS_INPUTCONVEYOR_INTERLOCK_1 = 410071;
const int ERR_TRS_INPUTCONVEYOR_NO_DEFINE = 410991;


/************************************************************************/
/* OBJ_PL_INVERSECONVEYOR : 415						                    */
/************************************************************************/
// OBJ_PL_INVERSECONVEYOR : 415
const int ERR_TRS_INVERSECONVEYOR_UNKNOWN_SET_DETECTED = 415001;

const int ERR_TRS_INVERSECONVEYOR_SHUTTLEHEAD_UP_TIMEOVER = 415002;
const int ERR_TRS_INVERSECONVEYOR_SHUTTLEHEAD_DOWN_TIMEOVER = 415003;
const int ERR_TRS_INVERSECONVEYOR_SHUTTLE_LEFT_TIMEOVER = 415004;
const int ERR_TRS_INVERSECONVEYOR_SHUTTLE_RIGHT_TIMEOVER = 415005;
const int ERR_TRS_INVERSECONVEYOR_GRIP_UP_TIMEOVER = 415006;
const int ERR_TRS_INVERSECONVEYOR_GRIP_DOWN_TIMEOVER = 415007;
const int ERR_TRS_INVERSECONVEYOR_GRIP_TURN_TIMEOVER = 415008;
const int ERR_TRS_INVERSECONVEYOR_GRIP_RETURN_TIMEOVER = 415009;
const int ERR_TRS_INVERSECONVEYOR_GRIP_CLOSE_TIMEOUT = 415010;
const int ERR_TRS_INVERSECONVEYOR_GRIP_OPEN_TIMEOUT = 415011;

const int ERR_TRS_INVERSECONVEYOR_GRIP_LEFT_TIMEOVER = 415012;
const int ERR_TRS_INVERSECONVEYOR_GRP_RIGHT_TIMEOVER = 415013;

const int ERR_TRS_INVERSECONVEYOR_MOTOR_RUN_TIMEOVER = 415014;
const int ERR_TRS_INVERSECONVEYOR_MOTOR_STOP_TIMEOVER = 415015;

const int ERR_TRS_INVERSECONVEYOR_VACUUM_ON_TIMEOVER = 415016;
const int ERR_TRS_INVERSECONVEYOR_VACUUM_OFF_TIMEOVER = 415017;

const int ERR_TRS_INVERSECONVEYOR_CYL_INITIALIZE_TIMEOVER = 415030;
const int ERR_TRS_INVERSECONVEYOR_CYLINDER_TIMEOVER = 415031;
const int ERR_TRS_INVERSECONVEYOR_CYLINDER_ERROR = 415032;
const int ERR_TRS_INVERSECONVEYOR_MOTOR_ERROR = 415033;
const int ERR_TRS_INVERSECONVEYOR_VACUUM_ERROR = 415034;
const int ERR_TRS_INVERSECONVEYOR_INTERLOCK_1 = 415071;

const int ERR_TRS_INVERSECONVEYOR_NO_DEFINE = 415991;

/************************************************************************/
/* OBJ_PL_FAILCONVEYOR : 420						                    */
/************************************************************************/
// OBJ_PL_FAILCONVEYOR : 420
const int ERR_TRS_FAILCONVEYOR_UNKNOWN_SET_DETECTED = 420001;

const int ERR_TRS_FAILCONVEYOR_CYL_DOWN_TIMEOVER = 420002;
const int ERR_TRS_FAILCONVEYOR_CYL1_UP_TIMEOVER = 420003;
const int ERR_TRS_FAILCONVEYOR_CYL2_UP_TIMEOVER = 420004;
const int ERR_TRS_FAILCONVEYOR_CYL3_FORWARD_TIMEOVER = 420005;
const int ERR_TRS_FAILCONVEYOR_CYL3_BACKWARD_TIMEOVER = 420006;

const int ERR_TRS_FAILCONVEYOR_INTERLOCK_1 = 420071;

const int ERR_TRS_FAILCONVEYOR_NO_DEFINE = 420991;

/************************************************************************/
/* OBJ_PL_TESTLOADER : 425						                    */
/************************************************************************/
// OBJ_PL_TESTLOADER : 425
const int ERR_TRS_TESTLOADER_UNKNOWN_SET_DETECTED = 425001;

const int ERR_TRS_TESTLOADER_HEAD_CYL_DOWN_TIMEOUT = 425002;
const int ERR_TRS_TESTLOADER_HEAD_CYL_UP_TIMEOUT = 425003;
const int ERR_TRS_TESTLOADER_ROLLER_CYL_CLOSE_TIMEOUT = 425004;
const int ERR_TRS_TESTLOADER_ROLLER_CYL_OPEN_TIMEOUT = 425005;
const int ERR_TRS_TESTLOADER_EDGE_CYL_CLOSE_TIMEOUT = 425006;
const int ERR_TRS_TESTLOADER_EDGE_CYL_OPEN_TIMEOUT = 425007;
const int ERR_TRS_TESTLOADER_GUIDE_CYL_UP_TIMEOUT = 425008;
const int ERR_TRS_TESTLOADER_GUIDE_CYL_DOWN_TIMEOUT = 425009;
const int ERR_TRS_TESTLOADER_WAIT4DONE_TEST_TIMEOUT = 425010;

const int ERR_TRS_TESTLOADER_VINYL_MIS_DETECT_ERROR = 425011;
const int ERR_TRS_TESTLOADER_PHONE_LOADER_CYL_TIMEOUT = 425012;
const int ERR_TRS_TESTLOADER_PHONE_CLAMP_CYL_TIMEOUT = 425013;
const int ERR_TRS_TESTLOADER_PHONE_CLAMP_CYL_CLOSE_TIMEOUT = 425014;
const int ERR_TRS_TESTLOADER_PHONE_CLAMP_CYL_OPNE_TIMEOUT = 425015;
const int ERR_TRS_TESTLOADER_WAIT4BUSY_TIMEOUT = 425016;
const int ERR_TRS_TESTLOADER_WAIT4_INPACK_OK_TIMEOUT = 425017;
const int ERR_TRS_TESTLOADER_PHONE_JIG_FORWARD_CYL_TIMEOUT = 425018;
const int ERR_TRS_TESTLOADER_PHONE_JIG_BACKWARD_CYL_TIMEOUT = 425019;

const int ERR_TRS_TESTLOADER_CYLINDER__ERROR = 425021;
const int ERR_TRS_TESTLOADER_VACUUM__ERROR = 425022;

const int ERR_TRS_TESTLOADER_CYL_INITIALIZE_TIMEOVER = 425030;

const int ERR_TRS_TESTLOADER_INTERFACE_ERROR = 425040;
const int ERR_TRS_TESTLOADER_SYSTEM_STATE_ERROR = 425041;

const int ERR_TRS_TESTLOADER_SYSTEM_STATE_PORT_FAIL = 425042;

const int ERR_TRS_TESTLOADER_INTERLOCK_1 = 425071;
const int ERR_TRS_TESTLOADER_NO_DEFINE = 425991;

/************************************************************************/
/* OBJ_PL_ROBOT : 430													*/
/************************************************************************/
// OBJ_PL_ROBOT : 430
const int ERR_TRS_ROBOT_INITIAL_FAIL = 430001;

const int ERR_TRS_ROBOT_INTERFACE_ERROR = 430002;

const int ERR_TRS_ROBOT_ERROR_STATE = 430003;
const int ERR_TRS_ROBOT_ERROR_GRIP = 430004;

const int ERR_TRS_ROBOT_MOVE_HOME_TIMEOUT = 430011;
const int ERR_TRS_ROBOT_JOB_IN_PICKU_TIMEOUT = 430012;
const int ERR_TRS_ROBOT_JOB_TEST_GETPHONE_TIMEOUT = 430013;
const int ERR_TRS_ROBOT_JOB_TEST_SETPHONE_TIMEOUT = 430014;
const int ERR_TRS_ROBOT_JOB_OUT_PLACE_TIMEOUT = 430015;
const int ERR_TRS_ROBOT_JOB_BUFF_PICK_TIMEOUT = 430016;
const int ERR_TRS_ROBOT_JOB_BUFF_PLACE_TIMEOUT = 430017;
const int ERR_TRS_ROBOT_JOB_NG_OUT_TIMEOUT = 430018;
const int ERR_TRS_ROBOT_START_MOVE_TIMEOUT = 430019;

const int ERR_TRS_ROBOT_UNEXPECTED_DETECTED_ONHEAD = 430021;


const int ERR_TRS_ROBOT_INTERLOCK_1 = 430071;
const int ERR_TRS_ROBOT_NO_DEFINE = 430991;


/************************************************************************/
/* OBJ_PL_LOADERJOB_MANGER : 440										*/
/************************************************************************/
// OBJ_PL_LOADERJOB_MANGER : 440
const int ERR_TRS_LOADERJOB_MANGER_INITIAL_FAIL = 440001;

const int ERR_TRS_LOADERJOB_MANGER_INTERLOCK_1 = 440071;
const int ERR_TRS_LOADERJOB_MANGER_NO_DEFINE = 440991;


#endif