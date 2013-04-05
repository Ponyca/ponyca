import operator
import importlib
import functools

_ = lambda x:x # We'll use later later for internationalization

print('Loading ponyca.py.')

class InvalidPlugin(Exception):
    pass

class Plugin:
    __plugins = set()
    @classmethod
    def get_plugins(cls):
        return cls.__plugins.copy()
    @classmethod
    def load(cls, name):
        try:
            plugin = getattr(importlib.import_module(name), name)
        except ImportError:
            raise InvalidPlugin(name)
        if not issubclass(plugin, Plugin):
            raise InvalidPlugin(plugin)
        cls.__plugins.add(plugin())

Plugin.load('Admin')

def _run_command(line, player):
    plugins = set()
    command = line.split(' ', 1)[0]
    for plugin in Plugin.get_plugins():
        if hasattr(plugin, command):
           plugins.add(plugin) 
    if len(plugins) > 1:
        print(_('Error: The following plugins have the %r command: %s') %
                (command, ', '.join(map(lambda x:x.__class__.__name__, plugins))))
        return False
    elif len(plugins) == 0:
        return False
    else:
        getattr(list(plugins)[0], command)(player, line, line.split(' ')[1:])
        return True

