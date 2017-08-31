/////   Variables Described , meaning and intented functions   ///////////////
  // error = 1 Res Is full and tried to mix new nutrients
  // fillstatus = -1  initial state
  // fillstatus = 101   FillBall Open
  // fillstatus = 1001  Fillsoyln Open
  // fillstatus = 102   FillBall Closed
  // fillstatus = 1002  FillSoyln Closed
  // dosedRes = 0  Dose restarted
  // dosedRes = 1-9  Dosing STarted
  // dosedRes = 11 22 33 44  Dosing Completed
  // feedcompleted = 1   Ready to make a Res and Feed
  // feedcompleted = 2   Feedpump on  Full Res
  // feedcompleted = 21  Feedpump off, Ready to make another res
  // feedcompleted = 3   Fill Valves open, Water filling Empty Res
  // run.resfull = -1  No readings taken
  // run.resfull = 0  Res Empty
  // run.resfull = 1  Res Full
  // readWaterLevel Flag   Flag for Timer  (Need to use flagges for internal timers to work)
  // readHumidTemp 	Flag for Temp Humidity reading  (Need to use flagges for internal timers to work)
///      Sensors       /////
  // ReadWaterEmpty A5
  // ReadWaterFull  A6
	// Flow 1   A4
	// Flow 2   A2
	// DHT   A7
	// Serial   RX
  //          TX
//// Functions    //////////
	//	Particle.function("feed", ExFeeder);  // m:#,g:#,b:#,ph:#,xfeed:#
	//  Particle.function("set", SetVar);   //
	//	    SetVar wfull:1  Analogval > Water_Full_Value sensor true
	//		  		 	 0  Analogval < Water_Full_Value sensor read
	//					-1 No sensor
	//			   wempty:1 Analogval > Water_Empty_Value sensor true
	//					  0 Analogval < Water_Empty_Value sensor true
	//					 -1 No sensor
//// Paricle.Variables Exposed Online   ///////
	//////////////////
	//  Particle.variable("FeedStat", feedcompleted);
	//  Particle.variable("FillStat", fillstatus);
	//  Particle.variable("DoseStat", dosedRes);
	//  Particle.variable("FeedCntr", EEPROMData.eevar.FeedCount);
	//  Particle.variable("Totcount", EEPROMData.eevar.FeedCounter);
	//  Particle.variable("ResFull", resful);
	//  Particle.variable("ResEmpty", resempty);
  // mixres    Flag for Feeder timer to Make a new Res
  // ballopen  Flag for Timer to Open Ball valve
  // ballclose  Flag for Timer to close ball valve
  // EEPROMData.eevar.FeedCounter    Counter for # of Res/Feed cycles that occured
  // EEPROMData.eevar.FeedCount      The Count for the counter to count to
  // lastfeedcomplete  // Used to check for changes of the Feedcomplete Flag, and Publish when change occures
//////    DHT 11         ////////////////////////////
#define DHT11 11
#define DHTPIN A7    // Digital pin D2
 // IMPORTANT !! Make sure you set this to your
 // sensor type.  Options: [DHT11, DHT22, DHT21, AM2301]
 #define DHTTYPE DHT11
 #define MAXTIMINGS 85
 #define cli noInterrupts
 #define sei interrupts
 #define DHT22 22
 #define DHT21 21
 #define AM2301 21
 #define NAN 999999
class DHT {
    private:
        uint8_t data[6];
        uint8_t _pin, _type, _count;
        bool read(void);
        unsigned long _lastreadtime;
        bool firstreading;

