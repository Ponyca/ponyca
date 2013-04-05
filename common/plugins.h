#ifndef COMMON_PLUGINS_H
#define COMMON_PLUGINS_H

#include <Python.h>
#include <string>
#include "common/singleton.h"
#include "common/player.h"

namespace Ponyca {
    class PluginsInterface : public Singleton<PluginsInterface> {
    public:
        PyObject *Python;
        PluginsInterface();
        ~PluginsInterface();

        bool runCommand(std::string const &line, AbstractPlayer &player);
    private:
        static PyObject *GetPythonPlayer(AbstractPlayer &player);
    };
};

#endif

