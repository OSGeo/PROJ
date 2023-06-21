"""Sphinx doc extension for HTML redirects.

Within conf.py, add a 'redirect_files' dict with alias/target pairs.

Originally from https://tech.signavio.com/2017/managing-sphinx-redirects (dead)
"""
from pathlib import Path

template = """\
<html>
  <head>
    <meta http-equiv="refresh" content="1; url={target}" />
    <script>
      window.location.href = "{target}"
    </script>
  </head>
</html>
"""


def copy_legacy_redirects(app, docname):  # Sphinx expects two arguments
    """Copy legacy redirects."""
    if app.builder.name == "html":
        for alias, target in app.config.redirect_files.items():
            pth = Path(app.outdir) / alias
            pth.write_text(template.format(target=target))


def setup(app):
    app.add_config_value("redirect_files", {}, "html")
    app.connect("build-finished", copy_legacy_redirects)
    return {"parallel_read_safe": False, "parallel_write_safe": True}
