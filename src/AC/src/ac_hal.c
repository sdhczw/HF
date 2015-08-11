/**
******************************************************************************
* @file     ac_app_main.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_interface.h>
#include <ac_api.h>
#include <ac_hal.h>
#include <ac_cfg.h>


u32 g_u32CloudStatus = CLOUDDISCONNECT;
typedef struct tag_STRU_LED_ONOFF
{		
    u8	     u8LedOnOff ; // 0:关，1：开，2：获取当前开关状态
    u8	     u8Pad[3];		 
}STRU_LED_ONOFF;

u32 g_u32WifiPowerStatus = WIFIPOWEROFF;

u8 g_u8DevMsgBuildBuffer[600];


/*************************************************
* Function: AC_SendDevStatus2Server
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_SendLedStatus2Server()
{
    STRU_LED_ONOFF struRsp;
    u16 u16DataLen;
    //struRsp.u8LedOnOff = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
    struRsp.u8LedOnOff = struRsp.u8LedOnOff>>2;
    AC_BuildMessage(MSG_SERVER_CLIENT_GET_LED_STATUS_RSP,0,
                    (u8*)&struRsp, sizeof(STRU_LED_ONOFF),
                    NULL, 
                    g_u8DevMsgBuildBuffer, &u16DataLen);
    AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
}
/*************************************************
* Function: AC_DealNotifyMessage
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealNotifyMessage(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    //处理wifi模块的通知类消息
    switch(pstruMsg->MsgCode)
    {
        case ZC_CODE_EQ_DONE://wifi模块启动通知
        AC_StoreStatus(WIFIPOWERSTATUS , WIFIPOWERON);
        break;
        case ZC_CODE_WIFI_CONNECTED://wifi连接成功通知
        break;
        case ZC_CODE_CLOUD_CONNECTED://云端连接通知
        AC_StoreStatus(CLOUDSTATUS,CLOUDCONNECT);
        break;
        case ZC_CODE_CLOUD_DISCONNECTED://云端断链通知
        AC_StoreStatus(CLOUDSTATUS,CLOUDDISCONNECT);
        break;
    }
}



/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealEvent(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{   
    //处理设备自定义控制消息
    switch(pstruMsg->MsgCode)
    {
        case MSG_SERVER_CLIENT_SET_LED_ONOFF_REQ:
        {
            AC_DealLed(pstruMsg, pstruOptList, pu8Playload);
        }
        break;
    }
}

/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_StoreStatus(u32 u32Type , u32 u32Data)
{
    
    switch(u32Type)
    {
        case CLOUDSTATUS:
        g_u32CloudStatus = u32Data;
        break;
        case WIFIPOWERSTATUS:
        g_u32WifiPowerStatus = u32Data;
        break;
    }
}
/*************************************************
* Function: AC_BlinkLed
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_BlinkLed(unsigned char blink)
{

}
/*************************************************
* Function: AC_DealLed
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void AC_DealLed(ZC_MessageHead *pstruMsg, AC_OptList *pstruOptList, u8 *pu8Playload)
{
    u16 u16DataLen;
    u8 test[] = "hello";
    STRU_LED_ONOFF struRsp;

    switch (((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff)
    {
        case 0://处理开关消息
        case 1:        
            AC_BlinkLed(((STRU_LED_ONOFF *)pu8Playload)->u8LedOnOff);
            AC_BuildMessage(CLIENT_SERVER_OK,pstruMsg->MsgId,
                    (u8*)test, sizeof(test),
                    pstruOptList, 
                    g_u8DevMsgBuildBuffer, &u16DataLen);
            AC_SendMessage(g_u8DevMsgBuildBuffer, u16DataLen);
            break;       
    }
    
}
/*************************************************
* Function: AC_DealEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 AC_GetStoreStatus(u32 u32Type)
{
    switch(u32Type)
    {
        case CLOUDSTATUS:
        return g_u32CloudStatus;
        case WIFIPOWERSTATUS:
        return g_u32WifiPowerStatus;
    }
   return ZC_RET_ERROR;
}

