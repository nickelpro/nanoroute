import timeit
import nanoroute

router = nanoroute.router()

router.get('/')(None)
router.get('/hello')(None)
router.get('/hello/world')(None)
router.get('/goodbye')(None)
router.get('/goodbyte/world')(None)

router.get("hello")

print(timeit.timeit("router.lookup('GET', '/hello/world')", globals=locals()))
