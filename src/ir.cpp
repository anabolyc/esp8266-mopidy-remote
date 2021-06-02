#include "ir.h"
#include "mopidy.h"
#include <Timer.h>

Timer* _ir_timer = NULL;
IRrecv IR::ir_recv(IR_RECV_PIN);
decode_results dresults;

void IR::start(Timer* timer)
{
	ir_recv.enableIRIn();
	Serial.println(F("IR service has started"));

	_ir_timer = timer;
	_ir_timer->after(IR_CHECK_EVERY_MS, receiveIR);
	Mopidy::start(timer);
}

void IR::receiveIR()
{
	dresults.decode_type = UNUSED;
	if (ir_recv.decode(&dresults))
	{
		serialPrintUint64(dresults.value, HEX);
		Serial.println();

		switch (dresults.value)
		{
		case BTN_A:
		case BTN_0A:
		 	Mopidy::toggleState();
		 	break;
		case BTN_B:
		//case BTN_STOP:
		case BTN_0STOP:
		 	Mopidy::stop();
			break;
		//case BTN_PLAY:
		case BTN_0PLAY:
		 	Mopidy::play();
			break;
		case BTN_C:
		//case BTN_VOLDN:
		case BTN_0VOLDN:
		 	Mopidy::volumeDown();
			break;
		case BTN_D:
		//case BTN_VOLUP:
		case BTN_0VOLUP:
		 	Mopidy::volumeUp();
			break;
		//case BTN_NEXT:
		case BTN_0NEXT:
			Mopidy::next();
			break;
		//case BTN_PREV:
		case BTN_0PREV:
			Mopidy::prev();
			break;
		//case BTN_MUTE:
		case BTN_0MUTE:
			Mopidy::toggleMute();
			break;
		//case BTN_1:
		case BTN_01:
			Mopidy::playTrackNo(0);
			break;
		//case BTN_2:
		case BTN_02:
			Mopidy::playTrackNo(1);
			break;
		//case BTN_3:
		case BTN_03:
			Mopidy::playTrackNo(2);
			break;
		//case BTN_4:
		case BTN_04:
			Mopidy::playTrackNo(3);
			break;
		//case BTN_5:
		case BTN_05:
			Mopidy::playTrackNo(4);
			break;
		//case BTN_6:
		case BTN_06:
			Mopidy::playTrackNo(5);
			break;
		//case BTN_7:
		case BTN_07:
			Mopidy::playTrackNo(6);
			break;
		//case BTN_8:
		case BTN_08:
			Mopidy::playTrackNo(7);
			break;
		//case BTN_9:
		case BTN_09:
			Mopidy::playTrackNo(8);
			break;
		//case BTN_0:
		case BTN_00:
			Mopidy::playTrackNo(9);
			break;
		}

		ir_recv.resume();
	}
	_ir_timer->after(IR_CHECK_EVERY_MS, receiveIR);
}