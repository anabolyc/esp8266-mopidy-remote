#ifndef _IR_H_
#define _IR_H_

#define IR_RECV_PIN        5
#define IR_SEND_PIN        4
#define IR_CHECK_EVERY_MS 50

#define IR_BUF_SIZE 1024
#define IR_TIMEOUT 50
#define IR_TOLERANCE 25

#include <Timer.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define BTN_A 0xE0E036C9
#define BTN_B 0xE0E028D7
#define BTN_C 0xE0E0A857
#define BTN_D 0xE0E06897

#define BTN_NEC_A 0x807FC837

#define BTN_NEC_PLAY 0x807F32CD
#define BTN_NEC_STOP 0x807F02FD

#define BTN_VOLUP 0xE0E0E01F
#define BTN_VOLDN 0xE0E0D02F

#define BTN_NEC_VOLUP 0x807F18E7
#define BTN_NEC_VOLDN 0x807F08F7

#define BTN_NEXT 0xE0E012ED 
#define BTN_PREV 0xE0E0A25D 

#define BTN_NEC_NEXT 0x807FF20D
#define BTN_NEC_PREV 0x807FD22D

#define BTN_NEC_0 0x807F807F
#define BTN_NEC_1 0x807F728D
#define BTN_NEC_2 0x807FB04F
#define BTN_NEC_3 0x807F30CF
#define BTN_NEC_4 0x807F52AD
#define BTN_NEC_5 0x807F906F
#define BTN_NEC_6 0x807F10EF
#define BTN_NEC_7 0x807F629D
#define BTN_NEC_8 0x807FA05F
#define BTN_NEC_9 0x807F20DF

#define BTN_NEC_TOGGLE_PLAYLIST 0x807F9867
// #define BTN_NEC_CHP 0xFFE21D
// #define BTN_NEC_CHM 0xFF629D
// #define BTN_NEC_EQ  0xFF22DD

class IR {

public:
    static void start(Timer* t);

protected:
    static void receiveIR();

private:
    static IRrecv ir_recv;
};

#endif