#include "TraderApi.h"
#include "MarketApi.h"

#pragma warning(disable:4996)

CTraderData::CTraderData()
{
    memset(&m_szTraderDesc, 0, sizeof(m_szTraderDesc));
    memset(&m_szInstrumentID, 0, sizeof(m_szInstrumentID));
    memset(&m_nFrontID, 0, sizeof(m_nFrontID));
    memset(&m_nSessionID, 0, sizeof(m_nSessionID));
    memset(&m_tTerminalTime, 0, sizeof(m_tTerminalTime));
    memset(&m_tTHostTime, 0, sizeof(m_tTHostTime));
    memset(&m_tExchangeTime, 0, sizeof(m_tExchangeTime));
    m_nUpdateCount = 0;
}

CTraderData::~CTraderData()
{

}

/* ���ñ�����Ϣ */
void CTraderData::SetOrderInfo(const char *szTraderDesc, TThostFtdcInstrumentIDType szInstrumentID, TThostFtdcFrontIDType nFrontID, TThostFtdcSessionIDType nSessionID, TThostFtdcOrderRefType szOrderRef)
{
    strcpy(m_szTraderDesc, szTraderDesc);
    strcpy(m_szInstrumentID, szInstrumentID);
    m_nFrontID = nFrontID;
    m_nSessionID = nSessionID;
    strcpy(m_szOrderRef, szOrderRef);
    memset(&m_tTerminalTime, 0, sizeof(m_tTerminalTime));
    memset(&m_tTHostTime, 0, sizeof(m_tTHostTime));
    memset(&m_tExchangeTime, 0, sizeof(m_tExchangeTime));
    m_nUpdateCount = 0;
}

/* ��������ʱ�� */
void CTraderData::UpdateReqTime(void)
{
    GetClockTime(&m_tTerminalTime);
    PrintOrderInfo();
    m_nUpdateCount++;
}

/* ���»ر�ʱ�� */
void CTraderData::UpdateRtnTime(void)
{
    if (m_nUpdateCount<=1)
    {
        GetClockTime(&m_tTHostTime);
        m_nUpdateCount = 1;
    }
    else
    {
        GetClockTime(&m_tExchangeTime);
    }
    PrintOrderInfo();
    m_nUpdateCount++;
}

/* ��ӡ������Ϣ */
void CTraderData::PrintOrderInfo(void)
{
    CPrintApi::LogPrint("\n");
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");
    CPrintApi::LogPrint("----      ��Լ���=%s", m_szInstrumentID);
    CPrintApi::LogPrint("----      ǰ�ñ��=%08u", m_nFrontID);
    CPrintApi::LogPrint("----      �Ự���=%08u", m_nSessionID);
    CPrintApi::LogPrint("----  �������ñ��=%s", m_szOrderRef);
    CPrintApi::LogPrint("----      ��������=%s", m_szTraderDesc);
    if (m_tTerminalTime.allUs)
    {
        CPrintApi::LogPrint("----  �ն˷���ʱ��=%02d:%02d:%02d,%d����-%lld΢��", 
            m_tTerminalTime.curTm.wHour, 
            m_tTerminalTime.curTm.wMinute, 
            m_tTerminalTime.curTm.wSecond, 
            m_tTerminalTime.curTm.wMilliseconds, 
            m_tTerminalTime.curUs);
    }
    if (m_tTHostTime.allUs)
    {
        CPrintApi::LogPrint("----  ƽ̨�ر�ʱ��=%02d:%02d:%02d,%d����-%lld΢��", 
            m_tTHostTime.curTm.wHour, 
            m_tTHostTime.curTm.wMinute, 
            m_tTHostTime.curTm.wSecond, 
            m_tTHostTime.curTm.wMilliseconds, 
            m_tTHostTime.curUs);
        if (m_tTerminalTime.allUs)
        {
            CPrintApi::LogPrint("----  ƽ̨�ر�ʱ��=%lld΢��", (m_tTHostTime.allUs-m_tTerminalTime.allUs));
        }
    }
    if (m_tExchangeTime.allUs)
    {
        CPrintApi::LogPrint("----�������ر�ʱ��=%02d:%02d:%02d,%d����-%lld΢��", 
            m_tExchangeTime.curTm.wHour, 
            m_tExchangeTime.curTm.wMinute, 
            m_tExchangeTime.curTm.wSecond, 
            m_tExchangeTime.curTm.wMilliseconds, 
            m_tExchangeTime.curUs);
        if (m_tTerminalTime.allUs)
        {
            CPrintApi::LogPrint("----�������ر�ʱ��=%lld΢��", (m_tExchangeTime.allUs-m_tTerminalTime.allUs));
        }
    }
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");
}

CTraderDataManager::CTraderDataManager()
{

}

CTraderDataManager::~CTraderDataManager()
{
    /* ��ն��� */
    while (!m_UnTradeList.empty())
    {
        m_UnTradeList.pop_front();
    }

    /* ���ӳ�� */
    map<string, CTraderData *>::iterator iter;
    for (iter=m_OrderMap.begin(); iter!=m_OrderMap.end(); iter++)
    {
        CTraderData *data = iter->second;
        delete data;
    }
    m_OrderMap.clear();
}

