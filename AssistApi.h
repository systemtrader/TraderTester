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
    TThostFtdcOrderRefType	    m_szOrderRef; /* �������� */

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

/* �����׶� */
typedef enum
{
    E_TRIGGER_STAGE_REQ,
    E_TRIGGER_STAGE_RSP,
    E_TRIGGER_STAGE_ERR,
    E_TRIGGER_STAGE_RTN,
    E_TRIGGER_STAGE_COUNT
}E_TRIGGER_STAGE_TYPE;

/* ������ */
class CTrigger
{
public:
    CTrigger(int nRequestID, char *szFuncName, double fTriggerTime, int nTriggerStage);
    CTrigger(CTrigger *trigger);
    ~CTrigger(void);

private:
    friend class CTriggerLogApi;
    friend class CTriggerList;
    int     m_nRequestID;
    char   *m_szFuncName;
    double  m_fTriggerTime;
    int     m_nTriggerStage;
};

/* �������� */
class CTriggerList
{
public:
    CTriggerList();
    ~CTriggerList();

public:
    void AddTrigger(CTrigger *trigger);
    BOOL IsComplete(void);
    void PrintLog(void);
private:
    vector<CTrigger *> m_vector;
    int   m_nRequestID;
    BOOL  m_bIsComplete;
};

/* ������־ */
class CTriggerLogApi
{
public:
    /* ���� */
    void Start(void);
    /* ֹͣ */
    void Stop(void);
    /* ���� */
    static void Trigger(int nReqeustID, char *szFuncName, int nTriggerStage);
#define TriggerEx(nRequestID, nTriggerStage)       Trigger(nRequestID, __FUNCTION__, nTriggerStage)
    /* ���� */
    void Process(void);

private:
    queue<CTrigger *>         m_triggerQueue;
    map<int, CTriggerList *>  m_trigerListMap;
    CEvent  m_event;

public:
    BOOL IsRunning(void);
private:
    BOOL  m_bRunning;

private:
    static DWORD WINAPI ThreadProc(LPVOID lpParam);
    HANDLE  m_hThread;

public:
    static CTriggerLogApi *GetInstance(void);
private:
    CTriggerLogApi(void);
    ~CTriggerLogApi(void);
    static CTriggerLogApi *m_pInstance;
    class CSelfRelease
    {
    public:
        CSelfRelease();
        ~CSelfRelease();
    };
    static CSelfRelease  m_self;
};

#ifdef _DEBUG
/* ��麯������ */
extern void CheckFuncName(const char *szFuncName);
/* ��������Ϣ */
extern BOOL CheckOnErrRtnInfo(const char *szFuncName, CThostFtdcRspInfoField *pRspInfo);
/* �����Ӧ��Ϣ */
extern BOOL CheckOnRspInfo(const char *szFuncName, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
#define CheckFuncNameEx()                CheckFuncName((const char *)__FUNCTION__)
#define CheckOnErrRtnInfoEx(x1)          CheckOnErrRtnInfo((const char *)__FUNCTION__, x1)
#define CheckOnRspInfoEx(x1, x2, x3)     CheckOnRspInfo((const char *)__FUNCTION__, x1, x2, x3)
#else
/* ��麯������ */
extern void CheckFuncName(void);
/* ��������Ϣ */
extern BOOL CheckOnErrRtnInfo(CThostFtdcRspInfoField *pRspInfo);
/* �����Ӧ��Ϣ */
extern BOOL CheckOnRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
#define CheckFuncNameEx()                CheckFuncName()
#define CheckOnErrRtnInfoEx(x1)          CheckOnErrRtnInfo(x1)
#define CheckOnRspInfoEx(x1, x2, x3)     CheckOnRspInfo(x1, x2, x3)
#endif /* _DEBUG */

#endif /* __ASSIST_API_H__ */