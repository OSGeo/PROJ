%define PACKAGE_NAME proj
%define PACKAGE_VERSION 5.0.0
%define PACKAGE_URL http://proj4.org
%define _prefix /usr

Summary: Cartographic projection and geodetic transformation software
Name: %PACKAGE_NAME
Version: %PACKAGE_VERSION
Release: 1
Source0: proj-5.0.0.tar.gz
License: MIT, Copyright (c) 2000, Frank Warmerdam
Group: Applications/GIS
Vendor: Intevation GmbH <http://intevation.net>
Distribution: FreeGIS CD

BuildRoot: %{_builddir}/%{name}-root
Prefix: %{_prefix}
BuildRequires: libtool
Conflicts: PROJ.4

%description
PROJ is a generic coordinate transformation software, that transforms
coordinates from one coordinate reference system (CRS) to another. This
includes cartographic projections as well as geodetic transformations.

%prep
%setup -D -n proj-5.0.0
%{_builddir}/%{name}-%{version}/autogen.sh
%configure

%build
make

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf %{_builddir}/proj-5.0.0
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/*
%{_bindir}/*
%{_includedir}/*
%{_datadir}/proj/*
%{_mandir}/man1/*
%{_mandir}/man3/*

%doc AUTHORS COPYING ChangeLog NEWS README.md
