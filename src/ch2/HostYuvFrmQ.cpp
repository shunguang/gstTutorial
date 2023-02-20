#include "HostYuvFrmQ.h"

using namespace std;
using namespace app;

HostYuvFrmQ::HostYuvFrmQ(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems)
	: m_v()
	, m_q()
	, m_items(0)
	, m_headW(0)
	, m_headR(0)
	, m_mutexRW()
	, m_w(imgW)
	, m_h(imgH)
	, m_wrtDropCnt(0)
{
	allocQ( nTotItems );
	cout << "HostYuvFrmQ::HostYuvFrmQ(): done!" << endl;
}

HostYuvFrmQ::~HostYuvFrmQ()
{
	freeQ();
}

void HostYuvFrmQ::resetSize(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems)
{
	m_w = imgW;
	m_h = imgH;
	freeQ();
	allocQ(nTotItems);

	//cout << "HostYuvFrmQ::resetSize(): Q allocted!" << endl;
}

void HostYuvFrmQ::reset()
{
	std::unique_lock<std::mutex> lock(m_mutexRW, std::try_to_lock);

	m_headW = 0;
	m_headR = 0;
	m_v.resize(m_items, 0);

}


void HostYuvFrmQ::freeQ()
{
	//boost::mutex::scoped_lock lock(m_mutexRW);
	std::unique_lock<std::mutex> lock(m_mutexRW, std::try_to_lock);
	m_q.clear();
	m_v.clear();
	m_headW = 0;
	m_headR = 0;
}

void HostYuvFrmQ::allocQ(const uint32_t nTotItems)
{
	//boost::mutex::scoped_lock lock(m_mutexRW);
	std::unique_lock<std::mutex> lock(m_mutexRW, std::try_to_lock);

	m_items = nTotItems;
	dumpLog("HostYuvFrmQ::allocQ m_w=%d, m_h=%d, m_items=%d", m_w, m_h, m_items);
	m_q.clear();
	for (uint32_t i = 0; i < m_items; ++i) {
		HostYuvFrmPtr tmp( new HostYuvFrm(m_w, m_h, 0) );
		m_q.push_back( tmp );
	}

	m_v.resize(m_items, 0);
	m_headW = 0;
	m_headR = 0;
	cout << "HostYuvFrmQ::allocQ done!" << endl;
}

//wrt from host
bool HostYuvFrmQ::wrtNext(const HostYuvFrm *src)
{
	bool sucWrt = false;
	{
		//boost::mutex::scoped_lock lock(m_mutexRW);
		std::unique_lock<std::mutex> lock(m_mutexRW, std::try_to_lock);
		uint32_t &idx = m_headW;
		int   &cnt = m_v[idx];
		if (cnt == 0) {
			HostYuvFrmPtr &dst = m_q[idx];
			src->hdCopyTo( dst.get() );	  //hard copy
			cnt = cnt + 1;

			//move head to the next slot
			++idx;
			if (idx >= m_items) {
				idx = 0;
			}
			sucWrt = true;
		}
	}

	if ( !sucWrt ) {
		++m_wrtDropCnt;
		if (m_wrtDropCnt > 999) {
			dumpLog("HostYuvFrmQ::wrtNext(): writen is too fast, %d frames droped", m_wrtDropCnt);
			m_wrtDropCnt = 0;
		}
	}
	return sucWrt;
}


//the consumer read  <x> from que
bool HostYuvFrmQ::readNext( HostYuvFrm *dst )
{
	bool hasData = false;
	{
		//this lock moved to readNext() for specific RawFrmXYZ Type
		//boost::mutex::scoped_lock lock(m_mutexRW);
		std::unique_lock<std::mutex> lock(m_mutexRW, std::try_to_lock);
		uint32_t &idx = m_headR;
		int   &cnt = m_v[idx];
		if (cnt > 0) {
			HostYuvFrmPtr  &src = m_q[idx];
			src->hdCopyTo(dst);		//hd copy to dst
			cnt = 0;
			hasData = true;
			//move head to the next slot
			++idx;
			if (idx >= m_items) {
				idx = 0;
			}
		}
	}
	return hasData;
}
