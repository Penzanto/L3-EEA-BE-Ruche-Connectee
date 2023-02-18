#import of the library
import serial #pyserial

#----------main function----------
def read_all(port, chunk_size=200):
    """Read all characters on the serial port and return them."""
    if not port.timeout:
        raise TypeError('Port needs to have a timeout set!')

    read_buffer = b''

    while True:
        # Read in chunks. Each chunk will wait as long as specified by
        # timeout. Increase chunk_size to fail quicker
        byte_chunk = port.read(size=chunk_size)
        read_buffer += byte_chunk
        if not len(byte_chunk) == chunk_size:
            break

    return read_buffer

def main():
    #init Serial
    ser = serial.Serial(
        port='COM6',
        baudrate=9600,
        bytesize = serial.EIGHTBITS,
        stopbits = serial.STOPBITS_ONE,
        parity = serial.PARITY_NONE,
        xonxoff = True,
        timeout=0.1
    )

    if ser.isOpen():    #verify that the serial port is open
        #flush the in and out serial buffer    
        ser.flushInput()
        ser.flushOutput()

        #type of message that could be sent
        ser.write('AT+PMSG="Data"'.encode())
        #ser.write('AT'.encode())
        #ser.write('AT+POWER'.encode())
        
         
        while ser.inWaiting() < 1: 
            True

        byteData = read_all(ser)
        print(byteData)


if __name__ == '__main__':
    main()

