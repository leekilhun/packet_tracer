#pragma once

/*


*/
namespace GMoons
{

    enum class moons_modbus_func_e {
        read_HoldingReg = 0x03,
        read_InputReg = 0x04,
        write_SingleReg = 0x06,
        write_MultiReg = 0x10

    };

    enum class moons_SS_Opcode_e {
        alarm_reset = 0xBA,
        start_jogging = 0x96,
        stop_jogging = 0xD8,
        encoder_function = 0xD6,
        encoder_position = 0x98,
        feed_to_double_sensor = 0x69,
        follow_encoder = 0xCC,
        feed_to_length = 0x66,
        feed_to_sensor_with_mask_distance = 0x6A,
        feedand_set_output = 0x68,
        feed_to_position = 0x67,
        feed_to_sensor = 0x6B,
        feed_to_sensor_with_safety_distance = 0x6C,
        jog_disable = 0xA3,
        jog_enable = 0xA2,
        motor_disable = 0x9E,
        motor_enable = 0x9F,
        seek_home = 0x6E,
        set_position = 0xA5,
        filter_input = 0xC0,
        filter_select_inputs = 0xD3,
        step_filter_freq = 0x06,
        analog_deadband = 0xD2,
        alarm_reset_input = 0x46,
        alarm_output = 0x47,
        analog_scaling = 0xD1,
        define_limits = 0x42,
        set_output = 0x8B,
        wait_for_input = 0x70,
        queue_load_and_execute = 0x78,
        wait_time = 0x6F,
        stop_move_kill_buffer = 0xE1,
        stop_move_kill_Buffer_normal_decel = 0xE2,

    };

    /*
    // SCL Command Encoding Table

    Function                              SCL  Opcode   Param1               Param2             Param3         Param4        Param5
    Alarm Reset                           AX    0xBA
    Start Jogging                         CJ    0x96
    Stop Jogging                          SJ    0xD8
    Encoder Function                      EF    0xD6       0,1,2 or 6
    Encoder Position                      EP    0x98       Position
    Feed to Double Sensor                 FD    0x69       I/O Point 1        Condition 1     I/O Point 2     Condition 2
    Follow Encoder                        FE    0xCC       I/O Point          Condition
    Feed to Length                        FL    0x66
    Feed to Sensor with Mask Distance     FM    0x6A       I/O Point          Condition
    Feed and Set Output                   FO    0x68       I/O Point          Condition
    Feed to Position                      FP    0x67
    Feed to Sensor                        FS    0x6B       I/O Point          Condition
    Feed to Sensor with Safety Distance   FY    0x6C       I/O Point          Condition
    Jog Disable                           JD    0xA3
    Jog Enable                            JE    0xA2
    Motor Disable                         MD    0x9E
    Motor Enable                          ME    0x9F
    Seek Home                             SH    0x6E       I/O Point          Condition
    Set Position                          SP    0xA5       Position
    Filter Input                          FI    0xC0       I/O Point          Filter Time
    Filter Select Inputs                  FX    0xD3
    Step Filter Freq                      SF    0x06       Freq
    Analog Deadband                       AD    0xD2       0.001 V
    Alarm Reset Input                     AI    0x46       Function(‘1’..’3’) I/O Point
    Alarm Output                          AO    0x47       Function(‘1’..’3’) I/O Point
    Analog Scaling                        AS    0xD1
    Define Limits                         DL    0x42       1..3
    Set Output                            SO    0x8B       I/O Point          Condition
    Wait for Input                        WI    0x70
    Queue Load & Execute                  QX    0x78       1..12
    Wait Time                             WT    0x6F       0.01 sec
    Stop Move, Kill Buffer                SK    0xE1
    Stop Move, Kill Buffer, Normal Decel  SKD   0xE2

    */


    /// <summary>
    /// ss타입의  moons 모터 레지스터 주소와 데이터 수
    /// </summary>
    struct moons_SS_reg_t
    {
        uint16_t addr;
        uint16_t reg_cnt;
    };


