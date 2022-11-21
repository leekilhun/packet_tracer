#pragma once


#define IO_OUTPUT_MAX				3000
#define IO_INPUT_MAX				3000
#define IO_ADDR_BASE_INPUT			1000 + SYSTEM_PERIPH_BASE
#define IO_ADDR_BASE_OUTPUT			IO_INPUT_MAX + IO_ADDR_BASE_INPUT

namespace IO
{
	namespace in 
	{
		enum
		{
			//X000 ~ X00F
			OP_SW_Estop = IO_ADDR_BASE_INPUT, //1000,X000
			OP_SW_start,//1001
			OP_SW_stop,//1002
			OP_SW_reset,//1003
			JogBoxX_cw,//1004
			JogBoxX_ccw,//1005
			JogBoxY_cw,//1006
			JogBoxY_ccw,//1007
			JogBoxZ_cw,//1008
			JogBoxZ_ccw,//1009
			JogBoxR_cw,//1010
			JogBoxR_ccw,//1011
			BattAlign_For,//1012
			BattAlign_Back,//1013
			BattGripperOpen,//1014
			BattGripperClose,//1015
			BattDetect_in,//1016
			BattDetect_mid,//1017
			BattDetect_end,//1018
			reserved_X013,//1019
			reserved_X014,//1020
			reserved_X015,//1021
			reserved_X016,//1022
			reserved_X017,//1023
			reserved_X018,//1024
			reserved_X019,//1025
			reserved_X01A,//1026
			reserved_X01B,//1027
			reserved_X01C,//1028
			reserved_X01D,//1029
			reserved_X01E,//1030
			reserved_X01F,//1031
			detect_front,//1032
			detect_rear,//1033
			detect_chip,//1034
			reserved_X023,//1035
			cutterUnit_For,//1036
			cutterUnit_Back,//1037
			pressGuide_R_up,//1038
			pressGuide_R_down,//1039
			pressGuide_L_up,//1040
			pressGuide_L_down,//1041
			cutterKnife_up,//1042
			cutterKnife_down,//1043
			tapePosition_up,//1044
			tapePosition_down,//1045
			holeDetector_up,//1046
			holeDetector_down,//1047
			centeringTape_open,//1048
			centeringTape_close,//1049
			reserved_X032,//1050
			reserved_X033,//1051
			reserved_X034,//1052
			reserved_X035,//1053
			reserved_X036,//1054
			reserved_X037,//1055
			reserved_X038,//1056
			reserved_X039,//1057
			reserved_X03A,//1058
			reserved_X03B,//1059
			reserved_X03C,//1060
			reserved_X03D,//1061
			reserved_X03E,//1062
			reserved_X03F,//1063
			detect_old_comein,//1064
			detect_new_comein,//1065
			reserved_X042,//1066
			reserved_X043,//1067
			loderUD_R_up,//1068
			loderUD_R_down,//1069
			loaderFB_R_for,//1070
			loaderFB_R_back,//1071
			loderUD_L_up,//1072
			loderUD_L_down,//1073
			loaderFB_L_for,//1074
			loaderFB_L_back,//1075
			tapePullOut_R_for,//1076
			tapePullOut_R_back,//1077
			tapePullOut_L_for,//1078
			tapePullOut_L_back,//1079
			reserved_X050,//1080
			reserved_X051,//1081
			reserved_X052,//1082
			reserved_X053,//1083
			reserved_X054,//1084
			reserved_X055,//1085
			reserved_X056,//1086
			reserved_X057,//1087
			reserved_X058,//1088
			reserved_X059,//1089
			reserved_X05A,//1090
			reserved_X05B,//1091
			reserved_X05C,//1092
			reserved_X05D,//1093
			reserved_X05E,//1094
			reserved_X05F,//1095
			reserved_X060,//1096
			reserved_X061,//1097
			reserved_X062,//1098
			reserved_X063,//1099
			reserved_X064,//1100
			reserved_X065,//1101
			reserved_X066,//1102
			reserved_X067,//1103
			reserved_X068,//1104
			reserved_X069,//1105
			reserved_X06A,//1106
			reserved_X06B,//1107
			reserved_X06C,//1108
			reserved_X06D,//1109
			reserved_X06E,//1110
			reserved_X06F,//1111


		};
	}
	

