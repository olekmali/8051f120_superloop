//* ECE471/571 Homework 2 File - provided header file
#ifndef _HW_SYSCLK
#define _HW_SYSCLK
#include <stdint.h>

#define SYSCLK_no_change        (0)
#define SYSCLK_INT_3062KHz      (1)
#define SYSCLK_INT_6125KHz      (2)
#define SYSCLK_INT_12250KHz     (3)
#define SYSCLK_INT_24500KHz     (4)
#define SYSCLK_INT_49000KHz     (5)
#define SYSCLK_INT_73500KHz     (6)
#define SYSCLK_INT_98000KHz     (7)

#define SYSCLK_EXT_22118KHz     (8)
#define SYSCLK_EXT_44237KHz     (9)
#define SYSCLK_EXT_66355KHz    (10)
#define SYSCLK_EXT_88474KHz    (11)


void reinit_sysclk(uint8_t mode);

// retrieve the last mode set
uint8_t get_sysclk_mode();


#endif
