

#include "Sys.h"
#include "Led.h"
#include "Com.h"
#include "Encouder.h"
#include "Button.h"

int main(void) {
    SysInit();
    ComInit();
    LedInit();
    EncouderInit();
    ButtonInit();
    INTEN
    while(1) {
        LedSetRing(0);
        if(ButtonReadMode() == 0x80) {
            ComSendCmd(0x22,0x10,ButtonSetMode(),0);
        }
        if(ButtonReadDirection() > 0x00) {
            ComSendCmd(0x22,ButtonGetFlag(),0,0);
            LedSet(ButtonGetFlag());
        }
        if(EncouderFlag() == 0x80) {
            ComSendCmd(0x23,0xff,0,0);
        } else if(EncouderFlag() == 0x40) {
            ComSendCmd(0x23,0x55,0,0);
        } 
    }
}