	namespace out
	{
		enum
		{
			OP_SW_Lamp_start = IO_ADDR_BASE_OUTPUT, //4000,Y000
			OP_SW_Lamp_stop,//4001
			OP_SW_Lamp_reset,//4002
			TowerLamp_red,//4003
			TowerLamp_yellow,//4004
			TowerLamp_green,//4005
			Buzzer,//4006
			SafetyPLC_reset,//4007
			BattAlign_For,//4008
			BattAlign_Back,//4009
			BattGripperOpen,//4010
			BattGripperClose,//4011
			reserved_Y00C,//4012
			reserved_Y00D,//4013
			reserved_Y00E,//4014
			reserved_Y00F,//4015
			reserved_Y010,//4016
			reserved_Y011,//4017
			reserved_Y012,//4018
			reserved_Y013,//4019
			reserved_Y014,//4020
			reserved_Y015,//4021
			reserved_Y016,//4022
			reserved_Y017,//4023
			reserved_Y018,//4024
			reserved_Y019,//4025
			reserved_Y01A,//4026
			reserved_Y01B,//4027
			reserved_Y01C,//4028
			reserved_Y01D,//4029
			reserved_Y01E,//4030
			reserved_Y01F,//4031
			cut_axisForBack_Run,//4032
			cut_axisForBack_Dir,//4033
			reserved_Y022,//4034
			reserved_Y023,//4035
			cutterUnit_For,//4036
			cutterUnit_Back,//4037
			pressGuide_R_up,//4038
			pressGuide_R_down,//4039
			pressGuide_L_up,//4040
			pressGuide_L_down,//4041
			cutterKnife_up,//4042
			cutterKnife_down,//4043
			tapePosition_up,//4044
			tapePosition_down,//4045
			holeDetector_up,//4046
			holeDetector_down,//4047
			centeringTape_open,//4048
			centeringTape_close,//4049
			reserved_Y032,//4050
			reserved_Y033,//4051
			reserved_Y034,//4052
			reserved_Y035,//4053
			reserved_Y036,//4054
			reserved_Y037,//4055
			reserved_Y038,//4056
			reserved_Y039,//4057
			reserved_Y03A,//4058
			reserved_Y03B,//4059
			reserved_Y03C,//4060
			reserved_Y03D,//4061
			reserved_Y03E,//4062
			reserved_Y03F,//4063
			sp_axisForBack_Run,//4064
			sp_axisForBack_Dir,//4065
			axisUpDw_Run,//4066
			axisUpDw_Dir,//4067
			loderUD_R_up,//4068
			loderUD_R_down,//4069
			loaderFB_R_for,//4070
			loaderFB_R_back,//4071
			loderUD_L_up,//4072
			loderUD_L_down,//4073
			loaderFB_L_for,//4074
			loaderFB_L_back,//4075
			tapePullOut_R_for,//4076
			tapePullOut_R_back,//4077
			tapePullOut_L_for,//4078
			tapePullOut_L_back,//4079
			reserved_Y050,//4080
			reserved_Y051,//4081
			reserved_Y052,//4082
			reserved_Y053,//4083
			reserved_Y054,//4084
			reserved_Y055,//4085
			reserved_Y056,//4086
			reserved_Y057,//4087
			reserved_Y058,//4088
			reserved_Y059,//4089
			reserved_Y05A,//4090
			reserved_Y05B,//4091
			reserved_Y05C,//4092
			reserved_Y05D,//4093
			reserved_Y05E,//4094
			reserved_Y05F,//4095
			reserved_Y060,//4096
			reserved_Y061,//4097
			reserved_Y062,//4098
			reserved_Y063,//4099
			reserved_Y064,//4100
			reserved_Y065,//4101
			reserved_Y066,//4102
			reserved_Y067,//4103
			reserved_Y068,//4104
			reserved_Y069,//4105
			reserved_Y06A,//4106
			reserved_Y06B,//4107
			reserved_Y06C,//4108
			reserved_Y06D,//4109
			reserved_Y06E,//4110
			reserved_Y06F,//4111

		};
	}



}
