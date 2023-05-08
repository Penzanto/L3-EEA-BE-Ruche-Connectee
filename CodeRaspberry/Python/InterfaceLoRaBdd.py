#importation des libraries utilisees
import serial   #import la lib serial pour la gestion du port serie du LoRa(pyserial 3.5)
import mariadb  #import la lib mariadB pour la base de donnee (mariadb 1.0.11)
import time #lib utilise pour faire un deltaT en secondes
from datetime import datetime   #lib utilise pour avoir le temp actuel en UTC
from pytz import timezone   #lib pour avoir les timezone
import sys

#---------------------------------------------------------Fonction MariaDB---------------------------------------------------------
def connectionDB(ipDB, portDB, nomDB, utilisateurDB, mdpDB):
    """fonction de connection a la base de donne maraidB

    Args:
        ipDB (string): addresse ip de la db
        portDB (int): port de la db
        nomDB (string): nom de la db que l on veut utiliser
        utilisateurDB (string): utilisateur de la db
        mdpDB (string): mdp utilisateur de la db
    
    Return:
        conn (): variable de connnection a la db
        cur (): curseur de la db utiliser pour les commande d exctaction de d insertion dans la db
        erreurInitDB (int): flag d erreur lors de l init de la DB. TRUE -> erreur lors de la connection a la DB 
    """

    #Variables local
    erreurInitDB =0

    #Connection a la base de donnee mariadB
    try:
        conn = mariadb.connect(
            user=utilisateurDB,
            password=mdpDB,
            host=ipDB,
            port=portDB,
            database=nomDB)
    except mariadb.Error as e:
        print(f"Erreur lors de la connction a la base de donnee: {e}")
        erreurInitDB = 1

    #Creation du curseur de la base de donnee
    cur = conn.cursor()
    conn.autocommit = False #desactive l'autocommit des vals dans la database
    return(conn, cur, erreurInitDB) #retourne le curseur de la base de donne


def insertionDB(DateHeure, Ruche, TempInter, HumidInter, TempExter, HumiExter, Masse, Proxi, Vbat, connectionDB, curseurDB):
    """fonction d insertion des valeurs dans la base de donnee

    Args:
        DateHeure (String): _description_
        Ruche (int): _description_
        TempInter (int): valeur de la temp a l interieur de la ruche
        HumidInter (int): valeur de l humid a l interieur de la ruche
        TempExter (int): valeur de la temp a l exterieur de la ruche
        HumiExter (int): valeur de l humid a exterieur de la ruche
        Masse (int): valeur du capteur de la jauge de contrainte
        Proxi (int): valeur du capteur PIR
        Vbat (int): tension de la batterie de la ruche
        connectionDB (): variable de connnection a la db
        curseurDB (): curseur de la db utiliser pour les commande d exctaction de d insertion dans la db
    """

    #commit des valeurs dans la database
    try:
        curseurDB.execute("INSERT INTO Mesure (DateHeure, Ruche, TempInter, HumidInter, TempExter, HumiExter, Masse, Proxi, Vbat) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", (DateHeure, Ruche, TempInter, HumidInter, TempExter, HumiExter, Masse, Proxi, Vbat))
        connectionDB.commit()
    except mariadb.Error as e:
        print(f"Erreur insertion des datas dans la DB: {e}")


#---------------------------------------------------------Fonction LoRa---------------------------------------------------------
def lectureLoRa(port, octetLecture=200, stringAttendu="", timeOut=5):
    """Fonction de lecture serie des information retourner par le module LoRa-E5
        si les octets rendu par la carte LoRa ne correspondent pas a "stringAttendu" alors la fonction atteindra un timeout et rendra la valeur "-1"

    Args:
        port (class): class utiliser pour declarer le port serie
        octetLecture (int): nombre d octet a lire a la fois
        stringAttendu (str): string que le module LoRa doit normalement nous rendre
        timeOut (int): valeur du timeout a attendre si l on ne recois pas le message "stringAttendu"
    """

    octetRecu = ""
    timeout_start = time.time()
    while((octetRecu.find(stringAttendu) == -1) & (time.time() < timeout_start + timeOut)):
        while(port.inWaiting() < 1) & (time.time() < timeout_start + timeOut): 
            time.sleep(0.25)
        while(1):
            octetLu = port.read(size=octetLecture).decode("utf-8")
            octetRecu = octetRecu + octetLu
            if len(octetLu) != octetLecture:
                break
    if(octetRecu.find(stringAttendu) == -1):
        octetRecu = "-1"
    return(octetRecu)