/* ���½������� */
void CTraderDataManager::UpdateTraderData(CThostFtdcInputOrderField *pInputOrder, E_UPDATE_STAGE_TYPE nStage)
{
    CTraderApi *api = CTraderApi::GetInstance();

    /* �������� */
    char szTraderDesc[128];
    switch (pInputOrder->Direction)
    {
    case THOST_FTDC_D_Buy:
        {
            /* ��ƽ��־ */
            switch (pInputOrder->CombOffsetFlag[0])
            {
            case THOST_FTDC_OF_Open:
                sprintf(szTraderDesc, "��������");
                break;
            case THOST_FTDC_OF_Close:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceClose:
                sprintf(szTraderDesc, "ǿƽ����");
                break;
            case THOST_FTDC_OF_CloseToday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_CloseYesterday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceOff:
                sprintf(szTraderDesc, "ǿ������");
                break;
            case THOST_FTDC_OF_LocalForceClose:
                sprintf(szTraderDesc, "����ǿƽ����");
                break;
            default:
                break;
            }
        }
        break;
    case THOST_FTDC_D_Sell:
        {
            /* ��ƽ��־ */
            switch (pInputOrder->CombOffsetFlag[0])
            {
            case THOST_FTDC_OF_Open:
                sprintf(szTraderDesc, "��������");
                break;
            case THOST_FTDC_OF_Close:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceClose:
                sprintf(szTraderDesc, "ǿƽ����");
                break;
            case THOST_FTDC_OF_CloseToday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_CloseYesterday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceOff:
                sprintf(szTraderDesc, "ǿ������");
                break;
            case THOST_FTDC_OF_LocalForceClose:
                sprintf(szTraderDesc, "����ǿƽ����");
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    /* ���ݲ�ͬ�׶ν��д��� */
    switch (nStage)
    {
        /* REQ��Ϊ�ն˷���ʱ�� */
    case E_UPDATE_STAGE_REQ:   /* ���󱨵� */
        {
            string strMapIndex = GetMapIndex(api->m_cfg.m_nFrontID, api->m_cfg.m_nSessionID, pInputOrder->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ� */
                CTraderData *new_data = new CTraderData;
                new_data->SetOrderInfo(szTraderDesc, pInputOrder->InstrumentID, api->m_cfg.m_nFrontID, api->m_cfg.m_nSessionID, pInputOrder->OrderRef);
                new_data->UpdateReqTime();
                m_OrderMap[strMapIndex] = new_data;
            }
            else
            {
                /* ���ҵ�-��ʾ�쳣 */
                CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
            }
        }
        break;

        /* RSP��ERR��Ϊƽ̨�ر�ʱ��  */
    case E_UPDATE_STAGE_RSP:   /* ������Ӧ */
    case E_UPDATE_STAGE_ERR:   /* �������� */
        {
            string strMapIndex = GetMapIndex(api->m_cfg.m_nFrontID, api->m_cfg.m_nSessionID, pInputOrder->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ�-��ʾ�쳣 */
                CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
            }
            else
            {
                /* ���ҵ� */
                CTraderData *old_data = iter->second;
                old_data->UpdateRtnTime();
            }
        }
        break;

    default:
        {
            /* δ֪-��ʾ�쳣 */
            CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
        }
        break;
    }
}

/* ���½������� */
void CTraderDataManager::UpdateTraderData(CThostFtdcInputOrderActionField *pInputAction, E_UPDATE_STAGE_TYPE nStage)
{
    /* �������� */
    char szTraderDesc[128];
    switch (pInputAction->ActionFlag)
    {
    case THOST_FTDC_AF_Delete:
        sprintf(szTraderDesc, "ɾ������");
        break;
    case THOST_FTDC_AF_Modify:
        sprintf(szTraderDesc, "�޸Ĳ���");
        break;
    default:
        break;
    }

    /* ���ݲ�ͬ�׶ν��д��� */
    switch (nStage)
    {
        /* REQ��Ϊ�ն˷���ʱ�� */
    case E_UPDATE_STAGE_REQ:   /* ������� */
        {
            string strMapIndex = GetMapIndex(pInputAction->FrontID, pInputAction->SessionID, pInputAction->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ�-��ʾ�쳣 */
                CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
            }
            else
            {
                /* ���ҵ� */
                CTraderData *old_data = iter->second;
                old_data->SetOrderInfo(szTraderDesc, pInputAction->InstrumentID, pInputAction->FrontID, pInputAction->SessionID, pInputAction->OrderRef);
                old_data->UpdateReqTime();
            }
        }
        break;

        /* RSP��Ϊƽ̨�ر�ʱ��  */
    case E_UPDATE_STAGE_RSP:   /* ������Ӧ */
        {
            string strMapIndex = GetMapIndex(pInputAction->FrontID, pInputAction->SessionID, pInputAction->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ�-��ʾ�쳣 */
                CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
            }
            else
            {
                /* ���ҵ� */
                CTraderData *old_data = iter->second;
                old_data->UpdateRtnTime();
            }
        }
        break;

    default:
        {
            /* δ֪-��ʾ�쳣 */
            CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
        }
        break;
    }
}

/* ���½������� */
void CTraderDataManager::UpdateTraderData(CThostFtdcOrderActionField *pInputOrder, E_UPDATE_STAGE_TYPE nStage)
{
    /* ���ݲ�ͬ�׶ν��д��� */
    switch (nStage)
    {
        /* ERR��Ϊƽ̨�ر�ʱ��  */
    case E_UPDATE_STAGE_ERR:   /* �������� */
        {
            string strMapIndex = GetMapIndex(pInputOrder->FrontID, pInputOrder->SessionID, pInputOrder->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ�-��ʾ�쳣 */
                CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
            }
            else
            {
                /* ���ҵ� */
                CTraderData *old_data = iter->second;
                old_data->UpdateRtnTime();
            }
        }
        break;

    default:
        {
            /* δ֪-��ʾ�쳣 */
            CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
        }
        break;
    }
}

/* ���½������� */
void CTraderDataManager::UpdateTraderData(CThostFtdcOrderField *pOrder, E_UPDATE_STAGE_TYPE nStage)
{
    /* �������� */
    char szTraderDesc[128];
    switch (pOrder->Direction)
    {
    case THOST_FTDC_D_Buy:
        {
            /* ��ƽ��־ */
            switch (pOrder->CombOffsetFlag[0])
            {
            case THOST_FTDC_OF_Open:
                sprintf(szTraderDesc, "��������");
                break;
            case THOST_FTDC_OF_Close:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceClose:
                sprintf(szTraderDesc, "ǿƽ����");
                break;
            case THOST_FTDC_OF_CloseToday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_CloseYesterday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceOff:
                sprintf(szTraderDesc, "ǿ������");
                break;
            case THOST_FTDC_OF_LocalForceClose:
                sprintf(szTraderDesc, "����ǿƽ����");
                break;
            default:
                break;
            }
        }
        break;
    case THOST_FTDC_D_Sell:
        {
            /* ��ƽ��־ */
            switch (pOrder->CombOffsetFlag[0])
            {
            case THOST_FTDC_OF_Open:
                sprintf(szTraderDesc, "��������");
                break;
            case THOST_FTDC_OF_Close:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceClose:
                sprintf(szTraderDesc, "ǿƽ����");
                break;
            case THOST_FTDC_OF_CloseToday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_CloseYesterday:
                sprintf(szTraderDesc, "ƽ������");
                break;
            case THOST_FTDC_OF_ForceOff:
                sprintf(szTraderDesc, "ǿ������");
                break;
            case THOST_FTDC_OF_LocalForceClose:
                sprintf(szTraderDesc, "����ǿƽ����");
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    /* ���ݲ�ͬ�׶ν��д��� */
    switch (nStage)
    {
        /* RTN��Ϊƽ̨�ر�ʱ��ͽ������ر�ʱ��  */
    case E_UPDATE_STAGE_RTN:   /* �����ر� */
        {
            string strMapIndex = GetMapIndex(pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
            map<string, CTraderData *>::iterator iter;
            iter = m_OrderMap.find(strMapIndex);
            if (iter==m_OrderMap.end())
            {
                /* δ�ҵ� */
                CTraderData *new_data = new CTraderData;
                new_data->SetOrderInfo(szTraderDesc, pOrder->InstrumentID, pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
                new_data->UpdateRtnTime();
                m_OrderMap[strMapIndex] = new_data;

                /* �����������ί��״̬�����δ�ɽ��������� */
                switch (pOrder->OrderStatus)
                {
                case THOST_FTDC_OST_PartTradedQueueing:
                case THOST_FTDC_OST_NoTradeQueueing:
                    {
                        list<CTraderData *>::iterator iter;
                        for (iter=m_UnTradeList.begin(); iter!=m_UnTradeList.end(); iter++)
                        {
                            CTraderData *cur_data = *iter;
                            if (0 == strcmp(cur_data->m_szInstrumentID, new_data->m_szInstrumentID) &&
                                cur_data->m_nFrontID == new_data->m_nFrontID &&
                                cur_data->m_nSessionID == new_data->m_nSessionID &&
                                0 == strcmp(cur_data->m_szOrderRef, new_data->m_szOrderRef))
                            {
                                break;
                            }
                        }
                        if (iter==m_UnTradeList.end())
                        {
                            m_UnTradeList.push_back(new_data);
                        }
                    }
                    break;
                case THOST_FTDC_OST_Canceled:
                case THOST_FTDC_OST_AllTraded:
                    {
                        list<CTraderData *>::iterator iter;
                        for (iter=m_UnTradeList.begin(); iter!=m_UnTradeList.end(); iter++)
                        {
                            CTraderData *cur_data = *iter;
                            if (0 == strcmp(cur_data->m_szInstrumentID, new_data->m_szInstrumentID) &&
                                cur_data->m_nFrontID == new_data->m_nFrontID &&
                                cur_data->m_nSessionID == new_data->m_nSessionID &&
                                0 == strcmp(cur_data->m_szOrderRef, new_data->m_szOrderRef))
                            {
                                break;
                            }
                        }
                        if (iter!=m_UnTradeList.end())
                        {
                            m_UnTradeList.erase(iter);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            else
            {
                /* ���ҵ� */
                CTraderData *old_data = iter->second;
                old_data->UpdateRtnTime();

                /* �����������ί��״̬�����δ�ɽ��������� */
                switch (pOrder->OrderStatus)
                {
                case THOST_FTDC_OST_PartTradedQueueing:
                case THOST_FTDC_OST_NoTradeQueueing:
                    {
                        list<CTraderData *>::iterator iter;
                        for (iter=m_UnTradeList.begin(); iter!=m_UnTradeList.end(); iter++)
                        {
                            CTraderData *cur_data = *iter;
                            if (0 == strcmp(cur_data->m_szInstrumentID, old_data->m_szInstrumentID) &&
                                cur_data->m_nFrontID == old_data->m_nFrontID &&
                                cur_data->m_nSessionID == old_data->m_nSessionID &&
                                0 == strcmp(cur_data->m_szOrderRef, old_data->m_szOrderRef))
                            {
                                break;
                            }
                        }
                        if (iter==m_UnTradeList.end())
                        {
                            m_UnTradeList.push_back(old_data);
                        }
                    }
                    break;
                case THOST_FTDC_OST_Canceled:
                case THOST_FTDC_OST_AllTraded:
                    {
                        list<CTraderData *>::iterator iter;
                        for (iter=m_UnTradeList.begin(); iter!=m_UnTradeList.end(); iter++)
                        {
                            CTraderData *cur_data = *iter;
                            if (0 == strcmp(cur_data->m_szInstrumentID, old_data->m_szInstrumentID) &&
                                cur_data->m_nFrontID == old_data->m_nFrontID &&
                                cur_data->m_nSessionID == old_data->m_nSessionID &&
                                0 == strcmp(cur_data->m_szOrderRef, old_data->m_szOrderRef))
                            {
                                break;
                            }
                        }
                        if (iter!=m_UnTradeList.end())
                        {
                            m_UnTradeList.erase(iter);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        break;

    default:
        {
            /* δ֪-��ʾ�쳣 */
            CPrintApi::LogPrint("----�쳣λ�ã��ļ�=[%s], ����=[%s], �к�=[%d]", __FILE__, __FUNCTION__, __LINE__);
        }
        break;
    }
}

/* ���½������� */
void CTraderDataManager::UpdateTraderData(CThostFtdcTradeField *pTrader, E_UPDATE_STAGE_TYPE nStage)
{ 
}

/* ����������ʶ */
string CTraderDataManager::GetMapIndex(int nFrontID, int nSessionID, char *szOrderRef)
{
    char szIndex[128];
    sprintf(szIndex, "%u%u%s", nFrontID, nSessionID, szOrderRef);
    string strIndex = szIndex;
    return strIndex;
}

CTraderSpi::CTraderSpi(void)
{
}

CTraderSpi::~CTraderSpi(void)
{
}

/* ����֪ͨ */
void CTraderSpi::OnFrontConnected()
{
    CPrintParams::PrintFunctionEx();

    /* ��������״̬ */
    CTraderApi *api = CTraderApi::GetInstance();
    api->m_bConnect = TRUE;
}

/* �Ͽ�֪ͨ */
void CTraderSpi::OnFrontDisconnected(int nReason)
{
    CPrintParams::PrintFunctionEx();
    CPrintApi::DbgPrint("----������nReason=0x%08x\n", nReason);

    /* ��������״̬ */
    CTraderApi *api = CTraderApi::GetInstance();
    api->m_bConnect = FALSE;
}

/* ����֪ͨ */
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
    CPrintParams::PrintFunctionEx();
    CPrintApi::DbgPrint("----������nTimeLapse=%08d", nTimeLapse);
}

/* ����֪ͨ */
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pOrder);

    /* �������� */
    CTraderApi *api = CTraderApi::GetInstance();
    api->m_mgr.UpdateTraderData(pOrder, E_UPDATE_STAGE_RTN);
}

/* ִ��֪ͨ */
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pExecOrder);
}

/* ����֪ͨ */
void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pQuote);
}

/* ѯ��֪ͨ */
void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pForQuoteRsp);
}

/* ����֪ͨ */
void CTraderSpi::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTradingNoticeInfo);
}

/* �ɽ�֪ͨ */
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTrade);

    /* �������� */
    CTraderApi *api = CTraderApi::GetInstance();
    api->m_mgr.UpdateTraderData(pTrade, E_UPDATE_STAGE_RTN);
}

/* ����¼����� */
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputOrder, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* �������� */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_mgr.UpdateTraderData(pInputOrder, E_UPDATE_STAGE_ERR);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ������������ */
void CTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pOrderAction, pRspInfo);
    
    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* �������� */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_mgr.UpdateTraderData(pOrderAction, E_UPDATE_STAGE_ERR);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ִ��¼����� */
void CTraderSpi::OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputExecOrder, pRspInfo);

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

/* ִ�в������� */
void CTraderSpi::OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pExecOrderAction, pRspInfo);

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

/* ����¼����� */
void CTraderSpi::OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputQuote, pRspInfo);

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

