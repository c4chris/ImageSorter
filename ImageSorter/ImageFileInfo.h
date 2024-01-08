// ImageFileInfo.h
#pragma once
#include "ImageFileInfo.g.h"
#include "ImagesRepository.g.h"

namespace winrt::ImageSorter::implementation
{
  struct ImageFileInfo : ImageFileInfoT<ImageFileInfo>
  {
    ImageFileInfo() = default;

    ImageFileInfo(
      winrt::Windows::Storage::FileProperties::ImageProperties const& properties,
      winrt::Windows::Storage::StorageFile const& imageFile,
      hstring const& name,
      hstring const& type,
      uint32_t const& width,
      uint32_t const& height,
      Microsoft::UI::Dispatching::DispatcherQueue const& queue);

    winrt::Windows::Storage::FileProperties::ImageProperties ImageProperties()
    {
      return m_imageProperties;
    }

    winrt::Windows::Storage::StorageFile ImageFile()
    {
      return m_imageFile;
    }

    hstring ImageName()
    {
      return m_imageName;
    }

    hstring ImageFileType()
    {
      return m_imageFileType;
    }

    hstring ImageDimensions()
    {
      return to_hstring(ImageWidth()) + L" x " + to_hstring(ImageHeight());
      //return L"0 x 0";
    }

    uint32_t ImageWidth()
    {
      return m_imageWidth;
    }

    uint32_t ImageHeight()
    {
      return m_imageHeight;
    }
    
    int32_t ImageClass();

    hstring ImageClassColor()
    {
      return L"red";
    }

    hstring FullName()
    {
      return L"unknown";
    }

    void FullName(hstring const& value)
    {

    }

    Windows::Foundation::IAsyncAction ImageClass(int32_t value);

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
      return m_propertyChanged.add(handler);
    }

    Microsoft::UI::Dispatching::DispatcherQueue UIQueue() const
    {
      return m_uiQueue;
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
      m_propertyChanged.remove(token);
    }

    Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Media::Imaging::BitmapImage> GetImageSourceAsync();

  private:
    // File and information fields.
    Windows::Storage::FileProperties::ImageProperties m_imageProperties{ nullptr };
    Windows::Storage::StorageFile m_imageFile{ nullptr };
    hstring m_imageName;
    hstring m_imageFileType;
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    Microsoft::UI::Dispatching::DispatcherQueue m_uiQueue;

    event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    void OnPropertyChanged(hstring propertyName)
    {
      m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }
  };

  struct ImagesRepository : ImagesRepositoryT<ImagesRepository>
  {
    ImagesRepository() :
      m_images{ Windows::Foundation::Collections::IObservableVector<ImageSorter::ImageFileInfo>() }
    {
    }

    Windows::Foundation::Collections::IObservableVector<ImageSorter::ImageFileInfo> Images()
    {
      return m_images;
    }

    void ImagesRepository::GetImages(hstring const& folder)
    {
      m_folder = folder;
    }

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
      m_propertyChanged.remove(token);
    }

  private:
    hstring m_folder;

    event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    Windows::Foundation::Collections::IObservableVector<ImageSorter::ImageFileInfo> m_images{ nullptr };
    void OnPropertyChanged(hstring propertyName)
    {
      m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }
  };
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
namespace winrt::ImageSorter::factory_implementation
{
  struct ImageFileInfo : ImageFileInfoT<ImageFileInfo, implementation::ImageFileInfo>
  {
  };
  struct ImagesRepository : ImagesRepositoryT<ImagesRepository, implementation::ImagesRepository>
  {
  };
}
