# We use Ubuntu image as OpenCascade (at least 7.4.0 won't compile on alpine).
FROM ubuntu:latest

# To avoid all sort of prompts from apt-get.
ENV DEBIAN_FRONTEND=noninteractive

# ======================
#  Prepare dependencies
# ======================
 
RUN apt-get update

# The build-essentials packages are meta-packages that are necessary for compiling software.
# They include the GNU debugger, g++/GNU compiler collection, and some more tools and libraries
# that are required to compile a program. For example, if you need to work on a C/C++ compiler,
# you need to install essential meta-packages on your system before starting the C compiler installation.
# When installing the build-essential packages, some other packages such as G++, dpkg-dev, GCC and make, etc.
# also install on your system.
RUN apt-get -y install build-essential

# Git to clone OpenCascade from its github repo.
RUN apt-get -y install git

# OpenCascade is built with cmake.
RUN apt-get -y install cmake

# Deps for DRAW.
RUN apt-get -y install tcl tcl-dev
RUN apt-get -y install tk tk-dev
RUN apt-get -y install libfontconfig1-dev
RUN apt-get -y install libxmu-dev libxi-dev

# /opencascade/src/InterfaceGraphic/InterfaceGraphic.hxx:41:10: fatal error: GL/glx.h: No such file or directory
RUN apt-get -y install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev

# ===========================================================
#  Clone OpenCascade from its public repo, build and install
# ===========================================================
 
# OpenCascade
RUN git clone https://github.com/Open-Cascade-SAS/OCCT.git opencascade
WORKDIR /opencascade
RUN git checkout V7_4_0 -b smru
RUN mkdir -p build
WORKDIR /opencascade/build
RUN cmake .. \
       -DCMAKE_BUILD_TYPE=release \
       -DCMAKE_INSTALL_RPATH="" \
       -DCMAKE_INSTALL_PREFIX=/usr \
       -DUSE_FREEIMAGE=OFF \
       -DUSE_FFMPEG=OFF \
       -DUSE_VTK=OFF \
       -DUSE_TBB=OFF
RUN make
RUN make install

# ==========
#  Finalize
# ==========

 
ENTRYPOINT [ "/usr/bin/DRAWEXE" ]
