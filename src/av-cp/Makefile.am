if UNINSTALLED
shareddir = $(abs_top_builddir)/data
else
shareddir = $(datadir)/gupnp-tools
endif

AM_CFLAGS = $(LIBGUPNP_CFLAGS) $(LIBGLADE_CFLAGS) -I$(top_srcdir) \
	    -DDATA_DIR='"$(shareddir)"' -I../common

bin_PROGRAMS = gupnp-av-cp

gupnp_av_cp_SOURCES = av-cp.c \
		      av-cp-gui.c \
		      av-cp-playlisttreeview.c \
		      av-cp-renderercombo.c \
		      media-renderer-proxy.c \
		      media-server-proxy.c \
		      didl-lite-parser.c \
		      didl-lite-object.c \
		      xml-util.c \
		      av-cp.h \
		      av-cp-gui.h \
		      av-cp-playlisttreeview.h \
		      av-cp-renderercombo.h \
		      media-renderer-proxy.h \
		      media-server-proxy.h \
		      didl-lite-parser.h \
		      didl-lite-object.h \
		      xml-util.h

gupnp_av_cp_LDADD = $(LIBGLADE_LIBS) $(LIBGUPNP_LIBS) \
		    ../common/libicons.a
gupnp_av_cp_LDFLAGS = -export-dynamic

MAINTAINERCLEANFILES = Makefile.in