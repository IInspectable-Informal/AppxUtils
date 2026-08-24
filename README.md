# AppxUtils

**[实验性，尚未完成]** 一个用于解析 AppX/MSIX 的轻量级进程内 Windows 运行时组件。  
它封装了复杂的 COM 接口，为 .NET 和 C++ 开发者提供了一套简洁的、类型安全的 API。

## 功能

* ✅ 解析应用包最基本信息（由 AppxUtils.IAppxPackageCore 接口提供）
* ✅ 解析 Win10 MSIX 包/MSIX 捆绑包（需 Win10 1709+（build 16299+））
* ✅ 解析 Win10 AppX/MSIX 包基本属性（由 AppxUtils.IAppxPackage 接口提供，需 Win10+）
* ✅ 解析 Win10 AppX/MSIX 包的可选包属性（由 AppxUtils.IAppxPackage3 接口提供，需 Win10 1607+（build 14393+））
* ✅ 解析 Win10 AppX/MSIX 包主应用包依赖项（由 AppxUtils.IAppxPackage4 接口提供，受限于操作系统提供的接口，需 Win10 1703+（build 15063+））
* ✅ 解析 Win10 AppX/MSIX 包的非限定资源包属性（由 AppxUtils.IAppxPackage6 接口提供，需 Win10 1803+（build 17134+））
* ✅ 解析 Win8 AppX 包遗留属性（由 AppxUtils.IAppxPackageLegacy 接口提供，除非需要解析为 Win8/8.1 开发的 Metro/Modern 应用程序安装包，否则不推荐使用）
* ✅ 通过 IAppxPackageInterop 低级别 COM 接口获取底层的 IAppxPackageReader 接口实例（⚠️这是低级别操作，除非必要，否则不建议在生产环境下使用）
* 🚧 获取 AppX/MSIX 包里的载荷文件（由 AppxUtils.IAppxPackagePayloadFilesReader 接口提供）
* 📋 解析 AppX 捆绑包（需 Win8.1/Win10+）
* 📋 解析 Win10 AppX/MSIX 包驱动依赖项、OS包依赖项、宿主运行时依赖项（由 AppxUtils.IAppxPackage10 接口提供，受限于操作系统提供的接口，需 Win10 2004+（build 19041+））

## 示例

* C#
```csharp
using System;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.Storage.Streams;
using AppxUtils;

// Get AppX package stream and get some basic properties.
StorageFile appxFile = await StorageFile.GetFileFromPathAsync("C:\\TestFolder\\SampleAppx.appx");
IRandomAccessStream appxStream = await appxFile.OpenReadAsync();
AppxPackage appx = AppxPackageFactory.GetAppxPackageFromStream(appxStream);
PackageVersion ver = appx.Version;
Console.WriteLine("Name: " + appx.Name);
Console.WriteLine("Publisher: " + appx.Publisher);
Console.WriteLine("DisplayName: " + appx.DisplayName);
Console.WriteLine($"Version: {ver.Major}.{ver.Minor}.{ver.Build}.{ver.Revision}");
var pkgDependencies = appx.PackageDependencies;
int count = pkgDependencies.Count;
Console.WriteLine($"PackageDependencies({ count }):");
for (int i = 0; i < count; ++i)
{
    AppxPackageDependency dependency = pkgDependencies[i];
    PackageVersion minVer = dependency.MinVersion;
    Console.WriteLine(" - DependencyName: " + dependency.Name);
    Console.WriteLine(" - DependencyPublisher: " + dependency.Publisher);
    Console.WriteLine($" - MinVersion: {minVer.Major}.{minVer.Minor}.{minVer.Build}.{minVer.Revision}");
    Console.WriteLine();
}
appxStream.Dispose();
```

