#!/usr/bin/env python

# Copyright 2016 Jeremy Sanders

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# this example fits a straight line to some data with parameters
# called "icept" and "grad" for the intercept and gradient.

import numpy as N
import os
import struct
import sys

# these data were generated from a Gaussian distribution with mean=0
# and sigma=1 (i.e. numpy.random.normal(size=20))
xvals = N.arange(20)
data = N.array([
    0.25844339, -0.42944506,  0.24051914, -1.91083027,  0.69343687,
    0.58315709, -1.47436764,  2.02874907, -1.10249608,  1.45577117,
    0.37475427,  1.24337795, -0.67232437,  0.71132063,  1.37279597,
    0.36225288,  0.10259248, -1.2627675 ,  0.34424622, -0.04330472])

def readlen(fd, length):
    """Read length bytes of data from file descriptor. read may return
    less than we expect, so this helper keeps reading until we get the
    required amount.
    """

    retn = b''
    while len(retn) < length:
        extra = os.read(fd, length-len(retn))
        if(len(extra)==0):
            print("End of data")
            sys.exit(0)
        retn += extra
    return retn

def params(fd):
    """Return list of parameters to dnest3 caller."""
    out = struct.pack('i', 2)
    out += struct.pack('dddd32s', 0, -100, 100, 200, "icept")
    out += struct.pack('dddd32s', 0, -100, 100, 200, "grad")
    os.write(fd, out)
    os.close(fd)

def run(fd):
    """Normal operation. Read a set of parameter values from the socket,
    then write out the likelihood. Parameters and result are binary
    double values."""

    while True:
        remotepars = readlen(fd, 8*2)
        pars = N.fromstring(remotepars, dtype=N.float64)

        model = pars[0]+xvals*pars[1]
        chi2 = N.sum((model-data)**2 / 1.)
        like = -0.5*chi2

        os.write(fd, struct.pack('d', float(like)))

def main():
    # this is the file descriptor we read and write to
    fd = int(sys.argv[2])

    # select correct mode
    if sys.argv[1] == "params":
        params(fd)
    elif sys.argv[1] == "run":
        run(fd)

if __name__ == '__main__':
    main()
