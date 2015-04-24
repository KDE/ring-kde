%define name        ring-kde
%define version     2.0.1
%define release     1
%define daemon_tag  2.1.0
%define lrc_tag     0.2.1
%define kde_tag     %{version}

Name:               %{name}
Version:            %{version}
Release:            %{release}
Summary:            Ring KDE client
Group:              Applications/Internet
License:            GPLv3
URL:                http://ring.cx/
Source:             ring-kde
BuildRequires:      cmake
BuildRequires:      git
BuildRequires:      qt5-qtbase-devel
BuildRequires:      extra-cmake-modules
BuildRequires:      qt5-qtsvg-devel
BuildRequires:      kf5-attica-devel
BuildRequires:      kf5-kconfig-devel
BuildRequires:      kf5-kguiaddons-devel
BuildRequires:      kf5-kdbusaddons-devel
BuildRequires:      kf5-ki18n-devel
BuildRequires:      kf5-kwindowsystem-devel
BuildRequires:      kf5-kxmlgui-devel
BuildRequires:      kf5-kiconthemes-devel
BuildRequires:      kf5-kinit-devel
BuildRequires:      kf5-kio-devel
BuildRequires:      kf5-kcompletion-devel
BuildRequires:      kf5-kcrash-devel
BuildRequires:      kf5-knotifications-devel
BuildRequires:      mesa-libGLU-devel
BuildRequires:      autoconf automake libtool dbus-devel pcre-devel yaml-cpp-devel gcc-c++
BuildRequires:      boost-devel dbus-c++-devel dbus-devel libupnp-devel qt5-qtbase-devel
BuildRequires:      gnome-icon-theme-symbolic chrpath check astyle gnutls-devel yasm git


%description
Ring KDE client
Ring is a secured and distributed communication software.


%prep
%setup -q
# Gnome
echo "# Get gnome client"
git init
git remote add origin http://anongit.kde.org/ring-kde.git
git fetch --all
git checkout packaging -f
git config user.name "joulupukki"
git config user.email "joulupukki@localhost"
git merge %{kde_tag} --no-edit
rm -rf .git
# Daemon
echo "# Downloading Ring Daemon ..."
rm -rf ring
git clone https://gerrit-ring.savoirfairelinux.com/ring-daemon daemon
cd daemon 
git checkout %{daemon_tag}
rm -rf .git
cd ..
# LibRingClient
echo "# Downloading Lib Ring Client ..."
rm -rf libringclient
git clone git://anongit.kde.org/libringclient.git libringclient
cd libringclient
git checkout %{lrc_tag}
rm -rf .git
cd ..


%build
rm -rf %{buildroot}
# fix fedora packaging problem
ln -s /usr/include/KF5/KConfigGui/kconfigskeleton.h /usr/include/
ln -s /usr/include/KF5/KConfigGui/kconfiggui_export.h /usr/include/
cd libringclient
mkdir -p build
mkdir -p install
cd build
cmake -DRING_BUILD_DIR=$(pwd)/../../daemon/src -DCMAKE_INSTALL_PREFIX=$(pwd)/../../libringclient/install -DENABLE_VIDEO=true -DENABLE_STATIC=true ..
make -j 2
make install
cd ../..
mkdir -p build
mkdir -p install
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/../libringclient/install -DLIB_RING_CLIENT_LIBRARY=$(pwd)/../libringclient/install/lib/libringclient_static.a -DENABLE_STATIC=true ..
# TODO test this
#cmake -DCMAKE_INSTALL_PREFIX=%{buildroot} -DLIB_RING_CLIENT_LIBRARY=$(pwd)/../libringclient/install/lib/libringclient_static.a -DENABLE_STATIC=true ..
LDFLAGS="-lpthread" make -j 2

%install
cd build
make install
# TODO clean this by a better cmake command
mkdir -p %{buildroot}/%{_bindir}
mv ../libringclient/install/bin/ring-kde %{buildroot}/%{_bindir} 
mkdir -p %{buildroot}/%{_datadir}
mv ../libringclient/install/share/*  %{buildroot}/%{_datadir}
#sed -i "s#Icon=.*#Icon=%{_datadir}/icons/hicolor/scalable/apps/ring.svg#g" %{buildroot}/%{_datadir}/applications/gnome-ring.desktop


%files 
%defattr(-,root,root,-)
%{_bindir}/ring-kde
%{_datadir}
#%{_datadir}/icons/hicolor/scalable/apps/ring.svg
#%{_datadir}/appdata/gnome-ring.appdata.xml


%changelog
* Fri Apr 24 2015 Thibault Cohen <thibault.cohen@savoirfairelinux.com> - 2.0.1-1
- New upstream version

* Fri Mar 27 2015 Thibault Cohen <thibault.cohen@savoirfairelinux.com> - 0.1.0-1
- New upstream version