* C++/WinRT
```cpp
#include <iostream>
#include <string>
#include <format>
#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/AppxUtils.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::ApplicationModel;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
    using namespace AppxUtils;
}

// Get AppX package stream and get some basic properties.
winrt::IAsyncAction ParseAppxAsync()
{
    winrt::StorageFile appxFile{ co_await winrt::StorageFile::GetFileFromPathAsync(L"C:\\TestFolder\\SampleAppx.appx") };
    winrt::IRandomAccessStream appxStream{ co_await appxFile.OpenReadAsync() };
    winrt::AppxPackage appx{ AppxPackageFactory::GetAppxPackageFromStream(appxStream) };
    winrt::PackageVersion ver{ appx.Version() };
    std::wcout << L"Name: " << appx.Name().c_str() << std::endl;
    std::wcout << L"Publisher: " << appx.Publisher().c_str() << std::endl;
    std::wcout << L"DisplayName: " << appx.DisplayName().c_str() << std::endl;
    std::wcout << std::format(L"Version: {}.{}.{}.{}", ver.Major, ver.Minor, ver.Build, ver.Revision) << std::endl;
    auto pkgDependencies{ appx.PackageDependencies() };
    uint32_t count{ pkgDependencies.Size() };
    std::wcout << std::format(L"PackageDependencies( {} ):", count) << std::endl;
    for (uint32_t i{ 0 }; i < count; ++i)
    {
        winrt::AppxPackageDependency dependency{ pkgDependencies.GetAt(i) };
        winrt::PackageVersion minVer{ dependency.MinVersion };
        std::wcout << L" - DependencyName: " << dependency.Name.c_str() << std::endl;
        std::wcout << L" - DependencyPublisher: " << dependency.Publisher.c_str() << std::endl;
        std::wcout << std::format(L" - MinVersion: {}.{}.{}.{}", minVer.Major, minVer.Minor, minVer.Build, minVer.Revision) << std::endl;
        std::wcout << std::endl;
    }
    appxStream.Close();
}

int main()
{
    winrt::init_apartment();
    ParseAppxAsync().get();
    winrt::clear_factory_cache();
    winrt::uninit_apartment();
    return 0;
}
```

输出示例
```text
Name: MySampleApp
Publisher: CN=SamplePublisher
DisplayName: My Sample App
Version: 1.0.0.0
PackageDependencies( 3 ):
 - DependencyName: Microsoft.VCLibs.140.00
 - DependencyPublisher: CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US
 - MinVersion: 14.0.33519.0

 - DependencyName: Microsoft.NET.Native.Framework.2.2
 - DependencyPublisher: CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US
 - MinVersion: 2.2.27405.0

 - DependencyName: Microsoft.NET.Native.Runtime.2.2
 - DependencyPublisher: CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US
 - MinVersion: 2.2.27328.0
```

## 受支持的平台

### Windows
* ✅ Windows 11
* ✅ Windows 10
* ✅ Windows 8.1
* ✅ Windows 8
* ❌ Windows 7
* ❌ Windows Vista
* ❌ Windows XP
* ❌ Windows 2000

### Windows Server
* ✅ Windows Server 2025
* ✅ Windows Server 2022
* ✅ Windows Server 2019
* ✅ Windows Server 2016
* ✅ Windows Server 2012 R2
* ✅ Windows Server 2012
* ❌ Windows Server 2008 R2
* ❌ Windows Server 2008
* ❌ Windows Server 2003 R2
* ❌ Windows Server 2003

## 限制

> [!Warning]
> 本项目目前处于**实验性**阶段，**API 和 ABI 可能在未来版本中发生不兼容变更**，请谨慎用于生产环境。

本项目最终依赖 Windows [AppX Packaging API](https://learn.microsoft.com/windows/win32/appxpkg/interfaces) 实现功能，因此**仅支持 Windows 8+ 环境**，暂不支持 Wine 或其它非 Windows 环境。  
如有需要在不受支持的环境下解析 AppX/MSIX，请自行解析文件流。  

## 许可证

[Apache License 2.0](LICENSE)
