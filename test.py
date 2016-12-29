#!/usr/bin/env python
import ctts
import sys
outfile = open("/tmp/hello.raw", "wb")
def cb(format, data, fin):
    print format, len(data), fin
    outfile.write(data)

e = ctts.engine_create()
ctts.engine_set_property(e, "pitch", 100)
ctts.engine_speak(e, "This is a test.", cb)
