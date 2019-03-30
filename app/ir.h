#ifndef _IR_H_
#define _IR_H_

#define IR_RECV_PIN  5 // GPIO5
#define IR_SEND_PIN  4 // GPIO4

#include <SmingCore/SmingCore.h>
#include "IRrecv.h"
#include "mopidy.h"

#define BTN_A 0xE0E036C9
#define BTN_B 0xE0E028D7
#define BTN_C 0xE0E0A857
#define BTN_D 0xE0E06897

#define BTN_0A 0xFFA25D

#define BTN_PLAY 0xE0E0E21D
#define BTN_STOP 0xE0E0629D

#define BTN_0PLAY 0xFF42BD
#define BTN_0STOP 0xFF52AD

#define BTN_VOLUP 0xE0E0E01F
#define BTN_VOLDN 0xE0E0D02F

#define BTN_0VOLUP 0xFFC23D
#define BTN_0VOLDN 0xFF02FD

#define BTN_NEXT 0xE0E012ED 
#define BTN_PREV 0xE0E0A25D 

#define BTN_0NEXT 0xFF906F
#define BTN_0PREV 0xFFA857

#define BTN_MUTE 0xE0E0F00F

#define BTN_0MUTE 0xFF4AB5

#define BTN_0 0xE0E08877
#define BTN_1 0xE0E020DF
#define BTN_2 0xE0E0A05F
#define BTN_3 0xE0E0609F
#define BTN_4 0xE0E010EF
#define BTN_5 0xE0E0906F
#define BTN_6 0xE0E050AF
#define BTN_7 0xE0E030CF
#define BTN_8 0xE0E0B04F
#define BTN_9 0xE0E0708F

#define BTN_00 0xFFE01F
#define BTN_01 0xFF6897
#define BTN_02 0xFF9867
#define BTN_03 0xFFB04F
#define BTN_04 0xFF30CF
#define BTN_05 0xFF18E7
#define BTN_06 0xFF7A85
#define BTN_07 0xFF10EF
#define BTN_08 0xFF38C7
#define BTN_09 0xFF5AA5

#define BTN_0CHP 0xFFE21D
#define BTN_0CHM 0xFF629D
#define BTN_0EQ  0xFF22DD

class IR {

public:
    static void start();

protected:
    static void receiveIR();

private:
    static Timer  ir_timer;
    static IRrecv ir_recv;
};

#endif