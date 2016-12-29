#!/usr/bin/env python
import ctts


class TtsEngine(object):

    def __init__(self, **kwargs):
        self.e = ctts.engine_create(**kwargs)

    def speak(self, text, callback=None):
        data = []
        append_callback = lambda format, audio, fin: data.append(audio)
        cb = callback if callback else append_callback
        ctts.engine_speak(self.e, text, cb)
        if data:
            return ''.join(data)
        else:
            return None

    def set_property(self, property, value):
        return ctts.engine_set_property(self.e, property, value)

    def get_property(self, property):
        return ctts.engine_get_property(self.e, property)
