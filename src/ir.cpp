#include "ir.h"
#include "mopidy.h"
#include <Timer.h>

Timer* _ir_timer = NULL;
IRrecv IR::ir_recv(IR_RECV_PIN, IR_BUF_SIZE, IR_TIMEOUT, true);
decode_results dresults;

void IR::start(Timer* timer)
{
	ir_recv.setTolerance(IR_TOLERANCE);
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
		case BTN_NEC_A:
		 	Mopidy::toggleState();
		 	break;
		case BTN_B:
		case BTN_NEC_STOP:
		 	Mopidy::stop();
			break;
		case BTN_NEC_PLAY:
		 	Mopidy::play();
			break;
		case BTN_C:
		case BTN_NEC_VOLDN:
		 	Mopidy::volumeDown();
			break;
		case BTN_D:
		case BTN_NEC_VOLUP:
		 	Mopidy::volumeUp();
			break;
		case BTN_NEC_NEXT:
			Mopidy::next();
			break;
		case BTN_NEC_PREV:
			Mopidy::prev();
			break;
		// case BTN_NEC_MUTE:
		// 	Mopidy::toggleMute();
		// 	break;
		case BTN_NEC_1:
			Mopidy::playTrackNo(0);
			break;
		case BTN_NEC_2:
			Mopidy::playTrackNo(1);
			break;
		case BTN_NEC_3:
			Mopidy::playTrackNo(2);
			break;
		case BTN_NEC_4:
			Mopidy::playTrackNo(3);
			break;
		case BTN_NEC_5:
			Mopidy::playTrackNo(4);
			break;
		case BTN_NEC_6:
			Mopidy::playTrackNo(5);
			break;
		case BTN_NEC_7:
			Mopidy::playTrackNo(6);
			break;
		case BTN_NEC_8:
			Mopidy::playTrackNo(7);
			break;
		case BTN_NEC_9:
			Mopidy::playTrackNo(8);
			break;
		case BTN_NEC_0:
			Mopidy::playTrackNo(9);
			break;
		case BTN_NEC_TOGGLE_PLAYLIST:
			Mopidy::togglePlaylist();
			break;
		}

		ir_recv.resume();
	}
	_ir_timer->after(IR_CHECK_EVERY_MS, receiveIR);
}