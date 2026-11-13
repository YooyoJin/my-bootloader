/**
 * @file    crc.h
 * @brief   CRC校验模块(纯逻辑,零依赖)
 *
 * 提供 Modbus CRC16 的三种计算方式:
 *   - Crc16Modbus:       查表法,整块计算(推荐)
 *   - Crc16ModbusStep:   查表法,单字节步进(流式场景)
 *   - Crc16ModbusBitwise:位运算法,不依赖查找表(省ROM场景)
 *
 * @version v0.2.0 (v0.1.0移植自My-OTA工程cmn_crc; v0.2.0查表改uint16_t单表)
 */
#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Modbus CRC16查表法,整块计算
 * @param   pu8Msg 数据指针
 * @param   u16Len  数据长度
 * @return  CRC寄存器值; Modbus发送时低字节在前
 * @note    初值0xFFFF, 多项式x^16+x^15+x^2+1 (Modbus RTU标准)
 *          上送填帧时低字节在前(小端线序): frame[i]=低字节, frame[i+1]=高字节
 */
uint16_t Crc16Modbus(const uint8_t *pu8Msg, uint16_t u16Len);

/**
 * @brief   Modbus CRC16查表法,单字节步进(流式计算用)
 * @param   u8Data    新增数据字节
 * @param   pu16Crc   CRC状态(使用前置0xFFFF, 出参更新)
 * @note    全部数据喂完后CRC即为*pu16Crc, 与Crc16Modbus一致
 */
void Crc16ModbusStep(uint8_t u8Data, uint16_t *pu16Crc);

/**
 * @brief   Modbus CRC16位运算法(逐位,不依赖查找表)
 * @param   pu8Msg 数据指针
 * @param   u16Len  数据长度
 * @return  CRC值(与查表法结果一致)
 */
uint16_t Crc16ModbusBitwise(const uint8_t *pu8Msg, uint16_t u16Len);

#ifdef __cplusplus
}
#endif

#endif // CRC_H
