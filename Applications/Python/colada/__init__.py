""" This module sets up root logging and loads the Colada library modules into its namespace."""

#-----------------------------------------------------------------------------
def _createModule(name, globals, docstring):
  import imp
  import sys
  moduleName = name.split('.')[-1]
  module = imp.new_module( moduleName )
  module.__file__ = __file__
  module.__doc__ = docstring
  sys.modules[name] = module
  globals[moduleName] = module

#-----------------------------------------------------------------------------
# Create colada.modules and colada.moduleNames

_createModule('colada.modules', globals(),
"""This module provides an access to all instantiated Colada modules.

The module attributes are the lower-cased Colada module names, the
associated value is an instance of ``qSlicerAbstractCoreModule``.
""")

_createModule('colada.moduleNames', globals(),
"""This module provides an access to all instantiated Colada module names.

The module attributes are the Colada modules names, the associated
value is the module name.
""")

#-----------------------------------------------------------------------------
# Load modules: Add VTK and PythonQt python module attributes into colada namespace

try:
  from .kits import available_kits
except ImportError as detail:
  available_kits = []

import string, os, sys
standalone_python = "python" in str.lower(os.path.split(sys.executable)[-1])

for kit in available_kits:
  # skip PythonQt kits if we are running in a regular python interpreter
  if standalone_python and "PythonQt" in kit:
    continue

  try:
    exec("from %s import *" % (kit))
  except ImportError as detail:
    print(detail)

  del kit

#-----------------------------------------------------------------------------
# Cleanup: Removing things the user shouldn't have to see.

del _createModule
del available_kits
del standalone_python