    public:
        DHT(uint8_t pin, uint8_t type, uint8_t count=6);
        void begin(void);
        float readTemperature(bool S=false);
        float convertCtoF(float);
        float readHumidity(void);

 };
 DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
    _pin = pin;
    _type = type;
    _count = count;
    firstreading = true;
 }
 void DHT::begin(void) {
    // set up the pins!
    pinMode(_pin, INPUT);
    digitalWrite(_pin, HIGH);
    _lastreadtime = 0;
 }
 //boolean S == Scale.  True == Farenheit; False == Celcius
 float DHT::readTemperature(bool S) {
    float _f;

    if (read()) {
        switch (_type) {
            case DHT11:
                _f = data[2];

                if(S)
                    _f = convertCtoF(_f);

                return _f;


            case DHT22:
            case DHT21:
                _f = data[2] & 0x7F;
                _f *= 256;
                _f += data[3];
                _f /= 10;

                if (data[2] & 0x80)
                    _f *= -1;

                if(S)
                    _f = convertCtoF(_f);

                return _f;
        }
    }

    return NAN;
 }


 float DHT::convertCtoF(float c) {
    return c * 9 / 5 + 32;
 }


 float DHT::readHumidity(void) {
    float _f;
    if (read()) {
        switch (_type) {
            case DHT11:
                _f = data[0];
                return _f;


            case DHT22:
            case DHT21:
                _f = data[0];
                _f *= 256;
                _f += data[1];
                _f /= 10;
                return _f;
        }
    }

    return NAN;
 }


 bool DHT::read(void) {
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    unsigned long currenttime;

    // pull the pin high and wait 250 milliseconds
    digitalWrite(_pin, HIGH);
    delay(250);

    currenttime = millis();
    if (currenttime < _lastreadtime) {
        // ie there was a rollover
        _lastreadtime = 0;
    }

    if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
        //delay(2000 - (currenttime - _lastreadtime));
        return true; // return last correct measurement
    }

    firstreading = false;
    Serial.print("Currtime: "); Serial.print(currenttime);
    Serial.print(" Lasttime: "); Serial.print(_lastreadtime);
    _lastreadtime = millis();

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // now pull it low for ~20 milliseconds
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20);
    cli();
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT);

    // read in timings
    for ( i=0; i< MAXTIMINGS; i++) {
        counter = 0;

        while (digitalRead(_pin) == laststate) {
            counter++;
            delayMicroseconds(1);

            if (counter == 255)
                break;
        }

        laststate = digitalRead(_pin);

        if (counter == 255)
            break;

        // ignore first 3 transitions
        if ((i >= 4) && (i%2 == 0)) {
            // shove each bit into the storage bytes
            data[j/8] <<= 1;

            if (counter > _count)
                data[j/8] |= 1;

            j++;
        }
    }

    sei();


    // check we read 40 bits and that the checksum matches
    if ((j >= 40) &&  (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
        return true;


    return false;
 }

 DHT dht(DHTPIN, DHTTYPE);

 float h;      // humidity
 float t;      // temperature
 int f = 0;    // failed?
 char h1[10];  // humidity string
 char t1[10];  // temperature string
TCPClient client;
PRODUCT_ID(5013); // replace by your product ID
PRODUCT_VERSION(5); // increment each time you upload to the console
#define PIN_FEED_PUMP A1
 #define PIN_RELAY_FLUSH D7
 #define PIN_RELAY_FILL D6
 #define PIN_WATER_FILL_BALL_OPEN A0
 #define PIN_WATER_FILL_BALL_CLOSE A2
 #define FLOW1_SENSOR_PIN A4
 #define FLOW2_SENSOR_PIN A3
 #define PIN_MICRO_DOSE D2
 #define PIN_GROW_DOSE D3
 #define PIN_BLOOM_DOSE D4
 #define PIN_PH_DOSE D5
 #define PIN_WATER_FULL A6
 #define PIN_WATER_EMPTY A5

Timer Watertimer(500, setReadWaterFullFlag);   // This Constantly evalues the water level sensor, and set the Res Full Flag, True/False
 Timer FeedPump(300000, FeedStop);
 Timer Dose1(2000, Dose1Stop);
 Timer Dose2(1000, Dose2Stop);
 Timer Dose3(1500, Dose3Stop);
 Timer Dose4(500, Dose4Stop);
 Timer FeedLoop(5000, Feeder);
 Timer PollSensor(300000, setDHTread);  // Should be every 5 min

int rawwater;
int rawwaterEmpty;
int fcntr=0;

struct runing_t {
      //User prefernces,  Ball Fill, Solenoid Fill, Magic Button, Door Alarm, Empty water Sensor,
      uint16_t fillstatus;
      uint16_t resfull;
      uint16_t error;

      uint16_t m;
      uint16_t g;
      uint16_t b;
      uint16_t ph; // stored in seconds

      // uint8_t resful = -1;
      uint16_t resempty;
      uint16_t mixres;
      uint16_t dosedRes;
      uint16_t doseFlush;
      uint16_t ballopened;
      uint16_t ballclosed;
      uint16_t feedcompleted;
      uint16_t lastfeedcomplete;

      bool readWaterLevelFlag;
      bool readAlrmFlag;
      bool readHumidTemp;

      bool alrmOverRide;
      bool alrmPublish;
      uint16_t alrmstatus;

      bool alrmdisarm;
      uint16_t Water_Full_Value = 2000; // Less Than 2000 is a full value.
      uint16_t Water_Empty_Value = 2000; // Greater Than 2000 water is empty

      char publishString[170];
} run;
///// EEProm Object ////////////
struct settings_t {
   //User prefernces,  Ball Fill, Solenoid Fill, Magic Button, Door Alarm, Empty water Sensor,
    uint8_t version;
    bool wfullLogic;  // 1 0   = <  or  >
    bool fillDevice;  // 0 = Ball CR03 wiring   1 = Solenoid/relay

    uint8_t DeviceSensorwEmptypin; // Voltage divide, digital read
    bool sensorwEmptyLogic;

    uint8_t DeviceSensorFlow1pin;  // Flow Pin, 3 wire + in -
    bool sensorFlow1Logic;

    uint8_t DeviceSensorFlow2pin;  //Flow pin, 3 wire + in -
    bool sensorFlow2Logic;

    uint16_t feedpumpofftime;

    uint16_t alrmPin;
    uint16_t alrmAction;
    uint16_t alrmComp;  // sensor compare threshold
    bool alrmLogic;   // 1  =  sensorVal < Threshold
    uint8_t alrmAction2;

   // Would be used for resume
    uint16_t fillstat;   // last state of filling....  might use in reboot
    uint16_t FeedCount;   // last state of filling....  might use in reboot
    uint16_t FeedCounter;   // last state of filling....  might use in reboot
  };
union{
     settings_t eevar;
     char eeArray[sizeof(settings_t)];
} EEPROMData;
struct pintype { // stores Pin usage
  uint8_t _pinD0; // SDA
  uint8_t _pinD1; // SCL
  uint8_t _pinD2; // Micro
  uint8_t _pinD3; // Grow
  uint8_t _pinD4; // Bloom
  uint8_t _pinD5; // Ph
  uint8_t _pinD6; // Fill
  uint8_t _pinD7; // Flush

  uint8_t _pinA0; // Ball open
  uint8_t _pinA1; // Feed
  uint8_t _pinA2; // Ball close
  uint8_t _pinA3; // Flow 2
  uint8_t _pinA4; // Flow 1
  uint8_t _pinA5; // Water Empty
  uint8_t _pinA6; // Water Full
  uint8_t _pinA7; // DHT
} pinstatus;
void readEEPROM(void) {
     for (int i=0; i<sizeof(settings_t); i++) {
         EEPROMData.eeArray[i] = EEPROM.read(i);
     }
  }
void writeEEPROM(void) {
     for (int i=0; i<sizeof(settings_t); i++) {
         EEPROM.write(i, EEPROMData.eeArray[i]);
     }
 }
///////  Mixer and Feeder  ////////////
int Feeder(void){
    // fillstatus = 101   FillBall Open
    // fillstatus = 1001  Fillsoyln Open
    // fillstatus = 102   FillBall Closed
    // fillstatus = 1002  FillSoyln Closed
    // dosedRes = 0  Dose restarted, Once all Dose Timers are completed.
    // dosedRos = 1-9  Dosing STarted
    // dosedRos = 10-100 Dosing Completed
    // feedcompleted = 1   Ready to make a Res and Feed
    // feedcompleted = 3   Fill Valves open, Water filling Empty Res
    // feedcompleted = 2   Feedpump on  Full Res
    // feedcompleted = 21   Feedpump off, Ready to make another res, Just needs to be reset to 1

    if(EEPROMData.eevar.FeedCount >= 1){  // if we still have a count, status here shouold be 1
      if(Dose1.isActive() == 0 && Dose2.isActive() == 0 && Dose3.isActive() == 0 && Dose4.isActive() == 0 && FeedPump.isActive() == 0 ){
    	   if( run.resfull == 1 ){
               FeedPumpOn();  // only feeds if Res reads full  && Dose completed
               return 3;
            }else if( run.resfull == 0 && run.feedcompleted == 1 ){  // 1 == New Fill
               run.mixres = 1;
               return 2;  // Filling
            }else if( run.resfull == 0 && run.feedcompleted == 21 && (run.fillstatus == 1002 || run.fillstatus == 102) ){
               FeedComplete();
               return 4;
            }
        return -1;
      }
   }
 return -1;
 }
int mixReservour(void){
    if (run.mixres == 1 && EEPROMData.eevar.FeedCount != 0 && !FeedPump.isActive() && run.resfull != 1  ){
        run.mixres = 0;
        for(int i=0; i <= 4; i++){  // for statment needed to start each timer in a different cpu cycle
            if(i == 0){
                run.feedcompleted = 3;
                fillCtrl("open", EEPROMData.eevar.fillDevice);    //opens both type of valves.
            }

            if( run.doseFlush == 1 || (run.dosedRes > 0 && run.dosedRes < 9) && (run.dosedRes >= 10 && run.dosedRes < 100) )
                i=500;  // Skips this round of Dosing, Check for Dose Compelted.

            if(i == 1){
                if(run.m > 0){
                run.dosedRes = 1;
                Dose1.changePeriod(run.m * 1000);
                pinCtrl(PIN_MICRO_DOSE, "Dose1", "on");
                }
            }
            if(i == 2){
                if(run.g > 0){
                run.dosedRes = 2;
                Dose2.changePeriod(run.g * 1000);
                pinCtrl(PIN_GROW_DOSE, "Dose2", "on");
                }
            }
            if(i == 3){
                if(run.b > 0){
                run.dosedRes = 4;
                Dose3.changePeriod(run.b * 1000);
                pinCtrl(PIN_BLOOM_DOSE, "Dose3", "on");
                }
            }
            if(i == 4){
                if(run.ph > 0){
                run.dosedRes = 8;
                Dose4.changePeriod(run.ph * 1000);
                pinCtrl(PIN_PH_DOSE, "Dose4", "on");
                }
            }
            if(i == 500){
                DoseCompleted();
            }
        }
    }
 }
void FeedComplete(void){
   if(fcntr >= EEPROMData.eevar.FeedCount){
       fcntr = 0;   // Finished with all feeding
       EEPROMData.eevar.FeedCount = 0;
       writeEEPROM();
   }
   run.feedcompleted = 1;  // 1 == restart feed loop
   run.dosedRes = 0;
 }
////        Sensors               ////////
int sensorOperator(bool logic, int inval, int threshold ){
 // 1 logic    Inval < compare  returns 1  else return 0
 // 0 logic    Inval >= compare  returns 1  else return 0
  if(logic == 1){
      if(inval < threshold){
         return 1;
      }else{
         return 0;
      }
    }else if(logic = 0){
      if(inval >= threshold){
         return 1;
      }else{
         return 0;
      }
    }
 }
void setReadWaterFullFlag(void){
    run.readWaterLevelFlag = 1;
    run.readAlrmFlag = 1;
 }
void ReadWaterLevel(void){
  run.readWaterLevelFlag = 0;
	ReadWaterFull();
	ReadWaterEmpty();
 }
void ReadWaterFull(void){
  rawwater = analogRead(PIN_WATER_FULL);
  run.resfull = sensorOperator(EEPROMData.eevar.wfullLogic, rawwater, run.Water_Full_Value);

  if(run.resfull == 1 && run.ballclosed == 0 ){
    fillCtrl("close", EEPROMData.eevar.fillDevice);
  }

 }
void ReadWaterEmpty(void){
  rawwaterEmpty = analogRead(PIN_WATER_EMPTY);
  run.resempty = sensorOperator(EEPROMData.eevar.sensorwEmptyLogic, rawwaterEmpty, run.Water_Empty_Value);
 }

void pollDHT(){

    f = dht.readTemperature(true);
    h = dht.readHumidity();
    t = dht.readTemperature();

    if (t==NAN || h==NAN) {
        f = 1; // not a number, fail.
    }
    else {
        f = 0; // both numbers! not failed.
        sprintf(h1, "%.2f", h); // convert Float to String
        sprintf(t1, "%.2f", t);
    }

    sprintf(run.publishString,"{\"(f)\":\"%d\",\"rH\":\"%.2f\",\"(c)\":\"%.2f\"}",f,h,t);
    Particle.publish("DHT", run.publishString);
	run.readHumidTemp = 0;
 }
bool ReadAlrmSensor(int alarmPin, int alarmAction, int alarmLogic, int alarmThreshold, int alarmOverride, int disarm){

  if(disarm == 1)
     return 0;
  int alarmraw = analogRead(alarmPin);
  run.alrmstatus = sensorOperator(alarmLogic, alarmraw, alarmThreshold);
  run.readAlrmFlag = 0;
  if(alarmOverride == 1){  // ensures the override is stays on, until toggled via web call, or reboot
     run.alrmstatus = 1;
   }
  if(run.alrmstatus != 1 ){
    AlarmAction(0, alarmAction);
    return 0;
   }else if(run.alrmstatus == 1){
    AlarmAction(1, alarmAction);
    return 1;
   }

 }
int publishAlarm(String args){
   // publish at 5 min internvals while events are set
     run.alrmPublish = 0;

     if(run.alrmdisarm == 1){
       Particle.publish("Alarm", "Disarmed");
     }

     if(run.alrmOverRide == 1){
       Particle.publish("Alarm action", "enabled");
     }
  return 1;
  }
////       Valve Control           ////////
int fillCtrl(String action, bool type){
    if(action == "open"){
        if(type == 1){
            ValveOpen();
            return 1;
        }else if(type == 0){
            BallOpened();
            return 2;
        }
    }else if(action == "close"){
        if(type == 1){
            ValveClose();
            return 11;
        }else if(type == 0){
            BallClosed();
            return 22;
        }
    }
 return -1;
 }
bool BallClosed(void){
    run.ballclosed = 1;
    run.fillstatus = 102;
    pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "BallValveClosing", "on");
    delay(6500);
    pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "BallValveClosing", "off");
    return 1;
 }
