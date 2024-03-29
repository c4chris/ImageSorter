// ImageFileInfo.h
#pragma once
#include "ImageFileInfo.g.h"
#include "ImagesRepository.g.h"

// Select the size of the detail image
// Valid values at the moment are 96 and 64
#define DETAIL_IMG_SIZE 96

namespace winrt::ImageSorter::implementation
{
  struct ImageFileInfo : ImageFileInfoT<ImageFileInfo>
  {
    ImageFileInfo() = default;

    ImageFileInfo::ImageFileInfo(Windows::Storage::StorageFile file) :
      m_file{ file }
    {
      m_rectIdx = 0;
      memset(m_detail, 0, sizeof(m_detail));
      if (ColorBrush[0] == nullptr)
      {
        ColorBrush[0] = Microsoft::UI::Xaml::Media::SolidColorBrush(Microsoft::UI::Colors::Gold());
        ColorBrush[1] = Microsoft::UI::Xaml::Media::SolidColorBrush(Microsoft::UI::Colors::White());
        ColorBrush[2] = Microsoft::UI::Xaml::Media::SolidColorBrush(Microsoft::UI::Colors::SpringGreen());
        ColorBrush[3] = Microsoft::UI::Xaml::Media::SolidColorBrush(Microsoft::UI::Colors::Red());
        ColorBrush[4] = Microsoft::UI::Xaml::Media::SolidColorBrush(Microsoft::UI::Colors::Blue());
      }
    }

    hstring Path()
    {
      if (m_path.empty())
        m_path = m_file.Path();
      return m_path;
    }

    hstring Name()
    {
      if (m_name.empty())
        m_name = m_file.Name();
      return m_name;
    }

    int32_t Class();

    void Class(int32_t value);

    hstring ClassColor();

    int32_t ClassFromDetails();

    int32_t RectIdx();

    void RectIdx(int32_t value);

    int32_t RectLeft();

    void NextRect();

    Microsoft::UI::Xaml::Window DetailWindow()
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
      return m_detailWindow;
    }

    void DetailWindow(Microsoft::UI::Xaml::Window value)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering set method " __FUNCTION__ "\n\n");
#endif
      m_detailWindow = value;
    }

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering add handler " __FUNCTION__ "\n\n");
#endif
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering remove token " __FUNCTION__ "\n\n");
#endif
      m_propertyChanged.remove(token);
    }

    int32_t getDetail(uint32_t idx)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
      return m_detail[idx];
    }

    void setDetail(uint32_t idx, int32_t value)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering set method " __FUNCTION__ "\n\n");
#endif
      m_detail[idx] = value;
    }

    Windows::Foundation::IAsyncAction rename(hstring desiredName);

#if DETAIL_IMG_SIZE == 96
    //   0 -  96
    //  78 - 174
    // 156 - 252
    // 234 - 330
    // 312 - 408
    // 390 - 486
    // 468 - 564
    // 546 - 642
    // 624 - 720
    // 702 - 798
    static const uint32_t NbDetailImg = 10;
    static const uint32_t TopSkipDetailImg = 0;
    static const uint32_t SizeDetailImg = 96;
    static const uint32_t HOvlDetailImg = 18;
    static const int PathDetailWidth = 5;
#elif DETAIL_IMG_SIZE == 64
    static const uint32_t NbDetailImg = 12;
    static const uint32_t TopSkipDetailImg = 16;
    static const uint32_t SizeDetailImg = 64;
    static const uint32_t HOvlDetailImg = 0;
    static const int PathDetailWidth = 6;
#else
#error "DETAIL_IMG_SIZE has an unsupported value"
#endif
    static const uint32_t WidthFullImg = 800;
    static const uint32_t HeightFullImg = 96;
    static const char PathDetailRE[], PathCoarseRE[];
    static Microsoft::UI::Xaml::Media::Brush ColorBrush[5];

  private:
    Windows::Storage::StorageFile m_file;
    int32_t m_rectIdx;
    int32_t m_detail[NbDetailImg];
    Microsoft::UI::Xaml::Window m_detailWindow{ nullptr };
    hstring m_path = L"";
    hstring m_name = L"";
    int32_t m_class = -1;
    hstring m_classColor = L"";

    event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    void OnPropertyChanged(hstring propertyName)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering method " __FUNCTION__ "\n\n");
#endif
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
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
      return m_images;
    }

    void ImagesRepository::ClearImages()
    {
      m_images.Clear();
      OnPropertyChanged(L"Images");
    }

    Windows::Foundation::IAsyncAction ImagesRepository::GetImages(hstring const& folderPath, Microsoft::UI::Dispatching::DispatcherQueue queue);

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering add handler " __FUNCTION__ "\n\n");
#endif
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering remove token " __FUNCTION__ "\n\n");
#endif
      m_propertyChanged.remove(token);
    }

  private:
    event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    Windows::Foundation::Collections::IObservableVector<ImageSorter::ImageFileInfo> m_images{ nullptr };
    void OnPropertyChanged(hstring propertyName)
    {
#ifdef _DEBUG
      OutputDebugStringA("\n\n########## - Entering method " __FUNCTION__ "\n\n");
#endif
      m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }
  };
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
