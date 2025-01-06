import timeit

import nanoroute

router = nanoroute.router()

router.get('/')(1)
router.get('/hello')(2)
router.get('/hello/world')(3)
router.get('/goodbye')(4)
router.get('/goodbye/world')(5)

nr = timeit.timeit("router.lookup('GET', '/hello/world')", globals=locals())

from werkzeug.routing import Map, Rule

urls = Map([
    Rule('/', endpoint=1),
    Rule('/hello', endpoint=2),
    Rule('/hello/world', endpoint=3),
    Rule('/goodbye', endpoint=4),
    Rule('/goodbye/world', endpoint=5),
]).bind('', '')

wz = timeit.timeit("urls.match('/hello/world', 'GET')", globals=locals())

from starlette.routing import Route, Match

starrouter = [
    Route('/', methods=['GET'], endpoint=1),
    Route('/hello', methods=['GET'], endpoint=2),
    Route('/hello/world', methods=['GET'], endpoint=3),
    Route('/goodbye', methods=['GET'], endpoint=4),
    Route('/goodbye/world', methods=['GET'], endpoint=5),
]


# https://github.com/encode/starlette/blob/7c0d1e6d1a499e6eeb68d447321838be3927e83b/starlette/routing.py#L729-L739
def star_route(scope, routes):
  for route in routes:
    match, _ = route.matches(scope)
    if match == Match.FULL:
      return route.endpoint
  return None


scope = {'type': 'http', 'path': '/hello/world', 'method': 'GET'}

sr = timeit.timeit("star_route(scope, starrouter)", globals=locals())

import bottle

btrtr = bottle.Router()

btrtr.add('/', 'GET', 1)
btrtr.add('/hello', 'GET', 2)
btrtr.add('/hello/world', 'GET', 3)
btrtr.add('/goodbye', 'GET', 4)
btrtr.add('/goodbye/world', 'GET', 5)

environ = {'PATH_INFO': '/hello/world', 'REQUEST_METHOD': 'GET'}

bt = timeit.timeit("btrtr.match(environ)", globals=locals())

import tornado


def tnd_handle(x):
  def internal(*_):
    return x

  return internal


tndrtr = {
    'GET':
    tornado.routing.RuleRouter([
        ('/', tnd_handle(1)),
        ('/hello', tnd_handle(2)),
        ('/hello/world', tnd_handle(3)),
        ('/goodbye', tnd_handle(4)),
        ('/goodbye/world', tnd_handle(5)),
    ])
}


def tnd_route(req):
  return tndrtr[req.method].find_handler(req).request_callback()


httpreq = tornado.httputil.HTTPServerRequest('GET', '/hello/world',
                                             connection=True)

tn = timeit.timeit("tnd_route(httpreq)", globals=locals())

lowest = min(nr, wz, sr, bt, tn)
print(f'nanoroute: {nr:.2f}, {nr / lowest:.2f}x\n'
      f'werkzeug:  {wz:.2f}, {wz / lowest:.2f}x\n'
      f'starlette: {sr:.2f}, {sr / lowest:.2f}x\n'
      f'bottle:    {bt:.2f}, {bt / lowest:.2f}x\n'
      f'tornado:   {tn:.2f}, {tn / lowest:.2f}x')
