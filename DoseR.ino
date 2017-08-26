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
  // resful = -1  No readings taken
  // resful = 0  Res Empty
  // resful = 1  Res Full
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
	//  Particle.variable("FeedCntr", feedcount);
	//  Particle.variable("Totcount", feedcounter);
	//  Particle.variable("ResFull", resful);
	//  Particle.variable("ResEmpty", resempty);
  // mixres    Flag for Feeder timer to Make a new Res
  // ballopen  Flag for Timer to Open Ball valve
  // ballclose  Flag for Timer to close ball valve
  // feedcounter    Counter for # of Res/Feed cycles that occured
  // feedcount      The Count for the counter to count to
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
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
TCPClient client;

PRODUCT_ID(5013); // replace by your product ID
PRODUCT_VERSION(5); // increment each time you upload to the console

#define PIN_FEED_PUMP A1
 #define PIN_RELAY_FILL D6
 #define PIN_WATER_FILL_BALL_OPEN A0
 #define PIN_WATER_FILL_BALL_CLOSE A2
 #define FEED_FLOW_SENSOR_PIN A4
 #define PIN_MICRO_DOSE D2
 #define PIN_GROW_DOSE D3
 #define PIN_BLOOM_DOSE D4
 #define PIN_PH_DOSE D5
 #define PIN_WATER_FULL A6
 #define PIN_WATER_EMPTY A5
const int Water_Full_Value = 2000; // Less Than 2000 is a full value.
 const int Water_Empty_Value = 2000; // Greater Than 2000 water is empty
int error = 0;
 int resful = -1;
 int resempty = -1;
 int mixres = 0;
 int dosedRes = 0;
 int doseFlush = 0;
 int ballopened = 0;
 int ballclosed = 0;
bool readWaterLevelFlag = 0;
 bool readHumidTemp = 0;
uint8_t WaterFullLogic = 0;   // -1 not set,  1 > Water_Full_Value, 0 < Water_Full_Value
 uint8_t WaterEmptyLogic = 1;  // -1 not set,  1 > Water_Empty_Value, 0 < Water_Full_Value
 uint8_t feedcounter = 0;
 uint8_t feedcount = 0;
 uint8_t feedcompleted = 1;
 uint8_t lastfeedcomplete = 1;
uint16_t fillstatus = -1;
 uint16_t feedpumpoffin = 200;  //  in seconds
unsigned long m = 0;
 unsigned long g = 0;
 unsigned long b = 0;
 unsigned long ph = 0;
char publishString[200];
 // Timer tSampler(frame_duration_ms, ProcessSamples);
Timer Watertimer(500, setReadWaterFullFlag);   // This Constantly evalues the water level sensor, and set the Res Full Flag, True/False
 Timer FeedPump(0, FeedStop);
 Timer Dose1(2000, Dose1Stop);
 Timer Dose2(1000, Dose2Stop);
 Timer Dose3(1500, Dose3Stop);
 Timer Dose4(500, Dose4Stop);
 Timer FeedLoop(5000, Feeder);
 Timer PollSensor(300000, setDHTread);  // Should be every 5 min

///// EEProm Object ////////////
struct settings_t {
  //User prefernces,  WaterFullLogic, Ball Fill, Solenoid Fill, Magic Button, Door Alarm, Empty water Sensor, WaterEmptyLogic,
   uint8_t version;
   uint8_t wfullLogic;  // 1 0   = <  or  >
   uint8_t fillDevice;  // 1 = Ball CR03 wiring   2 = Solenoid/relay
   uint8_t sensorwEmptypin; // Voltage divide, digital read
   uint8_t sensorFlow1pin;  // Flow Pin, 3 wire + in -
   uint8_t sensorFlow2pin;  //Flow pin, 3 wire + in -
   uint16_t feedpumpofftime;

