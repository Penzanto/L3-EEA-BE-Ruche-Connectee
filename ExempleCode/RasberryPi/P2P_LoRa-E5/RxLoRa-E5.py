#importation des libraries utilisees
import serial #pyserial
import time 
import sys


#----------Autre fonction----------
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
        int: 1 ou 0 selon si l initialisation c est bien passer ou non
    """

    #variable local
    initialisaitonLoRaOK = 1
    strAEnvoier = ""

    #passage du module Lora en "mode TEST"
    portSerieLoRa.write('AT+MODE=TEST'.encode())
    receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+MODE: TEST", timeOut=5)
    if(receptionLoRa =="-1"):
        initialisaitonLoRaOK = 0
        print("\n erreur ecriture mode TEST")
    else:
        print(receptionLoRa)

    #passage du module Lora en "mode STOP" (empeche le module de recevoir des information durant la reprogramation du mode)
    if(initialisaitonLoRaOK == 1):
        portSerieLoRa.write('AT+TEST=STOP'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+TEST: STOP", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 0
            print("\n erreur ecriture mode STOP")
        else:
            print(receptionLoRa)

    #reglage du LDRO du module Lora
    if(initialisaitonLoRaOK == 1):
        portSerieLoRa.write('AT+LW=LDRO,ON'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+LW: LDRO, ON", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 0
            print("\n erreur ecriture mode LDRO")
        else:
            print(receptionLoRa)

    #configuration des parametres d emission radio du module LoRa
    if(initialisaitonLoRaOK == 1):
        strAEnvoier = 'AT+TEST=RFCFG,' + str(frequence) + ',' + SpreadFactor + ',' + str(BandWidth) + ',' + str(TX_Preamble) + ',' + str(RX_Preamble) + ',' + str(Power) + ',' + CRC + ',' + Inverted_IQ + ',' + Public_LoRaWAN
        portSerieLoRa.write(strAEnvoier.encode())
        receptionLoRa = lectureLoRa(portSerieLoRa, octetLecture=200, stringAttendu="+TEST: RFCFG F", timeOut=5)
        if(receptionLoRa =="-1"):
            initialisaitonLoRaOK = 0
            print("\n erreur ecriture mode RFCFG")
        else:
            print(receptionLoRa)

    #configuration du module LoRa E5 en reception
    if(initialisaitonLoRaOK == 1):
        portSerieLoRa.write('AT+TEST=RXLRPKT'.encode())
        receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu="+TEST: RXLRPKT", timeOut=5)
    if(receptionLoRa =="-1"):
        initialisaitonLoRaOK = 0
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
        receptionLoRaOK (int): information sur la bonne reception d un message. si 1 message bien recu, si 0 erreur lors de la reception du message
    """
    #variable local
    receptionLoRa = "1"
    receptionLoRaOK = 1

    #lecture des donnees dans le buffer de la laision UART
    receptionLoRa = lectureLoRa(port=portSerieLoRa, octetLecture=200, stringAttendu='+TEST: RX "', timeOut=20)
    #gestion de l erreur
    if(receptionLoRa == "-1"):
        receptionLoRaOK = 0
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
        receptionLoRaOK = 0
        codeSysteme_recu = 0
        codeRuche_recu = 0
        indiceCapteur_recu = 0
        floatIeee32Bits = 0

    return codeSysteme_recu, codeRuche_recu, indiceCapteur_recu, floatIeee32Bits, receptionLoRaOK


#----------Fonction main----------
def main():
    #variable local
    codeSystemeRucheSet = 0x2C33
    receptionLoRaOK = 1

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

        #gestion du resultat de l initialisation
        if(resulInitLoRa != 1):
            print("intialisation du module LoRa KO")
            print("fermeture du programme")
            sys.exit()
        print("initialisation du module LoRa OK")

        #boucle de reception en continue
        while(True):
            if(ser.inWaiting() != 0):   #attente d un nouveau message dans le buffer UART
                codeSysteme, codeRuche, indiceCapteur, valeurCapteur, receptionLoRaOK = decodageMessageLoRa(ser)
                #traitement des erreurs possible, ou affichage des valeurs
                if(codeSysteme != codeSystemeRucheSet | receptionLoRaOK !=0):
                    print("Mauvais message recu")
                    print("")
                else:
                    print("codeSysteme: " + str(codeSysteme))
                    print("codeRuche: " + str(codeRuche))
                    print("indiceCapteur: " + str(indiceCapteur))
                    print("valeurCapteur: " + str(valeurCapteur))
                    print("")


#lancement de la fonction main
if __name__ == '__main__':
    main()