    constexpr int DEF_MOONS_SS_REGISTER_BASE = 40001;
    enum class moons_SS_reg_e
    {
        Alarm_Code_AL_f = 0,
        Status_Code_SC_s,
        Immediate_Expanded_Inputs_IS_y,
        Driver_Board_Inputs_ISX_i,
        Encoder_Position_IE_EP_e,
        Immediate_Absolute_Position_IP_I = 6,
        Absolute_Position_Command_SP_P = 8,
        Immediate_Actual_Velocity_IV0_v = 10,
        Immediate_Target_Velocity_IV1_w,
        Immediate_Drive_Temperature_IT_t,
        Immediate_Bus_Voltage_IU_u,
        Immediate_Position_Error_IX_x,
        Immediate_Analog_Input_Value_IA_a = 16,
        Q_Program_Line_Number_b,
        Immediate_Current_Command_IC_c,
        Relative_Distance_ID_d,
        Sensor_Position_g = 21,
        Condition_Code_h = 23,
        Analog_Input_1_IA1_j,
        Analog_Input_2_IA2_k,
        Command_Mode_CM_m,
        Point_to_Point_Acceleration_AC_A,
        Point_to_Point_Deceleration_DE_B,
        Velocity_VE_V,
        Point_to_Point_Distance_DI_D,
        Change_Distance_DC_C = 32,
        Change_Velocity_VC_U = 34,
        Velocity_Move_State_n,
        Point_to_Point_Move_State_o,
        Q_Program_Segment_Number_p,
        Reserved_0,
        Phase_Error_z,
        Position_Offset_E,
        Miscellaneous_Flags_F = 42,
        Current_Command_GC_G,
        Input_Counter_I,
        Jog_Accel_JA_ = 46,
        Jog_Decel_JL_,
        Jog_Velocity_JS__J,
        Max_Velocity_VM_,
        Running_Current_CC__N,
        Peak_Current_CP_,
        Steps_per_Revolution_EG__R,
        Pulse_Counter_S,
        Analog_Position_Gain_AP__X = 55,
        Analog_Threshold_AT__Y,
        Analog_Offset_AV__Z,
        Accumulator_0,
        User_Defined_1 = 60,
        User_Defined_2 = 62,
        User_Defined_3 = 64,
        User_Defined_4 = 66,
        User_Defined_5 = 68,
        User_Defined_6 = 70,
        User_Defined_7 = 72,
        User_Defined_8 = 74,
        User_Defined_9 = 76,
        User_Defined_a = 78,
        User_Defined_b = 80,
        User_Defined_c = 82,
        User_Defined_e = 84,
        User_Defined_f = 86,
        User_Defined_11 = 88,
        User_Defined_12 = 90,
        User_Defined_13 = 92,
        User_Defined_14 = 94,
        User_Defined_15 = 96,
        User_Defined_16 = 98,
        User_Defined_17 = 100,
        User_Defined_18 = 102,
        Brake_Release_Delay_BD_ = 104,
        Brake_Engage_Delay_BE_,
        Reserved_1,
        Reserved_2,
        Firmware_Version,
        Analog_Filter_Gain_AF_,
        reserved_10,
        Alarm_Code_Upper,
        reserved_11,
        reserved_12,
        reserved_13,
        reserved_14,
        reserved_15,
        reserved_16,
        reserved_17,
        reserved_18,
        Filter_Input_1 = 120,
        Filter_Input_2,
        Filter_Input_3,
        Filter_Input_4,
        Command_Opcode,
        Parameter_1,
        Parameter_2,
        Parameter_3,
        Parameter_4,
        Parameter_5,
        Reserved_3,
        Reserved_4,
        Smoothing_filter_frequency_SF_,
        Address_Upper_AU__SS_only_,
        Motor_Detail,
        Step_Mode_Input_noise_filter_SZ_,
        Reserved_5,
        Control_Mode_set_CM_,
        Operation_Mode_OM_,
        Servo_Enable_Input_SI_,
        Alarm_Reset_Input_AI_,
        Limit_Sensor_Input_DL_,
        Alarm_Output_AO_,
        Brake_Output_BO_,
        Move_Output_MO_,
        Reserved_6,
        Reserved_7,
        Low_Voltage_LV_,
        Baud_Rate_BR_,
        Protocol_PR_,
        Transmit_Delay_TD_,
        Reserved_8,
        Position_Proportional_Gain_KP_,
        Position_Derivative_Gain_KD_,
        Derivative_Filter_factor_KE_,
        Velocity_Proportional_Gain_VP_,
        Velocity_Integral_Gain_VI_,
        Acceleration_Feed_forward_KK_,
        Torque_Command_Filter_KC_,
        Max_Acceleration_AM_,
        Position_Fault_Window_PF_,
        Address_DA_,
        Analog_Velocity_Gain_AG_,
        Jog_Change_Velocity_JC_,
        Jog_Mode_JM_,
        Analog_Current_Gain_AN_,
        Homing_Acceleration_1,
        Homing_Acceleration_2,
        Homing_Acceleration_3,
        Homing_Deceleration_1,
        Homing_Deceleration_2,
        Homing_Deceleration_3,
        Homing_Velocity_1,
        Homing_Velocity_2,
        Homing_Velocity_3,
        HardStop_Current_Limit_HC_,
        Pulse_Complete_Timing_TT_,
        TSM__SSM_Tach_Output_TO_SS_Series_Dump,
        In_Position_Limit_PL_,
        In_Position_Timing_PE_,
        In_Position_Counts_PD_,
        Alarm_Mask_MA_,
    };



