#include "Ch1CppExample.h"
int ch1_ex6_cpp_ui(int argc, char* argv[])
{
	//we use its default values
	Ch1CppExampleCfg cfg;   
	Ch1CppExample app(cfg);
	app.start();

	//todo: there are some conflict between quit from by press 'q' and 'x' our the video window
	char c;
	std::cin >> c;
	while ( c != 'q' ) {
		std::cin >> c;
	}
  app.stop();
	return 0;
}

