//#include "DutDllVerNo.h"


#define DUTCTRL_VERSION_MAJOR1		1
#define DUTCTRL_VERSION_MAJOR2		0
#define DUTCTRL_VERSION_MINOR1		0
#define DUTCTRL_VERSION_MINOR2		0

//#define VERSTRING (0x30|DUTCTRL_VERSION_MAJOR1) "," "2" "," "0" ","  "," 0x0
//#define VERSTRING					0x31","0x32","0x30","0x30"\0"
#define DUTCTRL_VERSTRING	"1,0,0,0\0"

#define DUT_DLL_FILEVER				(DUTCTRL_VERSION_MAJOR1),(DUTCTRL_VERSION_MAJOR2),(DUTCTRL_VERSION_MINOR1),(DUTCTRL_VERSION_MINOR2) //123,45,67,89//1,2,1,0
#define DUT_DLL_PRODUCTVER			(DUTCTRL_VERSION_MAJOR1),(DUTCTRL_VERSION_MAJOR2),(DUTCTRL_VERSION_MINOR1),(DUTCTRL_VERSION_MINOR2) //123,45,67,89//1,2,1,0
#define DUT_DLL_FILEVER_STRING		DUTCTRL_VERSTRING
#define DUT_DLL_PRODUCTVER_STRING	DUTCTRL_VERSTRING  

#define DUT_DLL_COMMENTSTRING			"With MFG driver\0"
#define DUT_DLL_SPECIALBUILDSTRING		"\0"  
#define DUT_DLL_PRODUCTNAMESTRING		"802.11A/G MIMO Client Product (Low Power Solution)\0"
#define DUT_DLL_PRIVATEBUILDSTRING		"Preliminary\0"
#define DUT_DLL_FILEDESCRIPTIONSTRING	"Dut Api Dll\0"