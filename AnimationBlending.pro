# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = AnimationBlending

QT = core gui widgets

QT+=opengl
LIBS += -lGLU


HEADERS = \
   $$PWD/AnimationCycleWidget.h \
   $$PWD/BVHData.h \
   $$PWD/Cartesian3.h \
   $$PWD/Homogeneous4.h \
   $$PWD/HomogeneousFaceSurface.h \
   $$PWD/Matrix4.h \
   $$PWD/SceneModel.h \
   $$PWD/Terrain.h

SOURCES = \
   $$PWD/AnimationCycleWidget.cpp \
   $$PWD/BVHData.cpp \
   $$PWD/Cartesian3.cpp \
   $$PWD/Homogeneous4.cpp \
   $$PWD/HomogeneousFaceSurface.cpp \
   $$PWD/main.cpp \
   $$PWD/Matrix4.cpp \
   $$PWD/SceneModel.cpp \
   $$PWD/Terrain.cpp

INCLUDEPATH = \
    $$PWD/.

#DEFINES = 

