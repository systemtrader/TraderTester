#include "MarketApi.h"

#pragma warning(disable:4996)

CMarketData::CMarketData()
{
    m_nTickCnt = 0;
    m_nTickPeriod = 0;
    m_nLastMs = 0;
    m_bUpperTrend = FALSE;
    memset(&m_szInstrumentID, 0, sizeof(m_szInstrumentID));
    memset(&m_szExchangeID, 0, sizeof(m_szExchangeID));
    memset(&m_szExchangeInstID, 0, sizeof(m_szExchangeInstID));
    memset(&m_fLastPrice, 0, sizeof(m_fLastPrice));
    memset(&m_fPreSettlementPrice, 0, sizeof(m_fPreSettlementPrice));
    memset(&m_fPreClosePrice, 0, sizeof(m_fPreClosePrice));
    memset(&m_fPreOpenInterest, 0, sizeof(m_fPreOpenInterest));
    memset(&m_fSettlementPrice, 0, sizeof(m_fSettlementPrice));
    memset(&m_fOpenPrice, 0, sizeof(m_fOpenPrice));
    memset(&m_fClosePrice, 0, sizeof(m_fClosePrice));
    memset(&m_fHighestPrice, 0, sizeof(m_fHighestPrice));
    memset(&m_fLowestPrice, 0, sizeof(m_fLowestPrice));
    memset(&m_nVolume, 0, sizeof(m_nVolume));
    memset(&m_fTurnover, 0, sizeof(m_fTurnover));
    memset(&m_fOpenInterest, 0, sizeof(m_fOpenInterest));
    memset(&m_fUpperLimitPrice, 0, sizeof(m_fUpperLimitPrice));
    memset(&m_fLowerLimitPrice, 0, sizeof(m_fLowerLimitPrice));
    SetUpdatePeriod(E_TICK_PERIOD_MS);
}

CMarketData::~CMarketData()
{
    
}

/* ������������ */
void CMarketData::UpdateMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    LONGLONG cur_hour = 0;
    LONGLONG cur_min = 0;
    LONGLONG cur_sec = 0;
    LONGLONG cur_ms = 0;
    LONGLONG diff_ms = 0; /* ʱ���� */
    char *update_tm = NULL;
    int   update_ms = 0;
    BOOL bUpdate = FALSE;

    update_tm = pDepthMarketData->UpdateTime;
    update_ms = pDepthMarketData->UpdateMillisec;
    sscanf(update_tm, "%02d:%02d:%02d", &cur_hour, &cur_min, &cur_sec);
    cur_ms = cur_hour*1000*60*60 + cur_min*1000*60 + cur_sec*1000 + update_ms;

    /* ����������� */
    if (m_nTickCnt==0)
    {
        bUpdate = TRUE;
    }
    else
    {
        diff_ms = cur_ms - m_nLastMs;
        if (0 == diff_ms % m_nTickPeriod)
        {
            bUpdate = TRUE;
        }
    }

    /* ����������Ϣ */
    if (bUpdate)
    {
        if (m_fPreClosePrice<pDepthMarketData->LastPrice)
        {
            m_bUpperTrend = TRUE;
        }
        else
        {
            m_bUpperTrend = FALSE;
        }
        strcpy(m_szInstrumentID, pDepthMarketData->InstrumentID);
        strcpy(m_szExchangeID, pDepthMarketData->ExchangeID);
        strcpy(m_szExchangeInstID, pDepthMarketData->ExchangeInstID);
        m_fLastPrice = pDepthMarketData->LastPrice;
        m_fPreSettlementPrice = pDepthMarketData->PreSettlementPrice;
        m_fPreClosePrice = pDepthMarketData->PreClosePrice;
        m_fPreOpenInterest = pDepthMarketData->PreOpenInterest;
        m_fSettlementPrice = pDepthMarketData->SettlementPrice;
        m_fOpenPrice = pDepthMarketData->OpenPrice;
        m_fClosePrice = pDepthMarketData->ClosePrice;
        m_fHighestPrice = pDepthMarketData->HighestPrice;
        m_fLowestPrice = pDepthMarketData->LowestPrice;
        m_nVolume = pDepthMarketData->Volume;
        m_fTurnover = pDepthMarketData->OpenInterest;
        m_fOpenInterest = pDepthMarketData->LowestPrice;
        m_fUpperLimitPrice = pDepthMarketData->UpperLimitPrice;
        m_fLowerLimitPrice = pDepthMarketData->LowerLimitPrice;
        m_nLastMs = cur_ms;
        m_nTickCnt++;
        if (0==m_nTickCnt%20)
        {
            m_event.Wake();
        }
    }
}

