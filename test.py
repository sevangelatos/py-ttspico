import os
import ctts

def callback(data_format, data, fin):
    print(data_format, len(data), fin)

    with open('/tmp/hello.raw', 'ab') as outfile:
        outfile.write(data)

if __name__ == '__main__':
    language_dir = os.path.join(os.path.dirname(ctts.__file__), 'languages')

    engine = ctts.engine_create(language_dir=language_dir)
    ctts.engine_set_property(engine, "pitch", 100)

    ctts.engine_speak(engine, "This is a test.", callback)
