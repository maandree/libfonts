PREFIX    = /usr
MANPREFIX = $(PREFIX)/share/man

CC = c99

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
CFLAGS   = -Wall -g
LDFLAGS  =

# Add -DVALIDATE to CPPFLAGS to run extra code checks,
#   that are only useful for development, at run time