/* �ȴ��������� */
BOOL  CMarketData::WaitMarketData(void)
{
    return m_event.Wait();
}

/* ��ȡ�������� */
int  CMarketData::GetTickPeriod(void)
{
    return m_nTickPeriod;
}

/* ���ø������� */
void CMarketData::SetUpdatePeriod(E_TICK_PERIOD_TYPE type)
{
    switch (type)
    {
    case E_TICK_PERIOD_MS:
        m_nTickPeriod = 1;
        break;
    case E_TICK_PERIOD_SEC:
        m_nTickPeriod = 1000;
        break;
    case E_TICK_PERIOD_MIN:
        m_nTickPeriod = 1000*60;
        break;
    case E_TICK_PERIOD_HOUR:
        m_nTickPeriod = 1000*60*60;
        break;
    default:
        m_nTickPeriod = 1;
        break;
    }
}

CMarketSpi::CMarketSpi(void)
{
}

CMarketSpi::~CMarketSpi(void)
{
}

/* ����֪ͨ */
void CMarketSpi::OnFrontConnected()
{
    CPrintParams::PrintFunctionEx();

    /* ��������״̬ */
    CMarketApi *api = CMarketApi::GetInstance();
    api->m_bConnect = TRUE;
    if (!api->m_bHaveCli)
    {
        api->Login();
    }
}

/* �Ͽ�֪ͨ */
void CMarketSpi::OnFrontDisconnected(int nReason)
{
    CPrintParams::PrintFunctionEx();
    CPrintApi::DbgPrint("----������nReason=0x%08x\n", nReason);

    /* ��������״̬ */
    CMarketApi *api = CMarketApi::GetInstance();
    api->m_bConnect = FALSE;
}

/* ����֪ͨ */
void CMarketSpi::OnHeartBeatWarning(int nTimeLapse)
{
    CPrintParams::PrintFunctionEx();
    CPrintApi::DbgPrint("----������nTimeLapse=%08d", nTimeLapse);
}

/* ����֪ͨ */
void CMarketSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pDepthMarketData);

    /* �����г��������� */
    CMarketApi *api = CMarketApi::GetInstance();
    api->m_data.UpdateMarketData(pDepthMarketData);
}

/* ѯ��֪ͨ */
void CMarketSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pForQuoteRsp);
}

/* ������Ӧ */
void CMarketSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ��¼��Ӧ */
void CMarketSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pRspUserLogin, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* ע�� */
        CMarketApi *api = CMarketApi::GetInstance();
        api->m_bLogin = TRUE;
        if (!api->m_bHaveCli)
        {
            api->SubMarket();
            api->SubQuote();
        }
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ע����Ӧ */
void CMarketSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pUserLogout, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* ע�� */
        CMarketApi *api = CMarketApi::GetInstance();
        api->m_bLogin = FALSE;
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ����������Ӧ */
void CMarketSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pSpecificInstrument, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ȡ��������Ӧ */
void CMarketSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pSpecificInstrument, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ����ѯ����Ӧ */
void CMarketSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pSpecificInstrument, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ȡ��ѯ����Ӧ */
void CMarketSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pSpecificInstrument, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ��¼���� */
void CMarketApi::Login(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    memcpy(&req.Password, &m_cfg.m_szPassword, sizeof(m_cfg.m_szPassword));
    m_api->ReqUserLogin(&req, m_cfg.m_nRequestID++);
}

