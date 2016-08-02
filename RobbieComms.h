#include <EasyTransfer.h>
#include <Arduino.h>

struct MESSAGE {
  uint8_t to_device_id;
  uint8_t from_device_id;
  uint8_t message;
  uint8_t game_state;
};
#define COMM_BAUD 9600

#define ANNOUNCE_ADDR 0xFF

#define DEFAULT_TIMEOUT 2000

#define MESSAGE_Reset 0x01
#define MESSAGE_Enabled 0x02
#define MESSAGE_GameRunning 0x04

#define GAMESTATE_Unsolved  0
#define GAMESTATE_Solved  1

class RobbieComms {
	public:
		uint8_t deviceId;
		MESSAGE outgoingMessage;
		MESSAGE incomingMessage;
		EasyTransfer ETin, ETout;
		RobbieComms(uint8_t id);
		void begin();
		void setCallBacks(void (*fpNg)(), void (*fpEg)(), void (*fpDg)());
		
		bool sendMessage();
		bool receiveMessage();
		bool masterReceive();
		
		void gameState(uint8_t state);
		bool gameState();
		
		bool gameEnabled();
		void gameEnabled(bool enabled);
		
		bool gameRunning();
		void gameRunning(bool running);
		
		bool masterAnnounce(bool reset);
		bool masterRequestStatus(uint8_t id );
		bool masterRequestStatus(uint8_t id, uint16_t timeout);
		void prepMessage(uint8_t to_id);
		void prepMessage(uint8_t to_id, bool msg_reset);
		
	private:
		bool _gameState;
		bool _gameEnabled = false;
		bool _gameRunning = false;
		int txEnablePin;
		int rxPin;
		int txPin;
		void (*fpNewGame)() = 0;
		void (*fpEnableGame)() = 0;
		void (*fpDisableGame)() = 0;
		void triggerEvents();
};

