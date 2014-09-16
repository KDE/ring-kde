#!/bin/bash
echo 'Make sure config.ini  and all code version refewrences are ready [PRESS ENTER]'
read
mkdir -p build
cd build
if [ -d "kde-dev-scripts" ]; then
  cd kde-dev-scripts
  git pull
  cd ..
else
  git clone http://anongit.kde.org/kde-dev-scripts
fi
cp ../data/config.ini ./
cp kde-dev-scripts/createtarball/create_tarball.rb ./
ruby create_tarball.rb -n -a sflphone-kde && mv sflphone-kde-*.tar.* ../
