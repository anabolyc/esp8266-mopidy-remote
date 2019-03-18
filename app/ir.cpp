#include "ir.h"

Timer  *IR::ir_timer = new Timer();
IRrecv *IR::ir_recv = new IRrecv(IR_RECV_PIN);
IRsend *IR::ir_send = new IRsend(IR_SEND_PIN);

void IR::start() {

	ir_recv->enableIRIn();
	Serial.println();
	Serial.print("IRrecvDemo is now running");
    
    ir_timer->initializeMs(200, receiveIR).start();
}

void IR::receiveIR()
{
	ir_timer->stop();
	decode_results dresults;
	dresults.decode_type = UNUSED;
	if(ir_recv->decode(&dresults)) {
		Serial.println(resultToHumanReadableBasic(&dresults)); // Output the results as source code
		//Serial.println(resultToTimingInfo(&dresults));
	}
	if(dresults.decode_type > UNUSED) {
		Serial.println("Send IR Code");
		ir_send->send(dresults.decode_type, dresults.value, dresults.bits);
	}
	ir_timer->start();
}