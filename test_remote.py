#!/usr/bin/env python

import numpy as N
import os
import struct
import sys

xvals = N.arange(20)
data = N.array([
    0.25844339, -0.42944506,  0.24051914, -1.91083027,  0.69343687,
    0.58315709, -1.47436764,  2.02874907, -1.10249608,  1.45577117,
    0.37475427,  1.24337795, -0.67232437,  0.71132063,  1.37279597,
    0.36225288,  0.10259248, -1.2627675 ,  0.34424622, -0.04330472])

def params(fd):
    """Get parameters."""
    out = struct.pack('i', 2)
    out += struct.pack('dddd32s', 0, -100, 100, 200, "icept")
    out += struct.pack('dddd32s', 0, -100, 100, 200, "grad")
    os.write(fd, out)
    os.close(fd)

def readlen(fd, l):
    retn = b''
    while len(retn) < l:
        extra = os.read(fd, l-len(retn))
        if(len(extra)==0):
            print("End of data")
            sys.exit(0)
        retn += extra
    return retn

def run(fd):
    while True:
        remotepars = readlen(fd, 8*2)
        pars = N.fromstring(remotepars, dtype=N.float64)

        model = pars[0]+xvals*pars[1]
        chi2 = N.sum((model-data)**2 / 1.)
        like = -0.5*chi2

        os.write(fd, struct.pack('d', float(like)))

def main():
    fd = int(sys.argv[2])
    if sys.argv[1] == "params":
        params(fd)
    elif sys.argv[1] == "run":
        run(fd)

if __name__ == '__main__':
    main()
