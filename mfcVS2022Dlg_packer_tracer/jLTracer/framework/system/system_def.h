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
