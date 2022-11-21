#pragma once


#include "hal_def.h"


#ifdef USE_HW_NET_SOCKET
#include "_inc/netSocket.h"
#endif


#ifdef USE_HW_SERIAL_COMM
#include "_inc/SerialComm.h"
#endif

#ifdef USE_HW_DIST_SENSOR_SICK
#include "_inc/dist_measure_sick.h"
#endif


#ifdef USE_HW_NET_SOCKET
#include "_inc/io_fastech.h"
#endif

#ifdef USE_HW_MOTOR_FASTECH_E_LIB
#include "_inc/motor_fastech_E_lib.h"
#endif

#ifdef USE_HW_MOTOR_FASTECH
#endif

#ifdef USE_HW_MOTOR_ORIENTAL_LIB
#include "_inc/motor_oriental_lib.h"
#endif


#ifdef USE_HW_MOTOR_ORIENTAL
#include "_inc/motor_oriental.h"
#endif


#ifdef USE_HW_MOTOR_MOONS_LIB
#include "_inc/motor_moons_lib.h"
#endif

#ifdef USE_HW_MOTOR_MOONS
#include "_inc/motor_moons.h"
#endif