  // Would be used for resume
   uint16_t fillstat;   // last state of filling....  might use in reboot
   uint8_t statsFeedCount;   // last state of filling....  might use in reboot
   uint8_t statsFeedremaining;   // last state of filling....  might use in reboot
};

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

   if (resful == -1)   // -1 means the res full sensor has not been evaluated or is in some type of error.   0 = empty   1 = full
        return -10;
   if( (fillstatus == 1002 || fillstatus == 102) && resful == 0 && !FeedPump.isActive() && !Dose1.isActive() && !Dose2.isActive() && !Dose3.isActive() && !Dose4.isActive() && feedcompleted != 1 ){  // 21 == Feed Pump  Off   Done pumping,   Check if we're all done or just done this cycle
       FeedComplete();
       return 3;
    }
 //   if (resempty == 1 && feedcompleted != 1 ){   // prevents loop of constantly turning off Feed when Res is empty
 //        FeedStop();  // turns off if the res empty sensor is true, Else timer turns it off
 //        FeedComplete();
 //		return 4;
 //	}

   if(feedcount){  // if we still have a count, status here shouold be 1

	   if( resful == 1 && !Dose1.isActive() && !Dose2.isActive() && !Dose3.isActive() && !Dose4.isActive() && !FeedPump.isActive() ){
           FeedPumpOn();  // only feeds if Res reads full  && Dose completed
           return 2;
       }else if( resful == 0 && feedcompleted == 1 && !FeedPump.isActive() ){  // 1 == New Fill
           mixres = 1;
           return 1;  // Filling
       }
   }

 return -1;
}
int mixReservour(void){
    if (mixres == 1 && feedcount != 0 && !FeedPump.isActive() && resful != 1  ){
        mixres = 0;
        for(int i=0; i <= 4; i++){  // for statment needed to start each timer in a different cpu cycle
            if(i == 0){
                feedcompleted = 3;
                fillCtrl("open", "ball");    //opens both type of valves.
                fillCtrl("open", "soyln");   // fillCtrl("open", "latch");
            }
            if( doseFlush == 1 || (dosedRes > 0 && dosedRes < 9) && (dosedRes >= 10 && dosedRes < 100) )
                i=500;  // Skips this round of Dosing, Check for Dose Compelted.
            if(i == 1){
                if(m > 0){
                    dosedRes = dosedRes + 1;
                    Dose1.changePeriod(m);
                    pinCtrl(PIN_MICRO_DOSE, "Dose1", "on");
                }
            }
            if(i == 2){
                if(g > 0){
                dosedRes = dosedRes + 2;
                Dose2.changePeriod(g);
                pinCtrl(PIN_GROW_DOSE, "Dose2", "on");
                }
            }
            if(i == 3){
                if(b > 0){
                dosedRes = dosedRes + 4;
                Dose3.changePeriod(b);
                pinCtrl(PIN_BLOOM_DOSE, "Dose3", "on");
                }
            }
            if(i == 4){
                if(ph > 0){
                dosedRes = dosedRes + 8;
                Dose4.changePeriod(ph);
                pinCtrl(PIN_PH_DOSE, "Dose4", "on");
                }
            }
            if(i == 500){
                DoseCompleted();
            }
        }
    }
 error = 1; // Res Is full and tried to mix new nutrients
}
void FeedComplete(void){
   if(feedcounter >= feedcount){
       feedcounter = 0;   // Finished with all feeding
       feedcount = 0;
   }
   feedcompleted = 1;  // 1 == restart feed loop
   dosedRes = 0;
}

////        Sensors               ////////
void setReadWaterFullFlag(void){
    readWaterLevelFlag = 1;
}
void ReadWaterLevel(void){
  readWaterLevelFlag = 0;
	ReadWaterFull();
	ReadWaterEmpty();
}
void ReadWaterFull(void){
  int rawwater = analogRead(PIN_WATER_FULL);
  if( WaterFullLogic == 1 ){
		if ( rawwater < Water_Full_Value ){
			resful = 1;
			closeFill();
			return;
		}else if( rawwater >= Water_Full_Value){
			resful = 0;
			return;
		}
	}
  if( WaterFullLogic == 0 ){
		if ( rawwater > Water_Full_Value ){
			resful = 1;
			closeFill();
			return;
		}else if( rawwater <= Water_Full_Value){
			resful = 0;
			return;
		}
	}
 resful = -1;
 return;
}
void ReadWaterEmpty(void){
  int rawwaterEmpty = analogRead(PIN_WATER_EMPTY);
	if(	WaterEmptyLogic == 1) {
		if ( rawwaterEmpty < Water_Empty_Value ){
			resempty = 1;
			return;
		}else if( rawwaterEmpty >= Water_Empty_Value){
			resempty = 0;
			return;
		}
	}
	if(	WaterEmptyLogic == 0) {
		if ( rawwaterEmpty > Water_Empty_Value ){
			resempty = 1;
			return;
		}else if( rawwaterEmpty <= Water_Empty_Value){
			resempty = 0;
			return;
		}
	}
 resempty = -1;
 return;
}
void closeFill(void){

    if(ballclosed == 0){
        fillCtrl("close", "ball");
        ballclosed = 1;
    }

    fillCtrl("close", "soyln");
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

    sprintf(publishString,"{\"(f)\":\"%d\",\"rH\":\"%.2f\",\"(c)\":\"%.2f\"}",f,h,t);
    Particle.publish("DHT", publishString);
	readHumidTemp = 0;
}


