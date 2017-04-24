# Bootstrap a NEON/MAUI/NetRunner from scratch to create a clean
# base image for other Docker test runners. For the tests to be
# relevant, they need to run on the same base OS.
#
# 1) Start with a clean Ubuntu LTS base
# 2) Fetch the base Maui packages source
# 3) Build the base system .deb
# 4) Install them
# 5) Update everything
# 6) Cleanup

FROM ubuntu:16.04
MAINTAINER Emmanuel Lepage-Vallee (elv1313@gmail.com)

RUN apt update
RUN apt upgrade

RUN apt install -yy build-essential cmake git dpkg-dev devscripts equivs \
    dh-make apt-utils

RUN mkdir /sources /packages

WORKDIR sources

# Otherwise dh_make and apt wont work
ENV DEBIAN_FRONTEND=noninteractive 
ENV USER=root

RUN git clone https://github.com/maui-linux/base-files
RUN git clone https://github.com/ring-project/maui-apt-config.git
RUN git clone https://github.com/maui-linux/maui-fontconfig

# Create symlinks with proper Debian directory name policy
RUN  for PACK in `ls`; do \
    cd $PACK;\
    VERSION=$(dpkg-parsechangelog --show-field Version);\
    cd ..;\
    tar -C $PWD/$PACK -cjf /packages/${PACK}_${VERSION}.orig.tar.bz2 .; \
    mv $PWD/$PACK /packages/$PACK-$VERSION; \
 done

# Build the .deb
RUN for PACK in `find /packages/ -type d -maxdepth 1`; do \
   cd $PACK; \
   mk-build-deps \
     -t 'apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends -qqy' \
     -i -r debian/control;\
   dpkg-buildpackage; \
 done

# Install the .deb
RUN apt install -y /packages/*.deb

# Install the maui patched packages
RUN apt update
RUN apt upgrade -y

WORKDIR /

# One last time as the last upgrade could have changed /etc/apt
RUN apt update
RUN apt upgrade -y

RUN rm -rf /packages /sources
