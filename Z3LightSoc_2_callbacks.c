/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.
// user include
#include "userCharStruct.h"
#include "userFlash.h"
#include "userCommand.h"
#include "userBase64.h"
#include "em_msc.h"
#include "userRTCC.h"
// v2
#include "userFlashV2.h"
#include "userJsonStruct.h"
#include "app/framework/include/af.h"
#include "userPeripheral.h"
#include EMBER_AF_API_NETWORK_CREATOR
#include EMBER_AF_API_NETWORK_CREATOR_SECURITY
#include EMBER_AF_API_NETWORK_STEERING
#include EMBER_AF_API_FIND_AND_BIND_TARGET
#include EMBER_AF_API_ZLL_PROFILE



#define LIGHT_ENDPOINT (1)
EmberEventControl myDelay;

#define MY_DELAY_IN_MS 1000
EmberEventControl commissioningLedEventControl;
EmberEventControl findingAndBindingEventControl;

// test pwm
uint8_t test_duty=0;
//bool emberAfGroupsClusterAddGroupResponseCallback(uint8_t status,
//                                                     uint16_t groupId)
//{
//	emberAfCorePrintln("emberAfGroupsClusterAddGroupResponseCallback");
////	EmberNodeId userNewNodeId =emberGetSender();
////	uint8_t stt=addGroupRespond(status,userNewNodeId,groupId);
////	emberAfCorePrintln("AddGroupResponseCallback---status:%d---groupId:%d--userNewNodeId:%d--sendrespondstt:%d",status,groupId,userNewNodeId,stt);
//	return 1;
//}
// test RTCC
char* date ="010203";
char* time ="123030";
char* alarm_date ="010203";
char* alarm_time ="123130";


void emberAfGroupsClusterServerAttributeChangedCallback(int8u endpoint,
                                                        EmberAfAttributeId attributeId)
{
	emberAfCorePrintln("emberAfGroupsClusterServerAttributeChangedCallback");
}


void emberAfGroupsClusterServerDefaultResponseCallback(int8u endpoint,
                                                       int8u commandId,
                                                       EmberAfStatus status)
{
//	EmberNodeId userNewNodeId =emberGetSender();
//	uint8_t stt=addGroupRespond(status,userNewNodeId,groupId);
	emberAfCorePrintln("emberAfGroupsClusterServerDefaultResponseCallback:%d",status);
}
//
void printfSub(Sub s){
	emberAfCorePrintln("Address  :%d\n", s.Address);
	emberAfCorePrintln("Type     :%d\n", s.Type);
	emberAfCorePrintln("Endpoint :%d\n", s.Endpoint);
	emberAfCorePrintln("Profile  :%d\n", s.ProfileID);
	emberAfCorePrintln("Cluster  :%d\n", s.ClusterID);
	emberAfCorePrintln("Attribute:%d\n", s.AttributeID);
	emberAfCorePrintln("TypeValue:%d\n", s.ValueType);
	emberAfCorePrintln("StrValue format Light :%d\n", (int8_t)getIntFromString(s.StrValue, INT8));
}



void myDelayHandler(void)
{
	// setup peripheral
	static int mydelayCount;
	mydelayCount++;
	emberEventControlSetInactive(myDelay);
//	MSC_ErasePage((uint32_t*)USERDATAV2 );
	if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
	 emberEventControlSetDelayMS(myDelay, 10 * MY_DELAY_IN_MS);
	}
	else
	{
		if(mydelayCount%2)
		{
			reportLevelAttribute();
		}
		else
		{
			reportLightAttribute();
		}

//		emberAfCorePrintln("myDelayHandler--reportLightAttribute:%d",status);
		emberEventControlSetDelayMS(myDelay, 80 * MY_DELAY_IN_MS);
	}
	uint16_t nodeId =emberAfGetNodeId();
	emberAfCorePrintln("emberAfGetNodeId:%d",nodeId);
}

void emberAfMainInitCallback(void)
{
	MSC_ErasePage((uint32_t*)USERDATAV2 );
	uint8_t dimmingValue=0,status;
	bool onoffValue=1;
	// init value of cluster
	status= emberAfWriteManufacturerSpecificServerAttribute(1,ZCL_MANAGER_ID,ZCL_Dimming_ATTRIBUTE_ID,0x10A2,&dimmingValue,0x20);
	emberAfCorePrintln("emberAfWriteManufacturerSpecificServerAttribute:%d",status);
	status =emberAfWriteServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,(uint8_t *)&onoffValue,0x10);
	emberAfCorePrintln("emberAfReadServerAttribute:%d",status);
	pwmInit();
	TIMER_CompareBufSet(TIMER1, TIMER_CHANNEL, 100);

    emberEventControlSetActive(commissioningLedEventControl);
    emberEventControlSetDelayMS(myDelay, 10 * MY_DELAY_IN_MS);
}
// all callback

