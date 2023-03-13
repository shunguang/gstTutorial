#include "HostYuvFrm.h"

using namespace std;
using namespace app;

HostYuvFrm::HostYuvFrm(const int w, const int h, const uint64_t fn)
	: fn_(fn)
	, w_(w)
	, h_(h)
	, sz_(3 * (w * h / 2))
	, isKeyFrm_(false)
	, isAllocatedBuf_(true)
	, buf_(NULL)
{
	creatBuf();
}

HostYuvFrm::HostYuvFrm(const cv::Mat& bgr, const uint64_t fn, const uint64_t ts)
	: fn_(fn)
	, w_(bgr.cols)
	, h_(bgr.rows)
	, isKeyFrm_(false)
	, isAllocatedBuf_(true)
	, buf_(NULL)
{
	sz_ = 3 * (w_ * h_) / 2;
	creatBuf();
	hdCopyFromBgr(bgr, fn);
}


//soft copy 
HostYuvFrm::HostYuvFrm(const int w, const int h, uint8_t* buf, uint32_t bufSz, const uint64_t fn, const uint64_t ts)
	: fn_(fn)
	, w_(w)
	, h_(h)
	, sz_(bufSz)
	, buf_(buf)
	, isKeyFrm_(false)
	, isAllocatedBuf_(false)
{
#if DEBUG_
	appAssert(bufSz == (w * h * 3) / 2, "HostYuvFrm::HostYuvFrm(): size does not match!");
#endif
}

HostYuvFrm::HostYuvFrm(const HostYuvFrm& x)
	: fn_(x.fn_)
	, w_(x.w_)
	, h_(x.h_)
	, sz_(x.sz_)
	, isKeyFrm_(x.isKeyFrm_)
	, isAllocatedBuf_(true)
	, buf_(NULL)
{
	//dumpLog("HostYuvFrm::HostYuvFrm(const HostYuvFrm &x) called, x.fn=%d",x.fn_);
	creatBuf();
	memcpy(buf_, x.buf_, x.sz_);
}

HostYuvFrm::~HostYuvFrm()
{
	deleteBuf();
}

HostYuvFrm& HostYuvFrm::operator = (const HostYuvFrm& x)
{
	if (&x != this) { //check for self assignment
		if (sz_ != x.sz_) {
			deleteBuf();
			sz_ = x.sz_;
			creatBuf();
		}

		fn_ = x.fn_;
		w_ = x.w_;
		h_ = x.h_;
		isKeyFrm_ = x.isKeyFrm_;
		memcpy(buf_, x.buf_, x.sz_);
	}

	return *this; // enables x=y=z;
}


void HostYuvFrm::setToRand()
{
	for(uint32_t i=0; i<sz_; i++)
		buf_[i] = rand() % 255;
}

bool HostYuvFrm::readFromYuvBinaryFile(FILE* fid, const uint64_t fn)
{
	if (!feof(fid)) {
		fread(buf_, 1, sz_, fid);
		fn_ = fn;
		return true;
	}
	else {
		return false;
	}
}

bool HostYuvFrm::readFromImgFile(const std::string &imgFilePath, const uint64_t fn)
{
	cv::Mat bgr(h_, w_, CV_8UC3);
	cv::Mat x = cv::imread(imgFilePath, cv::IMREAD_COLOR);
	if (x.cols > 0 && x.rows > 0) {
		cv::resize(x, bgr, cv::Size(w_, h_));
		cv::Mat yuv420 = cv::Mat(h_ * 3 / 2, w_, CV_8UC1, buf_);  //a wrapper
		cv::cvtColor(bgr, yuv420, cv::COLOR_BGR2YUV_I420);				//convert to <_buf>
		return true;
	}
	return false;
}

void HostYuvFrm::resetSz(const int w, const int h)
{
	if (w != w_ || h != h_) {
		deleteBuf();
		w_ = w;
		h_ = h;
		sz_ = 3 * (w * h / 2);
		creatBuf();
	}
}

void HostYuvFrm::creatBuf()
{
	if (sz_ > 0) {
		isAllocatedBuf_ = true;
		buf_ = new uint8_t[sz_];

		pBuf_[0] = buf_;
		pBuf_[1] = buf_ + w_ * h_;
		pBuf_[2] = buf_ + w_ * h_ * 5 / 4;

		vSz_[0] = w_ * h_;
		vSz_[1] = w_ * h_ / 4;
		vSz_[2] = vSz_[1];

		//for debug
		//assert(sz_ == vSz_[0] + vSz_[1] + vSz_[2]);

		//dumpLog("HostYuvFrm::creatBuf() called, fn_=%lld, sz_=%ld", fn_, sz_);
		if (!buf_) {
			dumpLog("HostYuvFrm::creatBuf(): cannot allocate memory!");
			assert(0);
		}
	}
	else {
		buf_ = NULL;
		pBuf_[0] = NULL;
		pBuf_[1] = NULL;
		pBuf_[2] = NULL;

		vSz_[0] = 0;
		vSz_[1] = 0;
		vSz_[2] = 0;
	}
}

void HostYuvFrm::deleteBuf()
{
	if (isAllocatedBuf_) {
		delete[] buf_;
		buf_ = NULL;
	}
}

