#ifndef COMMON_PLUGINS_H
#define COMMON_PLUGINS_H

#include <Python.h>
#include "common/singleton.h"

namespace Ponyca {
    class PluginsInterface : public Singleton<PluginsInterface> {
    public:
        PyObject *Python;
        PluginsInterface();
        ~PluginsInterface();
    };
};

#endif