bool BallOpened(void){
    run.ballclosed = 0;
    run.fillstatus = 101;
    pinCtrl(PIN_WATER_FILL_BALL_OPEN, "BallOpening", "on");
    delay(2000);
    pinCtrl(PIN_WATER_FILL_BALL_OPEN, "BallOpening", "off");
    return 1;
 }
bool ValveOpen(void){
    run.ballclosed = 0;
    run.fillstatus = 1001;
    pinCtrl(PIN_RELAY_FILL, "ValveOpened", "on");
    return 1;
 }
bool ValveClose(void){
    run.fillstatus = 1002;
    pinCtrl(PIN_RELAY_FILL, "ValveClosed", "off");
    return 1;
 }
////       Dose Control  & Pump   ////////
void DoseCompleted(void){
    if( !Dose1.isActive() && !Dose2.isActive() && !Dose3.isActive() && !Dose4.isActive() ){
        run.dosedRes = 0;
        Particle.publish("Dosing", "Completed");
    }
 }
void Dose1Stop(void){
    run.dosedRes = 10;
    pinCtrl(PIN_MICRO_DOSE, "Dose1", "off");
    Dose1.stop();
    DoseCompleted();
 }
void Dose2Stop(void){
    run.dosedRes = 20;
    pinCtrl(PIN_GROW_DOSE, "Dose2", "off");
    Dose2.stop();
    DoseCompleted();
 }
