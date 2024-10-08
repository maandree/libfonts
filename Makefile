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


PUBLIC_OBJ =\
	libfonts_calculate_subpixel_order.o\
	libfonts_char_is_in_subset.o\
	libfonts_create_transcoding_table.o\
	libfonts_deallocate_encoding.o\
	libfonts_deallocate_encoding_mapping.o\
	libfonts_deallocate_encoding_mapping_entry.o\
	libfonts_deallocate_encoding_mapping_entry_index_to_name.o\
	libfonts_decode_font_description.o\
	libfonts_do_decoded_font_descriptions_match.o\
	libfonts_do_font_descriptions_match.o\
	libfonts_encode_font_description.o\
	libfonts_get_default_font.o\
	libfonts_get_default_font_name.o\
	libfonts_get_default_rendering_settings.o\
	libfonts_get_font_root_dirs.o\
	libfonts_get_output_dpi.o\
	libfonts_get_output_rendering_settings.o\
	libfonts_get_subpixel_expansion.o\
	libfonts_get_subpixel_map.o\
	libfonts_get_subpixel_order_class.o\
	libfonts_parse_alias_line.o\
	libfonts_parse_dir_line.o\
	libfonts_parse_encoding_line.o\
	libfonts_unget_subpixel_order_class.o\
	libfonts_used_environs.o

OBJ =\
	$(PUBLIC_OBJ)\
	libfonts_confsplit__.o\
	libfonts_getenv__.o\
	libfonts_gethome__.o\
	libfonts_getline__.o\
	libfonts_parse_aa__.o\
	libfonts_parse_double__.o\
	libfonts_parse_order__.o\
	libfonts_parse_uint32__.o

HDR =\
	common.h\
	libfonts.h

LOBJ = $(OBJ:.o=.lo)
TESTS = $(OBJ:.o=.test)


all: libfonts.a libfonts.$(LIBEXT) $(TESTS)
$(OBJ): $(HDR)
$(LOBJ): $(HDR)
$(TESTS): $(HDR) libfonts.a

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.lo:
	$(CC) -fPIC -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.test:
	$(CC) -o $@ $< libfonts.a $(CFLAGS) $(CPPFLAGS) -DTEST $(LDFLAGS)

libfonts.a: $(OBJ)
	@rm -f -- $@
	$(AR) rc $@ $(OBJ)
	$(AR) ts $@ > /dev/null

libfonts.$(LIBEXT): $(LOBJ)
	$(CC) $(LIBFLAGS) -o $@ $(LOBJ) $(LDFLAGS)

check: $(TESTS)
	@for t in $(TESTS); do printf './%s\n' $$t; ./$$t || exit 1; done

install: libfonts.a libfonts.$(LIBEXT)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	cp -- libfonts.a "$(DESTDIR)$(PREFIX)/lib/"
	cp -- libfonts.$(LIBEXT) "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMINOREXT)"
	$(FIX_INSTALL_NAME) "$(DESTDIR)$(PREFIX)/lib/libfonts.$(LIBMINOREXT)"
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
	-rm -f -- *.o *.a *.lo *.su *.so *.so.* *.dll *.dylib *.test
	-rm -f -- *.gch *.gcov *.gcno *.gcda *.$(LIBEXT)

.SUFFIXES:
.SUFFIXES: .lo .o .c .test

.PHONY: all check install uninstall clean
