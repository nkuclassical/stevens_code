%define name awscli
%define version 1.7.13
%define unmangled_version 1.7.13
%define release 1

Summary: Universal Command Line Environment for AWS.
Name: %{name}
Version: %{version}
Release: %{release}
Source0: %{name}-%{unmangled_version}.tar.gz
Source1: argparse-1.2.1.tar.gz
Source2: colorama-0.3.3.tar.gz
Source3: rsa-3.1.4.tar.gz
Source4: docutils-0.12.tar.gz
Source5: simplejson-3.3.0.tar.gz
Source6: virtualenv-1.10.1.tar.gz
Source7: jmespath-0.6.1.tar.gz
Source8: six-1.9.0.tar.gz
Source9: ordereddict-1.1.tar.gz
Source10: python-dateutil-2.4.0.tar.gz
Source11: bcdoc-0.13.0.tar.gz
Source12: pyasn1-0.1.7.tar.gz
Source13: botocore-0.94.0.tar.gz
License: Apache License 2.0
Group: Development/Libraries
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
Prefix: %{_prefix}
BuildArch: noarch
Vendor: Yanqiao Zhan
Url: http://aws.amazon.com/cli/

%description
=======
aws-cli
when install it, please use command "rpm -ivh --replacefiles awscli-1.7.13-1.noarch.rpm"
=======

%prep
%setup -q -b 0
%setup -q -b 1
%setup -q -b 2
%setup -q -b 3
%setup -q -b 4
%setup -q -b 5
%setup -q -b 6
%setup -q -b 7
%setup -q -b 8
%setup -q -b 9
%setup -q -b 10
%setup -q -b 11
%setup -q -b 12
%setup -q -b 13

%build
python setup.py build
cd %{_builddir}/argparse-1.2.1
python setup.py build
cd %{_builddir}/colorama-0.3.3
python setup.py build
cd %{_builddir}/python-dateutil-2.4.0
python setup.py build
cd %{_builddir}/docutils-0.12
python setup.py build
cd %{_builddir}/rsa-3.1.4
python setup.py build
cd %{_builddir}/jmespath-0.6.1
python setup.py build
cd %{_builddir}/simplejson-3.3.0
python setup.py build
cd %{_builddir}/bcdoc-0.13.0
python setup.py build
cd %{_builddir}/ordereddict-1.1
python setup.py build
cd %{_builddir}/six-1.9.0
python setup.py build
cd %{_builddir}/botocore-0.94.0
python setup.py build
cd %{_builddir}/pyasn1-0.1.7
python setup.py build
cd %{_builddir}/virtualenv-1.10.1
python setup.py build

%install
python setup.py install --single-version-externally-managed -O1 --root=$RPM_BUILD_ROOT --record=INSTALLED_FILES
cd %{_builddir}/argparse-1.2.1
python setup.py install --single-version-externally-managed -O1 --root=%{_buildrootdir}/argparse-1.2.1 --record=INSTALLED_FILES
cd %{_builddir}/colorama-0.3.3
python setup.py install --single-version-externally-managed -O1 --root=%{_buildrootdir}/colorama-0.3.3 --record=INSTALLED_FILES
cd %{_builddir}/python-dateutil-2.4.0
python setup.py install -O1 --root=%{_buildrootdir}/python-dateutil-2.4.0 --record=INSTALLED_FILES
cd %{_builddir}/docutils-0.12
python setup.py install -O1 --root=%{_buildrootdir}/docutils-0.12 --record=INSTALLED_FILES
cd %{_builddir}/rsa-3.1.4
python setup.py install  -O1 --root=%{_buildrootdir}/rsa-3.1.4 --record=INSTALLED_FILES
cd %{_builddir}/jmespath-0.6.1
python setup.py install -O1 --root=%{_buildrootdir}/jmespath-0.6.1 --record=INSTALLED_FILES
cd %{_builddir}/simplejson-3.3.0
python setup.py install -O1 --root=%{_buildrootdir}/simplejson-3.3.0 --record=INSTALLED_FILES
cd %{_builddir}/bcdoc-0.13.0
python setup.py install -O1 --root=%{_buildrootdir}/bcdoc-0.13.0 --record=INSTALLED_FILES
cd %{_builddir}/ordereddict-1.1
python setup.py install -O1 --root=%{_buildrootdir}/ordereddict-1.1 --record=INSTALLED_FILES
cd %{_builddir}/six-1.9.0
python setup.py install -O1 --root=%{_buildrootdir}/six-1.9.0 --record=INSTALLED_FILES
cd %{_builddir}/botocore-0.94.0
python setup.py install -O1 --root=%{_buildrootdir}/botocore-0.94.0 --record=INSTALLED_FILES
cd %{_builddir}/pyasn1-0.1.7
python setup.py install -O1 --root=%{_buildrootdir}/pyasn1-0.1.7 --record=INSTALLED_FILES
cd %{_builddir}/virtualenv-1.10.1
python setup.py install -O1 --root=%{_buildrootdir}/virtualenv-1.10.1 --record=INSTALLED_FILES


cp -r %{_buildrootdir}/argparse-1.2.1/usr/ %{buildroot}
cp -r %{_buildrootdir}/colorama-0.3.3/usr/ %{buildroot}
cp -r %{_buildrootdir}/python-dateutil-2.4.0/usr/ %{buildroot}
cp -r %{_buildrootdir}/docutils-0.12/usr/ %{buildroot}
cp -r %{_buildrootdir}/rsa-3.1.4/usr/ %{buildroot}
cp -r %{_buildrootdir}/jmespath-0.6.1/usr/ %{buildroot}
cp -r %{_buildrootdir}/simplejson-3.3.0/usr/ %{buildroot}
cp -r %{_buildrootdir}/bcdoc-0.13.0/usr/ %{buildroot}
cp -r %{_buildrootdir}/ordereddict-1.1/usr/ %{buildroot}
cp -r %{_buildrootdir}/six-1.9.0/usr/ %{buildroot}
cp -r %{_buildrootdir}/botocore-0.94.0/usr/ %{buildroot}
cp -r %{_buildrootdir}/pyasn1-0.1.7/usr/ %{buildroot}

%clean #clean up ~/rpmbuild/BUILD ~/rpmbuild/BUILDROOT directories
rm -rf %{buildroot}
rm -rf %{_buildrootdir}/*
rm -rf %{_builddir}/*

%files -f INSTALLED_FILES
/*
%defattr(-,root,root)

