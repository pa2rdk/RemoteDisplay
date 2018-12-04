//Version 2.6 -- 04/12/2018
//2.6 - Added SQL to remote display
#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <wire.h>
#include <EEPROM.h>

#define 	TFT_GREY 	0x5AEB // New colour

char receivedString[32];
bool debug=1;
bool firstRun = 0;
bool btnClicked = 0;
bool gotFromEeprom = 0;
bool sql = 0;
char Cal[7];
byte menuItem = 0;

byte txChannel = 255;
byte rxChannel = 255;
byte txTone = 255;
byte rxTone = 255;
int16_t last;
int16_t value;
byte shiftMode = 0;
long timePassed = 0;


typedef struct{
	byte checkByte;
	char call[7];
	char locatie[12];
	byte channel;  // = auto.
	byte txTone;
	byte rxTone;
} repeaterList;

struct StoreStruct {
	byte chkDigit;
	byte rxChannel;
	byte txChannel;
	byte rxTone;
	byte txTone;
	byte actualRepeater;
};

StoreStruct storage = {
		'X',0,0,0,0,0
};

repeaterList repeater;
byte offsetEEPROM = 10;
byte offsetConfig = 0;
byte chkDigit = '#';
byte actualRepeater = 255;
byte maxRepeater = 30;

String tones[39] = {"None " , "67.0 " , "71.9 " , "74.4 " , "77.0 " , "79.7 " , "82.5 " , "85.4 " , "88.5 " , "91.5 " , "94.8 " , "97.4 " , "100.0" , "103.5" , "107.2" , "110.9" , "114.8" , "118.8" , "123.0" , "127.3" , "131.8" , "136.5" , "141.3" , "146.2" , "151.4" , "156.7" , "162.2" , "167.9" , "173.8" , "179.9" , "186.2" , "192.8" , "203.5" , "210.7" , "218.1" , "225.7" , "233.6" , "241.8" , "250.3"};

TFT_ST7735 tft = TFT_ST7735();  // Invoke library, pins defined in User_Setup.h

void drawGPSBox() {
	tft.drawRect(78, 73, 80, 50, TFT_YELLOW);
	tft.setCursor(89, 69, 1);
	tft.setTextColor(TFT_GREEN, TFT_BLACK);
	tft.setTextSize(1);
	tft.print(F(" GPS INFO "));

	tft.drawRect(0, 73, 78, 50, TFT_RED);
	tft.setCursor(7, 69, 1);
	tft.setTextColor(TFT_GREEN, TFT_BLACK);
	tft.setTextSize(1);
	tft.print(F(" APRS INFO "));
}

void initScreen() {
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0, 2);
	tft.setTextColor(TFT_BLUE, TFT_ORANGE);
	tft.setTextSize(1);
	tft.fillRect(0, 0, 160, 16, TFT_ORANGE);
	tft.println("   PA2RDK APRS TRX  ");
}

void setup(void) {
	pinMode(2, INPUT_PULLUP);
	pinMode(3, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);
	tft.init();
	tft.setRotation(1);

	last = -1;
	firstRun = 0;
	gotFromEeprom = 0;

	Serial.begin(9600);
	Serial.println(F("Start remote display"));
	initScreen();
	drawGPSBox();

	Wire.begin(9);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent); // interrupt handler for when data is wanted
	noInterrupts(); // disable all interrupts
	attachInterrupt(0, isr_2, FALLING);   // Call isr_2 when digital pin 2 goes LOW
	attachInterrupt(1, isr_3, FALLING);   // Call isr_3 when digital pin 3 goes LOW
	interrupts(); // enable all interrupts
	menuItem = 0;
	btnClicked = 0;
	actualRepeater = 255;
	delay(1000);
}

