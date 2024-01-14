// ImageFileInfo.h
#pragma once
#include "ImageFileInfo.g.h"
#include "ImagesRepository.g.h"

namespace winrt::ImageSorter::implementation
{
  struct ImageFileInfo : ImageFileInfoT<ImageFileInfo>
  {
    ImageFileInfo() = default;

    ImageFileInfo::ImageFileInfo(Windows::Storage::StorageFile file) :
      m_file{ file }
    {
      m_rectIdx = 0;
      memset(detail, 0, sizeof(detail));
    }

    hstring Path()
    {
      return m_file.Path();
    }

    hstring Name()
    {
      return m_file.Name();
    }

    int32_t Class();

    Windows::Foundation::IAsyncAction Class(int32_t value);

    hstring ClassColor();

    int32_t ClassFromDetails();

    int32_t RectIdx();

    void RectIdx(int32_t value);

    int32_t RectLeft();

    void NextRect();

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
      m_propertyChanged.remove(token);
    }

    static const uint32_t NbDetailImg = 9;

  private:
    Windows::Storage::StorageFile m_file;
    int32_t m_rectIdx;
    int32_t detail[NbDetailImg];

    event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    void OnPropertyChanged(hstring propertyName)
    {
      m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }
  };

  struct ImagesRepository : ImagesRepositoryT<ImagesRepository>
  {
    ImagesRepository() :
      m_images{ single_threaded_observable_vector<ImageSorter::ImageFileInfo>() }
    {
#ifdef _DEBUG
      char buf[256];
      snprintf(buf, sizeof(buf), "\n\nCreated m_images of size %u\n\n", m_images.Size());
      OutputDebugStringA(buf);
#endif
    }

    Windows::Foundation::Collections::IObservableVector<ImageSorter::ImageFileInfo> Images()
    {
      return m_images;
    }

    Windows::Foundation::IAsyncAction ImagesRepository::GetImages(hstring const& folder, Microsoft::UI::Dispatching::DispatcherQueue queue);

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
      m_propertyChanged.remove(token);
    }

  private:
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
//        
//        private string fullName;
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
