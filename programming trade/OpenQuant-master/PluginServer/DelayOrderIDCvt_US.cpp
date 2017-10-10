#include "StdAfx.h"
#include "DelayOrderIDCvt_US.h"

//////////////////////////////////////////////////////////////////////////
//
#define TimeID_LoopFind			8 
#define TimeInterval_LoopFind	100    // 每100ms扫描一次 
#define DelayMax_LoopFind       (6000/TimeInterval_LoopFind)   //最多等6秒
//////////////////////////////////////////////////////////////////////////
//
CDelayOrderIDCvt_US::CDelayOrderIDCvt_US(void)
{ 
	m_pNotify = NULL;
	m_pTradeObj = NULL; 
	m_nTimerID_LoopFind = 0;
}

CDelayOrderIDCvt_US::~CDelayOrderIDCvt_US(void)
{
	Uninit();
}

void CDelayOrderIDCvt_US::Init( ITrade_US* pTradeObj, IOrderIDCvtNotify_US* pNotify)
{
	m_pTradeObj = pTradeObj;
	m_pNotify = pNotify;
}

void CDelayOrderIDCvt_US::Uninit()
{ 
	if (!m_pTradeObj)
		return; 

	m_pNotify = NULL;
	m_pTradeObj = NULL;
	if (m_TimerWnd.GetSafeHWnd())
	{ 
		m_TimerWnd.Destroy(); 
	}
	VT_DelayCvtInfo::iterator it = m_vtReq.begin();
	while (it != m_vtReq.end())
	{
		SAFE_DELETE(*it);
		++it;
	}
	m_vtReq.clear(); 
}

INT64 CDelayOrderIDCvt_US::FindSvrOrderID( Trade_Env eEnv, INT64 nLocalID, bool bDelayFindAndNotify)
{ 
	CHECK_RET(nLocalID != 0, 0);

	INT64 nOrderID = DoFindSvrOrderID(eEnv, nLocalID);
	if (0 == nOrderID && bDelayFindAndNotify) 
	{ 
		DoAddNewDelayReq(eEnv, nLocalID);
		StartTime_LoopFind(); 
	}
	return nOrderID;
}

void CDelayOrderIDCvt_US::DoAddNewDelayReq(Trade_Env eEnv, INT64 nLocalID)
{
	LP_DelayCvtInfo pInfo = DoFindReqObj(eEnv, nLocalID);
	if (pInfo)
		return; 

	pInfo = new DelayCvtInfo(eEnv, nLocalID);
	CHECK_RET(pInfo, NORET);

	m_vtReq.push_back(pInfo);
}

INT64 CDelayOrderIDCvt_US::DoFindSvrOrderID(Trade_Env eEnv, INT64 nLocalID)
{
	CHECK_RET(m_pTradeObj, 0);

	CHECK_RET(eEnv == Trade_Env_Real, 0); 
	return m_pTradeObj->FindOrderSvrID(nLocalID);
}

CDelayOrderIDCvt_US::LP_DelayCvtInfo CDelayOrderIDCvt_US::DoFindReqObj( Trade_Env eEnv, INT64 nLocalID, VT_DelayCvtInfo::iterator* pIter)
{
	VT_DelayCvtInfo::iterator it =m_vtReq.begin();
	while (it != m_vtReq.end())
	{
		LP_DelayCvtInfo pInfo = *it;
		if (pInfo && pInfo->eEnv == eEnv && pInfo->nLocalID == nLocalID)
		{ 
			if (pIter)
			{
				*pIter = it;
			}
			return pInfo;
		}
		++it;
	}
	return NULL;
}

void  CDelayOrderIDCvt_US::StartTime_LoopFind()
{
	if (!m_TimerWnd.GetSafeHWnd())
	{ 
		m_TimerWnd.Create(); 
		m_TimerWnd.SetEventInterface(this);
	}
	if (0 == m_nTimerID_LoopFind)
	{
		m_nTimerID_LoopFind = TimeID_LoopFind; 
		m_TimerWnd.StartMillionTimer(TimeInterval_LoopFind, m_nTimerID_LoopFind); 
	}
}

void  CDelayOrderIDCvt_US::KillTime_LoopFind()
{
	if (m_nTimerID_LoopFind != 0)
	{ 
		m_TimerWnd.StopTimer(m_nTimerID_LoopFind);
		m_nTimerID_LoopFind = 0;
	}
}

void CDelayOrderIDCvt_US::OnTimeEvent( UINT nEventID )
{
	if (nEventID != 0 && nEventID == m_nTimerID_LoopFind)
	{ 
		DoCheckDelayReq(); 
	}
} 

void  CDelayOrderIDCvt_US::DoCheckDelayReq()
{
	VT_DelayCvtInfo::iterator it = m_vtReq.begin();
	while (it != m_vtReq.end())
	{
		LP_DelayCvtInfo pInfo = *it;
		if (pInfo)
		{ 
			INT64 nSvrID = DoFindSvrOrderID(pInfo->eEnv, pInfo->nLocalID);
			if (0 != nSvrID || pInfo->nDelayCount++ > DelayMax_LoopFind)
			{ 
				if (m_pNotify)
				{ 
					int nResult = (nSvrID != 0)? 0: -1; 
					m_pNotify->OnCvtOrderID_Local2Svr(nResult, pInfo->eEnv, pInfo->nLocalID, nSvrID);
				}
				it = m_vtReq.erase(it); 
				SAFE_DELETE(pInfo);
				continue; 
			}
		}
		++it;
	}
	if (m_vtReq.size() == 0)
	{ 
		KillTime_LoopFind();
	}
}

