#include "GstRtspSenderCfg.h"
#include "GstRtspSender.h"

using namespace std;
using namespace app;

void createNewFrm( HostYuvFrmPtr &frm );

int ch2_testGstRtspSender(int argc, char* argv[])
{
	GstRtspSenderCfgPtr cfg(new GstRtspSenderCfg());

	cfg->isUdp = false;
	cfg->clientIp = ipConvertStr2Num("127.0.0.1");
	cfg->clientPort = 5000;
	cfg->serverIp = ipConvertStr2Num("127.0.0.1");
	cfg->serverPort = 4000;

	GstRtspSenderPtr x(new GstRtspSender(cfg));

	HostYuvFrmPtr frm(new HostYuvFrm( cfg->imgSz.w, cfg->imgSz.h));

	x->startStreamingThread();

	const int timeToSleep_ms = 30;
	
	cv::Mat  bgrImg(cfg->imgSz.h, cfg->imgSz.w, CV_8UC3);
	cv::Mat  bgrImgHalf(cfg->imgSz.h/2, cfg->imgSz.w/2, CV_8UC3);
	while (1) {
		createNewFrm( frm );
		x->addNewFrm( frm.get() );

		frm->hdCopyToBgr(bgrImg);
		cv::resize(bgrImg, bgrImgHalf, cv::Size(cfg->imgSz.w / 2, cfg->imgSz.h / 2));
		cv::imshow("Sender: press esc to quit ...", bgrImgHalf);
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
	const char* dir = std::getenv("SWU_GST_TUTORIAL_ROOT");

	const std::string imgFilePath = (dir==NULL) ? std::string("") : (std::string(dir) + "/dataset/I1920x1080.jpg");

	frm->fn_ = fn++;
	if (!frm->readFromImgFile(imgFilePath)) {
		frm->setToRand();
	}

	frm->wrtFrmNumOnImg();
}