void HostYuvFrm::hdCopyToBgr(cv::Mat& picBGR)
{
	cv::Mat picYV12 = cv::Mat(h_ * 3 / 2, w_, CV_8UC1, buf_);
	cv::cvtColor(picYV12, picBGR, CV_YUV420p2BGR);
}

void HostYuvFrm::wrtFrmNumOnImg()
{
	cv::Mat bgr;
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale =  (h_ <= 320) ? 1 : (double)h_ / 320.0f;
	int thickness = (h_ <= 320) ? 2 : (h_ / 320);
	cv::Point pt(5, h_ - 10);

	string txt = std::to_string(fn_) + ", " + std::to_string(w_) + "x" + std::to_string(h_);

	cv::Mat picYV12 = cv::Mat(h_ * 3 / 2, w_, CV_8UC1, buf_);
	cv::cvtColor(picYV12, bgr, CV_YUV420p2BGR);

	cv::putText(bgr, txt, pt, fontface, scale, cv::Scalar(255, 255, 255), thickness);

	cv::cvtColor(bgr, picYV12, cv::COLOR_BGR2YUV_I420);

}

void HostYuvFrm::drawRandomRoiAndwrtFrmNumOnImg(int nRois)
{
	cv::Mat bgr;
	int x0, y0, w0, h0;
	cv::Mat picYV12 = cv::Mat(h_ * 3 / 2, w_, CV_8UC1, buf_);
	cv::cvtColor(picYV12, bgr, CV_YUV420p2BGR);
	for (int i = 0; i < nRois; ++i) {
		x0 = rand() % w_;
		y0 = rand() % h_;
		w0 = rand() % (w_ - x0);
		h0 = rand() % (h_ - y0);

		cv::Rect r0(x0, y0, w0, h0);
		cv::rectangle(bgr, r0, cv::Scalar(rand() % 255, rand() % 255, rand() % 255));
	}

	cv::putText(bgr, std::to_string(fn_), cv::Point(5, h_ - 100), cv::FONT_HERSHEY_COMPLEX_SMALL, 2.5, cv::Scalar(255, 255, 0), 2);
	cv::cvtColor(bgr, picYV12, cv::COLOR_BGR2YUV_I420);
}


void HostYuvFrm::hdCopyTo(HostYuvFrm* dst) const
{
	*dst = *this;
}

void HostYuvFrm::hdCopyTo( uint8_t* buf, const uint32_t bufSz, uint64_t &fn) const
{
	if (bufSz != sz_) {
		printf("HostYuvFrm::hdCopyTo(): buf sz doest match, data loss!\n");
		return;
	}
	memcpy(buf, buf_, bufSz);
	fn = fn_;
}

void HostYuvFrm::hdCopyToLargerDst(HostYuvFrm* dst) const
{
	int wSrc = w_, hSrc = h_;
	int wDst = dst->w_;

	dst->fn_ = fn_;

	for (int ch = 0; ch < 3; ++ch) {
		if (ch > 0) {
			wSrc = w_ / 2;
			hSrc = h_ / 2;
			wDst = dst->w_ / 2;
		}

		uint8_t* pDst = dst->pBuf_[ch];
		uint8_t* pSrc = pBuf_[ch];
		for (int y = 0; y < hSrc; ++y, pDst += wDst, pSrc += wSrc) {
			memcpy(pDst, pSrc, wSrc);
		}
	}
}

uint32_t HostYuvFrm::hdCopyFrom(const uint8_t* buf, const uint32_t bufSz, const uint64_t fn)
{
	if (bufSz != sz_) {
		printf("HostYuvFrm::copyFrom(): buf sz doest match, data loss!\n");
		return 0;
	}
	memcpy(buf_, buf, bufSz);
	fn_ = fn;
	return bufSz;
}

void HostYuvFrm::hdCopyFromBgr(const cv::Mat& bgr, const uint64_t fn)
{
	appAssert(w_ == bgr.cols && h_ == bgr.rows, "HostYuvFrm::hdCopyFromBgr(): size dose not match!");

#if 1
	//not tested yet
	cv::Mat yuv420 = cv::Mat(h_ * 3 / 2, w_, CV_8UC1, buf_);
	cv::cvtColor(bgr, yuv420, cv::COLOR_BGR2YUV_I420);
#else
	//old workable, but too expensive
	cv::Mat yuv420;
	cv::cvtColor(bgr, yuv420, cv::COLOR_BGR2YUV_I420);
	hdCopyFrom(yuv420.data, sz_, fn);
#endif
	fn_ = fn;
}


void HostYuvFrm::dump(const std::string& folder, const std::string& tag, int roiW, int roiH, int L)
{
	char buf[1024];
	snprintf(buf, 1024, "%s/%s-fn-%06llu_L%d.png", folder.c_str(), tag.c_str(), fn_, L);
	cv::Mat bgr;
	hdCopyToBgr(bgr);
	if (roiW == 0 || roiH == 0) {
		cv::imwrite(buf, bgr);
	}
	else {
		cv::Rect roi(0, 0, roiW, roiH);
		cv::Mat I = bgr(roi);
		cv::imwrite(buf, I);
	}
}
