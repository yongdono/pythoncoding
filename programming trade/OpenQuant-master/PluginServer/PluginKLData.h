#pragma once
#include "Include/FTPluginCore.h"
#include "Include/FTPluginQuoteInterface.h"
#include "Include/FTPluginTradeInterface.h"
#include "Protocol/ProtoDataStruct_Quote.h"
#include "TimerWnd.h"
#include "MsgHandler.h"
#include "JsonCpp/json.h"

class CPluginQuoteServer;

class CPluginKLData : public CTimerWndInterface, public CMsgHandlerEventInterface
{
public:
	CPluginKLData();
	virtual ~CPluginKLData();

	void Init(CPluginQuoteServer* pQuoteServer, IFTQuoteData* pQuoteData);
	void Uninit();	
	void SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void NotifyQuoteDataUpdate(int nCmdID, INT64 nStockID, int nKLType);
	void SendAck(DWORD dwCookie, int nCSResult);

	void NotifySocketClosed(SOCKET sock);

protected:
	//CTimerWndInterface
	virtual void OnTimeEvent(UINT nEventID);

	//CMsgHandlerEventInterface
	virtual void OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam);

protected:
	struct	StockDataReq
	{
		SOCKET	sock;
		DWORD	dwReqTick;
		DWORD	dwLocalCookie;
		INT64	nStockID;
		KLData_Req req;
	};
	//tomodify 1
	typedef KLDataAckBody	QuoteAckDataBody;

	typedef std::vector<StockDataReq*>											VT_STOCK_DATA_REQ;
	typedef std::map<std::pair<INT64, int>, VT_STOCK_DATA_REQ>					MAP_STOCK_DATA_REQ;
	typedef std::map<std::pair<INT64, int>, std::map<int, QuoteAckDataBody>>	MAP_STOCK_CACHE_DATA;
	typedef std::map<std::pair<INT64, int>, DWORD>								MAP_CACHE_TO_DESTROY;

protected:
	void ClearQuoteDataCache();
	void HandleTimeoutReq();
	void ReplyAllReadyReq();
	void ReplyStockDataReq(StockDataReq *pReq, const QuoteAckDataBody &data);
	void ReplyDataReqError(StockDataReq *pReq, int nErrCode, LPCWSTR pErrDesc);	
	void SetTimerHandleTimeout(bool bStartOrStop);
	void SetTimerClearCache(bool bStartOrStop);
	void ClearAllReqCache();

private:
	void DoClearReqInfo(SOCKET socket);

protected:
	CPluginQuoteServer* m_pQuoteServer;
	IFTQuoteData*		m_pQuoteData;
	BOOL				m_bStartTimerClearCache;
	BOOL				m_bStartTimerHandleTimeout;

	MAP_STOCK_DATA_REQ		m_mapReqInfo;		//����δ�������������󣬳���5000����δ��Ӧ����ֱ��Ӧ�����
	MAP_STOCK_CACHE_DATA	m_mapCacheData;		//�л���Ϳ��������ظ�
	MAP_CACHE_TO_DESTROY	m_mapCacheToDel;	//��������ϢΪ��ʱ��ʼ��ʱ��ʱ(500����)ɾ������

	CTimerMsgWndEx		m_TimerWnd;
	CMsgHandler			m_MsgHandler;
};