/* ���۲������� */
void CTraderSpi::OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pQuoteAction, pRspInfo);

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

/* ѯ��¼����� */
void CTraderSpi::OnErrRtnForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputForQuote, pRspInfo);

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

/* ������Ӧ */
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

/* ��֤��Ӧ */
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pRspAuthenticateField, pRspInfo);

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
void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pRspUserLogin, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* ��¼ */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_cfg.m_nFrontID = pRspUserLogin->FrontID;
        api->m_cfg.m_nSessionID = pRspUserLogin->SessionID;
        memcpy(&api->m_cfg.m_szMaxOrderRef, &pRspUserLogin->MaxOrderRef, sizeof(pRspUserLogin->MaxOrderRef));
        sprintf(api->m_cfg.m_szOrderRef, "%d", 1+atoi(api->m_cfg.m_szMaxOrderRef));
        sprintf(api->m_cfg.m_szExecOrderRef, "%d", 1+atoi(api->m_cfg.m_szMaxOrderRef));
        sprintf(api->m_cfg.m_szQuoteRef, "%d", 1+atoi(api->m_cfg.m_szMaxOrderRef));
        api->m_bLogin = TRUE;

        /* �״ε�¼����ȷ�Ͻ��� */
        api->SettlementInfoConfirm();
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ע����Ӧ */
void CTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pUserLogout, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* ע�� */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_bLogin = FALSE;
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ȷ�Ͻ�����Ӧ */
void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pSettlementInfoConfirm, pRspInfo);

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

