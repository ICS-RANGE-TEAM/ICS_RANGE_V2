#include "beremiz.h"
#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

__DECLARE_ENUMERATED_TYPE(LOGLEVEL,
  LOGLEVEL__CRITICAL,
  LOGLEVEL__WARNING,
  LOGLEVEL__INFO,
  LOGLEVEL__DEBUG
)
// FUNCTION_BLOCK LOGGER
// Data part
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)
  __DECLARE_VAR(BOOL,ENO)
  __DECLARE_VAR(BOOL,TRIG)
  __DECLARE_VAR(STRING,MSG)
  __DECLARE_VAR(LOGLEVEL,LEVEL)

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,TRIG0)

} LOGGER;

void LOGGER_init__(LOGGER *data__, BOOL retain);
// Code part
void LOGGER_body__(LOGGER *data__);
// PROGRAM DISPATCH
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,INT_PYLONE)
  __DECLARE_VAR(BOOL,INT_PYLONE_STATUS)
  __DECLARE_VAR(BOOL,INT_TRANSFORM_A)
  __DECLARE_VAR(BOOL,INT_TRANSFORM_A_STATUS)
  __DECLARE_VAR(BOOL,INT_TRANSFORM_B_STATUS)
  __DECLARE_VAR(BOOL,INT_TRANSFORM_AETB_STATUS)

} DISPATCH;

void DISPATCH_init__(DISPATCH *data__, BOOL retain);
// Code part
void DISPATCH_body__(DISPATCH *data__);
#endif //__POUS_H
