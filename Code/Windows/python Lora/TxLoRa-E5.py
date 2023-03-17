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
    """Fonction d initialisation du module LoRa E5

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

    if(portSerieLoRa.isOpen()):
        portSerieLoRa.flushInput()
        portSerieLoRa.flushOutput()

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
    else:
        initialisaitonLoRaOK = 0

    return initialisaitonLoRaOK


def emission(portSerieLoRa, codeSysteme, codeRuche, indiceCapteur, valeurCapteur):
    """Fonction d emission d un message LoRa avec verification de la bonne emission d un message

    Args:
        portSerieLoRa (string): nom de la class du port serie physique du module LoRa
        codeSysteme (int): mot de passe utiliser pour la ruche
        codeRuche (int): indice de la ruche
        indiceCapteur (int): indice du capteur de la ruche
        valeurCapteur (int): valeur du capteur a envoyer
        
    Returns:
        int: 1 ou 0 selon si l emission du message c est bien passer ou non
    """

    #variable local
    emissionLoRaOK = 1

    #preparation et envoie de la trame 
    LoRaDataAEnvoyer = 'AT+TEST=TXLRPKT, "' + hex(codeSysteme) + hex(codeRuche) + hex(indiceCapteur) + hex(valeurCapteur) + '"' 
    portSerieLoRa.write(LoRaDataAEnvoyer.encode())

    #reception de l'acquittement du module LoRa 
    receptionLoRa = lectureLoRa(portSerieLoRa, octetLecture=200, stringAttendu="TX DONE", timeOut=5)
    if(receptionLoRa =="-1"):
        emissionLoRaOK = 0
    else:
        print(receptionLoRa)

    return emissionLoRaOK


#----------Fonction main----------
def main():
    #initialisation du port serie
    ser = serial.Serial(
        port='COM8',
        baudrate=9600,
        bytesize = serial.EIGHTBITS,
        stopbits = serial.STOPBITS_ONE,
        parity = serial.PARITY_NONE,
        xonxoff = True,
        timeout=0.1
    )

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

    #envoie la meme data en continue pour tester la reception des datas en mode P2P
    while(True):
        #emission d un message LoRa
        resulEmissionLoRa = emission(portSerieLoRa = ser,
            codeSysteme = 0x2c33,
            codeRuche = 0x00,
            indiceCapteur = 0x1,
            valeurCapteur = 0xAA)

        #gestion de l erreur demission
        if(resulEmissionLoRa !=1):
            print("Erreur emission message LoRa")
            print("fermeture du programme")
            sys.exit()

        #delay d attente
        time.sleep(10)


#lancement de la fonction main
if __name__ == '__main__':
    main()