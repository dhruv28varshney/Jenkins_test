========================================
Working with reStructuredText  documents
========================================

reStructuredText is the default plaintext markup language used by Sphinx.  This
section is a brief introduction to reStructuredText (reST) concepts and syntax,
intended to provide authors with enough information to author documents
productively.  

Table of content:
-----------------
- `Paragraphs`_
- `Sections`_
- `Inline markup`_
- `Lists and Quote-like blocks`_
- `Literal blocks`_
- `Tables`_
- `Hyperlinks`_
- `Images`_
- `Comments`_
- `References`_


Paragraphs
----------

The paragraph is the most basic block in a reST
document.  Paragraphs are simply chunks of text separated by one or more blank
lines.  As in Python, indentation is significant in reST, so all lines of the
same paragraph must be left-aligned to the same level of indentation.

.. _rst-sections:

Sections
--------

Section headers are created by underlining (and
optionally overlining) the section title with a punctuation character, at least
as long as the text::

   =================
   This is a heading
   =================

Normally, there are no heading levels assigned to certain characters as the
structure is determined from the succession of headings.  However, this
convention is used in `Python's Style Guide for documenting
<https://docs.python.org/devguide/documenting.html#style-guide>`_ which you may
follow:

* ``#`` with overline, for parts
* ``*`` with overline, for chapters
* ``=`` for sections
* ``-`` for subsections
* ``^`` for subsubsections
* ``"`` for paragraphs

Of course, you are free to use your own marker characters (see the reST
documentation).


.. _rst-inline-markup:

Inline markup
-------------

The standard reST inline markup is quite simple: use

* one asterisk: ``*text*`` for emphasis (italics),
* two asterisks: ``**text**`` for strong emphasis (boldface), and
* backquotes: ````text```` for code samples.

If asterisks or backquotes appear in running text and could be confused with
inline markup delimiters, they have to be escaped with a backslash.

Be aware of some restrictions of this markup:

* it may not be nested,
* content may not start or end with whitespace: ``* text*`` is wrong,
* it must be separated from surrounding text by non-word characters.  Use a
  backslash escaped space to work around that: ``thisis\ *one*\ word``.


Lists and Quote-like blocks
---------------------------

List markup is natural: just place an asterisk at
the start of a paragraph and indent properly.  The same goes for numbered
lists; they can also be autonumbered using a ``#`` sign::

   * This is a bulleted list.
   * It has two items, the second
     item uses two lines.

   1. This is a numbered list.
   2. It has two items too.

   #. This is a numbered list.
   #. It has two items too.

Nested lists are possible, but be aware that they must be separated from the
parent list items by blank lines::

   * this is
   * a list

     * with a nested list
     * and some subitems

   * and here the parent list continues

Definition lists are created as follows::

   term (up to a line of text)
      Definition of the term, which must be indented

      and can even consist of multiple paragraphs

   next term
      Description.

Note that the term cannot have more than one line of text.

Quoted paragraphs are created by just indenting
them more than the surrounding paragraphs.

Line blocks are a way of preserving line breaks::

   | These lines are
   | broken exactly like in
   | the source file.




.. _rst-literal-blocks:

Literal blocks
--------------

Literal code blocks are introduced by ending a
paragraph with the special marker ``::``.  The literal block must be indented
(and, like all paragraphs, separated from the surrounding ones by blank
lines)::

   This is a normal text paragraph. The next paragraph is a code sample::

      It is not processed in any way, except
      that the indentation is removed.

      It can span multiple lines.

   This is a normal text paragraph again.

The handling of the ``::`` marker is smart:

* If it occurs as a paragraph of its own, that paragraph is completely left out
  of the document.
* If it is preceded by whitespace, the marker is removed.
* If it is preceded by non-whitespace, the marker is replaced by a single
  colon.

That way, the second sentence in the above example's first paragraph would be
rendered as "The next paragraph is a code sample:".


.. _rst-tables:

Tables
------

For *grid tables*, you have to "paint" the cell
grid yourself.  They look like this::

   +------------------------+------------+----------+----------+
   | Header row, column 1   | Header 2   | Header 3 | Header 4 |
   | (header rows optional) |            |          |          |
   +========================+============+==========+==========+
   | body row 1, column 1   | column 2   | column 3 | column 4 |
   +------------------------+------------+----------+----------+
   | body row 2             | ...        | ...      |          |
   +------------------------+------------+----------+----------+

*Simple tables* are easier to write, but
limited: they must contain more than one row, and the first column cells cannot
contain multiple lines.  They look like this::

   =====  =====  =======
   A      B      A and B
   =====  =====  =======
   False  False  False
   True   False  False
   False  True   False
   True   True   True
   =====  =====  =======

Two more syntaxes are supported: *CSV tables* and *List tables*. They use an
*explicit markup block*. Refer to documentation


Hyperlinks
----------

External links
~~~~~~~~~~~~~~

Use ```Link text <https://domain.invalid/>`_`` for inline web links.  If the
link text should be the web address, you don't need special markup at all, the
parser finds links and mail addresses in ordinary text.

.. important:: There must be a space between the link text and the opening \< for the URL.

You can also separate the link and the target definition, like this::

   This is a paragraph that contains `a link`_.

   .. _a link: https://domain.invalid/

Internal links
~~~~~~~~~~~~~~

Internal linking is done like below

   This is link to section `Tables`_.



Images
------

reST supports an image directive, used like so::

   .. image:: gnu.png
            (options)

When used within Sphinx, the file name given (here ``gnu.png``) must either be
relative to the source file, or absolute which means that they are relative to
the top source directory.  For example, the file ``sketch/spam.rst`` could
refer to the image ``images/spam.png`` as ``../images/spam.png`` or
``/images/spam.png``.

Sphinx will automatically copy image files over to a subdirectory of the output
directory on building (e.g. the ``_static`` directory for HTML output.)

Interpretation of image size options (``width`` and ``height``) is as follows:
if the size has no unit or the unit is pixels, the given size will only be
respected for output channels that support pixels. Other units (like ``pt`` for
points) will be used for HTML and LaTeX output (the latter replaces ``pt`` by
``bp`` as this is the TeX unit such that ``72bp=1in``).

Sphinx extends the standard docutils behavior by allowing an asterisk for the
extension::

   .. image:: gnu.*

Sphinx then searches for all images matching the provided pattern and
determines their type.  Each builder then chooses the best image out of these
candidates.  For instance, if the file name ``gnu.*`` was given and two files
`gnu.pdf` and `gnu.png` existed in the source tree, the LaTeX
builder would choose the former, while the HTML builder would prefer the
latter.  


Comments
--------

Every explicit markup block which isn't a valid markup construct (like the
footnotes above) is regarded as a comment.  For
example::

   .. This is a comment.

You can indent text after a comment start to form multiline comments::

   ..
           This whole indented block
      is a comment.

      Still in the comment.




References
----------
   .. [1] https://docs.generic-mapping-tools.org/6.2/rst-cheatsheet.html.

   .. [2] https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html
