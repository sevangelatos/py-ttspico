#!/usr/bin/env python
import tts
def cb(format, data, fin):
    print format, len(data), fin
    open("hello.raw", "ab").write(data)

e = tts.engine_create()
tts.engine_set_property(e, "pitch", 150)
tts.engine_speak(e, "This is a test.", cb)
del e