void Dose3Stop(void){
    run.dosedRes = 40;
    pinCtrl(PIN_BLOOM_DOSE, "Dose3", "off");
    Dose3.stop();
    DoseCompleted();
 }
void Dose4Stop(void){
    run.dosedRes = 80;
    pinCtrl(PIN_PH_DOSE, "Dose4", "off");
    Dose4.stop();
    DoseCompleted();
 }
void DoseStopAll(void){
    pinCtrl(PIN_MICRO_DOSE, "Dose1Forced", "off");
    pinCtrl(PIN_GROW_DOSE, "Dose2Forced", "off");
    pinCtrl(PIN_BLOOM_DOSE, "Dose3Forced", "off");
    pinCtrl(PIN_PH_DOSE, "Dose4Forced", "off");
 }
void FeedPumpOn(void){

    run.feedcompleted = 2;  // 2 = Feed Pump started
    FeedPump.changePeriod(EEPROMData.eevar.feedpumpofftime * 1000);
    FeedPump.start();

    pinCtrl(PIN_FEED_PUMP, "FeedPump", "on");

 }
void FeedStop(void){
    ++fcntr;
    run.feedcompleted = 21;  // 21 = Feed Pump Completed
    pinCtrl(PIN_FEED_PUMP, "FeedPump", "off");
    FeedPump.stop();
 }
