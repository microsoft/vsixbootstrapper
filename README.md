VSIX Installer Bootstrapper
===========================

[![build status: master](https://ci.appveyor.com/api/projects/status/8kj7280gctbttf3b/branch/master?svg=true)](https://ci.appveyor.com/project/VisualStudioSetup/vsixbootstrapper)
[![github release](https://img.shields.io/github/release/Microsoft/vsixbootstrapper.svg?logo=github)](https://github.com/Microsoft/vsixbootstrapper/releases/latest)
[![github releases: all](https://img.shields.io/github/downloads/Microsoft/vsixbootstrapper/total.svg?logo=github&label=github)](https://github.com/Microsoft/vsixbootstrapper/releases)
[![nuget: all](https://img.shields.io/nuget/dt/vsixbootstrapper.svg?label=nuget)](https://nuget.org/packages/vsixbootstrapper)

One of the great new features of Visual Studio 2017 is an initial smaller and fast install. To compliment a smaller - but powerful - initial feature set, installing additional workloads and components on-demand is supported for both end users and package developers.

Package developers can install their VSIX extensions for Visual Studio using this bootstrapper to find the latest version of _VSIXInstaller.exe_ and install their extension(s). This may be preferable for extensions that support Visual Studio 2017 or newer than installing extensions in Windows Installer _.msi_ packages, since MSIs cannot run concurrently in separate processes.

Other deployments may also benefit since they no longer have to find where _VSIXInstaller.exe_ is installed. The command line passed to _VSIXBootstrapper.exe_ is passed through to _VSIXInstaller.exe_.

See our [samples](https://github.com/Microsoft/vsixbootstrapper/wiki/Samples) for ideas how to use _VSIXBootstrapper.exe_.

## Feedback

To file issues or suggestions, please use the [Issues][] page for this project on GitHub.

## License

This project is licensed under the [MIT license](LICENSE.txt).

## Status

This project uses a Git flow model releasing from the `master` branch with development based on and stabilized in the `develop` branch.

  [issues]: https://github.com/Microsoft/vsixbootstrapper/issues
  [wix]: http://wixtoolset.org
