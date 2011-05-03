Summary: Text mode based programming IDE for Linux
Name: motor
Version: 3.4.0
Release: 1
Copyright: GPL
Group: Development/Tools
Source: http://konst.org.ua/download/%{name}-%{version}.tar.gz
URL: http://konst.org.ua/motor/
Packager: Konstantin Klyagin <konst@konst.org.ua>
BuildRoot: /var/tmp/%{name}-buildroot/

%description
Motor is a text mode based programming environment for Linux. It
consists of a powerful editor with syntax highlight feature, project
manager, makefile generator, gcc and gdb front-end, etc. Deep CVS
integration is also provided. It can also generate distribution packages 
in any format. Almost everything is done using templates, thus any kind
of language or distribution can be added easily.  

%prep
%setup

%build
./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT/usr sysconfdir=$RPM_BUILD_ROOT/etc install

find $RPM_BUILD_ROOT/usr/ -type f -print | \
    egrep -v '(README|COPYING|INSTALL|TODO|ChangeLog|FAQ)$|tutorial\/.*|share\/motor\/templates\/.*' | \
    sed "s@^$RPM_BUILD_ROOT@@g" | sed 's/^\(.\+\/man.\+\)$/\1*/g' \
    > %{name}-%{version}-filelist

echo "/usr/share/motor/templates/*" \
    >> %{name}-%{version}-filelist

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/%{name}-%{version}

%files -f %{name}-%{version}-filelist
%defattr(-, root, root)

%doc README COPYING INSTALL TODO ChangeLog FAQ tutorial/*

%changelog
