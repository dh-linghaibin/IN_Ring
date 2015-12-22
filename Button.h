

#ifndef BUTTON_H
#define BUTTON_h

#include "type.h"

void ButtonInit(void);
u8 ButtonReadMode(void);
u8 ButtonReadDirection(void);
u8 ButtonGetFlag(void);
u8 ButtonSetMode(void);
u8 ButtonGetMode(void);

#endif

