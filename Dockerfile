FROM ubuntu:17.04
MAINTAINER Emmanuel Lepage-Vallee (elv1313@gmail.com)
RUN apt update

RUN apt install -yy build-essential cmake git dpkg-dev devscripts  equivs

RUN git clone http://anongit.kde.org/ring-kde

WORKDIR ring-kde

ADD debian /ring-kde/debian

# Install all dependencies
RUN mk-build-deps \
    -t 'apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends -qqy' \
    -i -r debian/control

CMD git pull --rebase && \
 tar -cj . -f ../../../ring-kde_16.12.0.orig.tar.bz2 && \
 dpkg-buildpackage && \
 cp /*.deb /exportdebs/
