import operator
import importlib
import functools

_ = lambda x:x # We'll use later later for internationalization

print('Loading ponyca.py.')

class InvalidPlugin(Exception):
    pass

class Chat:
    """Object passed to command functions in order to provide them
    with useful resources (reply functions, ...)."""
    pass

class Msg:
    """A string, with extra data."""
    def __init__(self, line, player):
        self._line = line
        self._player = player
    @property
    def line(self):
        return self._line
    @property
    def player(self):
        return self._player

class Player:
    """Represents a player."""
    def __init__(self, username, entityId):
        assert isinstance(username, str), username
        assert isinstance(entityId, int), entityId
        self._username = username
        self._entityId = entityId
    @property
    def username(self):
        return self._username
    @property
    def entityId(self):
        return self._entityId

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
        getattr(list(plugins)[0], command)(Chat(), Msg(line, player), line.split(' ')[1:])
        return True

