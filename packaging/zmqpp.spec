Name:           zmqpp
Version:        PKG_VERSION
Release:        PKG_RELEASE%{?dist}
Summary:        0mq 'highlevel' C++ bindings
Group:          System Environment/Libraries
License:        LGPLv3
URL:            https://github.com/zeromq/zmqpp
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:  gcc-c++
BuildRequires:  zeromq3-devel
BuildRequires:  boost-devel

%description
This C++ binding for 0mq/zmq is a 'high-level' library that hides most of
the c-style interface core 0mq provides. It consists of a number of header
and source files all residing in the zmq directory, these files are provided
under the MIT license (see LICENCE for details).

%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%prep
%setup -q

%build
make %{?_smp_mflags}

%check
make check

%install

rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}%{_libdir}
mkdir -p ${RPM_BUILD_ROOT}%{_includedir}/zmqpp
make install \
    PREFIX=/usr \
    DESTDIR=${RPM_BUILD_ROOT} \
    LIBDIR=${RPM_BUILD_ROOT}%{_libdir} \
    INCLUDEDIR=${RPM_BUILD_ROOT}%{_includedir}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libzmqpp.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/zmqpp/*
