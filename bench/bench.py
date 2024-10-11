import timeit
import nanoroute

router = nanoroute.router()

router.get('/')(None)
router.get('/hello')(None)
router.get('/hello/world')(None)
router.get('/goodbye')(None)
router.get('/goodbyte/world')(None)
router.get('/{a}/alpha')(None)
router.get('/{b}/beta')(None)

print(locals())

print(timeit.timeit("router.lookup('GET', '/hello/world')", globals=locals()))
