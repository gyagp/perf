#############################################################################
# Makefile for building: meegotouchGUI
# Generated by qmake (2.01a) (Qt 4.7.0) on: Thu Dec 23 06:48:59 2010
# Project:  meegotouchGUI.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile meegotouchGUI.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DM_APPLICATION_NAME=\"${QMAKE_TARGET}\" -DQT_NO_DEBUG -DQT_DBUS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtDBus -I/usr/include/qt4 -I. -I/usr/include/meegotouch -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I.
LINK          = g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/usr/lib
LIBS          = $(SUBLIBS)  -L/usr/lib -L/usr/lib -lmeegotouchcore -lmeegotouchsettings -lmeegotouchextensions -lQtDBus -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = meegotouchGUI.cpp \
		mtwindow.cpp moc_mtwindow.cpp
OBJECTS       = meegotouchGUI.o \
		mtwindow.o \
		moc_mtwindow.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_defines.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_common.prf \
		/usr/share/qt4/mkspecs/features/meegotouch.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/dbusinterfaces.prf \
		/usr/share/qt4/mkspecs/features/dbusadaptors.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		meegotouchGUI.pro
QMAKE_TARGET  = meegotouchGUI
DESTDIR       = 
TARGET        = meegotouchGUI

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: meegotouchGUI.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_defines.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_common.prf \
		/usr/share/qt4/mkspecs/features/meegotouch.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/dbusinterfaces.prf \
		/usr/share/qt4/mkspecs/features/dbusadaptors.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libmeegotouchcore.prl \
		/usr/lib/libmeegotouchsettings.prl \
		/usr/lib/libmeegotouchextensions.prl \
		/usr/lib/libQtDBus.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -o Makefile meegotouchGUI.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/meegotouch_defines.prf:
/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf:
/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf:
/usr/share/qt4/mkspecs/features/meegotouch_common.prf:
/usr/share/qt4/mkspecs/features/meegotouch.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/dbusinterfaces.prf:
/usr/share/qt4/mkspecs/features/dbusadaptors.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libmeegotouchcore.prl:
/usr/lib/libmeegotouchsettings.prl:
/usr/lib/libmeegotouchextensions.prl:
/usr/lib/libQtDBus.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile meegotouchGUI.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/meegotouchGUI1.0.0 || $(MKDIR) .tmp/meegotouchGUI1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/meegotouchGUI1.0.0/ && $(COPY_FILE) --parents mtwindow.h .tmp/meegotouchGUI1.0.0/ && $(COPY_FILE) --parents meegotouchGUI.cpp mtwindow.cpp .tmp/meegotouchGUI1.0.0/ && (cd `dirname .tmp/meegotouchGUI1.0.0` && $(TAR) meegotouchGUI1.0.0.tar meegotouchGUI1.0.0 && $(COMPRESS) meegotouchGUI1.0.0.tar) && $(MOVE) `dirname .tmp/meegotouchGUI1.0.0`/meegotouchGUI1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/meegotouchGUI1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_mgenerator_model_make_all:
compiler_mgenerator_model_clean:
	-$(DEL_FILE) ./gen_*
compiler_mgenerator_style_make_all:
compiler_mgenerator_style_clean:
	-$(DEL_FILE) ./gen_*
compiler_moc_header_make_all: moc_mtwindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mtwindow.cpp
moc_mtwindow.cpp: /usr/include/meegotouch/MWindow \
		/usr/include/meegotouch/mwindow.h \
		/usr/include/meegotouch/mexport.h \
		/usr/include/meegotouch/mnamespace.h \
		mtwindow.h
	mmoc $(DEFINES) $(INCPATH) mtwindow.h -o moc_mtwindow.cpp

compiler_dbus_interface_source_make_all:
compiler_dbus_interface_source_clean:
compiler_dbus_adaptor_source_make_all:
compiler_dbus_adaptor_source_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_dbus_interface_header_make_all:
compiler_dbus_interface_header_clean:
compiler_dbus_interface_moc_make_all:
compiler_dbus_interface_moc_clean:
compiler_dbus_adaptor_header_make_all:
compiler_dbus_adaptor_header_clean:
compiler_dbus_adaptor_moc_make_all:
compiler_dbus_adaptor_moc_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

meegotouchGUI.o: meegotouchGUI.cpp /usr/include/meegotouch/MApplication \
		/usr/include/meegotouch/mapplication.h \
		/usr/include/meegotouch/mexport.h \
		/usr/include/meegotouch/mnamespace.h \
		/usr/include/meegotouch/MSceneWindow \
		/usr/include/meegotouch/mscenewindow.h \
		/usr/include/meegotouch/mwidgetcontroller.h \
		/usr/include/meegotouch/mwidget.h \
		/usr/include/meegotouch/mwindow.h \
		/usr/include/meegotouch/mtheme.h \
		/usr/include/meegotouch/mscenewindowmodel.h \
		/usr/include/meegotouch/mwidgetmodel.h \
		/usr/include/meegotouch/MWidget \
		/usr/include/meegotouch/MPannableViewport \
		/usr/include/meegotouch/mpannableviewport.h \
		/usr/include/meegotouch/mpannablewidget.h \
		/usr/include/meegotouch/mphysics2dpanning.h \
		/usr/include/meegotouch/mpannablewidgetmodel.h \
		/usr/include/meegotouch/mpannableviewportmodel.h \
		/usr/include/meegotouch/MSceneManager \
		/usr/include/meegotouch/mscenemanager.h \
		/usr/include/meegotouch/MImageWidget \
		/usr/include/meegotouch/mimagewidget.h \
		/usr/include/meegotouch/mimagewidgetmodel.h \
		mtwindow.h \
		/usr/include/meegotouch/MWindow
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o meegotouchGUI.o meegotouchGUI.cpp

mtwindow.o: mtwindow.cpp mtwindow.h \
		/usr/include/meegotouch/MWindow \
		/usr/include/meegotouch/mwindow.h \
		/usr/include/meegotouch/mexport.h \
		/usr/include/meegotouch/mnamespace.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mtwindow.o mtwindow.cpp

moc_mtwindow.o: moc_mtwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mtwindow.o moc_mtwindow.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