bool AlarmAction(bool state, int pinAction){
  if(pinAction == 0){
    return 0;
  }else if(pinAction == 1){  // 0 == 0 disabled , A5 == 1 wempty  A4 == 2 Flow1 A3 == 3 Flow2
    // Ball open pin high   A0
    if(state = 1){
      pinCtrl(PIN_WATER_FILL_BALL_OPEN, "Alarm relay (Bclose)" ,"on");
      return 1;
    }else if(state == 0){
      pinCtrl(PIN_WATER_FILL_BALL_OPEN, "Alarm relay (Bclose)" ,"off");
      return 1;
    }
  }else if(pinAction == 2) {
    // Ball Close  A2
    if(state = 1){
      pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "Alarm relay (Bopen)" ,"on");
      return 1;
    }else if(state == 0){
      pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "Alarm relay (Bopen)" ,"off");
      return 1;
    }
  }else if(pinAction == 3){
    // Fill   D6
    if(state = 1){
      pinCtrl(PIN_RELAY_FILL, "Alarm relay (Fill)" ,"on");
      return 1;
    }else if(state == 0){
      pinCtrl(PIN_RELAY_FILL, "Alarm relay (Fill)" ,"off");
      return 1;
    }
  }
 }
void setDHTread(){
    run.readHumidTemp = 1;

    if(run.alrmstatus == 1)
          run.alrmPublish = 1;
 }
/////      Internet Control         ///////
int AlarmEnabDisab(String a){
  run.alrmdisarm = !run.alrmdisarm;  // Changes the stat

  if(run.alrmdisarm == 1){
    Particle.publish("Alarm", "Disabled");
  }else if(run.alrmdisarm == 0){
    Particle.publish("Alarm", "Enabled");
  }
 return 1;
 }
int AlarmOnOff(String a){
  run.alrmOverRide = !run.alrmOverRide;  // Changes the stat

  if(run.alrmOverRide == 1){
    AlarmAction(1, EEPROMData.eevar.alrmAction);
    Particle.publish("Alarm", "On");
  }else if(run.alrmOverRide == 0){
    AlarmAction(0, EEPROMData.eevar.alrmAction);
    Particle.publish("Alarm", "Off");
  }
 return 1;
 }
int ExreadDHT(String a) {
    run.readHumidTemp = 1;
 return 1;
 }
