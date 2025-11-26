Bij deze code wordt gebruik gemaakt van de publiekelijk toegankelijke ICMPLIB library die geschreven is door GitHub gebruiker 'markondej'. 
<br>Deze library voegt de mogelijkheid toe om via C++ code een PING-commando naar een adres te sturen en het resultaat op te vangen.

<h2>Applicatie starten:</h2>
Start bestand: https://github.com/JarmoMarczinkowsky/Robot-Ping-Logger/blob/main/ping_adress.cpp
Start de applicatie door in Linux met 'CD path/to/file' naar de folder te gaan in de terminal.
Run daarna de code met 
./ping_logger [adres] [aantal]

Parameters:
Adres: Adres van host die gepingt gaat worden.
Aantal: aantal ping commando's dat verzonden gaat worden.

Voorbeeld: 
./ping_logger 192.168.8.121 10

Link naar de library:
[icmplib](https://github.com/markondej/cpp-icmplib)
