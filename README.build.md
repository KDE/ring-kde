# Building Debian / Kubuntu / Neon packages using the Docker images

This branch provides both a classic `debian` directory and self
contained Docker desccription to generate .deb for both the
daemon and Ring-KDE (with Ring-LRC built-in).

sudo docker build ./ -t elv13/ringkde
mkdir -p debs
sudo docker run -it -v $PWD/debs:/exportdebs/ elv13/ringkde

cd dring
sudo docker build ./ -t elv13/dring
mkdir debs
sudo docker run -it -v $PWD/debs:/exportdebs/ elv13/dring

Edit the debian/changelog and change zesty to any other
valid Debian target if required.