/* ע������ */
void CMarketApi::Logout(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcUserLogoutField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    m_api->ReqUserLogout(&req, m_cfg.m_nRequestID++);
}

/* ����������� */
void CMarketApi::SubMarket(void)
{
    CPrintParams::PrintFunctionEx();
    char *ppInstrumentID[] = {"cu1601"};
    m_api->SubscribeMarketData(ppInstrumentID, 1);
}

/* ȡ��������� */
void CMarketApi::UnSubMarket(void)
{
    CPrintParams::PrintFunctionEx();
    char *ppInstrumentID[] = {"cu1601"};
    m_api->UnSubscribeMarketData(ppInstrumentID, 1);
}

/* ����ѯ�۲��� */
void CMarketApi::SubQuote(void)
{
    CPrintParams::PrintFunctionEx();
    char *ppInstrumentID[] = {"cu1601"};
    m_api->SubscribeForQuoteRsp(ppInstrumentID, 1);
}

/* ȡ��ѯ�۲��� */
void CMarketApi::UnSubQuote(void)
{
    CPrintParams::PrintFunctionEx();
    char *ppInstrumentID[] = {"cu1601"};
    m_api->UnSubscribeForQuoteRsp(ppInstrumentID, 1);
}

const char *CMarketApi::oper_name[E_OPER_TYPE_COUNT]= {
    "default",
    "Login()",
    "Logout()",
    "SubMarket()",
    "UnSubMarket()",
    "SubQuote()",
    "UnSubQuote()"
};

int CMarketApi::oper_count = 0;

/* ���� */
void CMarketApi::Start(void)
{
    if (!m_bRunning)
    {
        if (m_bHaveCli)
        {
            m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
            if (m_hThread)
            {
                m_bRunning = TRUE;
            }
        }
    }
}

/* ֹͣ */
void CMarketApi::Stop(void)
{
    if (m_bRunning)
    {
        m_bRunning = FALSE;
        if (m_hThread)
        {
            ::WaitForSingleObject(m_hThread, INFINITE);
            ::CloseHandle(m_hThread);
            m_hThread = NULL;
        }   
    }
}