/* ����¼����Ӧ */
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputOrder, pRspInfo);

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* �������� */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_mgr.UpdateTraderData(pInputOrder, E_UPDATE_STAGE_RSP);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ����������Ӧ */
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputOrderAction, pRspInfo);  

    /* �����Ӧ��� */
    BOOL bCheck = CheckRspInfo(pRspInfo);
    if (bCheck)
    {
        CPrintApi::DbgPrint("----%s��Ӧ�ɹ�", __FUNCTION__);

        /* �������� */
        CTraderApi *api = CTraderApi::GetInstance();
        api->m_mgr.UpdateTraderData(pInputOrderAction, E_UPDATE_STAGE_RSP);
    }
    else
    {
        CPrintApi::DbgPrint("----%s��Ӧʧ��", __FUNCTION__);
    }
}

/* ִ��¼����Ӧ */
void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputExecOrder, pRspInfo);  

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

/* ִ�в�����Ӧ */
void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputExecOrderAction, pRspInfo);  

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

/* ����¼����Ӧ */
void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputQuote, pRspInfo);  

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

/* ���۲�����Ӧ */
void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputQuoteAction, pRspInfo);  

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

/* ѯ��¼����Ӧ */
void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInputForQuote, pRspInfo);  

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

/* ��ѯ������Ӧ */
void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pOrder, pRspInfo);  

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

