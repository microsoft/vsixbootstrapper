VSIX Installer Bootstrapper
===========================

One of the great new features of Visual Studio 2017 is an initial smaller and fast install. To compliment a smaller - but powerful - initial feature set, installing additional workloads and components on-demand is supported for both end users and package developers.

Package developers can install their VSIX extensions for Visual Studio using this bootstrapper to find the latest version of _VSIXInstaller.exe_ and install their extension(s). This may be preferable for extensions that support Visual Studio 2017 or newer than installing extensions in Windows Installer _.msi_ packages, since MSIs cannot run concurrently in separate processes.

Other deployments may also benefit since they no longer have to find where _VSIXInstaller.exe_ is installed. The command line passed to _VSIXBootstrapper.exe_ is passed through to _VSIXInstaller.exe_.

## Authoring

An example using [WiX][] authoring to include a _.vsix_ package in the Burn chain itself.

```xml
<Bundle>
  <Chain>
    <ExePackage
      Id="MyVSIX"
      Name="packages\VSIXBootstrapper.exe"
      PerMachine="yes"
      InstallCommand="/q /admin /logFile:&quot;[WixBundleLog_MyVSIX]&quot; &quot;[WixBundleOriginalSource]\packages\MyVSIX.vsix&quot;"
      UninstallCommand="/q /admin /logFile:&quot;[WixBundleLog_MyVSIX]&quot; /u:MyVSIX"
      >
      <PayloadGroupRef Id="MyVSIX"/>
    </ExePackage>
  </Chain>
  <PayloadGroup Id="VSIXBootstrapper">
    <Payload
      SourceFile="packages\VSIXBootstrapper.exe"
      Compressed="yes"
      />
  </PayloadGroup>
  <PayloadGroup Id="MyVSIX">
    <PayloadGroupRef Id="VSIXBootstrapper"/>
    <Payload
      SourceFile="packages\MyVSIX.vsix"
      Compressed="yes"
      />
  </PayloadGroup>
</Bundle>
```

Here both _VSIXBootstrapper.exe_ and _MyVSIX.vsix_ (with extension ID "MyVSIX") are compressed in the attached bundle container. Separate `PayloadGroup` elements were used so that you can share _VSIXBootstrapper.exe_ for multiple extensions. You could also specify the `DownloadUrl` attribute instead of compressing these to support web downloads and layouts.

For more examples, please see the [Wiki][].

## Feedback

To file issues or suggestions, please use the [Issues][] page for this project on GitHub.

## License

This project is licensed under the [MIT license](LICENSE.txt).

  [issues]: https://github.com/Microsoft/vsixbootstrapper/issues
  [wiki]: https://github.com/Microsoft/vsixbootstrapper/wiki
  [wix]: http://wixtoolset.org
