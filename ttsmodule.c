#include <Python.h>
#include <stdio.h>

static PyObject *
say(PyObject *self, PyObject *args)
{
    const char *message;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;
    sts = printf("You said: %s\n", message);
    return Py_BuildValue("i", sts);
}


static PyObject *
n_times(PyObject *self, PyObject *args)
{
    PyObject *callback;
    int i, N;

    if (PyArg_ParseTuple(args, "iO:n_times", &N, &callback)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
 
    for (i = 0; i < N; i++) {
        PyObject_CallObject(callback, Py_BuildValue("(i)", i));
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef TtsMethods[] = {
    {"say",  say, METH_VARARGS, "Say something."},
    {"n_times",  n_times, METH_VARARGS, "Do something N times."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC inittts(void)
{
    (void) Py_InitModule("tts", TtsMethods);
}