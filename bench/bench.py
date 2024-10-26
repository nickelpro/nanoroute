import timeit
import nanoroute

router = nanoroute.router()

router.get('/')(1)
router.get('/hello')(2)
router.get('/hello/world')(3)
router.get('/goodbye')(4)
router.get('/goodbyte/world')(5)

print(timeit.timeit("router.lookup('GET', '/hello/world')", globals=locals()))

from werkzeug.routing import Map, Rule

urls = Map([
    Rule('/', endpoint=1),
    Rule('/hello', endpoint=2),
    Rule('/hello/world', endpoint=3),
    Rule('/goodbye', endpoint=4),
    Rule('/goodbyte/world', endpoint=5),
]).bind('', '')

print(timeit.timeit("urls.match('/hello/world', 'GET')", globals=locals()))
