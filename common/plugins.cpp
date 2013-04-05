#include "common/plugins.h"

#include <stdexcept>

Ponyca::PluginsInterface::PluginsInterface() {
    Py_Initialize();
    PyObject *sys_path = PySys_GetObject("path");
    PyObject *path = PyUnicode_FromString(".");
    PyList_Append(sys_path, path);
    Py_DECREF(path);

    PyObject *name = PyUnicode_FromString("ponyca");
    Python = PyImport_Import(name);
    Py_DECREF(name);
    if (Python == NULL) {
        PyErr_Print();
        throw std::runtime_error("Cannot load ponyca.py");
    }

    
}
Ponyca::PluginsInterface::~PluginsInterface() {
    Py_Finalize();
}
