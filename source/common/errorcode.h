/**
 * @file    errorcode.h
 * @brief   通用错误码定义(common层, 零依赖)
 * @note    现有错误码继承自原cmn_errorcode.h, 按模块段划分
 */
#ifndef ERRORCODE_H
#define ERRORCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS                           0x0000
#define FAILURE                           0x0001

// ---- Modbus协议段 0x01xx ----
#define ERROR_MODBUS_NOT_FIND_REGISTER    0x0101  // 当前协议中未找到指定寄存器错误
#define ERROR_MODBUS_REGISTER_READ_ONLY   0x0102  // 寄存器只读权限错误
#define ERROR_MODBUS_UNKNOW_DATA_TYPE     0x0103  // 未知的数据类型错误
#define ERROR_MODBUS_REGISTER_WRITE_ONLY  0x0104  // 寄存器只写权限错误
#define ERROR_MODBUS_FRAME_LENGTH_NG      0x0105  // 输入数据长度异常
#define ERROR_MODBUS_CRC_MISMATCH         0x0106  // 数据CRC不匹配错误
#define ERROR_MODBUS_UNSUPPORTED_FUNCTION 0x0107  // 不支持的功能码
#define ERROR_MODBUS_INVALID_PROTOCOL     0x0108  // 无效的协议
#define ERROR_MODBUS_EMPTY_REGISTER_TABLE 0x0109  // 空的寄存器表
#define ERROR_MODBUS_VARIABLE_LOCKED      0x010A  // 变量被锁定, 无法访问
#define ERROR_MODBUS_WAIT_ACK_TIME_OUT    0x010B  // 等待响应超时

// ---- 485通信段 0x03xx ----
#define ERROR_485_PARITY_ERR              0x0301  // USART奇偶校验错误
#define ERROR_485_FRAME_ERR               0x0302  // USART帧错误
#define ERROR_485_OVERRUN_ERR             0x0303  // USART溢出错误

#define ERROR_COMM_BUSY                   0x0401  // 正在发送

// ---- EEPROM段 0x05xx ----
#define ERROR_EEPROM_INIT_ERR             0x0501  // 初始化失败
#define ERROR_EEPROM_WRITE_ERR            0x0502  // 写入失败
#define ERROR_EEPROM_READ_ERR             0x0503  // 读取失败
#define ERROR_EEPROM_ADDR_ERR             0x0504  // 地址错误
#define ERROR_EEPROM_TIMEOUT              0x0505  // 超时错误
#define ERROR_EEPROM_BUSY                 0x0506  // 设备忙

#ifdef __cplusplus
}
#endif

#endif // ERRORCODE_H
