import nanoroute

verbs = ('DELETE', 'GET', 'HEAD', 'POST', 'PUT', 'CONNECT', 'OPTIONS', 'TRACE',
         'COPY', 'LOCK', 'MKCOL', 'MOVE', 'PROPFIND', 'PROPPATCH', 'SEARCH',
         'UNLOCK', 'BIND', 'REBIND', 'UNBIND', 'ACL', 'REPORT', 'MKACTIVITY',
         'CHECKOUT', 'MERGE', 'M-SEARCH', 'NOTIFY', 'SUBSCRIBE', 'UNSUBSCRIBE',
         'PATCH', 'PURGE', 'MKCALENDAR', 'LINK', 'UNLINK', 'SOURCE', 'PRI',
         'DESCRIBE', 'ANNOUNCE', 'SETUP', 'PLAY', 'PAUSE', 'TEARDOWN',
         'GET_PARAMETER', 'SET_PARAMETER', 'REDIRECT', 'RECORD', 'FLUSH',
         'QUERY')


def handle(environ, callback):
  callback(environ)


def test_get():
  called = False

  def cb(environ):
    assert environ['REQUEST_METHOD'] == 'GET'
    nonlocal called
    called = True

  router = nanoroute.router()
  router.get('/')(handle)
  router.wsgi_app({'REQUEST_METHOD': 'GET', 'PATH_INFO': '/'}, cb)

  assert called


def test_post():
  called = False

  def cb(environ):
    assert environ['REQUEST_METHOD'] == 'POST'
    nonlocal called
    called = True

  router = nanoroute.router()
  router.post('/')(handle)
  router.wsgi_app({'REQUEST_METHOD': 'POST', 'PATH_INFO': '/'}, cb)

  assert called


def test_put():
  called = False

  def cb(environ):
    assert environ['REQUEST_METHOD'] == 'PUT'
    nonlocal called
    called = True

  router = nanoroute.router()
  router.put('/')(handle)
  router.wsgi_app({'REQUEST_METHOD': 'PUT', 'PATH_INFO': '/'}, cb)

  assert called


def test_delete():
  called = False

  def cb(environ):
    assert environ['REQUEST_METHOD'] == 'DELETE'
    nonlocal called
    called = True

  router = nanoroute.router()
  router.delete('/')(handle)
  router.wsgi_app({'REQUEST_METHOD': 'DELETE', 'PATH_INFO': '/'}, cb)

  assert called


def test_route():

  it = iter(verbs)

  def cb(environ):
    assert environ['REQUEST_METHOD'] == next(it)

  router = nanoroute.router()
  router.route(verbs, '/')(handle)

  for verb in verbs:
    router.wsgi_app({'REQUEST_METHOD': verb, 'PATH_INFO': '/'}, cb)

  router.__init__()
  it = iter(verbs)

  for verb in verbs:
    router.route(verb, '/')(handle)

  for verb in verbs:
    router.wsgi_app({'REQUEST_METHOD': verb, 'PATH_INFO': '/'}, cb)


def test_lookup():
  router = nanoroute.router()
  for verb in verbs:
    router.route(verb, '/')(verb)

  for verb in verbs:
    val, _ = router.lookup(verb, '/')
    assert val == verb


def test_noroute():
  router = nanoroute.router()
  for verb in verbs:
    try:
      router.lookup(verb, '/')
    except LookupError:
      pass
    else:
      assert False


def test_init():
  router = nanoroute.router()
  router.route(verbs, '/')(handle)

  router.__init__()

  for verb in verbs:
    try:
      router.lookup(verb, '/')
    except LookupError:
      pass
    else:
      assert False


def test_decorator():
  router = nanoroute.router()

  @router.get('/')
  def handle(environ, _):
    assert environ['REQUEST_METHOD'] == 'GET'

  assert handle

  router.wsgi_app({'REQUEST_METHOD': 'GET', 'PATH_INFO': '/'}, lambda _: None)


def test_absolute():
  router = nanoroute.router()
  router.get('/a/b')('c')
  val, caps = router.lookup('GET', '/a/b')
  assert val == 'c'
  assert caps == {}


def test_capture():
  router = nanoroute.router()
  router.get('/:a/:b')(None)
  _, caps = router.lookup('GET', '/alpha/beta')
  assert caps['a'] == 'alpha'
  assert caps['b'] == 'beta'


def test_disambiguate():
  router = nanoroute.router()
  router.get('/:a/alpha')(1)
  router.get('/:b/beta')(2)

  val, caps = router.lookup('GET', '/foo/alpha')
  assert val == 1
  assert len(caps) == 1
  assert caps['a'] == 'foo'

  val, caps = router.lookup('GET', '/bar/beta')
  assert val == 2
  assert len(caps) == 1
  assert caps['b'] == 'bar'


def test_catchall():
  router = nanoroute.router()
  router.get('/:a/*b')(1)

  val, caps = router.lookup('GET', '/alpha/foo/bar')
  assert val == 1
  assert len(caps) == 2
  assert caps['a'] == 'alpha'
  assert caps['b'] == 'foo/bar'
