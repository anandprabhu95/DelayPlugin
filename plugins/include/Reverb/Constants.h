#define NUM_DELAY_LINES					8
#define N_CH_REV_BUFF					NUM_DELAY_LINES * 2

#define NUM_REV_STAGES					4

#define MIX_MAT_00						[1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1]
#define MIX_MAT_01						[1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1]
#define MIX_MAT_02						[1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1]
#define MIX_MAT_03						[1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1]
#define MIX_MAT_04						[1,  1,  1,  1, -1, -1, -1, -1,  1,  1,  1,  1, -1, -1, -1, -1]
#define MIX_MAT_05						[1, -1,  1, -1, -1,  1, -1,  1,  1, -1,  1, -1, -1,  1, -1,  1]
#define MIX_MAT_06						[1,  1, -1, -1, -1, -1,  1,  1,  1,  1, -1, -1, -1, -1,  1,  1]
#define MIX_MAT_07						[1, -1, -1,  1, -1,  1,  1, -1,  1, -1, -1,  1, -1,  1,  1, -1]
#define MIX_MAT_08						[1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1]
#define MIX_MAT_09						[1, -1,  1, -1,  1, -1,  1, -1, -1,  1, -1,  1, -1,  1, -1,  1]
#define MIX_MAT_10						[1,  1, -1, -1,  1,  1, -1, -1, -1, -1,  1,  1, -1, -1,  1,  1]
#define MIX_MAT_11						[1, -1, -1,  1,  1, -1, -1,  1, -1,  1,  1, -1, -1,  1,  1, -1]
#define MIX_MAT_12						[1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  1,  1,  1]
#define MIX_MAT_13						[1, -1,  1, -1, -1,  1, -1,  1, -1,  1, -1,  1,  1, -1,  1, -1]
#define MIX_MAT_14						[1,  1, -1, -1, -1, -1,  1,  1, -1, -1,  1,  1,  1,  1, -1, -1]
#define MIX_MAT_15						[1, -1, -1,  1, -1,  1,  1, -1, -1,  1,  1, -1,  1, -1, -1,  1]

#define MAX_DELAYLINE_TIME				0.2f
#define MIN_DELAYLINE_TIME				0.03f


#define BG_RED							0.24f
#define BG_GREEN						0.18f
#define BG_BLUE							0.58f
#define BG_ALPHA						1.0f

#define PI								3.14159f

#define HEIGHT							400
#define WIDTH							600

#define SLIDER_WIDTH					100
#define SLIDER_HEIGHT					100

#define LIN_SLIDER_WIDTH				70
#define LIN_SLIDER_HEIGHT				24

#define SLIDER_LABEL_WIDTH				100
#define SLIDER_LABEL_HEIGHT				24

#define TEXTBOX_WIDTH					40
#define TEXTBOX_HEIGHT					20

#define BUTTON_WIDTH					20
#define BUTTON_HEIGHT					20

#define GLOBAL_X						100
#define GLOBAL_Y						50

#define LABEL_FONTSIZE					10.0f
#define VALUELABEL_FONTSIZE				14.0f


#define ROW1_Y							15
#define ROW2_Y							165

#define COLUMN1_X						0
#define COLUMN2_X						100
#define COLUMN3_X						200
#define COLUMN4_X						300


#define ROW1_VALDISP_LABEL_Y			55
#define ROW2_VALDISP_LABEL_Y			205

#define COLUMN1_VALDISP_LABEL_X			0
#define COLUMN2_VALDISP_LABEL_X			100
#define COLUMN3_VALDISP_LABEL_X			200
#define COLUMN4_VALDISP_LABEL_X			300

#define VALUE_LABEL_HEIGHT				15
#define VALUE_LABEL_WIDTH				100


#define ROW1_LABEL_Y					-10
#define ROW2_LABEL_Y					140

#define COLUMN1_LABEL_X					0
#define COLUMN2_LABEL_X					100
#define COLUMN3_LABEL_X					200
#define COLUMN4_LABEL_X					300