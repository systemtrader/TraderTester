#include "TraderApi.h"
#include "MarketApi.h"
#include "AssistApi.h"

int main(int argc, char *argv[])
{
#if 1
    /* ���׽ӿڲ��� */
    CTraderApi *trader_api = CTraderApi::GetInstance();
    if (!trader_api)
    {
        trader_api = CTraderApi::CreateInstance();
    }
    trader_api->Start();
#endif
#if 1
    /* ����ӿڲ��� */
    CMarketApi *market_api = CMarketApi::GetInstance();
    if (!market_api)
    {
        market_api = CMarketApi::CreateInstance();
    }
    market_api->Start();
#endif
#if 1
    /* ��־�ӿڲ��� */
    CTriggerLogApi *log_api = CTriggerLogApi::GetInstance();
    log_api->Start();
    int i = 0;
    while (i++<20)
    {
    #if 0
        CTriggerLogApi::Trigger(i, 0);
        Sleep(2000);
        CTriggerLogApi::Trigger(i, 1);
        Sleep(2000);
        CTriggerLogApi::Trigger(i, 2);
        Sleep(2000);
    #else
        Sleep(4000);
    #endif
    }
#endif
#if 1
    /* ���̵߳ȴ� */
    while (1)
    {
        Sleep(300000);
    }
#endif
    return 0;
}