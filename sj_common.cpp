/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_common.setup("common");
		Group_common.add(b_webCam_Gray.setup("b_webCam_Gray", false));
	gui.add(&Group_common);
		
	Group_Particle.setup("Particle");
		Group_Particle.add(ParticleSpace.setup("Space", 1.0, 1.0, 10.0));
		Group_Particle.add(ParticleHeight.setup("Height", 50.0, 0.0, 400.0));
		Group_Particle.add(ParticleAlpha.setup("Alpha", 200.0, 0.0, 255.0));
		Group_Particle.add(ParticlePointSize.setup("PointSize", 3.0, 1.0, 10.0));
		Group_Particle.add(b_Particle_Mirror.setup("Mirror", true));
	gui.add(&Group_Particle);
	
	Group_Back.setup("Back");
		Group_Back.add(Alpba_img.setup("a_img", 0, 0, 255));
		Group_Back.add(Alpba_webCam.setup("a_webCam", 0, 0, 255));
	gui.add(&Group_Back);
	
	/********************
	********************/
	gui.minimizeAll();
}

