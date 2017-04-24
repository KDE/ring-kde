FROM elv13/maui
MAINTAINER Emmanuel Lepage-Vallee (elv1313@gmail.com)
RUN apt update

RUN apt install build-essential cmake git dpkg-dev curl \
   unbound-anchor chrpath check astyle devscripts equivs -yy

RUN git clone https://github.com/savoirfairelinux/ring-daemon.git

#RUN apt build-dep ring-daemon -yy

RUN git config --global user.email "you@example.com"
RUN git config --global user.name "Your Name"

WORKDIR ring-daemon

RUN git merge origin/packaging --no-commit

# Cheating
RUN apt build-dep ring-daemon -yy

# Install all dependencies
RUN mk-build-deps \
     -t 'apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends -qqy' \
     -i -r debian/control

RUN apt install unbound-anchor chrpath check astyle libva-dev nettle-dev -yy

# The API is too old
RUN apt remove dhtnode libopendht-dev -yy

CMD git fetch origin && git reset --hard origin/master && \
 git merge origin/packaging --no-commit &&\
 tar -cj . -f ../../ring-daemon_2.3.0.orig.tar.bz2 && \
 dpkg-buildpackage && \
 cp /*.deb /exportdebs/ -v && ls /exportdebs/
