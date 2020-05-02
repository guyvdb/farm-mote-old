#ifndef FLASH_H
#define FLASH_H

void system_initialize_flash(void);




void flash_set_master_address();
void flash_get_master_address();
void flash_set_master_port();
void flash_get_master_port();



/*
NVS operates on key-value pairs. Keys are ASCII strings; the maximum key length is currently 15 characters. 
Values can have one of the following types:

integer types: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t
zero-terminated string
variable length binary data (blob)

String values are currently limited to 4000 bytes. This includes the null terminator. Blob values are limited 
to 508000 bytes or 97.6% of the partition size - 4000 bytes, whichever is lower.
*/



#endif