void CMarketApi::Operation(void)
{
    /* �ӳ����÷�����Ϣ */
    Sleep(3000);

    if (m_bConnect)
    {
        int oper_type = 0;
        CPrintApi::AppPrint("\n\n");
        CPrintApi::AppPrint("-----------------------------------------------------------------------------\n");
        CPrintApi::AppPrint("----������ʾ��\n");
        if (m_bLogin)
        {
            for (oper_type=0; oper_type<E_OPER_TYPE_COUNT; oper_type++)
            {
                if (E_OPER_LOGIN!=oper_type)
                {
                    CPrintApi::AppPrint("    %2d. %s;\n", oper_type, oper_name[oper_type]);
                }
            }
            CPrintApi::AppPrint("----��ѡ����ţ�");

            oper_type = 0;
            scanf("%d", &oper_type);
            if (E_OPER_DEFAULT<=oper_type && E_OPER_TYPE_COUNT>oper_type && E_OPER_LOGIN!=oper_type)
            {
                CPrintApi::DbgPrint("\n");
                CPrintApi::DbgPrint("-----------------------------------------------------------------------------");
                CPrintApi::DbgPrint("----����: %s", __FUNCTION__);
                CPrintApi::DbgPrint("----��%d�β�����%2d. %s.", ++oper_count, oper_type, oper_name[oper_type]);
            }
            else
            {
                CPrintApi::DbgPrint("\n");
                CPrintApi::DbgPrint("-----------------------------------------------------------------------------");
                CPrintApi::DbgPrint("----����: %s", __FUNCTION__);
                CPrintApi::DbgPrint("----������Ŵ���");
            }
            switch (oper_type)
            {
            case E_OPER_LOGOUT:
                Logout();
                break;

            case E_OPER_SUB_MARKET:
                SubMarket();
                break;

            case E_OPER_UNSUB_MARKET:
                UnSubMarket();
                break;

            case E_OPER_SUB_QUOTE:
                SubQuote();
                break;

            case E_OPER_UNSUB_QUOTE:
                UnSubQuote();
                break;

            default:
                break;
            }
        }
        else
        {
            for (oper_type=0; oper_type<E_OPER_TYPE_COUNT; oper_type++)
            {
                switch (oper_type)
                {
                case E_OPER_LOGIN:
                    CPrintApi::AppPrint("    %2d. Login();\n", oper_type);
                    break;

                default:
                    break;
                }
            }
            CPrintApi::AppPrint("----��ѡ����ţ�");

            oper_type = 0;
            scanf("%d", &oper_type);
            if (E_OPER_LOGIN == oper_type)
            {
                CPrintApi::DbgPrint("\n");
                CPrintApi::DbgPrint("-----------------------------------------------------------------------------");
                CPrintApi::DbgPrint("----����: %s", __FUNCTION__);
                CPrintApi::DbgPrint("----��%d�β�����%2d. %s.", ++oper_count, oper_type, oper_name[oper_type]);
                Login();
            }
            else
            {
                CPrintApi::DbgPrint("\n");
                CPrintApi::DbgPrint("-----------------------------------------------------------------------------");
                CPrintApi::DbgPrint("----����: %s", __FUNCTION__);
                CPrintApi::DbgPrint("----������Ŵ���");
            }
        }
    }
}

BOOL CMarketApi::IsRunning(void)
{
    return m_bRunning;
}

DWORD CMarketApi::ThreadProc(LPVOID lpParam)
{
    CMarketApi *api = (CMarketApi *)lpParam;
    if (!api)
        return -1;

    /* ����ҵ�� */
    int  nTick = 20;
    while (api->IsRunning())
    {
        api->Operation();
    }

    return 0;
}

CMarketApi *CMarketApi::m_pInstance = NULL;

CMarketApi *CMarketApi::CreateInstance(void)
{
    if (!m_pInstance)
    {
        m_pInstance = new CMarketApi();
        //m_pInstance->m_bHaveCli = TRUE; /* �����������е���֮�ã�����ʱע������ */
    }
    return m_pInstance;
}

void CMarketApi::ReleaseInstance(void)
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

CMarketApi *CMarketApi::GetInstance(void)
{
    return m_pInstance;
}

CMarketApi::CMarketApi(void)
{
    m_bRunning = FALSE;
    m_bConnect = FALSE;
    m_bLogin = FALSE;
    m_bHaveCli = FALSE;
    m_hThread = NULL;
    char *szFlowDir = "./MarketFlow/";
    CreateDir(szFlowDir);
    m_api = CThostFtdcMdApi::CreateFtdcMdApi(szFlowDir);
    m_spi = new CMarketSpi;
    m_api->RegisterSpi(m_spi);
    m_api->RegisterFront(m_cfg.m_szMarketAddr);
    m_api->Init();
}

CMarketApi::~CMarketApi(void)
{
    /* ֹͣ���� */
    Stop();

    /* �ͷ��ڴ� */
    if (m_api)
    {
        m_api->Release();
        m_api = NULL;
    }
    if (m_spi)
    {
        delete m_spi;
        m_spi = NULL;
    }
}

CMarketApi::CSelfRelease CMarketApi::m_self;

CMarketApi::CSelfRelease::CSelfRelease()
{
}
CMarketApi::CSelfRelease::~CSelfRelease()
{
    if(CMarketApi::m_pInstance)
        delete CMarketApi::m_pInstance;
    CMarketApi::m_pInstance = NULL;
}