int SetVar(String args){
  readPubEEPROM("preSetVar");
  int ft = parse_args_int(args, "feedpumpoffin");
  int wf = parse_args_int(args, "wfull");
  int we = parse_args_int(args, "wempty");

  int fill = parse_args_int(args, "fill");

  int alP = parse_args_int(args, "alrmpin");
  int alC = parse_args_int(args, "alrmthresh");
  int alL = parse_args_int(args, "alrmlogic");
  int alAct = parse_args_int(args, "alrmaction");


  if(alAct != -1){
   if(alAct == 1){
       EEPROMData.eevar.alrmAction = 1;
   }else if(alAct == 2){
       EEPROMData.eevar.alrmAction = 2;
   }else if(alAct == 3){
       EEPROMData.eevar.alrmAction = 3;
   }else{
     EEPROMData.eevar.alrmAction = 0;
   }
   }
  if(fill != -1){
    if(fill == 1){
    EEPROMData.eevar.fillDevice = fill; // Solenoid fill device
    EEPROMData.eevar.alrmAction = 1; // ball open
    EEPROMData.eevar.alrmAction2 = 2; // ball close
   }else if(fill == 0){
     EEPROMData.eevar.fillDevice = fill; // ball fill device
     EEPROMData.eevar.alrmAction = 3; // Fill Solenoid
     EEPROMData.eevar.alrmAction2 = 0; //
   }
   }
  if(alP != -1){
   if(alP == 1){
    EEPROMData.eevar.alrmPin = 1;
    }else if(alP == 2){
     EEPROMData.eevar.alrmPin = 2;
    }else if(alP == 3){
     EEPROMData.eevar.alrmPin = 2;
    }else if(alP == 0){
     EEPROMData.eevar.alrmPin = 0;
    }
   }
  if(alC != -1)
    EEPROMData.eevar.alrmComp = alC;
  if(alL != -1)
    EEPROMData.eevar.alrmLogic = alL;
  if(ft != -1)
     EEPROMData.eevar.feedpumpofftime = ft;
  if(wf != -1)
     EEPROMData.eevar.wfullLogic = wf;
  if(we != -1)
     EEPROMData.eevar.sensorwEmptyLogic = we;

  writeEEPROM();
  delay(200);
  readPubEEPROM("SetVar");
 return 1;
 }
int ExFeeder(String args){
    int m = parse_args_int(args, "m");
    int g = parse_args_int(args, "g");
    int b = parse_args_int(args, "b");
    int ph = parse_args_int(args, "ph");
    EEPROMData.eevar.FeedCount = parse_args_int(args, "xfeed");
    writeEEPROM();

    if(m == -1){
       run.m = 0;
    }else{
      run.m = m;
    }

    if(g == -1){
       run.g = 0;
    }else{
      run.g = g;
    }

    if(b == -1){
       run.b = 0;
    }else{
       run.b = b;
    }

    if(ph == -1){
       run.ph = 0;
    }else{
       run.ph = ph;
    }

    if(EEPROMData.eevar.FeedCount == -1)
       EEPROMData.eevar.FeedCount = 0;

    sprintf(run.publishString,"{\"m\": %d, \"g\": %d, \"b\": %d, \"ph\": %d, \"xfeed\": %d, \"feedcnt\": %d}", run.m, run.g, run.b, run.ph, EEPROMData.eevar.FeedCount, fcntr);
    Particle.publish("ExFeeder", run.publishString);
 return 1;
 }
int ExStop(String args){

    EEPROMData.eevar.FeedCount = 0;
    writeEEPROM();

    fillCtrl("close", EEPROMData.eevar.fillDevice);
    DoseStopAll();
    FeedStop();

 return 1;
 }
int ExStart(String args){

    int f = parse_args_int(args, "fill");
    int p = parse_args_int(args, "pump");
    int d1 = parse_args_int(args, "dose1");
    int d2 = parse_args_int(args, "dose2");
    int d3 = parse_args_int(args, "dose3");
    int d4 = parse_args_int(args, "dose4");

    if(f >= 1){
      fillCtrl("open", EEPROMData.eevar.fillDevice);
    }else if(f == 0){
      fillCtrl("close", EEPROMData.eevar.fillDevice);
    }
    if(p >= 1){
      FeedPumpOn();
    }else if(p == 0){
      FeedStop();
    }
    if(d1 >= 1){
      Dose1.changePeriod(d1 * 1000);
      pinCtrl(PIN_MICRO_DOSE, "Dose1", "on");
    }else if(d1 == 0){
      Dose1Stop();
    }
    if(d2 >= 1){
      Dose2.changePeriod(d2 * 1000);
      pinCtrl(PIN_GROW_DOSE, "Dose2", "on");
    }else if(d2 == 0){
      Dose2Stop();
    }
    if(d3 >= 1){
      Dose3.changePeriod(d3 * 1000);
      pinCtrl(PIN_BLOOM_DOSE, "Dose3", "on");
    }else if(d3 == 0){
      Dose3Stop();
    }
    if(d4 >= 1){
      Dose4.changePeriod(d4 * 1000);
      pinCtrl(PIN_PH_DOSE, "Dose4", "on");
    }else if(d4 == 0){
      Dose4Stop();
    }

 return 1;
 }

