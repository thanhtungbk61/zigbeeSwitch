/*
 * userCommand.h
 *
 *  Created on: Apr 27, 2020
 *      Author: who_you_are
 */

#ifndef USERCOMMAND_H_
#define USERCOMMAND_H_

/*
 * userCommand.c
 *
 *  Created on: Dec 16, 2019
 *      Author: who_you_are
 */

#include "af.h"
#include "userCommand.h"


uint8_t addGroupRespond(uint8_t stt,EmberNodeId nodeid,uint16_t groupId);
//
void sendAddGroupCommand(uint16_t groupId,uint8_t* groupName,uint16_t destination);
//void addGroup(Sub frame);

uint8_t reportLevelAttribute();
uint8_t reportLightAttribute();
uint8_t reportDimming();
uint8_t reportOnOffSchAttribute();


#endif /* USERCOMMAND_H_ */
