#!/usr/bin/env python
"""
Text to speech based on svox pico tts.
"""
import ctts
import os

_LANG_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), "languages")


class TtsEngine(object):
    """
    A text to speech engine class based on svox pico tts.
    """

    def __init__(self, language="en-GB", lang_dir=None):
        """
        Initialize engine with the given language.
        Currently available languages are: en-GB, en-US, de-DE, es-ES, fr-FR, it-IT
        lang_dir: An optional language directory where the requested language 
        definition files can be found.
        """
        lang_dirs = ["/usr/share/pico/lang/", _LANG_DIR]
        if lang_dir:
            lang_dirs.insert(0, lang_dir)

        self.__e = None
        for ldir in lang_dirs:
            try:
                self.__e = ctts.engine_create(language_dir=ldir, language=language)
            except RuntimeError as ex:
                pass # Try next directory to find language...
            if self.__e:
                break

        if self.__e is None:
            raise RuntimeError("Could not instantiate TTS engine with language " + language)

    def speak(self, text, callback=None):
        """
        Speak some text using the initialized speech engine.
        callback: optinal callback should follow the signature
        (format, audio, fin) -> Boolean
        format: is the format of the PCM data within the audio buffer
        audio: pcm data of the generated speech
        fin: When True, indicates that this is the last audio chunk of the stream
        If the return value evaluates to false then the speech generation is stopped.

        If a callback is not supplied, the PCM audio data for the entire text are returned.
        """
        data = []
        if callback is None:
            callback = lambda format, audio, fin: data.append(audio)
        ctts.engine_speak(self.__e, text, callback)
        if data:
            return ''.join(data)

    @property
    def rate(self):
        """
        Get/Set the speech rate (speed).
        """
        return self.get_property("rate")

    @rate.setter
    def rate(self, value):
        i = self.set_property("rate", value)
        if i != value:
            raise ValueError("Requested rate is beyond the acceptable limits")

    @property
    def pitch(self):
        """
        Get/Set voice pitch.
        """
        return self.get_property("pitch")

    @pitch.setter
    def pitch(self, value):
        i = self.set_property("pitch", value)
        if i != value:
            raise ValueError("Requested pitch is beyond the acceptable limits")

    @property
    def volume(self):
        """
        Get/Set voice volume.
        """
        return self.get_property("volume")

    @volume.setter
    def volume(self, value):
        i = self.set_property("volume", value)
        if i != value:
            raise ValueError("Requested volume is beyond the acceptable limits")

    def set_property(self, property_name, value):
        """
        Set an engine property. Returns the effective property value.
        """
        return ctts.engine_set_property(self.__e, property_name, value)

    def get_property(self, property_name):
        """
        Get an engine property value.
        """
        return ctts.engine_get_property(self.__e, property_name)

    def stop(self):
        """
        Stop speech synthesis.
        """
        return ctts.engine_stop(self.__e)


def tts_engine_test():
    """
    Run a small test
    """
    engine = TtsEngine('en-US')
    audio = engine.speak("The rain in Spain stays mainly in the plain.")
    with open("/tmp/spain.raw", "wb") as outfile:
        outfile.write(audio)

if __name__ == "__main__":
    tts_engine_test()

    
    