    static moons_SS_reg_t m_regData[] =
    {
      {40001,1}
    };


    /*

    40001           Read Only         SHORT                 Alarm Code (AL) f
    40002           Read Only         SHORT                 Status Code (SC) s
    40003           Read Only         SHORT                 Immediate Expanded Inputs (IS) y
    40004           Read Only         SHORT                 Driver Board Inputs (ISX) i
    40005..6        Read Only         LONG                  Encoder Position (IE, EP) e
    40007..8        Read Only         LONG                  Immediate Absolute Position(IP) l
    40009..10       Read Only         LONG                  Absolute Position Command(SP) P
    40011           Read Only         SHORT                 Immediate Actual Velocity (IV0) v
    40012           Read Only         SHORT                 Immediate Target Velocity (IV1) w
    40013           Read Only         SHORT                 Immediate Drive Temperature (IT) t
    40014           Read Only         SHORT                 Immediate Bus Voltage (IU) u
    40015..16       Read Only         LONG                  Immediate Position Error (IX) x
    40017           Read Only         SHORT                 Immediate Analog Input Value (IA) a
    40018           Read Only         SHORT                 Q Program Line Number b
    40019           Read Only         SHORT                 Immediate Current Command (IC) c
    40020..21       Read Only         LONG                  Relative Distance (ID) d
    40022..23       Read Only         LONG                  Sensor Position g
    40024           Read Only         SHORT                 Condition Code h
    40025           Read Only         SHORT                 Analog Input 1 (IA1) j
    40026           Read Only         SHORT                 Analog Input 2 (IA2) k
    40027           Read Only         SHORT                 Command Mode (CM) m
    40028           R/W               SHORT                 Point-to-Point Acceleration (AC) A
    40029           R/W               SHORT                 Point-to-Point Deceleration (DE) B
    40030           R/W               SHORT                 Velocity (VE) V
    40031..32       R/W               LONG                  Point-to-Point Distance (DI) D
    40033..34       R/W               LONG                  Change Distance (DC) C
    40035           R/W               SHORT                 Change Velocity (VC) U
    40036           Read Only         SHORT                 Velocity Move State n
    40037           Read Only         SHORT                 Point-to-Point Move State o
    40038           Read Only         SHORT                 Q Program Segment Number p
    40039           Reserved
    40040           Read Only         SHORT                 Phase Error z
    40041..42       R / W             LONG                  Position Offset E
    40043           R / W             SHORT                 Miscellaneous Flags F
    40044           R / W             SHORT                 Current Command(GC) G
    40045..46       R / W             LONG                  Input Counter I
    40047           R / W             SHORT                 Jog Accel(JA)
    40048           R / W             SHORT                 Jog Decel(JL)
    40049           R / W             SHORT                 Jog Velocity(JS) J
    40050           R / W             SHORT                 Max Velocity(VM)
    40051           R / W             SHORT                 Running Current(CC) N
    40052           R / W             SHORT                 Peak Current(CP)
    40053           R / W             SHORT                 Steps per Revolution(EG) R
    40054..55       R / W             SHORT                 Pulse Counter S
    40056           R / W             SHORT                 Analog Position Gain(AP) X
    40057           R / W             SHORT                 Analog Threshold(AT) Y
    40058           R / W             SHORT                 Analog Offset(AV) Z
    40059..60       R / W             LONG                  Accumulator 0
    40061..62       R / W             LONG                  User Defined 1
    40063..64       R / W             LONG                  User Defined 2
    40065..66       R / W             LONG                  User Defined 3
    40067..68       R / W             LONG                  User Defined 4
    40069..70       R / W             LONG                  User Defined 5
    40071..72       R / W             LONG                  User Defined 6
    40073..74       R / W             LONG                  User Defined 7
    40075..76       R / W             LONG                  User Defined 8
    40077..78       R / W             LONG                  User Defined 9
    40079..80       R / W             LONG                  User Defined :
    40081..82       R / W             LONG                  User Defined;
    40083..84       R / W             LONG                  User Defined <

    40085..86       R / W             LONG                  User Defined =
    40087..88       R / W             LONG                  User Defined >
    40089..90       R / W             LONG                  User Defined ?
    40091..92       R / W             LONG                  User Defined @
    40093..94       R / W             LONG                  User Defined[
    40095..96       R / W             LONG                  User Defined \
    40097..98       R / W             LONG                  User Defined]
    40099..100      R / W             LONG                  User Defined ^
    40101..102      R / W             LONG                  User Defined _
    40103..104      R / W             LONG                  User Defined `
    40105           R / W             SHORT                 Brake Release Delay(BD)
    40106           R / W             SHORT                 Brake Engage Delay(BE)
    40107           Read Only         SHORT                 Reserved
    40108           R / W             SHORT                 Reserved
    40109           R / W             SHORT                 Firmware Version
    40110           R / W             SHORT                 Analog Filter Gain(AF)
    40111           Reserved
    40112           Read Only         SHORT                 Alarm Code Upper
    40113..120      Reserved
    40121           R / W             SHORT                 Filter Input 1#
    40122           R / W             SHORT                 Filter Input 2#
    40123           R / W             SHORT                 Filter Input 3#
    40124           R / W             SHORT                 Filter Input 4# (not SSM)
    40125           R / W             SHORT                 Command Opcode
    40126           R / W             SHORT                 Parameter 1
    40127           R / W             SHORT                 Parameter 2
    40128           R / W             SHORT                 Parameter 3
    40129           R / W             SHORT                 Parameter 4
    40130           R / W             SHORT                 Parameter 5
    40131           Read Only         SHORT                 Reserved
    40132           Read Only         SHORT                 Reserved
    40133           R / W             SHORT                 Smoothing filter frequency(SF)
    40134           R / W             SHORT                 Address Upper(AU)(SS only)
    40135           R / W             SHORT                 Motor Detail
    40136           R / W             SHORT                 Step Mode / Input noise filter(SZ)
    40137           Read Only         SHORT                 Reserved
    40138           R / W             SHORT                 Control Mode set(CM)
    40139           R / W             SHORT                 Operation Mode(OM)
    40140           R / W             SHORT                 Servo Enable Input(SI)
    40141           R / W             SHORT                 Alarm Reset Input(AI)
    40142           R / W             SHORT                 Limit Sensor Input(DL)
    40143           R / W             SHORT                 Alarm Output(AO)
    40144           R / W             SHORT                 Brake Output(BO)
    40145           R / W             SHORT                 Move Output(MO)
    40146           R / W             SHORT                 Reserved
    40147           R / W             SHORT                 Reserved
    40148           R / W             SHORT                 Low Voltage(LV)
    40149           R / W             SHORT                 Baud Rate(BR)
    40150           R / W             SHORT                 Protocol(PR)
    40151           R / W             SHORT                 Transmit Delay(TD)
    40152           R / W             SHORT                 Reserved
    40153           R / W             SHORT                 Position Proportional Gain(KP)
    40154           R / W             SHORT                 Position Derivative Gain(KD)
    40155           R / W             SHORT                 Derivative Filter factor(KE)
    40156           R / W             SHORT                 Velocity Proportional Gain(VP)
    40157           R / W             SHORT                 Velocity Integral Gain(VI)
    40158           R / W             SHORT                 Acceleration Feed - forward(KK)
    40159           R / W             SHORT                 Torque Command Filter(KC)
    40160           R / W             SHORT                 Max Acceleration(AM)
    40161           R / W             SHORT                 Position Fault Window(PF)
    40162           R / W             SHORT                 Address(DA)
    40163           R / W             SHORT                 Analog Velocity Gain(AG)
    40164           R / W             SHORT                 Jog Change Velocity(JC)
    40165           R / W             SHORT                 Jog Mode(JM)
    40166           R / W             SHORT                 Analog Current Gain(AN)
    40167           R / W             SHORT                 Homing Acceleration 1
    40168           R / W             SHORT                 Homing Acceleration 2
    40169           R / W             SHORT                 Homing Acceleration 3
    40170           R / W             SHORT                 Homing Deceleration 1
    40171           R / W             SHORT                 Homing Deceleration 2
    40172           R / W             SHORT                 Homing Deceleration 3
    40173           R / W             SHORT                 Homing Velocity 1
    40174           R / W             SHORT                 Homing Velocity 2
    40175           R / W             SHORT                 Homing Velocity 3
    40176           R / W             SHORT                 HardStop Current Limit(HC)
    40177           R / W             SHORT                 Pulse Complete Timing(TT)
    40178           R / W             SHORT                 TSM, SSM Tach Output(TO)  SS Series Dumping
    40179           R / W             SHORT                 In Position Limit(PL)
    40180           R / W             SHORT                 In Position Timing(PE)
    40181           R / W             SHORT                 In Position Counts(PD)
    40182           R / W             SHORT                 Alarm Mask(MA)
    40183..40200    R / W             SHORT                 Reserved

          */




