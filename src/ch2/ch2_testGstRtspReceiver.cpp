#include "GstRtspReceiverCfg.h"
#include "GstRtspReceiver.h"

using namespace std;
using namespace app;

int ch2_testGstRtspReceiver(int argc, char* argv[])
{
	GstRtspReceiverCfgPtr cfg(new GstRtspReceiverCfg());

	cfg->isUdp = false;
	cfg->udpPort = 5000;

	cfg->rtspUsrName = "admin";
	cfg->rtspUsrPassword = "admin";
	cfg->rtspUrl = "rtsp://127.0.0.1/5000";
	cfg->recDir = "c:/temp";
	cfg->mp4LocationAndPrefix ="c:/temp/tmp";

	GstRtspReceiverPtr x(new GstRtspReceiver(cfg));

	HostYuvFrm yuv(cfg->imgSz.w, cfg->imgSz.h);
	cv::Mat  bgrImg(cfg->imgSz.h, cfg->imgSz.w, CV_8UC3);
	cv::Mat  bgrImgHalf(cfg->imgSz.h / 2, cfg->imgSz.w / 2, CV_8UC3);

	x->startRcvThread();
	bool hasNewFrm;
	while (1) {
		hasNewFrm = x->getYuvFrm( &yuv );

		if (hasNewFrm) {
			yuv.hdCopyToBgr(bgrImg);
			cv::resize(bgrImg, bgrImgHalf, cv::Size(cfg->imgSz.w / 2, cfg->imgSz.h / 2));
			cv::imshow("Receiver: press esc to quit ...", bgrImgHalf);
		}
		char c = (char)cv::waitKey(1);
		if (c == 27) {
			break;
		}

		//sleep a while
		APP_SLEEP_MS(10);

	}
	x->endRcvThread();

	return 0;
}

