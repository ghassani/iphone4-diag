//
//  util.h
//  usblogger
//
//  Created by Joshua Hill on 1/29/13.
//
//

#ifndef usblogger_util_h
#define usblogger_util_h

#include <sys/types.h>

#define DLOAD WRITE                       0x01
#define DLOAD_ACK                         0x02
#define DLOAD_NAK                         0x03
#define DLOAD_ERASE                       0x04
#define DLOAD_EXECUTE                     0x05
#define DLOAD_NOP                         0x06
#define DLOAD_PARAM_REQ                   0x07
#define DLOAD_PARAM_RESP                  0x08
#define DLOAD_MEMORY_DUMP                 0x09
#define DLOAD_RESET                       0x0A
#define DLOAD_UNLOCK                      0x0B
#define DLOAD_SW_VER_REQ                  0x0C
#define DLOAD_SW_VERS_RESP                0x0D
#define DLOAD_POWERDOWN                   0x0E
#define DLOAD_WRITE_ADDR                  0x0F
#define DLOAD_MEMORY_DEBUG_QUERY          0x10  //Memory Debug Query
#define DLOAD_MEMORY_DEBUG_INFO           0x11  //Memory Debug Info
#define DLOAD_MEMORY_READ_REQ             0x12  //Memory Read Request
#define DLOAD_MEMORY_READ                 0x13  //Memory Read
#define DLOAD_MEMOY_UNFRAMED_READ_REQ     0x14  //Memory Unframed Read Request
#define DLOAD_SERIAL_NUMBER_READ_REQ      0x14  //Serial Number Read Request
#define DLOAD_SERIAL_NUMBER_READ_RESP     0x14  //Serial Number Read Response
#define DLOAD_MEMORY_UNFRAMED_READ_RESP   0x15  //Memory Unframed Read Response
//#define DLOAD_SERIAL_NUMBER_READ_REQ      0x16  //Serial Number Read Request
//#define DLOAD_SERIAL_NUMBER_READ_RESP     0x16

typedef u_int8_t boolean;
#ifndef TRUE
#define TRUE ((u_int8_t) 1)
#endif
#ifndef FALSE
#define FALSE ((u_int8_t) 0)
#endif

void hexdump (unsigned char *data, unsigned int amount);
void hexdump_log (FILE* file, unsigned char *data, unsigned int amount);
uint16_t flip_endian16(uint16_t value);
uint32_t flip_endian32(uint32_t value);

u_int16_t dm_crc16 (const char *buffer, size_t len);


#endif