bool emberAfManagerSetSubcriberCallback(uint8_t* setSubcriber)
{
//	// test
//	//
// trich xuat du lieu
	int len;
	Sub userSub;
	char * data =base64_decode((char*)setSubcriber,&len);
	if(len<19)
	{
		userSub = convert2group(data);
	}
	else
	{
		userSub = convert2sub(data);
		writeFlash(userSub);
	}
	printfSub(userSub);
// add group
	addGroup(userSub);
// add to memory;

	emberAfCorePrintln("SetSubcriberCallback:%s---len:%d",setSubcriber,len);

	Sub subFrame =readSubRam(1);
	printfSub(subFrame);
	return 1;
}



bool emberAfManagerDeleteSchedualCallback(uint8_t* deleteSchedual)
{
	return 1;
}
bool emberAfManagerDeleteSubcriberCallback(uint8_t* deleteSubcriber)
{
	return 1;
}

bool emberAfManagerOnTriggerCallback(uint16_t OnTrigger)
{
	EmberAfStatus status;
	uint8_t data =1;
	emberAfCorePrintln("emberAfManagerOnTriggerCallback---");
	Sub subFrame =readSubRam(OnTrigger);
	printfSub(subFrame);
	switch(subFrame.ClusterID)
	{
	case ONOFF:
		switch(subFrame.AttributeID)
		{
		case ONOFFATT:
			status= emberAfWriteServerAttribute(1,
			                                    6,
			                                     0,
			                                     &data,
			                                     0x10);
			halSetLed(ON_OFF_LIGHT_LED);
			break;
		}
		break;
	default:
		break;
	}
	return 1;
}



bool emberAfManagerSetSchedualCallback(uint8_t* setSchedual)
{
	return 1;
}


void commissioningLedEventHandler(void)
{
  emberEventControlSetInactive(commissioningLedEventControl);


  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    uint16_t identifyTime;
    emberAfReadServerAttribute(LIGHT_ENDPOINT,
                               ZCL_IDENTIFY_CLUSTER_ID,
                               ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                               (uint8_t *)&identifyTime,
                               sizeof(identifyTime));
    if (identifyTime > 0) {
      halToggleLed(COMMISSIONING_STATUS_LED);
      emberEventControlSetDelayMS(commissioningLedEventControl,
                                  LED_BLINK_PERIOD_MS << 1);
    } else {
      halSetLed(COMMISSIONING_STATUS_LED);
    }
  } else {
    EmberStatus status = emberAfPluginNetworkSteeringStart();
    emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
  }
}

void findingAndBindingEventHandler()
{
	uint8_t status = reportDimming();
	emberEventControlSetInactive(findingAndBindingEventControl);
	uint32_t time = RTCC_CounterGet();
	emberAfCorePrintln("--------------------------buttonIsr------------time:%d-----------------reportDimming:%d",time,status);
	update_calendar();
uint16_t year;
uint8_t month;
uint8_t day;
uint8_t weekday;
uint8_t hour;
uint8_t min;
uint8_t sec;
uint16_t ms;
get_date_and_time(&year,&month,&day,&weekday,&hour,&min,&sec,&ms);
emberAfCorePrintln("%4d-%02d-%02d %02d:%02d:%02d.%03d",year,month,day,weekday,hour,min,sec);

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
//    emberEventControlSetInactive(findingAndBindingEventControl);
//    EmberStatus x = emberLeaveNetwork();
//    emberAfCorePrintln("emberLeaveNetwork:%d",x);
//    emberAfCorePrintln("Find and bind target start: 0x%X",
//                       emberAfPluginFindAndBindTargetStart(LIGHT_ENDPOINT));
  }
  {
	  EmberStatus status = emberAfPluginNetworkSteeringStart();
	  emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
  }

}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  // Note, the ZLL state is automatically updated by the stack and the plugin.
  if (status == EMBER_NETWORK_DOWN) {
    halClearLed(COMMISSIONING_STATUS_LED);
  } else if (status == EMBER_NETWORK_UP) {
    halSetLed(COMMISSIONING_STATUS_LED);
  //  emberEventControlSetActive(findingAndBindingEventControl);
  }

// This value is ignored by the framework.
  return false;
}
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */

/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);

}

/** @brief Complete
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels Ver.: always
 */
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Form distributed",
                     "complete",
                     EMBER_SUCCESS);
}

