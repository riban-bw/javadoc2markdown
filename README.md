# javadoc2markdown
Convert javadoc code documentation to markdown wiki page

This small command line application converts javadoc entries in a source file or header
in to wiki markdown. It takes the source file (or header file) as the only command line parameter.
The resulting markdown is issued from standard output.

The following javadoc tags are recognised:

brief

param

return

note

todo

Not currently mapped to markdown:

author

version

see

Different markdown variants may be output. This currently requires changing the value nStyle in the source code.
Supported markdown:

github

tiddlywiki

