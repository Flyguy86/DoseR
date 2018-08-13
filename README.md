# DoseR
is a Process controller, 
in the folling design you'll see it appiled as a 
* hydroponic feeding system 
* recirculating resivour controler
* greenhouse enviornment control

## Particle actions
### xfeed
|   command |  description |
|  :----------: | --------: |
| xfeed:5,m:15,g:10,b:30,ph:4 | xfeed: number of cycles, m g b ph: seconds for each dose pump | 

### set
| command |  description |
|:----------: | --------: |
| m:19,g:32,b:100,ph:3 | sets dose values, seconds for each dose pump |
|    draintime:800          |  time in seconds to drain resivour | 
|    feedtime:400           |  time in seconds feed pump stays on |
|    hottarget:80  |  Target temperature in F |
|    coldtarget:50 |  Target temperature in F | 

### cycle
|   command |  description |
| :----------: | --------: |
|       feed  | Fill, dose, Pump on timer |
|       drain | pump, drain valve on timer |
|       fill  | fill valve open until full |
|       flush | drain, fill, drain |
|       dose1 | dose pump on 10 seconds |
|       dose2 | dose pump on 10 seconds |
|       dose3 | dose pump on 10 seconds |
|       dose4 | dose pump on 10 seconds |
|       pump  | dose pump on 10 seconds |
|       toohot | Env cooler on until next sensor check |
|       toocold | Env heaer on until next sensor check |
|       lightsout| RF lights off |
|       lights    | RF lights on |

### stop!  
* Closes fill vavles
* turns off pumps
* exits all current cycles

###   MQTT server
| action |   command |  description |
| :------ | :----------: | --------: |
| MQTT server| 192.168.1.1 | ipaddress or DNS of MQTT server | 
| MQTT user name| bob | auth user login | 
| MQTT password | ross | auth user password | 
| RF lights on | 3eff4453E2ead | RF hex code to turn lights on
| RF lights off | 3eff4453E2ead | RF hex code to turn lights off
| RF cooler on | 3eff4453E2ead | RF hex code to turn cooler on
| RF cooler off | 3eff4453E2ead | RF hex code to turn cooler off
| RF heater on | 3eff4453E2ead | RF hex code to turn heater on
| RF heater off | 3eff4453E2ead | RF hex code to turn heater off