          /*

          Hex Value BLu SV STAC6 ST STM
          0x0001     Position Limit
          0x0002     CCW Limit
          0x0004     CW Limit
          0x0008     Over Temp
          0x0010     Excess Regen* Internal Voltage Excess Regen Internal Voltage Internal Voltage
          0x0020     Over Voltage
          0x0040     Under Voltage* Under Voltage Under Voltage Under Voltage Under Voltage
          0x0080     Over Current
          0x0100     Bad Hall Sensor Open Motor Winding
          0x0200     Bad Encoder (not used)
          0x0400     Comm Error
          0x0800     Bad Flash
          0x1000     Wizard Failed No Move
          0x2000     Current Foldback Motor Resistance Out of Range (not used) (not used)
          0x4000     Blank Q Segment
          0x8000     No Move (not used)

          */
    union MOONS_SS_ALARM_STATUS
    {
        uint16_t	al_status;
        struct
        {
            unsigned	Position_Error : 1;	        // = 0x00000001;
            unsigned	CCW_Limit : 1;		        // = 0x00000002;
            unsigned	CW_Limit : 1;			    // = 0x00000004;
            unsigned	Over_Temp : 1;	            // = 0x00000008;
            unsigned	Internal_Voltage : 1;	    // = 0x00000010;
            unsigned	Over_Voltage : 1;		    // = 0x00000020;
            unsigned	Under_Voltage : 1;		    // = 0x00000040;
            unsigned	Over_Current : 1;	        // = 0x00000080;
            unsigned	Open_Winding : 1;	        // = 0x00000100;
            unsigned	Bad_Encoder : 1;		    // = 0x00000200;
            unsigned	Comm_Error : 1;		        // = 0x00000400;
            unsigned	Bad_Flash : 1;		        // = 0x00000800;
            unsigned	No_Move : 1;                // = 0x00001000;
            unsigned	Current_Foldback : 1;	    // = 0x00002000;
            unsigned	Blank_Q_Segment : 1;	    // = 0x00004000;
            unsigned	NV_Memory_Double_err : 1;	// = 0x00008000;
        };
    };
    /*

    Hex Value  Status Code bit definition
    0x0001     Motor Enabled (Motor Disabled if this bit = 0)
    0x0002     Sampling (for Quick Tuner)
    0x0004     Drive Fault (check Alarm Code)
    0x0008     In Position (motor is in position)
    0x0010     Moving (motor is moving)
    0x0020     Jogging (currently in jog mode)
    0x0040     Stopping (in the process of stopping from a stop command)
    0x0080     Waiting for an input (executing WI command)
    0x0100     Saving (parameter data is being saved)
    0x0200     Alarm present (check Alarm Code)
    0x0400     Homing (executing an SH command)
    0x0800     Wait Time (executing a WT command)
    0x1000     Q Program is running
    0x2000     Initializing (happens at power up)
    0x4000     not used
    0x8000     not used
    0x
    */
    union MOONS_SS_DRIVE_STATUS
    {
        uint16_t	sc_status;
        struct
        {
            unsigned	Motor_Enabled : 1;			// = 0x00000001;
            unsigned	Sampling : 1;			    // = 0x00000002;
            unsigned	Drive_Fault : 1;			// = 0x00000004;
            unsigned	In_Position : 1;	        // = 0x00000008;
            unsigned	Moving : 1;	                // = 0x00000010;
            unsigned	Jogging : 1;		        // = 0x00000020;
            unsigned	Stopping : 1;			    // = 0x00000040;
            unsigned	Waiting_for_input : 1;	    // = 0x00000080;
            unsigned	Saving : 1;	                // = 0x00000100;
            unsigned	Alarm_present : 1;		    // = 0x00000200;
            unsigned	Homing : 1;		            // = 0x00000400;
            unsigned	WaitTime : 1;		        // = 0x00000800;
            unsigned	QProgram_is_running : 1;    // = 0x00001000;
            unsigned	Initializing : 1;			// = 0x00002000;
            unsigned	not_used0 : 1;		        // = 0x00004000;
            unsigned	not_used1 : 1;			    // = 0x00008000;
        };
    };

}