TYPE
  LOGLEVEL : (CRITICAL, WARNING, INFO, DEBUG) := INFO;
END_TYPE

FUNCTION_BLOCK LOGGER
  VAR_INPUT
    TRIG : BOOL;
    MSG : STRING;
    LEVEL : LOGLEVEL := INFO;
  END_VAR
  VAR
    TRIG0 : BOOL;
  END_VAR

  IF TRIG AND NOT TRIG0 THEN
  {{
   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  }}
  END_IF;
  TRIG0:=TRIG;
END_FUNCTION_BLOCK


PROGRAM dispatch
  VAR
    Int_Pylone : BOOL := TRUE;
    Int_Pylone_status : BOOL;
    Int_transform_A : BOOL := TRUE;
    Int_transform_A_status : BOOL;
    Int_transform_B_status : BOOL;
    Int_transform_AetB_status : BOOL;
  END_VAR

  IF Int_Pylone THEN
    Int_Pylone_status := TRUE;
    Int_transform_AetB_status := Int_Pylone_status;
    Int_transform_A_status := Int_transform_A;
    Int_transform_B_status := NOT Int_transform_A;
   
  ELSE
    Int_Pylone_status := FALSE;
    Int_transform_A := FALSE;
    Int_transform_A_status := FALSE;
    Int_transform_B_status := FALSE;
    Int_transform_AetB_status := FALSE;
  END_IF;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : dispatch;
  END_RESOURCE
END_CONFIGURATION
