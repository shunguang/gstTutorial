#include "GstUdpSenderCfg.h"
#include "GstUdpSender.h"

using namespace std;
using namespace app;

void createNewFrm( HostYuvFrmPtr &frm );

int ch2_testGstUdpSender(int argc, char* argv[])
{
	const std::string serverIp = "127.0.0.1";
	const std::string clientIp = "127.0.0.1";
	const uint16_t rtspPort = 5000;
	
	GstUdpSenderCfgPtr cfg(new GstUdpSenderCfg(serverIp, clientIp, rtspPort));
	GstUdpSenderPtr x(new GstUdpSender(cfg));

	HostYuvFrmPtr frm(new HostYuvFrm( cfg->imgSz.w, cfg->imgSz.h));

	x->startStreamingThread();

	const int timeToSleep_ms = 30;
	
	cv::Mat  bgrImg(cfg->imgSz.w, cfg->imgSz.h, CV_8UC3);
	while (1) {
		createNewFrm( frm );
		x->addNewFrm( frm.get() );

		frm->hdCopyToBgr(bgrImg);
		cv::imshow("send", bgrImg);
		char c = (char)cv::waitKey(1);
		//std::cout << (int)c << std::endl;
		if (c == 27) {
			break;
		}

		//sleep a while
		APP_SLEEP_MS(timeToSleep_ms);

	}
	x->endStreamingThread();

	return 0;
}


void createNewFrm(HostYuvFrmPtr& frm)
{
	static uint64_t fn = 0;
	frm->fn_ = fn++;
	frm->setToRand();
	frm->wrtFrmNumOnImg();
}
