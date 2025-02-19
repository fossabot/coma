| OS | Status | Download |
|----------|--------|----------|
| Windows x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_WindowsBuild_Develop_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_WindowsBuild_Develop_BuildX64) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Windows x86 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_WindowsBuild_Develop_BuildX86/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_WindowsBuild_Develop_BuildX86) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Linux (Debian 10) x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_LinuxBuild_Develop_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_LinuxBuild_Develop_BuildX64) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Linux (Debian 10) armv7 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_LinuxBuild_Develop_BuildArmv7a/statusIcon.svg)](http://172.16.31.73:27015/admin/editBuildRunners.html?id=buildType:Coma_LinuxBuild_AliseZero_BuildArmv7a) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |

# Requires:  

- Python - https://www.python.org/ (for pip)
- Python-pip - https://pypi.org/project/pip/ (for conan)
- conan - https://conan.io/downloads.html (can be installed with python-pip)
- Qt 5.15.2 fully supported, a minimal functional can compiled with Qt 5.11.1 - https://www.qt.io/ 
- C++ compiler with full C++17 support (MSVC 2019 - 16.10+, gcc 9.3.0+, clang-11+)

C++ language feature support you can check here

MSVC - https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-160

Others - https://en.cppreference.com/w/cpp/compiler_support

* boost/header_only - conan
* LimeReport - submodule
  * JOM - build booster for LimeReport qmake - conan
* QXlsx - submodule
* hidapi - conan if Windows, system package if Linux
* ctti - inside project tree because project was abandoned
* qcustomplot - same as ctti 
* protobuf - conan
* zeromq - conan
# Build: 
How to initialize conan, just run command in repository root folder


```
$ mkdir build && cd build
$ cmake [<options>] ../src 
```


supported options:
* cmake
  * CMAKE_TOOLCHAIN_FILE - custom toolchain file if needed
  * CMAKE_BUILD_TYPE - build type of course
  * CMAKE_INSTALL_PREFIX=install_prefix - prefix for cmake --build --target install
  * CMAKE_PREFIX_PATH:STRING=path_to_qt - path to Qt location, for example C:\Qt\5.15.2\msvc2019
* only for limereport
  * QT_QMAKE_EXECUTABLE:STRING=path_to_qmake - neccessary if qmake not in PATH
  * LIMEREPORT_THREADS - count of threads used to build LimeReport (only for *nix), default value -16
* git 
  * COMMIT_COUNT - output of command ```git rev-list --count HEAD```, neccessary if you don't have git in PATH or you don't have git locally
  * SHORT_HASH - output of command ```git rev-parse --short"="8 HEAD```, neccessary if you don't have git in PATH or you don't have git locally
* cpack
  * CPACK_PACKAGE_DIRECTORY - output directory for deb packages
  * CPACK_PACKAGING_INSTALL_PREFIX=/usr/local - prefix for installed files (now ony for deb packages)
* other
  * CONAN_EXEC=path_to_conan - neccessary if conan is not in PATH
  * USER_DIRECTORY=user_directory - neccessary if you want to use non default conan directory, provide path contains .conan folder
  * A x64/Win32 - used to change target arch only with Visual Studio cmake generator, ninja uses only provided compilers
  * BUILD_WITH_ALISE:BOOL=ON (off by default) - do u wanna build Alise? Use only if you know her.
```$ cmake --build . [<options>]```

supported options for multi-config generators:
* config Release/Debug

```$ cmake --install . [<options>]```

supported options for multi-config generators same as for build step


#### Supported architectures:
|       | x86  | x86_64 | armv7    |
| ----- | ---- | ------ | -------- |
| core  | +    | +      | +        |
| gui   | +    | +      | -        |
| avtuk | +    | +      | only CCU |

Non gui parts can be compiled for x86 (i386, win32), x86_64 (amd64, win64), armv7(armv7a, armhf)

#### How to provide cmake options in QtCreator 

For example, if you want to provide path to conan executable ```CONAN_EXEC``` , you need to append -DCONAN_EXEC=path_to_conan to cmake configure command (look at image).

![example](example.jpg)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FJihadist%2Fcoma.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FJihadist%2Fcoma?ref=badge_shield)



## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FJihadist%2Fcoma.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2FJihadist%2Fcoma?ref=badge_large)