/////  Parse and Pin Control    /////
int pinCtrl(int pin, String name, String status){
  if(status == "on"){
        digitalWrite(pin, HIGH);
        Log(String(pin) + " : " + name + " : " + status + ":" + run.feedcompleted);
    return 1;
  }else if(status == "off"){
        digitalWrite(pin, LOW);
        Log(String(pin) + " : " + name + " : " + status + ":" + run.feedcompleted );
    return 2;
  }else{
        Log.warn("Pin Control status incorrect Status:" + status  + ":" + run.feedcompleted );
    return -1;
  }
 }
int parse_args_int(String &args, String q){
    if ( !args.length() || !q.length() || args.length() < q.length() )
    return -1;

    if ( q.substring(q.length() - 1, 1) != ":" ) q = q + ":";
    int ql = q.length();

    int r1 = args.indexOf(q);

    if ( r1 != -1 ) {
        int r2 = args.indexOf(",", r1+ql);
        if ( r2 == -1 ) r2 = args.length();
        return args.substring(r1+ql,r2).toInt();
    }
 return -1;
 }
int readPubEEPROM(String a){
  readEEPROM();
  delay(300);
  sprintf(run.publishString,"{\"version\": %d,\"wfullLogic\": %d,\"fillDevice\": %d,\"DevSwEmptypin\": %d,\"fdppofftime\": %d,\"flstat\": %d,\"FdCnt\": %d,\"FCnter\": %d,\"alPin\": %d,\"alAct\": %d,\"alCp\": %d,\"alLog\": %d }",
  EEPROMData.eevar.version, EEPROMData.eevar.wfullLogic, EEPROMData.eevar.fillDevice, EEPROMData.eevar.DeviceSensorwEmptypin, EEPROMData.eevar.feedpumpofftime, EEPROMData.eevar.fillstat, EEPROMData.eevar.FeedCount, fcntr, EEPROMData.eevar.alrmPin, EEPROMData.eevar.alrmAction, EEPROMData.eevar.alrmComp, EEPROMData.eevar.alrmLogic);
  Particle.publish("EEPROM Read " + a, run.publishString);
 return 1;
 }

int publishRunningVariables(String a){
  sprintf(run.publishString,"{\"fstat\": %d,\"rful\": %d,\"err\": %d,\"m\": %d,\"g\":%d,\"b\": %d,\"ph\":%d,\"rful\": %d,\"rempt\":%d,\"dostat\": %d,\"bopen\": %d,\"bclose\":%d,\"fcomp\":%d,\"flgwl\": %d,\"lstfed\": %d,\"cmwful\":%d,\"cmwept\":%d,\"D1\": %d,\"D2\": %d,\"D3\": %d,\"D4\": %d,\"FP\": %d,\"Feedcnt\": %d}",
  run.fillstatus,run.resfull,run.error,run.m,run.g,run.b,run.ph,run.resfull,run.resempty,run.dosedRes,run.ballopened,run.ballclosed,run.feedcompleted,run.readWaterLevelFlag,run.lastfeedcomplete,run.Water_Full_Value,run.Water_Empty_Value,Dose1.isActive(),Dose2.isActive(),Dose3.isActive(),Dose4.isActive(),FeedPump.isActive(), fcntr);
  Particle.publish("Run Vars" + a, run.publishString);
 return 1;
 }
