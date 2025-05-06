# dhbw-c-exam

Diego Rubio Carrera

- ReqFunc01: Das Programm soll einen Hilfetext besitzen, welcher per Kommandozeilenparameter -h oder --help aufrufbar ist und die Benutzung Ihres Programms erklärt

- ReqFunc02: Der Hilfetext soll sämtliche Kommandozeilenparameter Ihres Progamms und deren Benutzung erklären. Zugelassene Sprachen sind deutsch und englisch

- ReqFunc03 Um den Author identifizieren zu können soll (im JSON Format) eine Ausgabeblock erstellt werden: Der Namen der abgebenden Person, der Studiengang (TIT,TIK,TIM,TIS sind zulässig) und wenn möglich eine Mail Adresse zur Kontaktaufnahme. Codepage ist utf-8. Der Schalter hierfür ist --programmer-info

- ReqFunc04: Soll ein Text in Morsecode gewandelt werden, so wird der Schalter -e, --encode gewählt

- ReqFunc05: Default kann Encode eingestellt werden (wandeln von Text in Morse Code) um beim weglassen des Schalter -e, --encode eine Ausgabe zu erzeugen

- ReqFunc06: Soll ein Text aus Morsecode dekodiert werden, so wird der Schalter -d, --decode gewählt

- ReqFunc07: Ist sowohl der Schalter Encodieren -e, --encode wie auch Dokodieren -d, --decode gewählt, so soll das Programm mit einer Fehlermeldung abgebrochen werden

- ReqFunc08: Dateien, die gewandelt werden sollen, können in allen beliebigen Ober-, Unterverzeichnissen, hinter Links oder absoluten Pfaden abliegen

- ReqFunc09: Ist die per Option übergebene Datei nicht vorhanden, dann soll eine Fehlermeldung ausgegeben werden (Text passend zum Ereignis)

- ReqFunc10: Wird per Pipe ein Text an das Programm übergeben soll aus der Pipe dieser gelesen werden

- ReqFunc11: Über die Schalter -o, --out soll der Text in eine Datei umgeleitet werden

- ReqFunc12: Über die Schalter -o, --out an Dateien ausgegebene Texte können in allen beliebigen Ober-, Unterverzeichnissen, hinter Links oder absoluten Pfaden abliegen

- ReqFunc13: Die Buchstaben A-Z sollen in Morsecode wandelbar sein

- ReqFunc14: Die Buchstaben A-Z sollen in Klartext rückwandelbar sein

- ReqFunc15: Die Buchstaben a-z sollen in Morsecode wandelbar sein

- ReqFunc16: Die Buchstaben a-z sollen in Klartext als Großbuchstaben rückwandelbar sein

- ReqFunc17: Die Zahlen 0-9 sollen in Morsecode wandelbar sein

- ReqFunc18: Die Zahlen 0-9 sollen in Klartext rückwandelbar sein

- ReqFunc19: Die Satzzeichen .,:;? (Punkt, Komma, Doppelpunkt, Strichpunkt, Fragezeichen) sollen in Morsecode wandelbar sein

- ReqFunc20: Die Satzzeichen .,:;? (Punkt, Komma, Doppelpunkt, Strichpunkt, Fragezeichen) sollen in Klartext rückwandelbar sein

- ReqFunc21: Die mathematischen Symbole =-+ (Gleich, Minus, Plus) sollen in Morsecode wandelbar sein

- ReqFunc22: Die mathematischen Symbole =-+ (Gleich, Minus, Plus) sollen in Klartext rückwandelbar sein

- ReqFunc23: Die Formatier Symbole _()/@ (Unterstrich, Klammer auf/zu, Schrägstrich, Klammeraffe) sollen in Morsecode wandelbar sein

- ReqFunc24: Die Formatier Symbole_()/@ (Unterstrich, Klammer auf/zu, Schrägstrich, Klammeraffe) sollen in Klartext rückwandelbar sein

- ReqFunc25: Nicht vorhandene Buchstaben (Zeichen) sind beim Encodieren als *(Stern) aus zu geben

- ReqFunc26: Buchstaben werden durch ein SP (SPACE) getrennt

- ReqFunc27: Wörter werden durch SPSPSP (3*SPACE) getrennt

- ReqFunc28: Newline und Carridge Return werden ignoriert (Whitespaces)

- ReqNonFunc01: Die Software soll mit GNU-C/C++ für UNIX und WSL Linux compilierbar sein

- ReqNonFunc02: Die Software soll unter UNIX und WSL Linux ausführbar sein

- ReqNonFunc03: Das Erstellen der Software soll mit CMake erfolgen

- ReqNonFunc04: Der Name des ausführbaren Programm ist morse

- ReqNonFunc05: Das Parsen der Optionen soll über die LibC Funktionalität getopt(...) und getopt_long(...) aus getopt.h erfolgen

- ReqNonFunc06: Das Includieren der Header-Dateien soll nicht im aktuellen Verzeichnis erfolgen, sondern im Include-Pfad des Compilers

- ReqNonFunc07: Das Includieren von Source-Dateien(_.c /_.cpp) in Source-Dateien ist nicht erlaubt

- ReqNonFunc08 Die erstellten Header Dateien sollen gegen Mehrfacheinbindung geschützt werden und der Schutz soll kompatibel zum Compiler MinGW (Windows) und GNU-C (Unix) sein

- ReqOptFunc01: Kann die per Option übergebene Datei nicht geöffnet werden, dann soll eine Fehlermeldung ausgegeben werden (Text passend zum Ereignis)

- ReqOptFunc02: Über die Schalter --slash-wordspacer soll zwischen Wörter der Space SP,/,SP eingefügt werden. Der Buchstabentrenner verändert sich nicht

- ReqOptFunc03: Der Schalter --slash-wordspacer darf nur in Verbindung mit Encode verfügbar sein. Im Fehlerfall muß eine Warnung ausgegeben und das Programm beendet werden

- ReqOptFunc04: Die Software soll mit MinGW64 compilierbar sein

- ReqOptFunc05: Die Software soll unter Windows ausführbar sein

- ReqOptFunc06: Die Dokumentation des Codes soll mit dem Syntax von doxygen erfolgen

- ReqOptFunc07: Die Dokumentation des Codes mit doxygen soll jede Klasse, Methode (Funktion) und Variable abdecken

- ReqOptFunc08: Die Header- und Source-Dateien ihres Programms sollen sich in verschiedenen Unterverzeichnissen befinden
