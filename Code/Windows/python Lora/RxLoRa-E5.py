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


#----------Fonction main----------
def main():
    #variable local
    codeSystemeRuche = 0x2C33

    #initialisation du port serie
    ser = serial.Serial(
        port='COM6',
        baudrate=9600,
        bytesize = serial.EIGHTBITS,
        stopbits = serial.STOPBITS_ONE,
        parity = serial.PARITY_NONE,
        xonxoff = True,
        timeout=0.1
    )

    if(ser.isOpen()):
        ser.flushInput()
        ser.flushOutput()
        
        #initialisation du module LoRa'AT+MODE=TEST'
        ser.write('AT+MODE=TEST'.encode())
        receptionLoRa = lectureLoRa(port=ser, octetLecture=200, stringAttendu="+MODE: TEST", timeOut=5)
        print(receptionLoRa)
        if(receptionLoRa != "-1"):
            ser.write('AT+TEST=STOP'.encode())
            receptionLoRa = lectureLoRa(port=ser, octetLecture=200, stringAttendu="+TEST: STOP", timeOut=5)
            print(receptionLoRa)
            if(receptionLoRa != "-1"):
                ser.write('AT+LW=LDRO,ON'.encode())
                receptionLoRa = lectureLoRa(port=ser, octetLecture=200, stringAttendu="+LW: LDRO, ON", timeOut=5)
                print(receptionLoRa)
                if(receptionLoRa != "-1"):
                    ser.write('AT+TEST=RFCFG,868,SF12,125,12,15,14,ON,OFF,OFF'.encode())
                    receptionLoRa = lectureLoRa(port=ser, octetLecture=200, stringAttendu="+TEST: RFCFG F:868000000, SF12, BW125K, TXPR:12, RXPR:15, POW:14dBm, CRC:ON, IQ:OFF, NET:OFF", timeOut=5)
                    print(receptionLoRa)
                    if(receptionLoRa != "-1"):
                        print("initialisation du module LoRa OK")
                    else:
                        print("intialisation du module LoRa KO (ecriture des valeurs d emission 'freq, SF, bandepassante etc...')")
                        print("fermeture du programme")
                        sys.exit()
                else:
                    print("intialisation du module LoRa KO (ecriture de la valeur du LRDO)")
                    print("fermeture du programme")
                    sys.exit()
            else:
                print("intialisation du module LoRa KO (ecriture du mode STOP)")
                print("fermeture du programme")
                sys.exit()
        else:
            print("intialisation du module LoRa KO (ecriture du mode TEST)")
            print("fermeture du programme")
            sys.exit()

        #envoie la commande de reception de donnee en continue pour tester la reception des datas en mode P2P
        ser.write('AT+TEST=RXLRPKT'.encode())
        while(True):
            receptionLoRa = lectureLoRa(port=ser, octetLecture=200, stringAttendu='RX "', timeOut=20)
            if(receptionLoRa != "-1"):
                print(receptionLoRa)

                receptionLoRa_split = receptionLoRa.split('+TEST: RX "')
                valeurRecu = str(receptionLoRa_split[1])[:-3]
                if(len(valeurRecu) == 10):
                    codeSysteme_recu = int(valeurRecu[:4],16)
                    if(codeSysteme_recu == codeSystemeRuche):
                        codeRuche_recu = int(valeurRecu[4:6],16)
                        indiceCapteur_recu = int(valeurRecu[6:8],16)
                        valeurCapteur_recu = int(valeurRecu[8:],16)

                        print("codeSysteme: " + str(codeSysteme_recu))
                        print("codeRuche: " + str(codeRuche_recu))
                        print("indiceCapteur: " + str(indiceCapteur_recu))
                        print("valeurCapteur: " + str(valeurCapteur_recu))
                    else:
                        print("mauvaise cle systeme recu")


#lancement de la fonction main
if __name__ == '__main__':
    main()