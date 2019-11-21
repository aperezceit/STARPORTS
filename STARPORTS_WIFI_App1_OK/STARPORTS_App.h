/*
 * STARPORTS_App.h
 *
 *  Created on: 15 abr. 2019
 *      Author: airizar
 */

#ifndef STARPORTS_APP_H_
#define STARPORTS_APP_H_

#include <stdint.h>

struct Nodes {
    uint16_t NodeId;
    uint8_t NoSensors;
    uint16_t *SensorIdPtr;
    uint8_t *SensorDataLen;
    int16_t **SensorDataPtr;
};

#endif /* STARPORTS_APP_H_ */
