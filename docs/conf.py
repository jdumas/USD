# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Utilities ---------------------------------------------------------------

def GetUSDVersion():
    '''Parse Version.cmake file in USD distribution to determine
    current version for this documentation build.'''
    version_file = '../cmake/defaults/Version.cmake'
    with open(version_file, 'r') as f:
        content = f.read()

    def _ParseVersion(s):
        '''Find matches for strings like: set(PXR_MINOR_VERSION "21")
        '''
        import re
        expr = 'set\({} \"(\d+)\"\)'.format(s)
        versionString = re.search(expr, content).groups()[0]

        # USD versions are typically zero-padded to two characters,
        # like '21.08' instead of '21.8'. However, the number in
        # Version.cmake is *not* zero-padded. So, we do the padding
        # here.
        if versionString != '0':
            versionString = versionString.zfill(2)

        return versionString

    return { 'major' : _ParseVersion('PXR_MAJOR_VERSION'),
             'minor' : _ParseVersion('PXR_MINOR_VERSION'),
             'patch' : _ParseVersion('PXR_PATCH_VERSION') }

def GetDoxygenTagFile():
    '''Return path to Doxygen .tag file for USD documentation. This
    is set via the environment variable USD_DOXYGEN_TAG_FILE'''
    return os.environ.get('USD_DOXYGEN_TAG_FILE')

def GetDoxygenPrefix():
    '''Return path to directory containing Doxygen HTML files 
    relative to the directory where the Sphinx documentation will
    be installed. This is set via the environment variable
    USD_DOXYGEN_PREFIX.'''
    return os.environ.get('USD_DOXYGEN_PREFIX')

def GetSphinxLogger():
    '''Return logger object used by sphinx to report diagnostics.
    See https://www.sphinx-doc.org/en/master/extdev/logging.html'''
    from sphinx.util import logging
    return logging.getLogger(__name__)

# -- Project information -----------------------------------------------------

project = 'Universal Scene Description'
copyright = '2021, Pixar Animation Studios'
author = 'Pixar Animation Studios'
version = '{minor}.{patch}'.format_map(GetUSDVersion())
release = version

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autosectionlabel',
    'sphinxcontrib.doxylink',
    'sphinx_panels',
    'myst_parser'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The master toctree document.
master_doc = 'index'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

autosectionlabel_prefix_document = True

# -- Options for doxylink ----------------------------------------------------

doxygenTagFile = GetDoxygenTagFile()
if doxygenTagFile is None:
    GetSphinxLogger().warning(
        "Environment variable 'USD_DOXYGEN_TAG_FILE' not found. This "
        "should be set to the location of the Doxygen tag file generated by "
        "the USD build. Some Doxygen links may not appear properly.")
    doxygenTagFile = ""
else:
    GetSphinxLogger().info("Using Doxygen tag file: {}".format(doxygenTagFile))

doxygenPrefix = GetDoxygenPrefix()
if doxygenPrefix is None:
    GetSphinxLogger().warning(
        "Environment variable 'USD_DOXYGEN_PREFIX' not found. This "
        "should be set to the path of the Doxygen HTML files relative to the "
        "output directory for the Sphinx documentation. Some Doxygen links "
        "may not work properly.")
    doxygenPrefix = ""
else:
    GetSphinxLogger().info("Using Doxygen prefix: {}".format(doxygenPrefix))

doxylink = {
    'usdcpp' : (doxygenTagFile, doxygenPrefix)
}

add_function_parentheses = False

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'logo_only': False,
    'display_version': False,
    'collapse_navigation': False,
    'sticky_navigation': False,
    'navigation_depth': 4,
    'style_nav_header_background': '#606060'
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = [
    'css/pxr_custom.css'
]

html_js_files = [
    'js/versions.js',
]

html_sidebars = { '**': ['globaltoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html'] }

html_favicon = 'https://openusd.org/images/USDIcon.ico'

# Skip generating the "Built with Sphinx using a theme..." line in the footer, 
# as it creates confusing formatting with our Terms of Use footer link
html_show_sphinx = False
