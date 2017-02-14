#ifndef _BU_FLASH
#define _BU_FLASH
#include <stdint.h>

#define FLASH_Page0 (0x00)
#define FLASH_Page1 (0x80)

void FLASH_Init(void);
void FLASH_erase(uint8_t pos);   // 0..127 for page0, or 128 to 255 for page1
void FLASH_put(uint8_t pos, const void * buffer, uint8_t len);
void FLASH_get(uint8_t pos,       void * buffer, uint8_t len);

uint8_t getTouch(void);
void          putTouch(uint8_t value);

uint8_t ifFirstTime(void);
void          setNotFirstTime(void);

#endif
