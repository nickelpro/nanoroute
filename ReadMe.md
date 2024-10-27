[![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/nickelpro/nanoroute/test.yaml?style=for-the-badge)](https://github.com/nickelpro/nanoroute/actions/workflows/test.yaml)

# nanoroute

A small Python HTTP URL routing facility capable of sub-microsecond routing,
typically <200ns.

Nanoroute is a C++ implementation of a modified version of the priority
radix-tree algorithm pioneered by Julien Schmidt's
[httprouter](https://github.com/julienschmidt/httprouter). Like httprouter,
nanorouter's underlying implementation performs no allocations in the common
case, although some allocations are necessary to create the PyObjects necessary
for the CPython bindings.

The nanoroute package is suitable as a building block for more fully featured
HTTP frameworks. It also provides a simple WSGI interface for integration
directly into WSGI application stacks.

The intended use cases are high-performance message brokers, dispatchers, and
ingestion endpoints. The performance improvements of a high-speed router are
unlikely to be very significant in a typical database-backed Python REST API.

## Quickstart

For complete documentation, see the [docs]().

#### Installation

Nanoroute only supports Python 3.13+. It is available via PyPI, any mechanism
of installing Python packages from PyPI will work:

```
pip install nanoroute
```

#### Registering Routes

Nanoroute provides a single class, the `router`, which can be used to register
handlers for common HTTP verbs.

```python
import nanoroute

app = nanoroute.router()

@app.get('/')
def root(*args, **kwargs):
  ...

@app.post('/endpoint')
def endpoint(*args, **kwargs):
  ...
```

The verbs supported via these convenience methods are `GET`, `POST`, `PUT`, and
`DELETE`. Arbitrary sets of any ***valid*** HTTP verbs can be registered
using `router.route()`.

```python
# Register for a single HTTP verb
@app.route('PATCH', '/object')
def handle_patch(*args, **kwargs):
  ...

# Register for a multiple HTTP verbs
@app.route(['POST', 'PUT'], '/multi-meth')
def handle_multi(*args, **kwargs):
  ...
```

Finally, any arbitrary object can be registered with nanoroute. The decorator
syntax is merely convenient for typical usage.

```python
# Register arbitrary object for GET '/'
app.get('/')(object())
```

#### Capturing Parameters

Two forms of parameter capture are available, segment capture and catchall.
Segment captures are delimited by `:` and capture from the appearance of the
delimiter until the following `/` or the end of the URL. Catchalls are delimited
by `*` and capture the entire URL following their appearance.

Both types of parameter capture may be followed by a name, which will used as
the key associated with the parameter during route lookup. Anonymous parameters
act as wildcards, they have the same behavior as named parameters but the
captured data is not reported during lookup.

```python
# Captures the middle segment with the name "id"
@app.get('/user/:id/profile')
def get_profile(*args, **kwargs):
  ...

# Captures the article ID into "id", and then everything after the final "/"
# is captured as "slug" which might contain multiple path segments
@app.get('/article/:id/*slug')
def article(*args, **kwargs):
  ...

# The first path segment is a wildcard, anything may appear, but nothing is
# captured during route lookup
@app.get('/:/anonymous')
def anon(*args, **kwargs):
  ...
```

Captures are allowed to appear at arbtirary points in a given segment, so
long as multiple captures do not appear in the same segment.

```python
# Captures an "id" in the middle of a segment
@app.get('/user_:id/profile')
def get_profile(*args, **kwargs):
  ...

# Error: Invalid wildcard construction. Only one capture is allowed to appear
# in a given path segment
@app.get('/user_:id_:name')
def this_is_an_error(*args, **kwargs):
  ...
```

Captures that appear in the same place for different paths may have different
names, which will be recorded appropriately.

```python
# Captures the first segment as "foo"
@app.get('/:foo/alpha')
def alpha(*args, **kwargs):
  ...

# Captures the first segment as "bar"
@app.get('/:bar/beta')
def beta(*args, **kwargs):
  ...
```

#### Lookup

The base lookup facility is `router.lookup()`, which is intended for other
frameworks to use as a building block. It takes a method and path as arguments
and returns the registered handler and a parameter capture dictionary.

```python
@app.get('/user/:name')
def say_hello(params):
  print(f'Hello {params['name']}!')

handler, params = app.lookup('GET', '/user/Jane')
handler(params)

# >>> Hello Jane!
```

As a convenience, a WSGI application is also provided. It is directly equivalent
to the following code:
```python
def wsgi_app(environ, start_response):
  handler, params = app.lookup(environ['REQUEST_METHOD'], environ['PATH_INFO'])
  environ['nanoroute.params'] = params
  return handler(environ, start_response)

app.wsgi_app = wsgi_app
```
