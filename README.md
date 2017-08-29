# DoseR

hydro process control sysytem


## Beta 4 release notes 
 - beta 5
   - udpate documentaion in Read me
   - add pictures for the rest of components
   - Particle Function "start"
      - f:1 Opens fill  auto close on float sensor high
      - f:0 closes fill
      - p:1 Feed Pump on  - time length defined in setVar, or manual shut off
      - p:0 Feed Pump off
      - d1:10 Dose pump 1 on for 10 seconds
      - d2:20 Dose pump 2 on for 20 seconds
      - d3:30 Dose pump 3 on for 30 seconds
      - d4:40 Dose pump 4 on for 40 seconds
   - Particle Function stop
     - Stops all pumps, and closes the ball valve and fill valve
   - Particle Function set
     - feedpumpoffin:120  Set and store the time Feed pump will run in seconds
     - wfull:1  Set the logic of the Water full float sensor
     - wempty:1 Set the logic of the Water empty float sensor
     - fill:0   select Fill hardware    0=Ball valve,  1=Fill Solienoid
       - Fill hardware : 0,   defines alarm action pin,   Ball Open +12v ssr
       - Fill Hardware : 1,   defines alarm action pin,   Fill solenoid pin
     - alrmpin:1   Set alarm sensor pin,    1=water empty sensor,     2=Flow 1 sensor,    3=Flow2 sensor
     - alrmthresh:2000  the value the analog sensor compares against, values between 0-4096
     - alrmlogic:1    1  Sensor value < Threshold   0 Sensor value >= Threshold
   - AlarmOnOff      
     - Toggle Enable/Disable  Action pin  Broad casts enabled Action every 5 min
   - AlarmDisab"   
     - Toggle Enable/disable alarm,  Broadcasts disabled message every 5 min
   - Add the relay control, based on fill device..    
## Todo
 - Beta 5
   - Debug and test ?

   
### Work Flow
- #### Top Feed drip irrigation
  - IFTT monitors Google Calendar for Feed event
  - Check water level sensor
    - If Not full: Open Fill Valve
      - If full: Turn on feed pump
  - Activate dosing pumps (timer)
  - If Water level Full & dosing pumps off
    - Turn on Feed Pump (timer)
  - repeat xfeed times
- #### Reciruclation system
  - Comming in a bit


### Automation
````
1. IFTT -> Particle event
2. IFTT -> google Calendar name search
3. Create Google Calendar event
````

![Image of GrowStacks & Google Calendar](https://github.com/Flyguy86/DoseR/blob/master/google%20Calendar%20dosing.png)


## Hardware
- GrowStacks Doser board (IoT) 
  - DHT11 Temperature Humidity sensor
  - 12v power supply for board

![Image of GrowStacks & Google Calendar](https://github.com/Flyguy86/DoseR/blob/master/BreakoutBoardV8.png)

- 4 peristalic dosing pumps
- 12v 35 psi diaphram pump 3 lpm
  - 12v power supply for pump
- Stainless steel float sensor
- 5 gallon dosing bucket and Lid
   

### Commands
#### feed: 
example: **xfeed:1,m:1g:10,b:100,ph:0**
````  
    xfeed:int     //Number of times to repeat cycle
    m: int        //Number seconds to power dosing pump 1
    g: int        //Number seconds to power dosing pump 2
    b: int        //Number seconds to power dosing pump 3
    ph: int       //Number seconds to power dosing pump 4
````

#### set:
example: **wfull:1,pumpofftime:600,fill:0,alrmpin:0**

    - feedpumpoffin:600  Set and store the time Feed pump will run in seconds
    - wfull:0  Set the logic of the Water full float sensor
    - wempty:0 Set the logic of the Water empty float sensor
    - fill:1   select Fill hardware    0=Ball valve,  1=Fill Solienoid
      - Fill hardware : 0,   defines alarm action pin,   Ball Open +12v ssr
      - Fill Hardware : 1,   defines alarm action pin,   Fill solenoid pin
    - alrmpin:0   Set alarm sensor pin,    1=water empty sensor,     2=Flow 1 sensor,    3=Flow2 sensor
    - alrmthresh:2000  the value the analog sensor compares against, values between 0-4096
    - alrmlogic:0    1  Sensor value < Threshold   0 Sensor value >= Threshold

#### stop:
    - Stops all pumps, and closes the ball valve and fill valve

#### start:
example: **f:1,p:0,d1:300,d2:100,d3:300,d4:0**
  - f:1 Opens fill  auto close on float sensor high
  - f:0 closes fill
  - p:1 Feed Pump on  - time length defined in setVar, or manual shut off
  - p:0 Feed Pump off
  - d1:10 Dose pump 1 on for 10 seconds
  - d2:20 Dose pump 2 on for 20 seconds
  - d3:30 Dose pump 3 on for 30 seconds
  - d4:40 Dose pump 4 on for 40 seconds

#### AlarmOnOff
    - Toggle Enable/Disable  Action pin  Broad casts enabled Action every 5 min
#### AlarmDisab   
    - Toggle Enable/disable alarm,  Broadcasts disabled message every 5 min


**Optional:** Empty water Sensor
````
    wempty: int
      1 Analogval > Water_Empty_Value; returns true
      0 Analogval < Water_Empty_Value; returns true
     -1 No sensor
    pumpempty: int
       1 Pump off wempty true
      -1 No sensor
````

**Optional:** Door Alarm
````
    alrmpin: int
      0 Disabled
      1 Enable alerts, use Water empty sensor 
      2 Enable alerts, use Flow sensor 1 
      3 Enable alerts, use Flow sensor 2
      
    alrmlogic: int
      1 Analogval > Alert_Threshold; returns true
      0 Analogval < Alert_Threshold; returns true
      -1 No sensor
      
    alrmact:
      if fill-device
        alrm pin 1 == ball open
        alrm pin 2 == ball close
      else
        alrm pin1 == fill

    alrmthresh:       // sensor comapre to threshold.  2000 is the middle of the sensor range.
         default 2000   // 0 - 4096

````

**Optional:** Magic Button
````
    butnenable: int
      1 Enable alerts, use Flow sensor 1
      2 Enable alerts, use Flow sensor 2
      0 No sensor
      
    alrtsensor: int
      1 Analogval > Alert_Threshold; returns true
      0 Analogval < Alert_Threshold; returns true
      -1 No sensor
      
    alrtrelay: int
      1 Fill pin
      2 Flush pin
      0 No relay
````

**Optional:** Flush value / Flush Pump 
- requires Empty water Sensor, in Reciruclation system 
````
    flow: int 
      1 Top feed flow, Drain to waste
      2 Recirculation reservoir

    flushemptytime: int
       0 - 65000 seconds Feed pump run time in seconds

````

