**This plugin is discontinued. x64dbg already supports logging ANSI and Unicode strings directly. 2017-2026.**

# EasyDump

Currently in development. The following commands are supported:

## logA addr, [size]

Log an ANSI string pointed to by "addr". The size of the buffer in bytes may be specified by "size" argument. This command supports local code page.

## logW addr, [size]

Log an Unicode string pointed to by "addr". The size of the buffer in bytes may be specified by "size" argument.
