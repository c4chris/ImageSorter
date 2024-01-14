// ImageFileInfo.cpp
#include "pch.h"
#include "ImageFileInfo.h"
#include "ImageFileInfo.g.cpp"
#include "ImagesRepository.g.cpp"
#include <random>
#include <MainWindow.xaml.h>
#include <regex>

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
  int32_t ImageFileInfo::Class()
  {
    const std::regex base_regex("_[0-9a-f]{5}\\.png$");
    std::smatch base_match;
    auto fname = to_string(m_path);
    if (std::regex_search(fname, base_match, base_regex))
    {
      std::ssub_match base_sub_match = base_match[0];
      std::string base = base_sub_match.str();
      std::string s = base.substr(1, 5);
      uint32_t v = std::stoul(s, nullptr, 16);
      int cnt[4] = { 0 };
      for (int i = 0; i < NbDetailImg; i++)
      {
        uint32_t t = v & 3;
        v >>= 2;
        cnt[t] += 1;
      }
      if (cnt[0] > 0) return 0;
      if (cnt[3] > 0) return 3;
      if (cnt[2] > 0) return 2;
      return 1;
    }
    const std::regex base_regex2("_[egm]\\.png$");
    if (std::regex_search(fname, base_match, base_regex2))
    {
      std::ssub_match base_sub_match = base_match[0];
      std::string base = base_sub_match.str();
      char c = base[1];
      switch (c)
      {
      case 'e': return 1;
      case 'g': return 2;
      case 'm': return 3;
      }
    }
    return 0;
  }

  Windows::Foundation::IAsyncAction ImageFileInfo::Class(int32_t value)
  {
    //                if (value < 0 || value > 3) return;
    //                if (value == ImageClass) return;
    //                if (FullName.Length <= 6) return;
    //                string end = FullName.Substring(FullName.Length - 6);
    //                string start;
    //                if (end[0] == '_')
    //                {
    //                    start = FullName.Substring(0, FullName.Length - 6);
    //                }
    //                else
    //                {
    //                    start = FullName.Substring(0, FullName.Length - 4);
    //                }
    //                string code = "";
    //                switch (value)
    //                {
    //                    case 1:
    //                        code = "_e";
    //                        break;
    //                    case 2:
    //                        code = "_g";
    //                        break;
    //                    case 3:
    //                        code = "_m";
    //                        break;
    //                }
    //                string desiredName = start + code + ".png";
    //                File.Move(FullName, desiredName);
    //                FullName = desiredName;
    if (value < 0 || value > 3)
      co_return;
    /*auto iFile{ ImageFile() };
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
      OutputDebugStringA("--- Looks like we succeeded... ???\n");*/
    OnPropertyChanged(L"Class");
  }

  hstring ImageFileInfo::ClassColor()
  {
    switch (Class())
    {
    case 1: return L"White";
    case 2: return L"SpringGreen";
    case 3: return L"Red";
    }
    return L"Gold";
  }

  int32_t ImageFileInfo::ClassFromDetails()
  {
    //                int[] cnt = new int[4];
    //                for (int i = 0; i < NbDetailImg; i++)
    //                {
    //                    cnt[detail[i]] += 1;
    //                }
    //                if (cnt[0] > 0) return 0;
    //                if (cnt[3] > 0) return 3;
    //                if (cnt[2] > 0) return 2;
    return 1;
  }

  int32_t ImageFileInfo::RectIdx()
  {
    return m_rectIdx;
  }

  void ImageFileInfo::RectIdx(int32_t value)
  {
    //Debug.WriteLine($"Rect moved from {rectLeft}");
    m_rectIdx = value;
    OnPropertyChanged(L"RectIdx");
    //Debug.WriteLine($"Rect moved to {rectLeft}");
  }

  int32_t ImageFileInfo::RectLeft()
  {
    return m_rectIdx * 88;
  }

  void ImageFileInfo::NextRect()
  {
    auto next = RectIdx() + 1;
    if (next >= NbDetailImg) next = 0;
    RectIdx(next);
  }

  /*IAsyncOperation<BitmapImage> ImageFileInfo::GetImageSourceAsync()
  {
    IRandomAccessStream stream{ co_await ImageFile().OpenAsync(FileAccessMode::Read) };
    BitmapImage bitmap{};
    co_await bitmap.SetSourceAsync(stream);
    stream.Close();
    co_return bitmap;
  }*/

  IAsyncAction ImagesRepository::GetImages(hstring const& folderPath, Microsoft::UI::Dispatching::DispatcherQueue queue)
  {
    OutputDebugStringA("\n\n=== Trying to get images\n");
    auto fp{ folderPath }; // try to get a local copy of the thing
    auto q{ queue };
    co_await winrt::resume_background();
    std::string l1 = "=== GetFolderFromPathAsync " + to_string(fp) + "\n\n";
    OutputDebugStringA(l1.c_str());
    Windows::Storage::StorageFolder folder{ nullptr };
    try
    {
      folder = co_await Windows::Storage::StorageFolder::GetFolderFromPathAsync(fp);
    }
    catch (winrt::hresult_error const& ex)
    {
      winrt::hresult hr = ex.code(); // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
      winrt::hstring message = ex.message(); // The system cannot find the file specified.
      std::string e = "\n\n### Caught exception " + to_string(message) + "\n###\n\n";
      OutputDebugStringA(e.c_str());
    }
    if (folder == nullptr)
    {
      OutputDebugStringA("\n\nfolder is nullptr\n\n");
      co_return;
    }
    OutputDebugStringA("\n\n=== Got the storage folder\n");
    std::vector<hstring> filter{ L".png" };
    Windows::Storage::Search::QueryOptions options = Windows::Storage::Search::QueryOptions(Search::CommonFileQuery::OrderByName, filter);
    auto result = folder.CreateFileQueryWithOptions(options);
    Collections::IVectorView<StorageFile> imageFiles = co_await result.GetFilesAsync();
    OutputDebugStringA("\n\n=== Got the list of files\n");
    // I think we need to go back on the UI thread now...
    co_await wil::resume_foreground(q);
    m_images.Clear();
    for (auto&& file : imageFiles)
    {
      std::string line = "*** Would load image " + to_string(file.Name()) + " from " + to_string(file.Path()) + "\n";
      OutputDebugStringA(line.c_str());
      m_images.Append(ImageSorter::ImageFileInfo(file.Path(), file.Name()));
    }
  }