////       Valve Control           ////////
int fillCtrl(String action, String type){
    if(action == "open"){
        if(type == "soyln"){
            ValveOpen();
            return 1;
        }else if(type == "ball"){
            BallOpened();
            return 2;
        }
    }else if(action == "close"){
        if(type == "soyln"){
            ValveClose();
            return 11;
        }else if(type == "ball"){
            BallClosed();
            return 22;
        }
    }
 return -1;
}
bool BallClosed(void){
    fillstatus = 102;
    pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "BallValveClosing", "on");
    delay(6500);
    pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "BallValveClosing", "off");
    return 1;
}
bool BallOpened(void){
    ballclosed = 0;
    fillstatus = 101;
    pinCtrl(PIN_WATER_FILL_BALL_OPEN, "BallOpening", "on");
    delay(2000);
    pinCtrl(PIN_WATER_FILL_BALL_OPEN, "BallOpening", "off");
    return 1;
}
bool ValveOpen(void){
    fillstatus = 1001;
    pinCtrl(PIN_RELAY_FILL, "ValveOpened", "on");
    return 1;
}
bool ValveClose(void){
    fillstatus = 1002;
    pinCtrl(PIN_RELAY_FILL, "ValveClosed", "off");
    return 1;
}

////       Dose Control  & Pump   ////////
void DoseCompleted(void){
    if( !Dose1.isActive() && !Dose2.isActive() && !Dose3.isActive() && !Dose4.isActive() )
        dosedRes = 0;
}
void Dose1Stop(void){
    dosedRes = dosedRes + 10;
    pinCtrl(PIN_MICRO_DOSE, "Dose1", "off");
    Dose1.stop();
    DoseCompleted();
}
void Dose2Stop(void){
    dosedRes = dosedRes + 20;
    pinCtrl(PIN_GROW_DOSE, "Dose2", "off");
    Dose2.stop();
    DoseCompleted();
}
void Dose3Stop(void){
    dosedRes = dosedRes + 40;
    pinCtrl(PIN_BLOOM_DOSE, "Dose3", "off");
    Dose3.stop();
    DoseCompleted();
}
void Dose4Stop(void){
    dosedRes = dosedRes + 80;
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
    feedcounter++;
    feedcompleted = 2;  // 2 = Feed Pump started
    FeedPump.changePeriod(feedpumpoffin);
    pinCtrl(PIN_FEED_PUMP, "FeedPump", "on");
    FeedPump.start();
}
void FeedStop(void){
    feedcompleted = 21;  // 21 = Feed Pump Completed
    pinCtrl(PIN_FEED_PUMP, "FeedPump", "off");
    FeedPump.stop();
}

void setDHTread(){
    readHumidTemp = 1;
}

/////      Internet Control           /////
int ExreadDHT(String a) {
    readHumidTemp = 1;
 return 1;
}
int SetVar(String args){

  int f = parse_args_int(args, "feedpumpoffin");
  int wf = parse_args_int(args, "wfull");
  int we = parse_args_int(args, "wempty");

  if(feedpumpoffin != -1)
     feedpumpoffin = f * 1000;  // converts ms to seconds
  if(WaterFullLogic != -1)
     WaterFullLogic = wf;
  if(WaterEmptyLogic == -1)
     WaterEmptyLogic = we;

  sprintf(publishString,"{\"FullLogic\": %d, \"EmptyLogic\": %d, \"FeedPumpOffin\": %d, \"EEPROMEmptyLogic\": %d, \"EEPROMEnableOS\": %d}", WaterFullLogic, WaterEmptyLogic, feedpumpoffin);
  Particle.publish("SetVar", publishString);

 return 1;
}
int ExPump(String args){
    int on = parse_args_int(args, "on");
    int off = parse_args_int(args, "off");

    if (off == 1){
        FeedStop();
        return 2;
    }

    if (on == 1){
        FeedPumpOn();
        return 1;
    }

    sprintf(publishString,"{\"resfull\": %d, \"status\": %d, \"m\": %d, \"g\": %d, \"b\": %d, \"ph\": %d, \"xfeed\": %d, \"feedcnt\": %d}", resful, feedcompleted, m, g, b, ph, feedcount, feedcounter);    Particle.publish("Feed Status", publishString);
    Particle.publish("ExPump", publishString);

 return -1;
}
int ExFeeder(String args){
    m = parse_args_int(args, "m");
    g = parse_args_int(args, "g");
    b = parse_args_int(args, "b");
    ph = parse_args_int(args, "ph");
    feedcount = parse_args_int(args, "xfeed");

    if(m == -1)
       m = 0;
    if(g == -1)
       g = 0;
    if(b == -1)
       b = 0;
    if(ph == -1)
       ph = 0;
    if(feedcount == -1)
       feedcount = 0;

    m = m * 1000;
    g = g * 1000;
    b = b * 1000;
    ph = ph * 1000;

    sprintf(publishString,"{\"m\": %d, \"g\": %d, \"b\": %d, \"ph\": %d, \"xfeed\": %d, \"feedcnt\": %d}", m, g, b, ph, feedcount, feedcounter);
    Particle.publish("ExFeeder", publishString);
 return 1;
}
int ExStop(String args){

    feedcount = 0;

    closeFill();
    DoseStopAll();
    ExPump("off:1");

 return 1;
}
int ExStart(String args){

    int w = parse_args_int(args, "water");
    int f = parse_args_int(args, "feed");
    int b = parse_args_int(args, "bclose");

    if (w == 1){
        Watertimer.start(); // start the water level sensor sample timer
        return 21;
    }else if(w == 0){
        Watertimer.stop(); // start the water level sensor sample timer
        return 20;
    }

    if (f == 1){
        FeedLoop.start();   // freqency of Feeder actions,
        return 11;
    }else if(f == 0){
        FeedLoop.stop();   // freqency of Feeder actions,
        return 10;
    }

    if (b == 1){
        return pinCtrl(PIN_WATER_FILL_BALL_CLOSE, "BallValveClosing", "on");
    }

 return -1;
}

