#ifndef __ASSIST_API_H__
#define __ASSIST_API_H__

#include <cstdio>
#include <windows.h>
#include <vector>
#include <queue>
#include <map>
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

using namespace std;

/* ������Ϣ�� */
class CConfigInfo
{
public:
    CConfigInfo(void);
    ~CConfigInfo(void);

    /* ������Ϣ */
public:
    char                        m_szTraderAddr[256]; /* ����ǰ�õ�ַ */
    char                        m_szMarketAddr[256]; /* ����ǰ�õ�ַ */

    /* ��֤��Ϣ */
public:
    TThostFtdcBrokerIDType      m_szBrokerID; /* ���͹�˾���� */
    TThostFtdcInvestorIDType    m_szInvestorID; /* Ͷ���ߴ��� */
    TThostFtdcUserIDType        m_szUserID; /* �û����� */
    TThostFtdcPasswordType      m_szPassword; /* �û����� */
    TThostFtdcProductInfoType   m_szUserProductInfo; /* �û���Ʒ��Ϣ */
    TThostFtdcAuthCodeType      m_szAuthCode; /* ��֤�� */

    /* �Ự��Ϣ */
public:
    TThostFtdcFrontIDType       m_nFrontID; /* ǰ�ñ�� */
    TThostFtdcSessionIDType     m_nSessionID; /* �Ự��� */
    int                         m_nRequestID; /* ������ */

    /* ������Ϣ */
public:
    TThostFtdcOrderRefType      m_szMaxOrderRef;/* ��󱨵����� */
    TThostFtdcOrderRefType	    m_szOrderRef; /* �ڻ��������� */
    TThostFtdcOrderRefType      m_szExecOrderRef; /* ��Ȩ�������� */
    TThostFtdcOrderRefType      m_szQuoteRef; /* ??? */

    /* ��Լ��Ϣ */
public:
    TThostFtdcInstrumentIDType   m_szInstrumentID; /* ��Լ���� */
    TThostFtdcInstrumentNameType m_szInstrumentName; /* ��Լ���� */
    TThostFtdcExchangeIDType     m_szExchangeID; /* ���������� */
    TThostFtdcExchangeInstIDType m_szExchangeInstID; /* ��������Լ���� */
};

/* ʱ���� */
class CClock
{
public:
    CClock();
    ~CClock();

public:
    double GetClockTime();

private:
    LONGLONG         counter;
    static LONGLONG  freq;
};

/* ������ */
class CLock
{
public:
    CLock();
    ~CLock();

public:
    BOOL Lock(void);
    BOOL UnLock(void);

private:
    HANDLE m_hLock;
};

/* �¼��� */
class CEvent
{
public:
    CEvent();
    ~CEvent();

public:
    BOOL Wait(void);
    BOOL Wake(void);

private:
    HANDLE m_hEvent;
};

/* ������ӡ�� */
class CPrintParams
{
public:
    static void PrintFunction(const char *szFuncName);
#define PrintFunctionEx()         PrintFunction((const char *)__FUNCTION__)
    static void PrintParams(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcForQuoteRspField *pForQuoteRsp, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo);
    static void PrintParams(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo = NULL);
    static void PrintParams(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo = NULL);
private:
    static CPrintParams *GetInstance(void);
    CPrintParams(void);
    ~CPrintParams(void);
    static CPrintParams *m_pInstance;
    BOOL   m_bPrintParams;
};

/* ��ӡ�� */
class CPrintApi
{
public:
    static void AppPrint(const char *format, ...);
    static void DbgPrint(const char *format, ...);
    static void LogPrint(const char *format, ...);

private:
    static CPrintApi *GetInstance(void);
    CPrintApi(void);
    ~CPrintApi(void);
    static CPrintApi *m_pInstance;

private:
    FILE   *m_hDbgFile;
    BOOL    m_bDbgPrint;
    BOOL    m_bDbgCliPrint;
    FILE   *m_hLogFile;
    BOOL    m_bLogPrint;
    CLock   m_lock;
};

/* ʱ������ */
typedef struct  
{
    SYSTEMTIME  curTm;  /* ��ǰʱ�� */
    time_t      curUs;  /* ��ǰ΢���� */
    time_t      allUs;  /* ��ǰ������ */
}ClockTime;

/* ��ȡʱ��ʱ��-��΢����� */
void GetClockTime(ClockTime *clock);

/* ����Ŀ¼ */
BOOL CreateDir(const char *szDir);

/* �ַ���ת������ */
int ConvertTStrToCStr(CHAR *dst,  int dst_size, const TCHAR *src);
int ConvertCStrToTStr(TCHAR *dst, int dst_size, const CHAR  *src);
int ConvertTStrToWStr(WCHAR *dst, int dst_size, const TCHAR *src);
int ConvertWStrToTStr(TCHAR *dst, int dst_size, const WCHAR *src);
int ConvertWStrToCStr(CHAR *dst,  int dst_size, const WCHAR *src);
int ConvertCStrToWStr(WCHAR *dst, int dst_size, const CHAR  *src);

/* �����Ӧ������� */
BOOL CheckRspInfo(CThostFtdcRspInfoField *pRspInfo);

#endif /* __ASSIST_API_H__ */