void LOGGER_init__(LOGGER *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->TRIG,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MSG,__STRING_LITERAL(0,""),retain)
  __INIT_VAR(data__->LEVEL,LOGLEVEL__INFO,retain)
  __INIT_VAR(data__->TRIG0,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void LOGGER_body__(LOGGER *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  if ((__GET_VAR(data__->TRIG,) && !(__GET_VAR(data__->TRIG0,)))) {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  
    #undef GetFbVar
    #undef SetFbVar
;
  };
  __SET_VAR(data__->,TRIG0,,__GET_VAR(data__->TRIG,));

  goto __end;

__end:
  return;
} // LOGGER_body__() 





void DISPATCH_init__(DISPATCH *data__, BOOL retain) {
  __INIT_VAR(data__->INT_PYLONE,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->INT_PYLONE_STATUS,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INT_TRANSFORM_A,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->INT_TRANSFORM_A_STATUS,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INT_TRANSFORM_B_STATUS,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INT_TRANSFORM_AETB_STATUS,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void DISPATCH_body__(DISPATCH *data__) {
  // Initialise TEMP variables

  if (__GET_VAR(data__->INT_PYLONE,)) {
    __SET_VAR(data__->,INT_PYLONE_STATUS,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,INT_TRANSFORM_AETB_STATUS,,__GET_VAR(data__->INT_PYLONE_STATUS,));
    __SET_VAR(data__->,INT_TRANSFORM_A_STATUS,,__GET_VAR(data__->INT_TRANSFORM_A,));
    __SET_VAR(data__->,INT_TRANSFORM_B_STATUS,,!(__GET_VAR(data__->INT_TRANSFORM_A,)));
  } else {
    __SET_VAR(data__->,INT_PYLONE_STATUS,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,INT_TRANSFORM_A,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,INT_TRANSFORM_A_STATUS,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,INT_TRANSFORM_B_STATUS,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,INT_TRANSFORM_AETB_STATUS,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // DISPATCH_body__() 





