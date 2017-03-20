#include <Python.h>
#include <assert.h>
#include "tts_engine.h"

static const char *engine_name = "tts_engine";
static bool tts_callback(void *user, uint32_t rate, uint32_t format, int channels, uint8_t *audio, uint32_t audio_bytes, bool final);

// Unwrap the engine and destroy it
void engine_destroy(PyObject *engine_capsule)
{
    TtsEngine_Destroy((TTS_Engine *) PyCapsule_GetPointer(engine_capsule, engine_name));
}

// Create engine and wrap it in capsule
static PyObject *engine_create(PyObject *self, PyObject *args, PyObject *kw)
{
    char *lang_dir = "/usr/share/pico/lang/", *language = "en-GB";
    static char *kwlist[] = {"language", "language_dir", NULL};
    TTS_Engine *engine = NULL;

    PyArg_ParseTupleAndKeywords(args, kw, "|ss:engine_create", kwlist, &language, &lang_dir);
    engine = TtsEngine_Create(lang_dir, language, tts_callback);

    if (!engine) {
        PyErr_SetString(PyExc_RuntimeError, "Could not initialize engine with the specified language");
        return NULL;
    }

    return PyCapsule_New(engine, engine_name, engine_destroy);
}

// Helper function to unwrap engine handle from capsule
static TTS_Engine *get_engine(PyObject *engine_capsule)
{
    TTS_Engine *engine = (TTS_Engine *) PyCapsule_GetPointer(engine_capsule, engine_name);
    if (!engine) {
        PyErr_SetString(PyExc_TypeError, "Invalid engine handle");
    }
    return engine;
}

// Change engine properties
static PyObject *engine_set_property(PyObject *self, PyObject *args, PyObject *kw)
{
    PyObject *engine_capsule = NULL;
    const char *property_name = "";
    int value = 0;
    static char *kwlist[] = {"engine", "property", "value", NULL};
    TTS_Engine *engine = NULL;
    PyArg_ParseTupleAndKeywords(args, kw, "Osi:engine_set_property", kwlist, &engine_capsule, &property_name, &value);
    engine = get_engine(engine_capsule);
    if (!engine) {
        return NULL;
    }

    if (strcmp(property_name, "pitch") == 0) {
        value = TtsEngine_SetPitch(engine, value);
        return Py_BuildValue("i", value);
    } else if (strcmp(property_name, "rate") == 0) {
        value = TtsEngine_SetRate(engine, value);
        return Py_BuildValue("i", value);
    } else if (strcmp(property_name, "volume") == 0) {
        value = TtsEngine_SetVolume(engine, value);
        return Py_BuildValue("i", value);
    }

    // Error, no such property
    PyErr_SetString(PyExc_NotImplementedError, "Setting property not implemented");
    return NULL;
}

// Get engine properties
static PyObject *engine_get_property(PyObject *self, PyObject *args, PyObject *kw)
{
    PyObject *engine_capsule = NULL;
    const char *property_name = "";
    static char *kwlist[] = {"engine", "property", NULL};
    TTS_Engine *engine = NULL;
    PyArg_ParseTupleAndKeywords(args, kw, "Os:engine_get_property", kwlist, &engine_capsule, &property_name);
    engine = get_engine(engine_capsule);
    if (!engine) {
        return NULL;
    }

    if (strcmp(property_name, "pitch") == 0) {
        return Py_BuildValue("i", TtsEngine_GetPitch(engine));
    } else if (strcmp(property_name, "rate") == 0) {
        return Py_BuildValue("i", TtsEngine_GetRate(engine));
    } else if (strcmp(property_name, "volume") == 0) {
        return Py_BuildValue("i", TtsEngine_GetVolume(engine));
    } else if (strcmp(property_name, "format") == 0) {
        return Py_BuildValue("(iii)", 16000, 16, 1);
    }

    // Error, no such property
    PyErr_SetString(PyExc_NotImplementedError, "Property does not exist");
    return NULL;
}

typedef struct {
    PyThreadState *thread_save;
    PyObject *py_callback;
} CbContext;

static bool tts_callback(void *user, uint32_t rate, uint32_t format, int channels, uint8_t *audio, uint32_t audio_bytes, bool final)
{
    CbContext *cb = (CbContext *)user;
	bool keep_going = false;
    assert(cb);
    assert(cb->py_callback);
    assert(cb->thread_save);

    PyEval_RestoreThread(cb->thread_save); // Acquire GIL to run callback

    {
        // handover data to callback
        int r = rate, f = format;
        char *data = (char *)audio;
        int len = audio_bytes, fin = final;
        PyObject *ret = PyObject_CallFunction(cb->py_callback, "(iii)s#i", r, f, channels, data, len, fin);
        if (ret) {
            // None implies that no return value was implemented
            // For convenience, we assume this to mean lets keep going...
            keep_going = (Py_None == ret) || PyObject_IsTrue(ret);
            // Dispose return value
            Py_DECREF(ret);
        }
    }

    cb->thread_save = PyEval_SaveThread(); // Release GIL so other threads may run

    return keep_going;
}

static PyObject *engine_speak(PyObject *self, PyObject *args, PyObject *kw)
{
    CbContext context;
    PyObject *callback, *engine_capsule;
    const char *text;
    bool ok;
    TTS_Engine *engine = NULL;

    static char *kwlist[] = {"engine", "text", "callback", NULL};
    PyArg_ParseTupleAndKeywords(args, kw, "OsO:engine_speak", kwlist, &engine_capsule, &text, &callback);
    engine = get_engine(engine_capsule);
    if (!engine) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    // We need the callback and engine to stay alive during execution
    Py_INCREF(callback);
    Py_INCREF(engine_capsule);

    context.py_callback = callback;
    context.thread_save = PyEval_SaveThread(); // Release GIL
    ok = TtsEngine_Speak(engine, text, (void *)&context);
    PyEval_RestoreThread(context.thread_save); // Acquire GIL

    // Not needed any more...
    Py_DECREF(callback);
    Py_DECREF(engine_capsule);

    if (!ok) {
        PyErr_SetString(PyExc_RuntimeError, "Text to speech failed");
        return NULL;
    }

    // Return None
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *engine_stop(PyObject *self, PyObject *args)
{
    PyObject *engine_capsule = NULL;
    TTS_Engine *engine = NULL;
    PyArg_ParseTuple(args, "O:engine_stop", &engine_capsule);
    engine = get_engine(engine_capsule);
    if (!engine) {
        return NULL;
    }

    TtsEngine_Stop(engine);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef TtsMethods[] = {
    {"engine_create", (PyCFunction)engine_create, METH_VARARGS | METH_KEYWORDS, "Create TTS engine"},
    {"engine_set_property", (PyCFunction)engine_set_property, METH_VARARGS | METH_KEYWORDS, "Change engine properties"},
    {"engine_get_property", (PyCFunction)engine_get_property, METH_VARARGS | METH_KEYWORDS, "Read engine properties"},
    {"engine_speak", (PyCFunction)engine_speak, METH_VARARGS | METH_KEYWORDS, "Synthesize speech"},
    {"engine_stop", engine_stop, METH_VARARGS, "Interrupt speech synthesis"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyMODINIT_FUNC initctts(void)
{
    (void)Py_InitModule("ctts", TtsMethods);
}