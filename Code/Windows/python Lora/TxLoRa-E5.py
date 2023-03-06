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

    if(ser.isOpen()):
        ser.flushInput()
        ser.flushOutput()

        #initialisation du module LoRa
        ser.write('AT+MODE=TEST'.encode())
        receptionLoRa = lectureLoRa(ser, octetLecture=200, stringAttendu="TEST", timeOut=5)
        print(receptionLoRa)
        if(receptionLoRa != "-1"):
            ser.write('AT+LW=LDRO,ON'.encode())
            receptionLoRa = lectureLoRa(ser, octetLecture=200, stringAttendu="LDRO, ON", timeOut=5)
            print(receptionLoRa)
            if(receptionLoRa != "-1"):
                ser.write('AT+TEST=RFCFG,868,SF12,125,12,15,14,ON,OFF,OFF'.encode())
                receptionLoRa = lectureLoRa(ser, octetLecture=200, stringAttendu="RFCFG F:868000000, SF12, BW125K, TXPR:12, RXPR:15, POW:14dBm, CRC:ON, IQ:OFF, NET:OFF", timeOut=5)
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
            print("intialisation du module LoRa KO (ecriture du mode test)")
            print("fermeture du programme")
            sys.exit()

        #envoie la meme data en continue pour tester la reception des datas en mode P2P
        while(True):
            ser.write('AT+TEST=TXLRPKT, "2C33 00 01 AA"'.encode())
            receptionLoRa = lectureLoRa(ser, octetLecture=200, stringAttendu="TX DONE", timeOut=5)
            print(receptionLoRa)
            time.sleep(10)

#lancement de la fonction main
if __name__ == '__main__':
    main()