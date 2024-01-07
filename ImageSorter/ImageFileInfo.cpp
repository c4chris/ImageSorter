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
    uint32_t const& width,
    uint32_t const& height,
    Microsoft::UI::Dispatching::DispatcherQueue const& queue) :
    m_imageProperties{ properties },
    m_imageName{ name },
    m_imageFileType{ type },
    m_imageFile{ imageFile },
    m_imageWidth{ width },
    m_imageHeight{ height },
    m_uiQueue{ queue }
  {
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

  ImagesRepository::ImagesRepository(
    hstring const& folder) :
    m_folder{ folder }
  {
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
//        public int ImageClass
//        {
//            get
//            {
//                string pattern = @"_[0-9a-f]{5}\.png$";
//                Match m = Regex.Match(FullName, pattern, RegexOptions.IgnoreCase);
//                if (m.Success)
//                {
//                    var v = Convert.ToUInt32(m.Value.Substring(1,5),16);
//                    int[] cnt = new int[4];
//                    for (int i = 0; i < NbDetailImg; i++)
//                    {
//                        uint t = v & 3;
//                        v >>= 2;
//                        cnt[t] += 1;
//                    }
//                    if (cnt[0] > 0) return 0;
//                    if (cnt[3] > 0) return 3;
//                    if (cnt[2] > 0) return 2;
//                    return 1;
//                }
//                string p2 = @"_[egm]\.png$";
//                Match m2 = Regex.Match(FullName, p2, RegexOptions.IgnoreCase);
//                if (m2.Success)
//                {
//                    switch (m2.Value[1])
//                    {
//                        case 'e':
//                            return 1;
//                        case 'g':
//                            return 2;
//                        case 'm':
//                            return 3;
//                    }
//                }
//                return 0;
//            }
//            set
//            {
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
//            }
//        }
//        public string ImageClassColor
//        {
//            get
//            {
//                switch(ImageClass)
//                {
//                    case 1: return "White";
//                    case 2: return "SpringGreen";
//                    case 3: return "Red";
//                    default: return "Gold";
//                }
//            }
//        }
//        public int ClassFromDetail
//        {
//            get
//            {
//                int[] cnt = new int[4];
//                for (int i = 0; i < NbDetailImg; i++)
//                {
//                    cnt[detail[i]] += 1;
//                }
//                if (cnt[0] > 0) return 0;
//                if (cnt[3] > 0) return 3;
//                if (cnt[2] > 0) return 2;
//                return 1;
//            }
//        }
//        public int RectIdx
//        {
//            get { return rectIdx; }
//            set
//            {
//                //Debug.WriteLine($"Rect moved from {rectLeft}");
//                rectIdx = value;
//                OnPropertyChanged(nameof(RectIdx));
//                //Debug.WriteLine($"Rect moved to {rectLeft}");
//            }
//        }
//        public void NextRect ()
//        {
//            var next = RectIdx + 1;
//            if (next >= NbDetailImg) next = 0;
//            RectIdx = next;
//        }
//        public int RectLeft
//        {
//            get
//            {
//                return rectIdx * 88;
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
//        public void GetImages(string folderPath)
//        {
//            if (Images == null)
//            {
//                Images = new ObservableCollection<ImageInfo>();
//            }
//            else
//            {
//                Images.Clear();
//            }
//            var di = new DirectoryInfo(folderPath);
//            var files = di.GetFiles("*.png");
//            foreach (var file in files)
//            {
//                Images.Add(new ImageInfo(file.FullName, file.Name));
//            }
//        }
//    }
}
