#ifndef __MARKET_API_H__
#define __MARKET_API_H__

#include <cstdio>
#include <windows.h>

#include "ThostFtdcMdApi.h"
#include "AssistApi.h"

/* ���������� */
class CMarketData
{
public:
    CMarketData();
    ~CMarketData();

public:
    /* ������������ */
    void UpdateMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
    /* �ȴ��������� */
    BOOL WaitMarketData(void);
    /* ��ȡ�������� */
    int  GetTickPeriod(void);
private:
    /* ������������ */
    typedef enum
    {
        E_TICK_PERIOD_MS, /* ���� */
        E_TICK_PERIOD_SEC, /* �� */
        E_TICK_PERIOD_MIN, /* �� */
        E_TICK_PERIOD_HOUR, /* ʱ */
        E_TICK_PERIOD_COUNT
    }E_TICK_PERIOD_TYPE;
    /* ���ø������� */
    void SetUpdatePeriod(E_TICK_PERIOD_TYPE type = E_TICK_PERIOD_MS);
    int m_nTickCnt; /* ���¸��� */
    int m_nTickPeriod; /* �������� */
    LONGLONG m_nLastMs; /* �ϴ�ʱ�� */
    CEvent   m_event;

public:
    TThostFtdcInstrumentIDType   m_szInstrumentID; /* ��Լ���� */
    TThostFtdcExchangeIDType     m_szExchangeID; /* ���������� */
    TThostFtdcExchangeInstIDType m_szExchangeInstID; /* ��������Լ���� */
    TThostFtdcPriceType          m_fLastPrice; /* ���¼� */
    TThostFtdcPriceType          m_fPreSettlementPrice; /* ������ */
    TThostFtdcPriceType	         m_fPreClosePrice; /* ���ռ� */
    TThostFtdcLargeVolumeType	 m_fPreOpenInterest; /* ��ֲ� */
    TThostFtdcPriceType	         m_fSettlementPrice; /* ������ */
    TThostFtdcPriceType          m_fOpenPrice;  /* �񿪼� */
    TThostFtdcPriceType          m_fClosePrice; /* ���ռ� */
    TThostFtdcPriceType          m_fHighestPrice; /* ��߼� */
    TThostFtdcPriceType          m_fLowestPrice; /* ��ͼ� */
    TThostFtdcVolumeType	     m_nVolume; /* �ɽ��� */
    TThostFtdcMoneyType	         m_fTurnover; /* �ɽ��� */
    TThostFtdcLargeVolumeType	 m_fOpenInterest; /* �ֲ��� */
    TThostFtdcPriceType          m_fUpperLimitPrice; /* ��ͣ�� */
    TThostFtdcPriceType          m_fLowerLimitPrice; /* ��ͣ�� */
    BOOL                         m_bUpperTrend; /* �Ƿ����� */
};

/* ������Ӧ�� */
class CMarketSpi : public CThostFtdcMdSpi
{
public:
    CMarketSpi(void);
    ~CMarketSpi(void);

    /* ֪ͨ�� */
public:
    /* ����֪ͨ */
    virtual void OnFrontConnected();
    /* �Ͽ�֪ͨ */
    virtual void OnFrontDisconnected(int nReason);
    /* ����֪ͨ */
    virtual void OnHeartBeatWarning(int nTimeLapse);
    /* ����֪ͨ */
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
    /* ѯ��֪ͨ */
    virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

    /* ������ */
public:
    /* ������Ӧ */
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /* ��Ӧ�� */
public:
    /* ��¼��Ӧ */
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    /* ע����Ӧ */
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    /* ����������Ӧ */
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    /* ȡ��������Ӧ */
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    /* ����ѯ����Ӧ */
    virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    /* ȡ��ѯ����Ӧ */
    virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

/* ����ִ���� */
class CMarketApi
{
    /* ������ */
public:
    /* ��¼���� */
    void Login(void);
    /* ע������ */
    void Logout(void);
    /* ����������� */
    void SubMarket(void);
    /* ȡ��������� */
    void UnSubMarket(void);
    /* ����ѯ�۲��� */
    void SubQuote(void);
    /* ȡ��ѯ�۲��� */
    void UnSubQuote(void);

public:
    typedef enum
    {
        E_OPER_DEFAULT,
        E_OPER_LOGIN,
        E_OPER_LOGOUT,
        E_OPER_SUB_MARKET,
        E_OPER_UNSUB_MARKET,
        E_OPER_SUB_QUOTE,
        E_OPER_UNSUB_QUOTE,
        E_OPER_TYPE_COUNT
    }E_OPER_TYPE;
private:
    static const char *oper_name[E_OPER_TYPE_COUNT];
    static int oper_count;

public:
    CThostFtdcMdApi  *m_api; /* ִ����ʵ�� */
    CThostFtdcMdSpi  *m_spi; /* ��Ӧ��ʵ�� */
    CConfigInfo       m_cfg; /* ������Ϣ */
    CMarketData       m_data; /* �������� */
    BOOL m_bConnect;
    BOOL m_bLogin;
    BOOL m_bHaveCli;

public:
    /* ���� */
    void Start(void);
    /* ֹͣ */
    void Stop(void);
    /* ���� */
    void Operation(void);

public:
    BOOL IsRunning(void);
private:
    BOOL  m_bRunning;

private:
    static DWORD WINAPI ThreadProc(LPVOID lpParam);
    HANDLE  m_hThread;

public:
    static CMarketApi *CreateInstance(void);
    static void ReleaseInstance(void);
    static CMarketApi *GetInstance(void);
private:
    CMarketApi(void);
    ~CMarketApi(void);
    static CMarketApi *m_pInstance;
    class CSelfRelease
    {
    public:
        CSelfRelease();
        ~CSelfRelease();
    };
    static CSelfRelease  m_self;
};

#endif /* __MARKET_API_H__ */