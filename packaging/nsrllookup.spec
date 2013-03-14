Name:		nsrllookup
Version:	1.2.2
Release:	1%{?dist}
Summary:	provides an interface to NSRL RDS servers

License:	ISC
URL:		http://rjhansen.github.com/nsrllookup
Source0:	nsrllookup-1.2.2.tar.gz

BuildRequires:	gcc-c++
Requires:	libstdc++

%description
nsrllookup provides a simple command-line interface allowing users to easily query NIST's 
National Software Reference Library Reference Data Set (NSRL RDS).

%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/*
%{_datadir}/*

%doc



%changelog
* Wed Mar 13 2013 Robert J. Hansen <rjh@sixdemonbag.org> 1.2.2
- Introduced a new flag, -S, to query the nsrlserver's status.

* Mon Jan 30 2012 Robert J. Hansen <rjh@secret-alchemy.com> 1.11
- First Fedora RPM of nsrllookup
