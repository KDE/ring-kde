#!/bin/bash
echo 'Make sure config.ini  and all code version refewrences are ready [PRESS ENTER]'
read
mkdir -p build
cd build
git clone git://anongit.kde.org/kde-dev-scripts
cp ../data/config.ini ./
cp kde-dev-scripts/createtarball/create_tarball.rb ./
ruby create_tarball.rb -n -a sflphone-client-kde && mv sflphone-client-kde-*.tar.* ../