import ponyca

class Admin(ponyca.Plugin):
    def load(self, player, msg, args):
        """<plugin name>

        Loads the plugin."""
        try:
            ponyca.Plugin.load(args[0])
        except ponyca.InvalidPlugin as e:
            print('%r is not a valid plugin' % e.args[0])
