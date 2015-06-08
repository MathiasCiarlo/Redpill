# Redpill - An automatic circular pill dispencer
Aruino project for the course "inf1510" at UiO.

##Manual
###Red Pill consists of:
- Rotating disc with 28 pill chambers
- 4 switches to control which times to dipsose pills
- Button to control the pill hatch
- LED
- Servo
- Stepper motor
- Arduino UNO

###How it works
The dispenser rotates each preset time, for testing puposes, theese are set to 2, 10, 18, 26 seconds. The clock resets at 32 secs. The switches on the side of the dispencer control which of the times a pill can be dispenced. When the dispencer rotates to a time corresponding to a switch in **ON** position, the LED will turn **green** and the button is activated. When the button is pressed a hatch opens and the pills falls into a shelf beneath. The shelf can be extracted.

###Details
Green LED means a dose of pills is ready and the hatch can open to serve it.
Blue LED means the there are no current dose ready, but the system is waiting for the next serving time. The button is disabled.
LED off means all switches are in **OFF** position and the button is disabled.

###Code
Some lines are marked blocking. This means the code has a delay built in.