void requestEvent()
{
	Serial.println(F("Sending data"));
	if (firstRun == 1){
		Wire.print(F("#"));
		if (gotFromEeprom == 0){
			gotFromEeprom = 1;
			loadConfig();
			if (storage.chkDigit == chkDigit){
				Serial.println(F("Load config"));
				rxChannel = storage.rxChannel;
				txChannel = storage.txChannel;
				rxTone = storage.rxTone;
				txTone = storage.txTone;
				actualRepeater = storage.actualRepeater;
				menuItem=0;
				if (actualRepeater<255){
					menuItem=4;
					loadRepeater(actualRepeater);
				}
			}
		}
	}
	else
		Wire.print(F("@"));

	Wire.write(txChannel);
	Wire.write(rxChannel);
	Wire.write(txTone);
	Wire.write(rxTone);
	timePassed++;
}

void receiveEvent() {
	int i = 0;
	while (Wire.available()){
		receivedString[i] = Wire.read();
		i++;
	}
	receivedString[i]=0;
	if (i<32)
		processData();
}

void processData(){
	if (receivedString[0] == '#'){
		Serial.println(F("Receiving data"));
		tft.setTextSize(1);
		if (firstRun == 0) initScreen();
		firstRun = 1;
		txChannel = receivedString[2];
		rxChannel = receivedString[3];
		txTone = receivedString[4];
		rxTone = receivedString[5];

		tft.setCursor(3, 80, 1);
		long frq = 1440000+(receivedString[1]*125);  		//APRS Freq
		frq=frq/10;
		tft.setTextColor(TFT_RED,TFT_BLACK);
		tft.print(F("APR:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(frq/1000);
		tft.print(F("."));
		tft.print(frq - ((frq/1000)*1000));

		if (receivedString[16] > 1){
			if (menuItem<4)
				menuItem = 0;
			tft.fillRect(0,18,160,28,TFT_WHITE);
		} else {
			tft.fillRect(0,18,160,28,TFT_BLACK);
		}

		printFreqInfo();

		tft.setCursor(3, 100, 1);
		tft.setTextColor(TFT_RED,TFT_BLACK);
		tft.print(F("TIM:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		if (receivedString[6]<10) tft.print('0');
		tft.print(byte(receivedString[6]));				//Hour
		tft.print(':');
		if (receivedString[7]<10) tft.print('0');
		tft.print(byte(receivedString[7]));				//Minute
		tft.print(':');
		if (receivedString[8]<10) tft.print('0');
		tft.print(byte(receivedString[8]));				//Second

		tft.setCursor(81, 100, 1);
		tft.setTextColor(TFT_YELLOW,TFT_BLACK);
		tft.print(F("Sat:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(byte(receivedString[9]));				//#Sat
		tft.print(F("   "));

		tft.setCursor(81, 80, 1);
		tft.setTextColor(TFT_YELLOW,TFT_BLACK);
		tft.print(F("Lat:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(byte(receivedString[10]));				//Lat (3bytes)
		tft.print('.');
		if (receivedString[11]<10) tft.print('0');
		tft.print(byte(receivedString[11]));
		if (receivedString[12]<10) tft.print('0');
		tft.print(byte(receivedString[12]));

		tft.setCursor(81, 90, 1);
		tft.setTextColor(TFT_YELLOW,TFT_BLACK);
		tft.print(F("Lon:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(byte(receivedString[13]));				//Lon (3bytes)
		tft.print('.');
		if (receivedString[14]<10) tft.print('0');
		tft.print(byte(receivedString[14]));
		if (receivedString[15]<10) tft.print('0');
		tft.print(byte(receivedString[15]));
		long l=0;
		l += long(byte(receivedString[17]))<<24;
		l += long(byte(receivedString[18]))<<16;
		l += long(byte(receivedString[19]))<<8;
		l += long(byte(receivedString[20]));
		tft.setCursor(81, 110, 1);
		tft.setTextColor(TFT_YELLOW,TFT_BLACK);
		tft.print(F("Alt:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(l/100);tft.print(F("."));tft.print(abs(l)-((abs(l)/100)*100));
		tft.print(F("M"));
		tft.print(F("   "));

		if (receivedString[23]=='#'){
			sql = (receivedString[24]==0);
			if (sql==1){
				tft.setTextColor(TFT_RED,TFT_BLACK);
			}
			else
			{
				tft.setTextColor(TFT_GREEN,TFT_BLACK);
			}
			tft.setCursor(140, 52, 1);
			tft.print(F("SQL"));
			memcpy(Cal, &receivedString[25],sizeof(receivedString)-25);
			Cal[sizeof(receivedString)-24] = 0;
		}
		else
		{
			memcpy(Cal, &receivedString[23],sizeof(receivedString)-23);
			Cal[sizeof(receivedString)-22] = 0;
		}

		tft.setCursor(3, 90, 1);
		tft.setTextColor(TFT_RED,TFT_BLACK);
		tft.print(F("CAL:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(Cal);

		tft.setCursor(3, 110, 1);
		tft.setTextColor(TFT_RED,TFT_BLACK);
		tft.print(F("SYM:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(char(receivedString[22]));				//Symbol
		tft.print(' ');
		tft.setTextColor(TFT_RED,TFT_BLACK);
		tft.print(F("SID:"));
		tft.setTextColor(TFT_WHITE,TFT_BLACK);
		tft.print(byte(receivedString[21]));				//SSID
		drawGPSBox();
	}
}

void printFreqInfo() {
	tft.setCursor(110, 24, 1);
	long frq;
	if (receivedString[16] == 1) {
		frq = 40000 + (txChannel * 125); //RX -> TX Freq
		frq=(frq/10)+140000;
		tft.setTextColor(TFT_CYAN, TFT_BLACK);
	} else {
		frq = 40000 + (rxChannel * 125); //Else -> RX Freq
		frq=(frq/10)+140000;
		tft.setTextColor(TFT_CYAN, TFT_WHITE);
	}
	char buff[8];

	sprintf(buff,"%03d.%03d",int(frq/1000),int(frq - ((frq / 1000) * 1000)));
	tft.print(buff);

	tft.setCursor(3, 20, 4);
	if (receivedString[16] == 1) {			//1=RX, 2=TX, 3=Beacon
		frq = 40000 + (rxChannel * 125); 	//RX -> RX Freq
		frq=(frq/10)+140000;
		tft.setTextColor(TFT_CYAN, TFT_BLACK);
	}
	if (receivedString[16] == 2) {
		frq = 40000 + (txChannel * 125);		 //TX -> TX Freq
		frq=(frq/10)+140000;
		tft.setTextColor(TFT_RED, TFT_WHITE);
	}
	if (receivedString[16] == 3) {
		frq = 40000 + (receivedString[1] * 125); //Beacon ->APRS Freq
		frq=(frq/10)+140000;
		tft.setTextColor(TFT_BLUE, TFT_WHITE);
	}
	sprintf(buff,"%03d.%03d",int(frq/1000),int(frq - ((frq / 1000) * 1000)));
	tft.print(buff);

	if (menuItem == 1)
		tft.setTextColor(TFT_BLACK, TFT_CYAN);

	if (txChannel < rxChannel){
		tft.print(F("-"));
		shiftMode=2;
	}

	if (txChannel > rxChannel){
		tft.print(F("+"));
		shiftMode=1;
	}

	if (txChannel == rxChannel){
		tft.print(F("="));
		shiftMode=0;
	}

	if (menuItem == 1)
		tft.setTextColor(TFT_CYAN, TFT_BLACK);

	if (menuItem >1 && menuItem<4)
		tft.setTextColor(TFT_BLACK, TFT_CYAN);

	tft.setCursor(110, 34, 1);
	if (menuItem==3) tft.print(F("TT:")); else tft.print(F("TR:"));
	if (receivedString[16] == 2 || menuItem==3) {
		tft.print(tones[txTone]); //TX Tone
	} else {
		tft.print(tones[rxTone]); //RX Tone
	}
	if (menuItem >1)
		tft.setTextColor(TFT_CYAN, TFT_BLACK);

	tft.setTextColor(TFT_YELLOW, TFT_BLACK);
	if (actualRepeater<255 && menuItem==4){
		tft.setCursor(5, 46, 2);
		tft.print(repeater.call);
		tft.print(F(" "));
		tft.setCursor(60, 52, 1);
		tft.print(repeater.locatie);
		tft.print(F(" "));
	}
	else
	{
		tft.setCursor(5, 46, 2);
		tft.print(F("        "));
		tft.setCursor(60, 52, 1);
		tft.print(F("                  "));
	}
}

void loop(){
	if (btnClicked==1){
		menuItem++;
		if (menuItem==4){
			actualRepeater=0;
			loadRepeater(0);
		}
		if (menuItem==5){
			menuItem=0;
			actualRepeater = 255;
		}
		last = value;
		btnClicked=0;
		printFreqInfo();
		timePassed = 0;
	}
	if (menuItem==0){
		if (value != last) {
			if (value<last)
			{
				rxChannel--;
				if (rxChannel==255) rxChannel = 160;
				last = value;
			}
			if (value>last)
			{
				rxChannel++;
				if (rxChannel==161) rxChannel = 0;
				last = value;
			}
			if (rxChannel<126){
				txChannel=rxChannel;
				txTone = 0;
				rxTone = 0;
			} else {
				txChannel = rxChannel-48;
			}
			printFreqInfo();
			timePassed = 0;
		}
	}
	if (menuItem==1){
		if (value != last) {
			shiftMode++;
			if (shiftMode==3) shiftMode=0;
			if (shiftMode==2){	//- shift
				txChannel=rxChannel-48;
			}
			if (shiftMode==0){	//No shift
				txChannel=rxChannel;
			}
			if (shiftMode==1){	//+ shift
				txChannel=rxChannel+ 48;
			}
			printFreqInfo();
			timePassed = 0;
			last = value;
		}
	}
	if (menuItem==2){
		if (value != last) {
			if (value<last)
			{
				rxTone--;
				if (rxTone==255) rxTone = 38;
				last = value;
			}
			if (value>last)
			{
				rxTone++;
				if (rxTone==39) rxTone = 0;
				last = value;
			}
			txTone=rxTone;
			printFreqInfo();
			timePassed = 0;
		}
	}
	if (menuItem==3){
		if (value != last) {
			if (value<last)
			{
				txTone--;
				if (txTone==255) txTone = 38;
				last = value;
			}
			if (value>last)
			{
				txTone++;
				if (txTone==39) rxTone = 0;
				last = value;
			}
			printFreqInfo();
			timePassed = 0;
		}
	}
	if (menuItem==4){
		if (value != last) {
			if (value<last)
			{
				actualRepeater--;
				if (actualRepeater==255) actualRepeater = 0;
				last = value;
			}
			if (value>last)
			{
				actualRepeater++;
				if (actualRepeater>maxRepeater) actualRepeater = maxRepeater;
				last = value;
			}
			loadRepeater(actualRepeater);
			printFreqInfo();
			timePassed = 0;
		}
	}
	if (timePassed==5){
		Serial.println(F("Save config"));
		storage.chkDigit = chkDigit;
		storage.rxChannel = rxChannel;
		storage.txChannel = txChannel;
		storage.rxTone = rxTone;
		storage.txTone = txTone;
		storage.actualRepeater = actualRepeater;
		saveConfig();
		timePassed++;
	}
}

void loadRepeater(int i) {
	unsigned int l = i*23;

	if (EEPROM.read(offsetEEPROM + l) == chkDigit){
		for (int t=l; t < l+23; t++)
			*((char*)&repeater + (t-l)) = EEPROM.read(offsetEEPROM + t);

		rxChannel=repeater.channel;
		txChannel=rxChannel-48;
		txTone=repeater.txTone;
		rxTone=repeater.rxTone;
	}
}

void isr_2(){                                              // Pin2 went LOW
	delay(1);                                              // Debounce time
	if(digitalRead(2) == LOW){                             // Pin2 still LOW ?
		if(digitalRead(4)== HIGH)
			value--;
		else
			value++;
	}
}

void isr_3(){                                              // Pin2 went LOW
	delay(100);                                              // Debounce time
	btnClicked=1;
}

void saveConfig() {
	for (unsigned int t = 0; t < sizeof(storage); t++)
		EEPROM.update(offsetConfig + t, *((char*)&storage + t));
}

void loadConfig() {
	if (EEPROM.read(offsetConfig + 0) == chkDigit)
		for (unsigned int t = 0; t < sizeof(storage); t++)
			*((char*)&storage + t) = EEPROM.read(offsetConfig + t);
}





