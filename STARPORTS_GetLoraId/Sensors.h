/*
 * Sensors.h
 *
 *  Created on: 2 may. 2019
 *      Author: airizar
 */

#ifndef SENSORS_H_
#define SENSORS_H_


uint8_t GetSensorData(uint8_t *DataPacket);

uint8_t IniPacket(uint8_t *DataPacket, uint16_t NodeId);

uint8_t Add_s16Data2Packet(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen);

uint8_t Add_s32Data2Packet(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int32_t *DataSensor, uint8_t DataSensorLen);


#endif /* SENSORS_H_ */
