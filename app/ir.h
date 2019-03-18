#ifndef _IR_H_
#define _IR_H_

#define IR_RECV_PIN  5 // GPIO5
#define IR_SEND_PIN  4 // GPIO4

#include <SmingCore/SmingCore.h>
#include <Libraries/IR/src/IRrecv.h>
#include <Libraries/IR/src/IRsend.h>
#include <Libraries/IR/src/IRutils.h>

class IR {

public:
    void start();

protected:
    static void receiveIR();

private:
    static Timer  *ir_timer;
    static IRrecv *ir_recv;
    static IRsend *ir_send;

};

#endif