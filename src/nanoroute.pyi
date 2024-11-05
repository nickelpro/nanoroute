from typing import Any, Callable, Iterable, TypeVar

T = TypeVar('handle')


class router:
  """Core class which represents a collection of HTTP verbs, routes, and
  associated Python object handles.
  """
  def __init__() -> None:
    ...

  def route(self, methods: str | Iterable[str], route: str,
            /) -> Callable[[T], T]:
    """Base routing decorator

    :param methods: Either a string or iterable of strings representing the
        HTTP verbs to associate the route with
    :param route: The route definition

    :return: A registration decorator method

    Usage::

      import nanoroute
      app = nanoroute.router()

      @app.route('GET', '/about')
      def handle_about():
        ...
    """
    ...

  def get(self, route: str, /) -> Callable[[T], T]:
    """
    Convenience routing decorator, equivalent to ``route('GET', ...)``

    :param route: The route definition

    :return: A registration decorator method
    """
    ...

  def post(self, route: str, /) -> Callable[[T], T]:
    """
    Convenience routing decorator, equivalent to ``route('POST', ...)``

    :param route: The route definition

    :return: A registration decorator method
    """
    ...

  def put(self, route: str, /) -> Callable[[T], T]:
    """
    Convenience routing decorator, equivalent to ``route('PUT', ...)``

    :param route: The route definition

    :return: A registration decorator method
    """
    ...

  def delete(self, route: str, /) -> Callable[[T], T]:
    """
    Convenience routing decorator, equivalent to ``route('DELETE', ...)``

    :param route: The route definition

    :return: A registration decorator method
    """
    ...

  def lookup(self, method: str, path: str, /) -> tuple[Any, dict[str, str]]:
    """
    Base lookup function, finds a handle object given a method and a request
    path

    :param method: The HTTP verb from the associated request
    :param path: The URL path from the associated request

    :raises LookupError: If no matching handler is found

    :return: The associated handle object and a ``{key: value}`` dictionary of
        captured parameters

    Usage::

      handler, params = app.lookup('GET', '/about')
    """
    ...

  def wsgi_app(
      self,
      environ: dict[str, str],
      start_response: Callable[[str, dict[str, str]], Callable[[str], None]],
      /,
  ) -> bytes | Iterable[bytes]:
    """
    Convenience lookup function for compatibility with PEP 3333 WSGI servers,
    stores the captured parameter dict in the ``nanoroute.params`` environ key.

    :param environ: WSGI ``environ`` dict
    :param start_response: WSGI ``start_response()`` method

    :return: WSGI-compatible iterable

    Equivalent to the following::

      def wsgi_app(environ, start_response):
        handler, params = app.lookup(
            environ['REQUEST_METHOD'],
            environ['PATH_INFO']
        )
        environ['nanoroute.params'] = params
        return handler(environ, start_response)

      app.wsgi_app = wsgi_app
    """
    ...
