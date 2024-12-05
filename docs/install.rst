Installation
============

**nanoroute** requires Python 3.13 or newer. Pre-compiled wheels are available
for:

* Windows (x86_64)
* MacOS 13.3+ (arm64)
* manylinux_2_39 (x86_64)

On the supported platforms installing **nanoroute** is as simple as invoking
your favorite Python package manager, for example with ``pip``::

  $ python -m pip install nanoroute


Building From Source
--------------------

**nanoroute** is built using
`py-build-cmake <https://tttapa.github.io/py-build-cmake/>`_, a PEP 517
compliant build backend for building CMake-based Python extensions. At a
minimum building from source requires the following:

* A C++23 supporting compiler
* CPython 3.13+
* CMake 3.25+
* A suitable build system for CMake

Given these, **nanoroute** can be built with any PEP 517 compliant build
frontend. For example, using `build <https://build.pypa.io/en/stable/>`_::

  $ git clone https://github.com/nickelpro/nanoroute.git
  $ python -m build nanoroute

Configuration
^^^^^^^^^^^^^

Local overrides for **nanoroute** configuration can be passed using
``py-build-cmake.local.toml`` or via command line overrides like so::

  python -m build nanoroute -C--local=overrides.toml

See the
`py-build-cmake documentation <https://tttapa.github.io/py-build-cmake/Config.html>`_
for a more complete discussion of using overrides.

**nanoroute** mostly uses native CMake variables, so to override the C++
compiler it would appropriate to modify ``CMAKE_CXX_COMPILER``, or to control
the MSVC runtime modify ``CMAKE_MSVC_RUNTIME_LIBRARY``, etc. **nanoroute** has
two additional options worth noting:

* ``NANOROUTE_USE_IPO``: Use interprocedural optimization if supported
  (**Default:** On)
* ``NANOROUTE_STRIP``: Run system strip on the compiled module (**Default:** On)
