# Redpill - An automatic circular pill dispencer
Arduino project for the course "inf1510" at UiO.

##Red Pill consists of:
- Rotating disc with 28 pill chambers
- 4 switches to control which times to dipsose pills
- Button to control the pill hatch
- LED
- Servo
- Stepper motor
- Arduino UNO

##How it works
The dispenser rotates each preset time, for testing puposes, theese are set to 2, 10, 18, 26 seconds. The clock resets at 32 secs. The switches on the side of the dispencer control which of the times a pill can be dispenced. When the dispencer rotates to a time corresponding to a switch in **ON** position, the LED will turn **green** and the button is activated. When the button is pressed a hatch opens and the pills falls into a shelf beneath. The shelf can be extracted.

###Details
Green LED means a dose of pills is ready and the hatch can open to serve it.
Blue LED means the there are no current dose ready, but the system is waiting for the next serving time. The button is disabled.
LED off means all switches are in **OFF** position and the button is disabled.

##Code
Some lines are marked blocking. This means the code has a delay built in.

###System
Arduinoen er programmert som en tilstandsmaskin. Det er fire tilstander: 0, 1, 2 og 3 som samsvarer med morgen, middag, ettermiddag og kveld - klokken 08:00, 12:00, 16:00 og 20:00. Arduinoen har en innbygget klokke som starter på 0 timer, 0 minutter og 0 sekunder når systemet starter. Denne klokken brukes for å avgjøre når vi skal gå inn i de forskjellige tilstandene.

###Variable
Globale variable som er verdt å nevne er *currentState*, *pillTimes[]*, *switchValues[]* og *boolean readyToServe*. *pillTimes[]* holder på de fire utleveringstidspunkene. *switchValues[]* holder på tilstanden til de fire bryterne, 1 betyr på, 0 betyr av.

###Hovedløkke
Etter at initialtilstand og input/output er initialisert i funksjonen *setup()* entres hovedløkken, altså funksjonen *loop()*. En stor del av koden er innkapslet i en if-test som kun aktiveres en gang i sekundet. Dette er gjort for at utskrift til terminal under testing blir leselig. Følgende operasjoner er innkapslet:
- Bryternes tilstand leses (switchValues[] oppdateres)
- Det sjekkes om det er tid for å gå inn i neste state. I så fall kalles goToNextState() som korrekt endrer *currentState* til neste. For eksempel kan vi gå fra tilstand 2 til 3 og deretter fra 3 til 0. Videre sjekkes det om bryteren for denne tilstanden er *på*, som betyr at en dose skal utleveres for dette tidsrommet. Knappen aktiveres og grønn LED skrus på. Hvis bryteren var *av* deaktiveres knappen og det blå lyset skrus på.
- Til slutt sjekkes det om vi har nådd resetTime (32 sekunder), og i så fall settes tiden til 0 igjen.

###Annet
I en reell brukssituasjon er ville det ikke vært nødvendig å sjekke om vi har gått inn i neste tidsperiode eller om bryterne er skrudd på hvert sekund, men mye sjeldnere. Dette er gjort på grunn av testingen, utleveringstidspunkene er nemlig satt til 2, 10, 18 og 26 sekunder.