def initLoRa(frequence, SpreadFactor, BandWidth, TX_Preamble, RX_Preamble, Power, CRC, Inverted_IQ, Public_LoRaWAN, portSerieLoRa):
    """Fonction d initialisation du module LoRa E5 en recepteur

    Args:
        frequence (int): frequence d emission du module
        SpreadFactor (string): 
        BandWidth (int): bande passante 
        TX_Preamble (int): 
        RX_Preamble (int): 
        Power (int): puissance d emission en dBm
        CRC (string): activation du CRC (activation du CRC "ON")
        Inverted_IQ (string): 
        Public_LoRaWAN (string): 
        portSerieLoRa (string): nom de la class du port serie physique du module LoRa
        
    Returns:
        initialisaitonLoRaOK (int): 0 si init OK, 1 si erreur lors de l init du module LoRa
    """

    #variable local
    initialisaitonLoRaOK = 0
    strAEnvoier = ""

    #passage du module Lora en "mode TEST"
    portSerieLoRa.write('AT+MODE=TEST'.encode())
    receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+MODE: TEST", timeOut=5)
    if(receptionLoRa =="-1"):
        initialisaitonLoRaOK = 1
        print("\n erreur ecriture mode TEST")
    else:
        print(receptionLoRa)

    #passage du module Lora en "mode STOP" (empeche le module de recevoir des information durant la reprogramation du mode)
    if(initialisaitonLoRaOK == 0):
        portSerieLoRa.write('AT+TEST=STOP'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+TEST: STOP", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 1
            print("\n erreur ecriture mode STOP")
        else:
            print(receptionLoRa)

    #reglage du LDRO du module Lora
    if(initialisaitonLoRaOK == 0):
        portSerieLoRa.write('AT+LW=LDRO,ON'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+LW: LDRO, ON", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 1
            print("\n erreur ecriture mode LDRO")
        else:
            print(receptionLoRa)

    #configuration des parametres d emission radio du module LoRa
    if(initialisaitonLoRaOK == 0):
        strAEnvoier = 'AT+TEST=RFCFG,' + str(frequence) + ',' + SpreadFactor + ',' + str(BandWidth) + ',' + str(TX_Preamble) + ',' + str(RX_Preamble) + ',' + str(Power) + ',' + CRC + ',' + Inverted_IQ + ',' + Public_LoRaWAN
        portSerieLoRa.write(strAEnvoier.encode())
        receptionLoRa = lectureLoRa(portSerieLoRa, octetLecture=200, stringAttendu="+TEST: RFCFG F", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 1
            print("\n erreur ecriture mode RFCFG")
        else:
            print(receptionLoRa)

    #configuration du module LoRa E5 en reception
    if(initialisaitonLoRaOK == 0):
        portSerieLoRa.write('AT+TEST=RXLRPKT'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+TEST: RXLRPKT", timeOut=5)
    if(receptionLoRa =="-1"):
        initialisaitonLoRaOK = 1
        print("\n erreur passage du module LoRa en reception")
    else:
        print(receptionLoRa)

    return initialisaitonLoRaOK


def decodageMessageLoRa(portSerieLoRa):
    """Fonction de traitement des donnees recu en LoRa pour extraire le codeSysteme, codeRuche, indiceCapteur et du valeurCapteur

    Args:
        portSerieLoRa (string): nom de la class du port serie physique du module LoRa

    Returns:
        codeSysteme_recu (int): valeur recu par le module LoRa pour le codeSysteme
        codeRuche_recu (int): valeur recu par le module LoRa pour le codeRuche
        indiceCapteur_recu (int): valeur recu par le module LoRa pour le indiceCapteur
        valeurCapteur_recu (int): valeur recu par le module LoRa pour le valeurCapteur
        receptionLoRaOK (int): information sur la bonne reception d un message. si 0 message bien recu, si 1 erreur lors de la reception du message
    """

    #variable local
    receptionLoRa = "1"
    receptionLoRaOK = 0

    #lecture des donnees dans le buffer de la laision UART
    receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu='+TEST: RX "', timeOut=20)
    #gestion de l erreur
    if(receptionLoRa == "-1"):
        receptionLoRaOK = 1
        print("erreur lors de la reception du message LoRa")

    #decoupage du message
    receptionLoRa_split = receptionLoRa.split('+TEST: RX "')
    valeurRecu = str(receptionLoRa_split[1])[:-3]
    
    #tri des differentes valeurs
    if(len(valeurRecu) == 16):  #premiere pre-verification du message
        codeSysteme_recu = int(valeurRecu[:4],16)
        codeRuche_recu = int(valeurRecu[4:6],16)
        indiceCapteur_recu = int(valeurRecu[6:8],16)
        valeurCapteur_recu = int(valeurRecu[8:],16) #valeur recu en ieee 32bits ont doit alors le convertir en decimal avant de l utiliser 

        #convertion de la valeur du capteur recu en decimal (ieee 32bits -> decimal)
        sign = valeurCapteur_recu >> 31
        exponent = valeurCapteur_recu >> 23 & 0xFF
        mantissaBin = valeurCapteur_recu & 0x7FFFFF
        #calcul de la mantisse
        mantisse = 1
        for i in range(24):
            if (mantissaBin >> 23-i & 0b1) == 1:
                mantisse = mantisse + 2**-i
        #calcul de la valeur decimal
        floatIeee32Bits = (-1)**sign * (2**(exponent-127) * mantisse)

    else:
        print("erreur taille message LoRa recu")
        receptionLoRaOK = 1
        codeSysteme_recu = 0
        codeRuche_recu = 0
        indiceCapteur_recu = 0
        floatIeee32Bits = 0

    return codeSysteme_recu, codeRuche_recu, indiceCapteur_recu, floatIeee32Bits, receptionLoRaOK


#---------------------------------------------------------Fonction MAIN---------------------------------------------------------
def main():
    #variable local
    erreurInitDB =0
    resulInitLoRa =0
    receptionLoRaOK =0
    codeSystemeRucheSet =0x2C33
    idRuche =0
    idRuchePrecedent =-1
    cptVal =0
    DateHeure = ""
    TempExter =0
    HumiExter =0
    TempInter =0
    HumidInter =0
    Proxi =0
    Masse =0
    Vbat =0
    
    #initialisation du port serie
    ser = serial.Serial(
        port='/dev/ttyAMA0',
        baudrate=9600,
        bytesize = serial.EIGHTBITS,
        stopbits = serial.STOPBITS_ONE,
        parity = serial.PARITY_NONE,
        xonxoff = True,
        timeout=0.1
    )

    #verification de l initialisation de l UART
    if(ser.isOpen()):
        ser.flushInput()
        ser.flushOutput()
        
        #initialisation du module LoRa
        resulInitLoRa = initLoRa(
            frequence = 868, 
            SpreadFactor = "SF12", 
            BandWidth = 125, 
            TX_Preamble = 12, 
            RX_Preamble = 15, 
            Power = 14, 
            CRC = "ON", 
            Inverted_IQ = "OFF", 
            Public_LoRaWAN = "OFF", 
            portSerieLoRa =ser)

    #connection a la base de donne
    conn, cur, erreurInitDB = connectionDB("localhost", 3306, "BERUCHE", "userBERUCHE", "BERUCHE")

    #gestion du resultat de l initialisation
    if((resulInitLoRa == 1) or (erreurInitDB == 1)):
        print("intialisation du programme KO")
        print("fermeture du programme")
        sys.exit()
    print("intialisation du programme OK")

    #boucle de reception en continue
    while(True):
        if(ser.inWaiting() != 0):   #attente d un nouveau message dans le buffer UART
            codeSysteme, idRuche, indiceCapteur, valeurCapteur, receptionLoRaOK = decodageMessageLoRa(ser)    #lecture du message LoRa
            #traitement des erreurs possible, ou affichage des valeurs
            if((codeSysteme == codeSystemeRucheSet) and (receptionLoRaOK == 0)):
                #reception d un stream de donnee
                if(idRuchePrecedent != idRuche):
                    t0 = time.time()
                #
                if(int(time.time() - t0) > 20):
                    idRuchePrecedent =-1
                    cptVal = 0
                    TempExter =0
                    HumiExter =0
                    TempInter =0
                    HumidInter =0
                    Proxi =0
                    Masse =0
                    Vbat =0
                else:
                    idRuchePrecedent = idRuche
                    cptVal = cptVal + 1
                    if(indiceCapteur == 0x0):   #SHT31_Temp
                        TempExter = valeurCapteur
                    elif(indiceCapteur == 0x1): #SHT31_Humid
                        HumiExter = valeurCapteur
                    elif(indiceCapteur == 0x2): #DHT11_Temp
                        TempInter = valeurCapteur
                    elif(indiceCapteur == 0x3): #DHT11_Humid
                        HumidInter = valeurCapteur
                    elif(indiceCapteur == 0x4): #PIR
                        Proxi = valeurCapteur
                    elif(indiceCapteur == 0x5): #HX711
                        Masse = valeurCapteur
                    else:   #Vbat
                        Vbat = valeurCapteur
                    #affichage des valeurs une fois qu elle sont toutes recu
                    if(cptVal >= 7):
                        idRuchePrecedent =-1
                        cptVal =0
                        DateHeure = str(datetime.now(timezone('UTC')).astimezone(timezone('Europe/Paris')))[:19]    #recuperation de l heure de Paris
                        print("")
                        print("Tous les valeurs recu sont:")
                        print("Date :" + DateHeure)
                        print("idRuche: " +str(idRuche))
                        print("TempInter: " +str(TempInter) +"°C")
                        print("HumidInter: " +str(HumidInter) +"%")
                        print("TempExter: " +str(TempExter) +"°C")
                        print("HumiExter: " +str(HumiExter) +"%")
                        print("Proxi: " +str(Proxi))
                        print("Masse: " +str(Masse) +"g")
                        print("Vbat: " +str(Vbat) +"V")
                        #insertion des datas dans la DB
                        insertionDB(DateHeure, idRuche, TempInter, HumidInter, TempExter, HumiExter, Masse, Proxi, Vbat, conn, cur)

#lancement de la fonction main
if __name__ == '__main__':
    main()