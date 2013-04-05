#include "common/plugins.h"

#include <stdexcept>

Ponyca::PluginsInterface::PluginsInterface() {
    Py_Initialize();
    PyObject *sys_path = PySys_GetObject("path");
    PyObject *path = PyUnicode_FromString("../plugins/");
    PyList_Append(sys_path, path);
    Py_DECREF(path);

    PyObject *name = PyUnicode_FromString("ponyca");
    Python = PyImport_Import(name);
    Py_DECREF(name);
    if (!Python) {
        PyErr_Print();
        throw std::runtime_error("Cannot load ponyca.py");
    }

    
}
Ponyca::PluginsInterface::~PluginsInterface() {
    Py_Finalize();
}

bool Ponyca::PluginsInterface::runCommand(std::string const &line, AbstractPlayer &player) {
    PyObject *func = PyObject_GetAttrString(Python, "_run_command");
    if (!func) {
        PyErr_Print();
        throw std::runtime_error("ponyca.py lacks the run_command function.");
    }
    if (!PyCallable_Check(func))
        throw std::runtime_error("ponyca.run_command is not callable.");
    PyObject *args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, PyUnicode_FromString(line.c_str()));
    PyTuple_SetItem(args, 1, GetPythonPlayer(player));

    PyObject *p_handled = PyObject_CallObject(func, args);
    Py_DECREF(func);
    Py_DECREF(args);
    if (p_handled) {
        return (p_handled == Py_True);
    }
    else {
        PyErr_Print();
        return false;
    }

}

PyObject *Ponyca::PluginsInterface::GetPythonPlayer(AbstractPlayer &player) {
    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(player.getUsername().c_str()));
    PyTuple_SetItem(tuple, 1, PyLong_FromLong(player.getEntityId()));
    return tuple;
}
