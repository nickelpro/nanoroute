API Reference
=============

Routing Definition Syntax
-------------------------

Route definition strings may contain two kinds of parameter delimiters, path
segments and catchalls.

Path segment parameters are delimited by a colon (``:``), and capture from their
appearance in the route definition until the following forward slash (``/``) or
the end of the URL. Only a single path segment parameter may exist in a given
URL path segment.

Path segment delimiters may optionally be a name, in which case the
captured text from the URL will be associated with that name in the returned
parameter dictionary when a lookup is performed.

Examples::

  '/blog/post/:post_name' # Captures final segment as "post_name"

  '/user_:id/profile'     # Captures URL between "user_" and "/" as "id"

  '/article/:/:id'        # Captures final segment as "id", the first ":"
                          # matches as a wildcard but does not report the
                          # captured text


Catchall are delimited by an asterisk (``*``) and work the same as path
segment delimiters, except they always match to the end of the URL including
any forward slashes.

Examples::

  '/article/:id/*slug' # Captures the second path segment as "id" and
                       # all following segments as "slug"

  '/random*'           # Matches any URL following "random", but does
                       # not report the captured text

Methods
-------

.. automodule:: nanoroute
  :members:
