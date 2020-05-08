/*
 * userCommand.c
 *
 *  Created on: Dec 16, 2019
 *      Author: who_you_are
 */

#include "af.h"
#include "userCommand.h"


uint8_t addGroupRespond(uint8_t stt,EmberNodeId nodeid,uint16_t groupId)
{
	uint8_t status=1;
	emberAfFillCommandGroupsClusterAddGroupResponse(stt,groupId);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0006;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;
	status =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,nodeid);

	return status;
}
//
void sendAddGroupCommand(uint16_t groupId,uint8_t* groupName,uint16_t destination)
{
	EmberStatus  test2;
	//emberAfCorePrintln("---testSendCommandUnicast--final: %d",test2);
	emberAfFillCommandGroupsClusterAddGroup(groupId, groupName);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
	emAfCommandApsFrame->clusterId =0x0004;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;

	test2 =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,destination);
	emberAfCorePrintln("---sendAddGroupCommand--final: %d",test2);
}
//void addGroup(Sub frame)
//{
//	EmberStatus  test2;
//	//uint16_t groupId =1;
//	//nodeId = emberAfGetNodeId();
//	emberAfCorePrintln("---addGroup-- address:%d",frame.Address);
//	// tu them dia chi
//	emberAfFillCommandGroupsClusterAddGroup(frame.Address,"");
//	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0004;
//	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
//	emAfCommandApsFrame->destinationEndpoint =0x0001;
//	test2 =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,emberAfGetNodeId());
//	emberAfCorePrintln("---sendAddGroupCommand--final: %d",test2);
//}

uint8_t reportLevelAttribute()
{
	uint8_t level;
	uint8_t status=1;
    if (emberAfReadServerAttribute(1,
			   ZCL_LEVEL_CONTROL_CLUSTER_ID,
			   ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
            (uint8_t *)&level,
            sizeof(level))
        == EMBER_ZCL_STATUS_SUCCESS)
    {
	//uint8_t attributeId[2] ={0x40,0x02};
	uint8_t buf[5];
	buf[0] =0;  // attribute
	buf[1]=0;   // attribute
	buf[2]=0;   // status
	buf[3]=0x20;  // type value
	buf[4]=level;  //  value
	emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_LEVEL_CONTROL_CLUSTER_ID,buf,5);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0006;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;
	status =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,0x0000);
    }
	return status;
}
uint8_t reportLightAttribute()
{
	bool onOff;
	uint8_t status=1;
    if (emberAfReadServerAttribute(1,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS)
    {
	//uint8_t attributeId[2] ={0x40,0x02};
	uint8_t buf[5];
	buf[0] =0;  // attribute
	buf[1]=0;   // attribute
	buf[2]=0;   // status
	buf[3]=16;  // type value
	buf[4]=onOff;  //  value
	emberAfFillCommandGlobalServerToClientReportAttributes(0x0006,buf,5);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0006;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;
	status =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,0x0000);
    }
	return status;
}

uint8_t reportDimming()
{
	//dimmingValue
	uint8_t  dimming;
	uint8_t status=1;
	status =emberAfReadManufacturerSpecificServerAttribute(1,ZCL_MANAGER_ID,ZCL_Dimming_ATTRIBUTE_ID,0x10A2,&dimming,sizeof(dimming));
	emberAfCorePrintln("reportDimming:%d",status);
	//uint8_t attributeId[2] ={0x40,0x02};
	uint8_t buf[5];
	buf[0] =0;  // attribute
	buf[1]=17;   // attribute
	buf[2]=0;   // status
	buf[3]=0x20;  // type value // uint8_t
	buf[4]=dimming;  //  value
	emberAfFillCommandGlobalServerToClientReportAttributes(0xFC10,buf,5);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0006;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;
	status =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,0x0000);
	return status;
}
uint8_t reportOnOffSchAttribute()
{
	bool onOff;
	uint8_t status=1;
    if (emberAfReadServerAttribute(1,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS)
    {
	//uint8_t attributeId[2] ={0x40,0x02};
	uint8_t buf[5];
	buf[0] =0;  // attribute
	buf[1]=0;   // attribute
	buf[2]=0;   // status
	buf[3]=16;  // type value
	buf[4]=onOff;  //  value
	emberAfFillCommandGlobalServerToClientReportAttributes(0x0006,buf,5);
	emAfCommandApsFrame->profileId=emberAfProfileIdFromIndex(0);
//	emAfCommandApsFrame->clusterId =0x0006;
	emAfCommandApsFrame->sourceEndpoint	=emberAfEndpointFromIndex(0);
	emAfCommandApsFrame->destinationEndpoint =0x0001;
	status =emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,0x0000);
    }
	return status;
}