/////  Parse and Pin Control    /////
int pinCtrl(int pin, String name, String status){
  if(status == "on"){
        digitalWrite(pin, HIGH);
        Log(String(pin) + " : " + name + " : " + status + ":" + feedcompleted);
    return 1;
  }else if(status == "off"){
        digitalWrite(pin, LOW);
        Log(String(pin) + " : " + name + " : " + status + ":" + feedcompleted );
    return 2;
  }else{
        Log.warn("Pin Control status incorrect Status:" + status  + ":" + feedcompleted );
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

void setup() {
  Particle.connect();
  waitFor(Particle.connected, 30000);

  Particle.function("feed", ExFeeder);  // m:#,g:#,b:#,ph:#,xfeed:#
 //  Particle.function("start", ExStart);   // no params, just turns everything off
  Particle.function("stop", ExStop);   // no params, just turns everything off
  Particle.function("set", SetVar);     //
  Particle.function("ReadTemp", ExreadDHT);   //
  Particle.variable("FeedStat", feedcompleted);
  Particle.variable("FillStat", fillstatus);
  Particle.variable("DoseStat", dosedRes);
  Particle.variable("FeedCntr", feedcount);
  Particle.variable("Totcount", feedcounter);
  Particle.variable("ResFull", resful);
  Particle.variable("ResEmpty", resempty);
  Particle.variable("Micro", m);
  Particle.variable("Grow", g);
  Particle.variable("Bloom", b);
  Particle.variable("PH", ph);

  pinMode(PIN_WATER_FULL, INPUT);
  pinMode(PIN_FEED_PUMP, OUTPUT);
  pinMode(PIN_RELAY_FILL, OUTPUT);
  pinMode(PIN_WATER_FILL_BALL_OPEN, OUTPUT);
  pinMode(PIN_WATER_FILL_BALL_CLOSE, OUTPUT);
  pinMode(FEED_FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PIN_MICRO_DOSE, OUTPUT);
  pinMode(PIN_GROW_DOSE, OUTPUT);
  pinMode(PIN_BLOOM_DOSE, OUTPUT);
  pinMode(PIN_PH_DOSE, OUTPUT);
  setADCSampleTime(ADC_SampleTime_144Cycles);

  BallClosed();
  Watertimer.start();
  FeedLoop.start();
  dht.begin();
  PollSensor.start();

}

/*
// How to break ints into bytes for EEPROM storage
 //  https://community.particle.io/t/saving-values-in-the-firmware/11822/6
void readEEPROM(){
  // Read an object from the EEPROM addres
  int addr = 20;

  settings_t myObj;
  EEPROM.get(addr, myObj);
  if(myObj.version != 0) {
    // EEPROM was empty -> initialize myObj
    MyObject defaultObj = { 0, 12.34f, 25, "Test!" };
    myObj = defaultObj;
  }
}


void writeEEPROM(){
 // EXAMPLE USAGE
 // Write a value (2 bytes in this case) to the EEPROM address
  int addr = 10;
  uint16_t value = 12345;
  EEPROM.put(addr, value);
 // Write an object to the EEPROM address
    addr = 20;
    MyObject myObj = { 0, 12.34f, 25, "Test!" };
    EEPROM.put(addr, myObj);
}
*/

void loop() {
    if(feedcompleted != lastfeedcomplete){
        sprintf(publishString,"{\"Then\": %d, \"Now\": %d, \"Count\": %d, \"Res\": %d, \"Dose\": %d, \"Fillstatus\": %d}", lastfeedcomplete, feedcompleted, feedcounter, resful, dosedRes, fillstatus );
        Particle.publish("StatusChange", publishString);
        lastfeedcomplete = feedcompleted;
    }

	if (readWaterLevelFlag == 1)
        ReadWaterLevel();

   if (mixres == 1)
        mixReservour();

   if (readHumidTemp == 1 && feedcompleted == 1 )  // only check temp/humid when not dosing or feeding
        pollDHT();
}
