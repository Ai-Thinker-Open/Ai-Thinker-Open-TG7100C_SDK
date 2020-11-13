# Date    : 2018/07/26
# Author  : Snow Yang
# Mail    : yangsw@mxchip.com

import sys
from struct import pack

textimage = sys.argv[1]
dataimage = sys.argv[2]
outimage  = sys.argv[3]

image = b''
with open(textimage, 'rb') as f:
    data = f.read()
    size = len(data)
    image += b'81958711'
    image += pack('<L', size)
    image += pack('<L', 0)
    image += b'\xFF'*16
    image += data
with open(dataimage, 'rb') as f:
    data = f.read()
    size = len(data)
    image += b'81958711'
    image += pack('<L', size)
    image += pack('<L', 0x1000D000)
    image += b'\xFF'*16
    image += data
image = image[:0x10] + pack('<L', len(image)) + image[0x14:]

with open(outimage, 'wb') as f:
    f.write(image)