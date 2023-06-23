"""Sphinx doc extension for simple word replacements.

Within conf.py, add a 'replacement_pairs' dict with key/replacement pairs.

With inspiration from
https://github.com/sphinx-doc/sphinx/issues/4054#issuecomment-329097229
"""


def replace_words(app, docname, source):
    result = source[0]
    for key, value in app.config.replacement_pairs.items():
        result = result.replace(key, value)
    source[0] = result


def setup(app):
    app.add_config_value("replacement_pairs", {}, True)
    app.connect("source-read", replace_words)
