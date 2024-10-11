[![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/nickelpro/nanoroute/test.yaml?style=for-the-badge)](https://github.com/nickelpro/nanoroute/actions/workflows/test.yaml)

# nanoroute

A small Python HTTP URL routing facility, capable of sub-microsecond routing.
This is mostly pedagogical, demonstrating what is _possible_ to accomplish, but
a little too spartan for most projects.

That said, when combined with a very fast application server, nanoroute is
capable of rivaling the best routers and dispatchers in performance-oriented
languages; just don't expect it to do session management or anything like that.

Nanoroute is ~4400x faster than Flask / Werkzeug.
