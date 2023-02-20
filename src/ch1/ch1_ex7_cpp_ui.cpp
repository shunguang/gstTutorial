#include "Ch1CppExample.h"
int ch1_ex7_cpp_ui(int argc, char* argv[])
{
	//we use its default values
	Ch1CppExampleCfg cfg;   
	Ch1CppExample x(cfg);
	x.start();
	return 0;
}

