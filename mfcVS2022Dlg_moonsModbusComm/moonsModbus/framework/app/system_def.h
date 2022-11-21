#pragma once

/*
 * system_def.h
 * 작성자 : 이길훈
 * 22.04.04 - system define 및 include
 */

 /* 시스템 전역에 접근할 헤더를 include*/
 /* 아래 순서대로 절차를 진행 */
 /*

       hal_def.h   <-  def_obj.h   <-   system_def.h
                                            |
                                       system_obj.h
                                       */


#include "hal.h"


#include "def_obj.h"


//#include "def_err.h"

//#include "def_io.h"



 /****************************************************
  *	system config
  ****************************************************/
#define USE_IO_DATA_FORMAT_JSON
  //#define USE_IO_DATA_FORMAT_DAT



namespace AP_OBJ
{



  /****************************************************
   *	system define
   ****************************************************/

  enum class NET_OBJ
  {

    _max
  };


  enum class MOTION_LIB_OBJ
  {
    moons_modbus_lib,
    _max
  };

  enum class SERIAL_OBJ
  {
    moons_motor,
    //   ,
    _max
  };

  enum class AXIS_OBJ
  {
    moons_modbus,
    _max
  };



  enum class COMM_TYPE
  {
    net,
    serial,
    _max
  };


}

/*


struct prc_step_t
{
  uint8_t curr_step{};
  uint8_t pre_step{};
  uint8_t wait_step{};

  inline void SetStep(uint8_t step) {
    pre_step = curr_step;
    curr_step = step;
  }

  inline uint8_t GetStep() const {
    return curr_step;
  }

};

*/