AM_CPPFLAGS=@THREADFLAGS@ $(BOOST_CPPFLAGS)

AM_LDFLAGS = $(BOOST_LDFLAGS)

EXTRA_DIST=OBJECTFILES OBJECTLIBS

lib_LTLIBRARIES = libglbbackend.la

libglbbackend_la_SOURCES=glbbackend.cc glbbackend.hh GlbContainer.cc GlbContainer.hh IPRecord.cc IPRecord.hh GLBCollection.cc GLBCollection.hh SOAContainer.hh SOAContainer.cc ServerSingleton.cc ServerSingleton.hh ServerJsonBuilder.cc ServerJsonBuilder.hh GLBCommandServer.cc GLBCommandServer.hh StartUpClient.cc StartUpClient.hh
libglbbackend_la_LIBADD = -lboost_thread-mt -lboost_system-mt -lpthread
libglbbackend_la_LDFLAGS=-module -avoid-version
