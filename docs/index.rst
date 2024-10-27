=========
nanoroute
=========

.. image:: https://img.shields.io/github/actions/workflow/status/nickelpro/nanoroute/test.yaml
  :target: https://github.com/nickelpro/nanoroute/actions/workflows/test.yaml
  :alt: CI Status

.. image:: https://img.shields.io/badge/license-CC0--1.0-blue
  :target: https://creativecommons.org/publicdomain/zero/1.0/legalcode
  :alt: CC0-1.0

**nanoroute** is a fast, efficient, HTTP URL router for Python.

----

.. toctree::
  :hidden:

  install
  api


Features
========

* I should write stuff here
* And here
* And also here if I can think of a third thing


Project background
==================

**nanoroute** is a C++ implementation of a modified version of the priority
radix-tree algorithm pioneered by Julien Schmidt's httprouter. Like httprouter,
**nanoroute**'s underlying implementation performs no allocations in the common
case, although some allocations are necessary to create the PyObjects necessary
for the CPython bindings.

The **nanoroute** package is suitable as a building block for more fully
featured HTTP frameworks. It also provides a simple WSGI interface for
integration directly into WSGI application stacks.

The intended use cases are high-performance message brokers, dispatchers, and
ingestion endpoints. The performance improvements of a high-speed router are
unlikely to be very significant in a typical database-backed Python REST API.
