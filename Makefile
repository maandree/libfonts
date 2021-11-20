.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)

OS = linux
# Linux:   linux
# Mac OS:  macos
# Windows: windows
include mk/$(OS).mk


LIB_MAJOR = 1
LIB_MINOR = 0
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)
LIB_NAME = fonts


OBJ =\
	libfonts_calculate_subpixel_order.o\
	libfonts_decode_font_description.o\
	libfonts_encode_font_description.o\
	libfonts_get_output_dpi.o

HDR =\
	libfonts.h

LOBJ = $(OBJ:.o=.lo)


all: libfonts.a libfonts.$(LIBEXT)
$(OBJ): $(HDR)
$(LOBJ): $(HDR)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.lo:
	$(CC) -fPIC -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

libfonts.a: $(OBJ)
	@rm -f -- $@
	$(AR) rc $@ $(OBJ)

libfonts.$(LIBEXT): $(LOBJ)
	$(CC) $(LIBFLAGS) -o $@ $(LOBJ) $(LDFLAGS)

install: libfonts.a libfonts.$(LIBEXT)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	cp -- libfonts.a "$(DESTDIR)$(PREFIX)/lib/"
	cp -- libfonts.$(LIBEXT) "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMINOREXT)"
	ln -sf -- libfonts.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMAJOREXT)"
	ln -sf -- libfonts.$(LIBMAJOREXT) "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBEXT)"
	cp -- libfonts.h "$(DESTDIR)$(PREFIX)/include/"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libfonts.a"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMAJOREXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMINOREXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBEXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/include/libfonts.h"

clean:
	-rm -f -- *.o *.a *.lo *.su *.so *.so.* *.dll *.dylib
	-rm -f -- *.gch *.gcov *.gcno *.gcda *.$(LIBEXT)

.SUFFIXES:
.SUFFIXES: .lo .o .c

.PHONY: all install uninstall clean
