 #include "RobbieComms.h"

RobbieComms::RobbieComms(uint8_t id) {
	deviceId = id;
}

void RobbieComms::begin() {
	
	digitalWrite(txEnablePin, LOW);
	pinMode(txEnablePin, OUTPUT);
	ETin.begin(details(incomingMessage), &Serial);
	ETout.begin(details(outgoingMessage), &Serial);
}

void RobbieComms::setCallBacks(void (*fpNg)(), void (*fpEg)(), void (*fpDg)()) {
	fpNewGame = fpNg;
	fpEnableGame = fpEg;
	fpDisableGame = fpDg;
}

bool RobbieComms::gameEnabled() {
	return _gameEnabled;
}

void RobbieComms::gameEnabled(bool enabled) {
	_gameEnabled = enabled;
}

bool RobbieComms::gameRunning() {
	return _gameRunning;
}

void RobbieComms::gameRunning(bool running) {
	_gameRunning = running;
}

void RobbieComms::gameState(uint8_t state) {
	_gameState = state;
}

bool RobbieComms::gameState() {
	return _gameState;
}

void RobbieComms::prepMessage(uint8_t to_id) {
	prepMessage(to_id, false);
}

void RobbieComms::prepMessage(uint8_t to_id, bool msg_reset) {
	outgoingMessage.message = 0x00;
	if (_gameEnabled) {
		outgoingMessage.message |= MESSAGE_Enabled;
	}
	if (_gameRunning) {
		outgoingMessage.message |= MESSAGE_GameRunning;
	}
	if (msg_reset) {
		outgoingMessage.message |= MESSAGE_Reset;
	}
	outgoingMessage.to_device_id = to_id;
	outgoingMessage.from_device_id = deviceId;
}
bool RobbieComms::masterRequestStatus(uint8_t id) {
	return masterRequestStatus(id, DEFAULT_TIMEOUT);
}

bool RobbieComms::masterRequestStatus(uint8_t id, uint16_t timeout ) {
	uint32_t start = millis();
	prepMessage(id, false);
	if (sendMessage()) {
		
		while (!masterReceive()) {
			if ((millis() - start) > timeout) {
				return false;
			}
		}
		if (incomingMessage.from_device_id == id) {
			return true;
		}
	} 
	return false;
}

bool RobbieComms::masterAnnounce(bool msg_reset) {
	prepMessage(ANNOUNCE_ADDR, msg_reset);
	return sendMessage();
}


		
bool RobbieComms::sendMessage() {
	ETout.sendData();	
	return true;
}

bool RobbieComms::masterReceive() {
	if (!ETin.receiveData()) {
		return false;
	}
	return true;
}

bool RobbieComms::receiveMessage() {
	if (!ETin.receiveData()) {
		return false;
	}
	if (incomingMessage.to_device_id == deviceId) {
		prepMessage(incomingMessage.from_device_id);
		outgoingMessage.game_state = _gameState;
		sendMessage();
		triggerEvents();
	}
	if (incomingMessage.to_device_id == ANNOUNCE_ADDR) {
		triggerEvents();
	}
	return true;
}

void RobbieComms::triggerEvents() {
	if (incomingMessage.message & MESSAGE_Reset) {
		if ( 0 != fpNewGame) {
			(*fpNewGame)();
		}
	}
	if ((incomingMessage.message & MESSAGE_GameRunning) != _gameRunning) {
		_gameRunning = (bool)(incomingMessage.message & MESSAGE_GameRunning);
	}
	if ((incomingMessage.message & MESSAGE_Enabled) != _gameEnabled) {
		_gameEnabled = (bool)(incomingMessage.message & MESSAGE_Enabled);
		if (_gameEnabled) {
			if ( 0 != fpEnableGame) {
				(*fpEnableGame)();
			}
		} else {
			if ( 0 != fpDisableGame) {
				(*fpDisableGame)();
			}
		}
	}
}