/** @brief On/off Cluster Server Post Init
 *
 * Following resolution of the On/Off state at startup for this endpoint, perform any
 * additional initialization needed; e.g., synchronize hardware state.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPluginOnOffClusterServerPostInitCallback(uint8_t endpoint)
{
  // At startup, trigger a read of the attribute and possibly a toggle of the
  // LED to make sure they are always in sync
  emberAfOnOffClusterServerAttributeChangedCallback(endpoint,
                                                    ZCL_ON_OFF_ATTRIBUTE_ID);
}

/** @brief Server Attribute Changed
 *
 * On/off cluster, Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId)
{

  // When the on/off attribute changes, set the LED appropriately.  If an error
  // occurs, ignore it because there's really nothing we can do.
  if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
		emberAfCorePrintln("emberAfOnOffClusterServerAttributeChangedCallback");
    bool onOff;
    if (emberAfReadServerAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS) {
      if (onOff) {
    	  emberAfCorePrintln("halSetLed");
    	  GPIO_PinOutSet(LED_PORT,LED_PIN);
        //halSetLed(ON_OFF_LIGHT_LED);
      } else {
    	  emberAfCorePrintln("halClearLed");
//        halClearLed(ON_OFF_LIGHT_LED);
    	  //GPIO_PinOutClear(LED_PORT,LED_PIN);
    	  setLevel(0);
      }
    }
  }
}

void emberAfLevelControlClusterServerAttributeChangedCallback(int8u endpoint,
                                                              EmberAfAttributeId attributeId)
{
    bool onOff;
    emberAfCorePrintln("emberAfLevelControlClusterServerAttributeChangedCallback");
    emberAfReadServerAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff));
    if(onOff==1)
    {
  	  if (attributeId == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID) {
  			//emberAfCorePrintln("emberAfOnOffClusterServerAttributeChangedCallback");
  		  uint8_t level=0;

  	    if (emberAfReadServerAttribute(endpoint,
  	    							   ZCL_LEVEL_CONTROL_CLUSTER_ID,
  									   ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
  	                                   (uint8_t *)&level,
  	                                   sizeof(level))
  	        == EMBER_ZCL_STATUS_SUCCESS) {
  	    	emberAfCorePrintln("level--run:%d",level-1);
  			  setLevel(level-1);
  	    }

  	  }
    }

}
void emberAfOnOffClusterServerDefaultResponseCallback(int8u endpoint,
                                                      int8u commandId,
                                                      EmberAfStatus status)
{
	//emberAfCorePrintln("emberAfOnOffClusterServerDefaultResponseCallback");
}

void emberAfLevelControlClusterServerDefaultResponseCallback(int8u endpoint,
                                                             int8u commandId,
                                                             EmberAfStatus status)
{
//	  if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
	//emberAfCorePrintln("emberAfLevelControlClusterServerDefaultResponseCallback");
//	  }
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{

  if (state == BUTTON_RELEASED) {
    emberEventControlSetActive(findingAndBindingEventControl);
  }
}

// all  custom callback

bool emberAfManagerGetOnOffScheduleCallback(int8u* OnOffscheduleStr)
{
	emberAfCorePrintln("emberAfManagerGetOnOffScheduleCallback");
	for(int i=0;i<strlen((char*)OnOffscheduleStr);i++)
	{
		emberAfCorePrintln("value[%d]:%d",i,OnOffscheduleStr[i]);
	}
	return 1;
}

bool emberAfManagerPutOnOffScheduleCallback(int8u* OnOffscheduleStr)
{
	emberAfCorePrintln("emberAfManagerPutOnOffScheduleCallback");

	emberAfCorePrintln("%d",strlen((char*)OnOffscheduleStr));
	for(int i=0;i<strlen((char*)OnOffscheduleStr);i++)
	{
		emberAfCorePrintln("value[%d]:%c",i,OnOffscheduleStr[i]);
	}
//	 process
	writeOnOffSch2FlashV2((char*)OnOffscheduleStr);
    for (int i = 0; i < ONOFFSCHEDULEMAX; i++)
    {
    	emberAfCorePrintln("\n\taddress=%d", onOffSchRam[i].address);
    	emberAfCorePrintln("\n\treserve=%d-hour=%d-minute=%d-second=%d", onOffSchRam[i].time.reserve, onOffSchRam[i].time.hour, onOffSchRam[i].time.minute, onOffSchRam[i].time.second);
    	emberAfCorePrintln("\n\tvalue=%d\n", onOffSchRam[i].value);
    }
	return 1;
}

boolean emberAfManagerPutDimmingScheduleCallback(int8u* dimmingScheduleStr)
{
	emberAfCorePrintln("emberAfManagerPutDimmingScheduleCallback");
	writeDimmingSch2FlashV2((char*)dimmingScheduleStr);
	return 1;
}
//can  xu ly group//
boolean emberAfManagerPutGroupCallback(int8u* groupStr)
{
	emberAfCorePrintln("emberAfManagerPutGroupCallback");
	writeGroupSch2FlashV2((char*)groupStr);
	return 1;
}

boolean emberAfManagerPutReportRealTimeCallback(int8u* reportTime)
{
	emberAfCorePrintln("emberAfManagerPutReportRealTimeCallback");
	schedule_time_t time = schedule_time_cast((char*)reportTime);
	uint16_t year=0;
	uint8_t month=0;
	uint8_t day=0;
	uint8_t weekday=0;
	uint8_t hour=time.hour;
	uint8_t min= time.minute;
	uint8_t sec= time.second;
	uint16_t ms =0;
	set_date_and_time(year,month,day,weekday,hour,min,sec,ms);
	return 1;
}
boolean emberAfManagerPutDimmingCallback(int16u dimming)
{
	emberAfCorePrintln("emberAfManagerPutDimmingCallback");
	dimmingValue = dimming;
	setLevel(dimming);
	return 1;
}


boolean emberAfManagerPutHistoricalEventCallback(int8u* HistoryStr)
{
	emberAfCorePrintln("emberAfManagerPutHistoricalEventCallback");
	return 1;
}


boolean emberAfManagerPutReportTimeCallback(int16u reportStr)
{
	return 1;
}
// emberAfExternalAttributeWriteCallback();
//
// emberAfExternalAttributeReadCallback();
//void RTCC_IRQHandler(void)
//{
//	uint32_t rtccFlag,time;
//	time = RTCC_TimeGet();
//  // Read the interrupt source
//  rtccFlag = RTCC_IntGet();
//
//  // Clear interrupt flag
//  RTCC_IntClear(rtccFlag);
//  emberAfCorePrintln("RTCC_IRQHandler:%lld",time);
//  // Toggle LED to turn it on
// // GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN); // LEDS active low
//}


// test flash
//emberAfCorePrintln("test flash");
//char test_onoff_array_have_error[4 * 7] = {0x12, 0x34, 0, 21, 15, 30, 1, 0x12, 0x34, 0, 0, 0, 0, 1, 0x12, 0x34, 0, 23, 30, 15, 0,0x12, 0x34, 0, 21, 15, 30, 1};
//schedule_onoff_t schs[4];
//
//schedule_onoff_array_cast(schs, 4, test_onoff_array_have_error);
//emberAfCorePrintln("test string input ");
//for (int i = 0; i < 4; i++)
//	 {
//	  emberAfCorePrintln("\n\taddress=%d", schs[i].address);
//	   emberAfCorePrintln("\n\treserve=%d-hour=%d-minute=%d-second=%d", schs[i].time.reserve, schs[i].time.hour, schs[i].time.minute, schs[i].time.second);
//	   emberAfCorePrintln("\n\tvalue=%d\n", schs[i].value);
//	 }
//resetOnOffSchRam();
//for (int i = 0; i < 4; i++)
//{
//	emberAfCorePrintln("\n\taddress=%d", onOffSchRam[i].address);
//	emberAfCorePrintln("\n\treserve=%d-hour=%d-minute=%d-second=%d", onOffSchRam[i].time.reserve, onOffSchRam[i].time.hour, onOffSchRam[i].time.minute, onOffSchRam[i].time.second);
//	emberAfCorePrintln("\n\tvalue=%d\n", onOffSchRam[i].value);
//}
// writeOnOffSch2FlashV2(test_onoff_array_have_error);
// emberAfCorePrintln("output");
//for (int i = 0; i < 4; i++)
//{
//	emberAfCorePrintln("\n\taddress=%d", onOffSchRam[i].address);
//	emberAfCorePrintln("\n\treserve=%d-hour=%d-minute=%d-second=%d", onOffSchRam[i].time.reserve, onOffSchRam[i].time.hour, onOffSchRam[i].time.minute, onOffSchRam[i].time.second);
//	emberAfCorePrintln("\n\tvalue=%d\n", onOffSchRam[i].value);
//}
//
// joing
//	    EmberStatus status = emberAfPluginNetworkSteeringStart();
//	    emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
//


//	 uint8_t key[16]={0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
//	 EmberKeyData key2;
//	 for(i=0;i<16;i++)
//	{
//		key2.contents[i]=key[i];
//	}
//	 //TOKEN_MFG_INSTALLATION_CODE
//	 halCommonSetMfgToken(CREATOR_MFG_INSTALLATION_CODE,&key2);
//	emberAfCorePrintln("halCommonSetMfgToken");
//	EmberEUI64 eui64;
	//CREATOR_MFG_INSTALLATION_CODE;
	//halCommonGetMfgToken(&eui64, TOKEN_MFG_EMBER_EUI_64);