//    public class ImageInfo : INotifyPropertyChanged
//    {
//        public ImageInfo(string fullName, string name)
//        {
//            FullName = fullName;
//            Name = name;
//            detail = new uint[NbDetailImg];
//        }
//        public string Name { get; set; }
//        public string FullName
//        {
//            get { return fullName; }
//            set
//            {
//                if (value != fullName)
//                {
//                    fullName = value;
//                    OnPropertyChanged("FullName");
//                    OnPropertyChanged(nameof(ImageClass));
//                    OnPropertyChanged(nameof(ImageClassColor));
//                }
//            }
//        }
//        public event PropertyChangedEventHandler PropertyChanged;
//        protected virtual void OnPropertyChanged(string propertyName)
//        {
//            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
//        }
//        private int rectIdx = 0;
//        public static SolidColorBrush[] ColorBrush = {
//            new SolidColorBrush(Colors.Gold),
//            new SolidColorBrush(Colors.White),
//            new SolidColorBrush(Colors.SpringGreen),
//            new SolidColorBrush(Colors.Red),
//            new SolidColorBrush(Colors.Blue),
//        };
//        private Window detailWindow = null;
//        public Window DetailWindow
//        {
//            get { return detailWindow; }
//            set {
//                detailWindow = value;
//                OnPropertyChanged(nameof(DetailWindow));
//            }
//        }
//        public uint[] detail;
//        private string fullName;
//        public const uint NbDetailImg = 9;
//    }
//
//    public class ImagesRepository
//    {
//        public ObservableCollection<ImageInfo> Images;
//

}