void setup() {
  Particle.connect();
  waitFor(Particle.connected, 30000);

  readPubEEPROM("OnBoot");
  if (EEPROMData.eevar.version != 1 ){
     EEPROMData.eevar.version = 1;
     EEPROMData.eevar.wfullLogic = 1;
     EEPROMData.eevar.fillDevice = 0;
     EEPROMData.eevar.DeviceSensorwEmptypin = 0;
     EEPROMData.eevar.sensorwEmptyLogic = 1;
     EEPROMData.eevar.DeviceSensorFlow1pin = 0;
     EEPROMData.eevar.sensorFlow1Logic = 0;
     EEPROMData.eevar.DeviceSensorFlow2pin = 0;
     EEPROMData.eevar.sensorFlow2Logic = 0;
     EEPROMData.eevar.feedpumpofftime = 200;

     EEPROMData.eevar.FeedCount = 0;

     EEPROMData.eevar.alrmPin = 0;
     EEPROMData.eevar.alrmAction = 0;
     EEPROMData.eevar.alrmAction2 = 0;
     EEPROMData.eevar.alrmComp = 2000;
     EEPROMData.eevar.alrmLogic = 1;


     writeEEPROM();
     delay(200);
     readEEPROM();
     delay(100);
     sprintf(run.publishString,"{\"version\": %d,\"wfullLogic\": %d,\"fillDevice\": %d,\"DeviceSensorwEmptypin\": %d,\"sensorwEmptyLogic\":%d,\"DeviceSensorFlow1pin\": %d,\"sensorFlow1Logic\":%d,\"DeviceSensorFlow2pin\": %d,\"sensorFlow2Logic\":%d,\"feedpumpofftime\": %d,\"FeedCount\": %d,\"FeedCounter\":%d }",
     EEPROMData.eevar.version, EEPROMData.eevar.wfullLogic, EEPROMData.eevar.fillDevice, EEPROMData.eevar.DeviceSensorwEmptypin, EEPROMData.eevar.sensorwEmptyLogic, EEPROMData.eevar.DeviceSensorFlow1pin, EEPROMData.eevar.sensorFlow1Logic, EEPROMData.eevar.DeviceSensorFlow2pin, EEPROMData.eevar.sensorFlow2Logic, EEPROMData.eevar.feedpumpofftime, EEPROMData.eevar.FeedCount, fcntr);
     Particle.publish("EEPROM first write", run.publishString);
   }

   run.fillstatus = 0;
   run.error = 0;
   run.m = 0;
   run.g = 0;
   run.b = 0;
   run.ph = 0; // stored in seconds

   run.mixres = -1;
   run.dosedRes = -1;
   run.doseFlush = -1;

   run.ballopened = 0;

   run.feedcompleted = 1;  // 1 is the start state, 0 will not in the feed routine
   run.readWaterLevelFlag = 0;
   run.readHumidTemp = 0;
   run.readAlrmFlag = 0;

   run.alrmPublish = 0;
   run.alrmstatus = 0;
   run.alrmdisarm = 0;
   run.alrmOverRide = 0;

   run.lastfeedcomplete = 0;

   run.Water_Full_Value = 2000; // Less Than 2000 is a full value.
   run.Water_Empty_Value = 2000; // Greater Than 2000 water is empty

   run.publishString[100];

  Particle.function("feed", ExFeeder);  // m:#,g:#,b:#,ph:#,xfeed:#
  Particle.function("RunVar", publishRunningVariables);
  Particle.function("EEProm", readPubEEPROM);

  Particle.function("start", ExStart);   // Sart,  Pump1 on, Dose1 on,Dose2 on,Dose3 on,Dose4 on, Fill on,
  Particle.function("stop", ExStop);   // Stop,   Pump1 off,  Dose1 Off, Dose2 Off, Dose3 Off, Dose4 Off, Fill Close,
  Particle.function("set", SetVar);     // setting
  Particle.function("ReadTemp", ExreadDHT);   //

  Particle.function("AlrmOnOff", AlarmOnOff);      // Toggle Alarm On and Off
  Particle.function("AlrmEnDis", AlarmEnabDisab);   // Toggle Alarm Disarm

  Particle.variable("ResFull", rawwater);
  Particle.variable("ResEmpty", rawwaterEmpty);

  pinMode(PIN_WATER_FULL, INPUT);
  pinMode(PIN_FEED_PUMP, OUTPUT);
  pinMode(PIN_RELAY_FILL, OUTPUT);
  pinMode(PIN_WATER_FILL_BALL_OPEN, OUTPUT);
  pinMode(PIN_WATER_FILL_BALL_CLOSE, OUTPUT);
  pinMode(FLOW1_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PIN_MICRO_DOSE, OUTPUT);
  pinMode(PIN_GROW_DOSE, OUTPUT);
  pinMode(PIN_BLOOM_DOSE, OUTPUT);
  pinMode(PIN_PH_DOSE, OUTPUT);
  setADCSampleTime(ADC_SampleTime_144Cycles);



  Watertimer.start();
  FeedLoop.start();
  dht.begin();
  PollSensor.start();

 if(EEPROMData.eevar.fillDevice == 0 )
   BallClosed();

   ReadWaterLevel();

  publishRunningVariables("on Statup ");
 }
void loop() {
   if(run.feedcompleted != run.lastfeedcomplete){
        sprintf(run.publishString,"{\"Then\": %d, \"Now\": %d, \"Count\": %d, \"Res\": %d, \"Dose\": %d, \"Fillstatus\": %d}",
            run.lastfeedcomplete, run.feedcompleted, fcntr, run.resfull, run.dosedRes, run.fillstatus );
        Particle.publish("StatusChange", run.publishString);
        run.lastfeedcomplete = run.feedcompleted;
     }

 	 if(run.readWaterLevelFlag == 1)
        ReadWaterLevel();

   if(run.readAlrmFlag == 1)
        ReadAlrmSensor(EEPROMData.eevar.alrmPin, EEPROMData.eevar.alrmAction, EEPROMData.eevar.alrmLogic, EEPROMData.eevar.alrmComp, run.alrmOverRide, run.alrmdisarm);

   if(run.alrmPublish == 1)
      publishAlarm("Active");

   if(run.mixres == 1)
        mixReservour();

   if(run.readHumidTemp == 1 && run.feedcompleted == 1 )  // only check temp/humid when not dosing or feeding
        pollDHT();
 }
