import os
import sys
import struct
import zlib
import time

def pack_image(fw_file, boot_file):

    print 'app file_name:', fw_file 
    print 'boot file_name:', boot_file

    fname = os.path.splitext(fw_file)
    f = file(boot_file, 'rb')
    bootloader = f.read()
    f.close()

    f = file(fw_file, 'rb')
    data_fw = f.read()
    f.close()

    f = file(fname[0] + '_all.bin', 'wb')
    i = len(bootloader)
    
    f.write(bootloader)

    j = 0x11000 - i
    i = 0 
    while i < j:
        i += 1
        f.write("\xff")

    f.write(data_fw)
    f.close();

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0], "firmware file"
        exit(0)
    pack_image(sys.argv[1], sys.argv[2])