/* ��ѯִ����Ӧ */
void CTraderSpi::OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pExecOrder, pRspInfo);  

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

/* ��ѯ������Ӧ */
void CTraderSpi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pQuote, pRspInfo);

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

/* ��ѯѯ����Ӧ */
void CTraderSpi::OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pForQuote, pRspInfo);

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

/* ��ѯ������Ӧ */
void CTraderSpi::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTradingNotice, pRspInfo);

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

/* ��ѯ�ɽ���Ӧ */
void CTraderSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTrade, pRspInfo);

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

/* ��ѯ֪ͨ��Ӧ */
void CTraderSpi::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pNotice, pRspInfo);

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

/* ��ѯ��������Ӧ */
void CTraderSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pExchange, pRspInfo);

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

/* ��ѯ���ױ�����Ӧ */
void CTraderSpi::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTradingCode, pRspInfo);

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

/* ��ѯ��Ʒ��Ӧ */
void CTraderSpi::OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pProduct, pRspInfo);

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

/* ��ѯ��Լ��Ӧ */
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInstrument, pRspInfo);

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

/* ��ѯ������Ӧ */
void CTraderSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pDepthMarketData, pRspInfo);

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

/* ��ѯ�ֲ���Ӧ */
void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInvestorPosition, pRspInfo);

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

/* ��ѯ�ʽ���Ӧ */
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pTradingAccount, pRspInfo);

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

/* ��ѯͶ������Ӧ */
void CTraderSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CPrintParams::PrintFunctionEx();
    CPrintParams::PrintParams(pInvestor, pRspInfo);

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

