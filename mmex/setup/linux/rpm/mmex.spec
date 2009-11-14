Name:           mmex
Version:        0.9.6.0
Release:        1%{?dist}
Summary:        Cross-platform simple to use financial management software
Group:          Applications/Databases
License:        GPL2
URL:            http://www.codelathe.com/mmex
Packager:       VaDiM
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       libc6, libwxgtk2.8-0

%description
Money Manager Ex is a free, open-source, cross-platform, easy-to-use personal 
finance software. It primarily helps organize one's finances and keeps track 
of where, when and how the money goes.

MMEX includes all the basic features that 90% of users would want to see in a
personal finance application. The design goals are to concentrate on simplicity
and user-friendliness - something one can use everyday.


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
%defattr(-,root,root,-)
/mmex
/contrib.txt
/currency.db3
/kaching.wav
/license.txt
/mmex.ico
/splash.png
/version.txt
/en
/help


%changelog
