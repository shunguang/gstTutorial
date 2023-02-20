//----------------------------------------------------------------------------------------
// A circular <T> que support multi-thread read/write activities
// Shunguang Wu 3/13/19
//----------------------------------------------------------------------------------------
#ifndef _HOST_YUV_FRM_Q_H_
#define _HOST_YUV_FRM_Q_H_

#include "appDefs.h"
#include "appUtils.h"
#include "HostYuvFrm.h"
namespace app {
	class  HostYuvFrmQ {
	public:
		HostYuvFrmQ(const uint32_t imgW=64, const uint32_t imgH=32, const uint32_t nTotItems=10);
		~HostYuvFrmQ();

		//reset size
		void resetSize(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems);

		//reset <m_headW> and <m_headR> as zeros
		void reset();

		//HD copy <x> into q[m_headW]
		bool wrtNext(const HostYuvFrm *src);  //host interface
		bool readNext(HostYuvFrm *dst);
	protected:
		void allocQ(const uint32_t nTotItems);
		void freeQ();

	private:
		std::vector<HostYuvFrmPtr>	m_q;
		std::vector<int>	m_v;        //count the wrt (++) / read(--) activities in m_q[i]

		//-----------------------------------------------------------------------------
		//access queue
		//-----------------------------------------------------------------------------
		uint32_t		m_items;	//the predefined # of elements of the queue.
		uint32_t		m_headW;	//the index to write	
		uint32_t		m_headR;	//the index to write	

		std::mutex m_mutexRW;
		uint32_t m_w;
		uint32_t m_h;
		uint32_t m_wrtDropCnt;
	};
	typedef std::shared_ptr<HostYuvFrmQ> HostYuvFrmQPtr;
}

#endif		