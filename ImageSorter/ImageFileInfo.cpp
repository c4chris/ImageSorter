// ImageFileInfo.cpp
#include "pch.h"
#include "ImageFileInfo.h"
#include "ImageFileInfo.g.cpp"
#include <random>
#include <MainWindow.xaml.h>

using namespace std;
namespace winrt
{
  using namespace Microsoft::UI::Xaml;
  using namespace Microsoft::UI::Xaml::Media::Imaging;
  using namespace Windows::Foundation;
  using namespace Windows::Storage;
  using namespace Windows::Storage::FileProperties;
  using namespace Windows::Storage::Streams;
}

namespace winrt::ImageSorter::implementation
{
  ImageFileInfo::ImageFileInfo(
    winrt::Windows::Storage::FileProperties::ImageProperties const& properties,
    winrt::Windows::Storage::StorageFile const& imageFile,
    hstring const& name,
    hstring const& type,
    Microsoft::UI::Dispatching::DispatcherQueue const& queue) :
    m_imageProperties{ properties },
    m_imageName{ name },
    m_imageFileType{ type },
    m_imageFile{ imageFile },
    m_uiQueue{ queue }
  {
  }

  void ImageFileInfo::ImageTitle(hstring const& value)
  {
    if (ImageProperties().Title() != value)
    {
      ImageProperties().Title(value);
      ImageProperties().SavePropertiesAsync();
      OnPropertyChanged(L"ImageTitle");
    }
  }

  int32_t ImageFileInfo::ImageClass()
  {
    string n = to_string(ImageFile().Path());
    std::string line = "ImageClass() on " + n + "\n";
    OutputDebugStringA(line.c_str());
    if (n.size() > 6) {
      string end = n.substr(n.size() - 6);
      if (end[0] == '_') {
        switch (end[1]) {
        case 'e':
          return int32_t(ImageClassValue::Empty);
        case 'g':
          return int32_t(ImageClassValue::Good);
        case 'm':
          return int32_t(ImageClassValue::Mixed);
        }
      }
    }
    return int32_t(ImageClassValue::Unclassified);
  }

  IAsyncAction ImageFileInfo::ImageClass(int32_t value)
  {
    if (value < 0 || value > 3)
      co_return;
    auto iFile{ ImageFile() };
    auto iClass{ ImageClass() };
    auto q{ UIQueue() };
    if (iClass == value)
      co_return;
    co_await winrt::resume_background();
    char buf[256];
    snprintf(buf, sizeof(buf), "ImageClass(%d) from %d\n", value, iClass);
    OutputDebugStringA(buf);
    string n = to_string(iFile.Name());
    if (n.size() > 6) {
      string end = n.substr(n.size() - 6);
      string start;
      if (end[0] == '_') {
        start = n.substr(0, n.size() - 6);
      }
      else {
        start = n.substr(0, n.size() - 4);
      }
      string code = "";
      switch (value) {
      case int32_t(ImageClassValue::Empty):
        code = "_e";
        break;
      case int32_t(ImageClassValue::Good):
        code = "_g";
        break;
      case int32_t(ImageClassValue::Mixed):
        code = "_m";
        break;
      }
      hstring desiredName = to_hstring(start + code + ".png");
      std::string line = "*** Trying to rename " + n + " to " + to_string(desiredName) + "\n";
      OutputDebugStringA(line.c_str());
      co_await iFile.RenameAsync(desiredName, NameCollisionOption::GenerateUniqueName);
      co_await wil::resume_foreground(q);
      OutputDebugStringA("--- Looks like we succeeded... ???\n");
      OnPropertyChanged(L"ImageClass");
    }
  }

  IAsyncOperation<BitmapImage> ImageFileInfo::GetImageSourceAsync()
  {
    IRandomAccessStream stream{ co_await ImageFile().OpenAsync(FileAccessMode::Read) };
    BitmapImage bitmap{};
    co_await bitmap.SetSourceAsync(stream);
    stream.Close();
    co_return bitmap;
  }
}