/* ��֤���� */
void CTraderApi::Authe(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcReqAuthenticateField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    memcpy(&req.UserProductInfo, &m_cfg.m_szUserProductInfo, sizeof(m_cfg.m_szUserProductInfo));
    memcpy(&req.AuthCode, &m_cfg.m_szAuthCode, sizeof(m_cfg.m_szAuthCode));
    int result = m_api->ReqAuthenticate(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ��¼���� */
void CTraderApi::Login(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    memcpy(&req.Password, &m_cfg.m_szPassword, sizeof(m_cfg.m_szPassword));
    int result = m_api->ReqUserLogin(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ע������ */
void CTraderApi::Logout(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcUserLogoutField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    int result = m_api->ReqUserLogout(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ȷ�Ͻ��� */
void CTraderApi::SettlementInfoConfirm(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    int result = m_api->ReqSettlementInfoConfirm(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ����¼�� */
void CTraderApi::OrderInsert(void)
{
    CPrintParams::PrintFunctionEx();

    CMarketApi *api = CMarketApi::GetInstance();
    if (!api)
        return;

    /* ������� */
    CPrintApi::LogPrint("\n");
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");
#if 0
    /* ���ֵ�ͣ������ */
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    memcpy(&req.OrderRef, &m_cfg.m_szOrderRef, sizeof(m_cfg.m_szOrderRef));
    sprintf(m_cfg.m_szOrderRef, "%d", 1+atoi(req.OrderRef));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice; /* �����ԣ�THOST_FTDC_OPT_AnyPrice����֧�� */
    req.Direction = THOST_FTDC_D_Buy;
    req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    req.LimitPrice = api->m_data.m_fLowerLimitPrice;
    req.VolumeTotalOriginal = 1;
    req.TimeCondition = THOST_FTDC_TC_GFD;
    req.VolumeCondition = THOST_FTDC_VC_AV;
    req.MinVolume = 1;
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    req.StopPrice = 0;
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    req.RequestID = m_cfg.m_nRequestID;
    req.IsAutoSuspend = 0;
    req.IsSwapOrder = 0;
    CPrintApi::LogPrint("----��ͣ�ۿ��������ڻ���Լ��%s�����������ñ�ţ�%s��", req.InstrumentID, req.OrderRef);
#endif
#if 1
    /* ������ͣ������ */
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    memcpy(&req.OrderRef, &m_cfg.m_szOrderRef, sizeof(m_cfg.m_szOrderRef));
    sprintf(m_cfg.m_szOrderRef, "%d", 1+atoi(req.OrderRef));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  /* �����ԣ�THOST_FTDC_OPT_AnyPrice����֧�� */
    req.Direction = THOST_FTDC_D_Sell;
    req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    req.LimitPrice = api->m_data.m_fUpperLimitPrice;
    req.VolumeTotalOriginal = 1;
    req.TimeCondition = THOST_FTDC_TC_GFD;
    req.VolumeCondition = THOST_FTDC_VC_AV;
    req.MinVolume = 1;
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    req.StopPrice = 0;
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    req.RequestID = m_cfg.m_nRequestID;
    req.IsAutoSuspend = 0;
    req.IsSwapOrder = 0;
    CPrintApi::LogPrint("----��ͣ�ۿ��������ڻ���Լ��%s�����������ñ�ţ�%s��", req.InstrumentID, req.OrderRef);
#endif
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");

    /* �������� */
    m_mgr.UpdateTraderData(&req, E_UPDATE_STAGE_REQ);
    int result = m_api->ReqOrderInsert(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* �������� */
void CTraderApi::OrderAction(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    req.RequestID = m_cfg.m_nRequestID;
    strcpy(req.OrderRef, "2"); /* ע���޸� */
    req.FrontID = 1; /* ע���޸� */
    req.SessionID = 1350645504; /* ע���޸� */
    req.ActionFlag = THOST_FTDC_AF_Delete;
    req.LimitPrice = 0;
    req.VolumeChange = 0;
    m_mgr.UpdateTraderData(&req, E_UPDATE_STAGE_REQ);
    int result = m_api->ReqOrderAction(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ִ��¼�� */
void CTraderApi::ExecOrderInsert(void)
{
    CPrintParams::PrintFunctionEx();
#if 0
    CThostFtdcInputExecOrderField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    memcpy(&req.ExecOrderRef, &m_cfg.m_szExecOrderRef, sizeof(m_cfg.m_szExecOrderRef));
    sprintf(m_cfg.m_szExecOrderRef, "%d", 1+atoi(req.ExecOrderRef));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    int result = m_api->ReqExecOrderInsert(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
#endif
}

/* ִ�в��� */
void CTraderApi::ExecOrderAction(void)
{
    CPrintParams::PrintFunctionEx();
#if 0
    CThostFtdcInputExecOrderActionField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqExecOrderAction(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
#endif
}

/* ����¼�� */
void CTraderApi::QuoteInsert(void)
{
    CPrintParams::PrintFunctionEx();
#if 0
    CThostFtdcInputQuoteField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQuoteInsert(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
#endif
}

/* ���۲��� */
void CTraderApi::QuoteAction(void)
{
    CPrintParams::PrintFunctionEx();
#if 0
    CThostFtdcInputQuoteActionField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQuoteAction(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
#endif
}

/* ѯ��¼�� */
void CTraderApi::ForQuoteInsert(void)
{
    CPrintParams::PrintFunctionEx();
#if 0
    CThostFtdcInputForQuoteField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqForQuoteInsert(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
#endif
}

/* ������ѯ */
void CTraderApi::QryOrder(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryOrderField req;
    memset(&req, 0, sizeof(req)); /* Ϊ�����ѯ�������б��� */
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    int result = m_api->ReqQryOrder(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ִ�в�ѯ */
void CTraderApi::QryExecOrder(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryExecOrderField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQryExecOrder(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ���۲�ѯ */
void CTraderApi::QryQuote(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryQuoteField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQryQuote(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ѯ�۲�ѯ */
void CTraderApi::QryForQuote(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryForQuoteField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQryForQuote(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ���ײ�ѯ */
void CTraderApi::QryTradingNotice(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryTradingNoticeField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    int result = m_api->ReqQryTradingNotice(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* �ɽ���ѯ */
void CTraderApi::QryTrade(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryTradeField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    int result = m_api->ReqQryTrade(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ֪ͨ��ѯ */
void CTraderApi::QryNotice(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryNoticeField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    int result = m_api->ReqQryNotice(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ��������ѯ */
void CTraderApi::QryExchange(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryExchangeField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.ExchangeID, &m_cfg.m_szExchangeID, sizeof(m_cfg.m_szExchangeID)); /* ��ָ�����ѯ���н����� */
    int result = m_api->ReqQryExchange(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ���ױ����ѯ */
void CTraderApi::QryTradingCode(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryTradingCodeField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.ExchangeID, &m_cfg.m_szExchangeID, sizeof(m_cfg.m_szExchangeID));
    int result = m_api->ReqQryTradingCode(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ���ײ�Ʒ��ѯ */
void CTraderApi::QryProduct(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryProductField req;
    memset(&req, 0, sizeof(req));
    int result = m_api->ReqQryProduct(&req, m_cfg.m_nRequestID++); /* ��ָ�����ѯ���в�Ʒ */
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ��Լ��ѯ */
void CTraderApi::QryInstrument(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID)); /* ��ָ�����ѯ���к�Լ */
    int result = m_api->ReqQryInstrument(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* �����ѯ */
void CTraderApi::QryDepthMarketData(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryDepthMarketDataField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID)); /* ��ָ�����ѯ���к�Լ */
    int result = m_api->ReqQryDepthMarketData(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* �ֲֲ�ѯ */
void CTraderApi::QryInvestorPosition(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID)); /* ��ָ�����ѯ���к�Լ */
    int result = m_api->ReqQryInvestorPosition(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* �ʽ��ѯ */
void CTraderApi::QryTradingAccount(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    int result = m_api->ReqQryTradingAccount(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* Ͷ���߲�ѯ */
void CTraderApi::QryInvestor(void)
{
    CPrintParams::PrintFunctionEx();
    CThostFtdcQryInvestorField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    int result = m_api->ReqQryInvestor(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

const char *CTraderApi::oper_name[E_OPER_TYPE_COUNT]= {
    "default",
    "Authe()",
    "Login()",
    "Logout()",
    "SettlementInfoConfirm()",
    "OrderInsert()",
    "OrderAction()",
    "ExecOrderInsert()",
    "ExecOrderAction()",
    "QuoteInsert()",
    "QuoteAction()",
    "ForQuoteInsert()",
    "QryOrder()",
    "QryExecOrder()",
    "QryQuote()",
    "QryForQuote()",
    "QryTradingNotice()",
    "QryTrade()",
    "QryNotice()",
    "QryExchange()",
    "QryTradingCode()",
    "QryProduct()",
    "QryInstrument()",
    "QryDepthMarketData()",
    "QryInvestorPosition()",
    "QryTradingAccount()",
    "QryInvestor()"
};

int CTraderApi::oper_count = 0;

/* ���� */
void CTraderApi::Start(void)
{
    if (!m_bRunning)
    {
        m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
        if (m_hThread)
        {
            m_bRunning = TRUE;
        }
    }
}

/* ֹͣ */
void CTraderApi::Stop(void)
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

/* �����в��� */
void CTraderApi::CliOperation(void)
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
            case E_OPER_AUTHE:
                Authe();
                break;

            case E_OPER_LOGIN:
                break;

            case E_OPER_LOGOUT:
                Logout();
                break;

            case E_OPER_SETTLEMENT_INFO_CONFIRM:
                SettlementInfoConfirm();
                break;

            case E_OPER_ORDER_INSERT:
                OrderInsert();
                break;

            case E_OPER_ORDER_ACTION:
                OrderAction();
                break;

            case E_OPER_EXEC_ORDER_INSERT:
                ExecOrderInsert();
                break;

            case E_OPER_EXEC_ORDER_ACTION:
                ExecOrderAction();
                break;

            case E_OPER_QUOTE_INSERT:
                QuoteInsert();
                break;

            case E_OPER_QUOTE_ACTION:
                QuoteAction();
                break;

            case E_OPER_FOR_QUETE_INSERT:
                ForQuoteInsert();
                break;

            case E_OPER_QRY_ORDER:
                QryOrder();
                break;

            case E_OPER_QRY_EXEC_ORDER:
                QryExecOrder();
                break;

            case E_OPER_QRY_QUOTE:
                QryQuote();
                break;

            case E_OPER_QRY_FOR_QUOTE:
                QryForQuote();
                break;

            case E_OPER_QRY_TRADING_NOTICE:
                QryTradingNotice();
                break;

            case E_OPER_QRY_TRADE:
                QryTrade();
                break;

            case E_OPER_QRY_NOTICE:
                QryNotice();
                break;

            case E_OPER_QRY_EXCHANGE:
                QryExchange();
                break;

            case E_OPER_QRY_TRADING_CODE:
                QryTradingCode();
                break;

            case E_OPER_QRY_PRODUCT:
                QryProduct();
                break;

            case E_OPER_QRY_INSTRUMENT:
                QryInstrument();
                break;

            case E_OPER_QRY_DEPTH_MARKET_DATE:
                QryDepthMarketData();
                break;

            case E_OPER_QRY_INVESTOR_POSITION:
                QryInvestorPosition();
                break;

            case E_OPER_QRY_TRADING_ACCOUNT:
                QryTradingAccount();
                break;

            case E_OPER_QRY_INVESTOR:
                QryInvestor();
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

/* �Զ������� */
void CTraderApi::AutoOperation(void)
{
    if (m_bConnect)
    {
        if (m_bLogin)
        {
            CMarketApi *api = CMarketApi::GetInstance();
            if (api)
            {
                /* �ȴ��������� */
                if (api->m_data.WaitMarketData())
                {
                    /* ί�� */
                    CreateOrder(); /* ÿ��20��Tickִ��һ��ί�� */

                    /* �ȴ� */
                    int nTickPeriod = api->m_data.GetTickPeriod();
                    int nTickTime = 2*nTickPeriod;
                    Sleep(nTickTime);

                    /* ���� */
                    RevokeOrder(); /* ÿ��2��Tickִ��һ�γ��� */
                }
            }
        }
        else
        {
            /* ��¼ */
            Login();
            Sleep(5000);
        }
    }
    else
    {
        /* �ȴ� */
        Sleep(5000);
    }
}

/* ί�� */
void CTraderApi::CreateOrder(void)
{
    CPrintParams::PrintFunctionEx();

    CMarketApi *api = CMarketApi::GetInstance();
    if (!api)
        return;

    CPrintApi::LogPrint("\n");
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
    memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
    memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
    memcpy(&req.OrderRef, &m_cfg.m_szOrderRef, sizeof(m_cfg.m_szOrderRef));
    sprintf(m_cfg.m_szOrderRef, "%d", 1+atoi(req.OrderRef));
    memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    if (api->m_data.m_bUpperTrend)
    {
        /* �������ƣ��Ե�ͣ������ */
        req.Direction = THOST_FTDC_D_Buy;
        req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
        req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
        req.LimitPrice = api->m_data.m_fLowerLimitPrice;
        CPrintApi::LogPrint("----��ͣ�ۿ��������ڻ���Լ��%s�����������ñ�ţ�%s��", req.InstrumentID, req.OrderRef);
    }
    else
    {
        /* �µ����ƣ�����ͣ������ */
        req.Direction = THOST_FTDC_D_Sell;
        req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
        req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
        req.LimitPrice = api->m_data.m_fUpperLimitPrice;
        CPrintApi::LogPrint("----��ͣ�ۿ��������ڻ���Լ��%s�����������ñ�ţ�%s��", req.InstrumentID, req.OrderRef);
    }
    req.VolumeTotalOriginal = 1;
    req.TimeCondition = THOST_FTDC_TC_GFD;
    req.VolumeCondition = THOST_FTDC_VC_AV;
    req.MinVolume = 1;
    req.ContingentCondition = THOST_FTDC_CC_Immediately; /* �������ɽ���Ԥ�� */
    req.StopPrice = 0;
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    req.RequestID = m_cfg.m_nRequestID;
    req.IsAutoSuspend = 0;
    req.IsSwapOrder = 0;
    CPrintApi::LogPrint("-----------------------------------------------------------------------------");

    /* �������� */
    m_mgr.UpdateTraderData(&req, E_UPDATE_STAGE_REQ);
    int result = m_api->ReqOrderInsert(&req, m_cfg.m_nRequestID++);
    if (result)
    {
        CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
    }
    else
    {
        CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
    }
}

/* ���� */
void CTraderApi::RevokeOrder(void)
{
    CPrintParams::PrintFunctionEx();

    /* �������� */
    while (!m_mgr.m_UnTradeList.empty())
    {
        CTraderData *data = m_mgr.m_UnTradeList.front();

        /* �������� */
        CPrintApi::LogPrint("\n");
        CPrintApi::LogPrint("-----------------------------------------------------------------------------");
        CThostFtdcInputOrderActionField req;
        memset(&req, 0, sizeof(req));
        memcpy(&req.BrokerID, &m_cfg.m_szBrokerID, sizeof(m_cfg.m_szBrokerID));
        memcpy(&req.InvestorID, &m_cfg.m_szInvestorID, sizeof(m_cfg.m_szInvestorID));
        memcpy(&req.InstrumentID, &m_cfg.m_szInstrumentID, sizeof(m_cfg.m_szInstrumentID));
        memcpy(&req.UserID, &m_cfg.m_szUserID, sizeof(m_cfg.m_szUserID));
        req.RequestID = m_cfg.m_nRequestID;
        memcpy(&req.OrderRef, &data->m_szOrderRef, sizeof(data->m_szOrderRef));
        req.FrontID = data->m_nFrontID;
        req.SessionID = data->m_nSessionID;
        req.ActionFlag = THOST_FTDC_AF_Delete;
        req.LimitPrice = 0;
        req.VolumeChange = 0;
        CPrintApi::LogPrint("----����ί���ڻ���Լ��%s�����������ñ�ţ�%s��", req.InstrumentID, req.OrderRef);
        CPrintApi::LogPrint("----FrontID=%08u��SessionID=%08u", req.FrontID, req.SessionID);
        CPrintApi::LogPrint("-----------------------------------------------------------------------------");

        /* �������� */
        m_mgr.UpdateTraderData(&req, E_UPDATE_STAGE_REQ);
        int result = m_api->ReqOrderAction(&req, m_cfg.m_nRequestID++);
        if (result)
        {
            CPrintApi::DbgPrint("----ִ��%sʧ��", __FUNCTION__);
        }
        else
        {
            CPrintApi::DbgPrint("----ִ��%s�ɹ�", __FUNCTION__);
        }

        m_mgr.m_UnTradeList.pop_front();
    }
}

BOOL CTraderApi::IsRunning(void)
{
    return m_bRunning;
}

DWORD CTraderApi::ThreadProc(LPVOID lpParam)
{
    CTraderApi *api = (CTraderApi *)lpParam;
    if (!api)
        return -1;

    /* ����ҵ�� */
    while (api->IsRunning())
    {
        if (api->m_bHaveCli)
        {
            api->CliOperation();
        }
        else
        {
            api->AutoOperation();
        }
    }

    return 0;
}

CTraderApi *CTraderApi::m_pInstance = NULL;

CTraderApi *CTraderApi::CreateInstance(void)
{
    if (!m_pInstance)
    {
        m_pInstance = new CTraderApi();
        m_pInstance->m_bHaveCli = FALSE;
    }
    return m_pInstance;
}

void CTraderApi::ReleaseInstance(void)
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

CTraderApi *CTraderApi::GetInstance(void)
{
    return m_pInstance;
}

CTraderApi::CTraderApi(void)
{
    m_bRunning = FALSE;
    m_bConnect = FALSE;
    m_bLogin = FALSE;
    m_bHaveCli = FALSE;
    m_hThread = NULL;
    char *szFlowDir = "./TraderFlow/";
    CreateDir(szFlowDir);
    m_api = CThostFtdcTraderApi::CreateFtdcTraderApi(szFlowDir);
    m_spi = new CTraderSpi;
    m_api->RegisterSpi(m_spi);
    m_api->SubscribePublicTopic(THOST_TERT_RESTART);
    m_api->SubscribePrivateTopic(THOST_TERT_RESTART);
    m_api->RegisterFront(m_cfg.m_szTraderAddr);
    m_api->Init();
}

CTraderApi::~CTraderApi(void)
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

CTraderApi::CSelfRelease CTraderApi::m_self;

CTraderApi::CSelfRelease::CSelfRelease()
{
}
CTraderApi::CSelfRelease::~CSelfRelease()
{
    if(CTraderApi::m_pInstance)
        delete CTraderApi::m_pInstance;
    CTraderApi::m_pInstance = NULL;
}
