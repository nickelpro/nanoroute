from typing import Any, Iterable, Callable, TypeVar

T = TypeVar('handle')


class router:
  def __init__() -> None:
    ...

  def get(route: str, /) -> Callable[[T], T]:
    ...

  def post(route: str, /) -> Callable[[T], T]:
    ...

  def put(route: str, /) -> Callable[[T], T]:
    ...

  def delete(route: str, /) -> Callable[[T], T]:
    ...

  def route(route: str | Iterable[str], /) -> Callable[[T], T]:
    ...

  def lookup(method: str, path: str, /) -> tuple[Any, dict[str, str]]:
    ...

  def wsgi_app(
      environ: dict[str, str],
      start_response: Callable[[str, dict[str, str]], Callable[[str], None]],
      /,
  ) -> bytes | Iterable[bytes]:
    ...
