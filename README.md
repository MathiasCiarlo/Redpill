# Redpill - An automatic circular pill dispencer
Arduino project for the course "inf1510" at UiO.
[Video](https://youtu.be/IycjOfMeGy4)

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

##Koden
###Systemet
Arduinoen er programmert som en tilstandsmaskin. Det er fire tilstander: 0, 1, 2 og 3 som samsvarer med morgen, middag, ettermiddag og kveld - klokken 08:00, 12:00, 16:00 og 20:00. Arduinoen har en innbygget klokke som starter på 0 timer, 0 minutter og 0 sekunder når systemet starter. Denne klokken brukes for å avgjøre når vi skal gå inn i de forskjellige tilstandene. Tidspunkene er satt til 2, 10, 18 og 26 sekunder, og etter 32 sekunder resettes klokken til 0 igjen. Dette er for å teste systemet.

###Feilsøking
Hvis arduinoen kobles til en datamaskin vil status sendes via seriell kommunikasjon over USB til port 9600 slik at det kan skrives ut til terminal. Det finnes flere hjelpefunksjoner for å skape pene utskrifter og å generellt skape en god oversikt over systemets tilstand til enhver tid.

###Biblioteker
- Time.h    (For å holde tiden)
- Stepper.h (For å rotere kamprene, steppermotoren)
- Servo.h   (For å åpne fallemmen, servoen)

###Variable
Globale variable som er verdt å nevne er *currentState*, *pillTimes[]*, *switchValues[]* og *boolean readyToServe*. *pillTimes[]* holder på de fire utleveringstidspunkene. *switchValues[]* holder på tilstanden til de fire bryterne, 1 betyr på, 0 betyr av.

Konstanter er HATCH_TIME og RESET_TIME. Disse styrer hvor lenge fallemmen er åpen, og etter hvor mange sekunder klokken skal resettes under testing. Disse er deklarert øverst i koden som konstanter for å skape oversikt og unngå hardkoding.

###Hovedløkke
Etter at initialtilstand og input/output er initialisert i funksjonen *setup()* entres hovedløkken, altså funksjonen *loop()*. Her er en stor del av koden innkapslet i en if-test som kun aktiveres en gang i sekundet. Dette er gjort for at utskrift til terminal under testing blir leselig. Følgende operasjoner er innkapslet:
- Bryternes tilstand leses (switchValues[] oppdateres)
- Det sjekkes om det er tid for å gå inn i neste state gjennom funksjonen *timeForNextState()*, denne er beskrevet nedenfor. I så fall kalles goToNextState() som korrekt endrer *currentState* til neste. For eksempel kan vi gå fra tilstand 2 til 3 og deretter fra 3 til 0. Så roterer kamrene ett hakk ved hjelp av funkjsonen *rotateSteps()*. Videre sjekkes det om bryteren for denne tilstanden er *på*, som betyr at en dose skal utleveres for dette tidsrommet. I så fall aktiveres knappen og grønn LED skrus på. Hvis bryteren var *av* deaktiveres knappen og det blå lyset skrus på.

Til slutt testes det om knappen er trykket inn. Dette skjer utenfor ett-sekunds-innkapslingen for å sørge for at hendelsen blir fanget opp. Når knappetrykket registreres sjekkes det om knappen er aktivert eller ikke, dette gjennom variablen *readyToServe*. Hvis ja kalles funksjonen *openHatch()* for å åpne fallemmen, blått lys aktiveres og knappen deaktiveres igjen.

###timeForNextState()
Denne funksjonen er en svært viktig del av systemet. Funksjonens hensikt er å avklare om vi tidsmessig har beveget oss inn i neste state. Her sjekkes det først hva klokken er, og så slås det opp i tabellen *pillTimes[]* for neste state (eller indeks) for å sjekke om nåtid er større enn neste tid. For å realisere dette måtte tiden trikses med. Dette skjer i funksjonen *getTimeString()*:

Her hentes først time, minutt og sekund. Så gjøres hver av disse om til to tegn lange tekststrenger. For eksempel vil minutt=8 bli gjort om til "08". Så konkatineres de tre strengene, slik at vi får et klokkeslett på formen "hhmmss". Dette returneres.

Tilbake i funksjonen vår gjøres denne tekststrengen om til et heltall. Klokken 8 om morgenen vil bli tallet 80000 (nullet foran forsvinner). Nå kan programmet sjekke om nåtid er større enn de forhåndssatte tidene i *pillTimes[]* (tidene er lagt inn på denne formen i *setup()*). Hvis vi er over tiden returneres true, ellers false. Det er lagt inn spesialkode for den situasjonen der vi er i siste tilstand, men klokken er mindre enn midnatt. I dette tilfellet vil nemlig neste state være 8 om morgenen og ha tallverdien 80000, som er *mindre* enn nåtid.

###Annet
- I en reell brukssituasjon er ville det ikke vært nødvendig å sjekke om vi har gått inn i neste tidsperiode eller om bryterne er skrudd på hvert sekund, men mye sjeldnere. Dette er gjort på grunn av testingen, utleveringstidspunkene er nemlig satt til 2, 10, 18 og 26 sekunder.
- Annenhver gang kamrene roterer brukes verdien -29 og -28, dette er hvor langt steppermotoren skal rotere. For å få det til å gå opp at 360 grader skal tilsvare 28 kamre, må steppermotoren gå et sted mellom 28 og 29 "steps". Dette oppnår vi ved hjelp av å gå til 28 og 29 annenhver gang.
