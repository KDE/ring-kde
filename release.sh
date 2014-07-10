#!/bin/bash
echo 'Make sure config.ini  and all code version refewrences are ready [PRESS ENTER]'
read
mkdir -p build
cd build
git clone http://anongit.kde.org/kde-dev-scripts
cp ../data/config.ini ./
cp kde-dev-scripts/createtarball/create_tarball.rb ./
ruby create_tarball.rb -n -a sflphone-kde && mv sflphone-kde-*.tar.* ../
