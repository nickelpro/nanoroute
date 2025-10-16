project = 'nanoroute'
author = 'Vito Gamberini'
release = '2.0.4'

html_show_copyright = False

html_theme = 'alabaster'
html_theme_options = {
    'description': 'A fast HTTP URL router',
    'github_user': 'nickelpro',
    'github_repo': 'nanoroute',
    'github_type': 'star',
    'fixed_sidebar': True,
    'github_banner': True,
}
html_logo = 'logo.svg'

ogp_site_url = 'https://nanoroute.dev'
ogp_image = 'logo.webp'

autodoc_typehints = 'description'

extensions = [
    'sphinxext.opengraph',
    'sphinx.ext.autodoc'
]

import importlib.machinery
import pathlib
import sys

sys.modules[project] = type(sys)(project)
importlib.machinery.SourceFileLoader(
    project,
    f'{(pathlib.Path(__file__).parent.parent / 'src' /
         'nanoroute.pyi').resolve()}').exec_module(sys.modules[project])
