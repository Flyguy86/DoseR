# DoseR

hydro process control sysytem

## Process

### Flow
- #### Top Feed drip irrigation
  - IFTT monitors Google Calendar for Feed event
  - Check water level sensor
    - If empty Open Fill Valve
      - If full Turn on feed pump
  - Activate dosing pumps (timer)
  - Turn on Feed Pump (timer)
    - If Water level Full & dosing pumps off
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
example: **wfull:1,pumpofftime:600**
````
    wfull: int
      1  Analogval > Water_Full_Value; returns true
      0  Analogval < Water_Full_Value; returns true
      -1 No sensor
    pumpofftime: int
       0 - 65000 seconds Feed pump run time in seconds
````

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
    alrtenable: int
      1 Enable alerts, Disables Water empty sensor 
      2 Enable alerts, Disables Flow sensor 1 
      3 Enable alerts, Disables Flow sensor 2
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
- requires Empty water Sensor
````
    flow: int 
      1 Top feed flow, Drain to waste
      2 Recirculation reservoir

    flushemptytime: int
       0 - 65000 seconds Feed pump run time in seconds

````

