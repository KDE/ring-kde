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
cp kde-dev-scripts/createtarball/create_tarball_kf5.rb ./
ruby create_tarball_kf5.rb -n -a ring-kde && mv ring-kde-*.tar.* ../
