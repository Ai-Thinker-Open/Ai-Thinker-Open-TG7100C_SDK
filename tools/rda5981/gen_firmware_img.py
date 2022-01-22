import os
import sys
import struct
import zlib
import time

def pack_image(fw_file, version, boot_file, enc_arg, factory_fw):

    fname = os.path.splitext(fw_file)
    with_factory = "false"
    print 'firmware file:', fw_file
    print 'enc image file:', fname[0] + '.bin.lzma'
    print 'bootloader:', boot_file
    print 'enc arg:', enc_arg
    if os.access(factory_fw, os.F_OK):
        with_factory = "true"
        print 'factory firmware:', factory_fw
    f = file(boot_file, 'rb')
    bootloader = f.read()
    f.close()

    f = file(fw_file, 'rb')
    data_fw = f.read()
    f.close()

    f = file(fname[0] + '.bin.lzma', 'rb')
    data_img = f.read()
    f.close()

    magic_img = 0xAEAE
    magic_fw  = 0x0000EAEA
    if(enc_arg == "HF"):
        encrypt_algo = 0
    else:
        encrypt_algo = 1
    rescv_img = 0

    time_fmt='%Y%m%d%H%M%S'

    crc32_img = zlib.crc32(data_img, 0) & 0xFFFFFFFF
    crc32_img ^= 0xFFFFFFFF
    crc32_fw = zlib.crc32(data_fw, 0) & 0xFFFFFFFF
    crc32_fw ^= 0xFFFFFFFF
    if(enc_arg == "HF"):
        version = str(crc32_fw)
    else:
        version = time.strftime( time_fmt, time.localtime() )

    size_img = len(data_img)
    size_fw  = len(data_fw)

    if(enc_arg == "HF"):
        header_img = struct.pack("<HBB24sLL", magic_img, encrypt_algo, rescv_img, version, crc32_img, size_img)
    else:
        header_img = struct.pack("<HBB16sLLLL", magic_img, encrypt_algo, rescv_img, version, crc32_fw, size_fw, crc32_img, size_img)

    f = file(fname[0] + '_ota.bin', "wb")
    f.write(header_img)
    f.write(data_img)
    f.close()

    i = len(bootloader)
    if(i > 8192):
        print 'bootloader sizeover flow!!!'
        exit(1)
        
    f = file(fname[0] + '_all.bin', "wb")
    f.write(bootloader)

    print 'ota image version:', version
    print 'ota image size:', size_img
    print 'ota image crc32: %08x' % crc32_img
    print 'firmware size:', size_fw
    print 'firmware crc32: %08x' % crc32_fw
    print 'bootloader size:', i

    while i < 8192:
        i += 1
        f.write("\xff")

    if("true" == with_factory):
        f_fact = open(factory_fw, 'r')
        buffer_fact = f_fact.read()
        f_fact.close()
        size_fact = len(buffer_fact)
        crc32_fact = zlib.crc32(buffer_fact, 0) & 0xFFFFFFFF
        crc32_fact ^= 0xFFFFFFFF
        magic_fact = 0x4FAC
        addr_fact =  0x18095000
        resv_fact = 0
        header_fact = struct.pack("<L16sLLLLL", magic_fact, version, addr_fact, size_fact, crc32_fact, resv_fact, resv_fact)
        i += len(header_fact)
        f.write(header_fact)
    elif(enc_arg != "HF"):
        addr_fw = 0x18004000
        resv_fw = 0
        header_fw = struct.pack("<L16sLLLLL", magic_fw, version, addr_fw, size_fw, crc32_fw, resv_fw, resv_fw)
        i += len(header_fw)
        f.write(header_fw)
    while i < 12288:
        i += 1
        f.write("\xff")
    i += len(data_fw)
    f.write(data_fw)
    if("true" == with_factory):
        while i < (12288 + 593920):
            i += 1
            f.write("\xff")
        f.write(buffer_fact)
    f.close()

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print sys.argv[0], "firmware file"
        exit(0)

    pack_image(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
