import Graph

class _AssertRaisesBaseContext(object):
    def __init__(self, expected, callable_obj=None):
        self.expected = expected
        if callable_obj is not None:
            try:
                self.obj_name = callable_obj.__name__
            except AttributeError:
                self.obj_name = str(callable_obj)
        else:
            self.obj_name = None


class _AssertRaisesContext(_AssertRaisesBaseContext):
    """A context manager used to implement TestCase.assertRaises* methods."""
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, tb):
        if exc_type is None:
            try:
                exc_name = self.expected.__name__
            except AttributeError:
                exc_name = str(self.expected)
            if self.obj_name:
                raise AssertionError("{0} not raised by {1}".format(exc_name, self.obj_name))
            else:
                raise AssertionError("{0} not raised".format(exc_name))
        if not issubclass(exc_type, self.expected):
            # let unexpected exceptions pass through
            return False
        # store exception, without traceback, for later retrieval
        self.exception = exc_value.with_traceback(None)
        return True

def assertRaises(excClass, callableObj=None, *args, **kwargs):
    """Same as unittest.assertRaises()"""
    context = _AssertRaisesContext(excClass, callableObj)
    if callableObj is None:
        return context
    with context:
        callableObj(*args, **kwargs)


def Run(Level = 0):
    print("Running automatic tests...")
    Modules = Graph.LoadPlugins(Graph.GraphDir + "\\Test")
    for Module in Modules:
        Module.Run(Level)
