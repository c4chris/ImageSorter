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
      return m_file.Path();
    }

    hstring Name()
    {
      return m_file.Name();
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
      return m_detailWindow;
    }

    void DetailWindow(Microsoft::UI::Xaml::Window value)
    {
      m_detailWindow = value;
    }

    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
      return m_propertyChanged.add(handler);
    }

    void PropertyChanged(winrt::event_token const& token) noexcept
    {
      m_propertyChanged.remove(token);
    }

    int32_t getDetail(uint32_t idx)
    {
      return m_detail[idx];
    }

    void setDetail(uint32_t idx, int32_t value)
    {
      m_detail[idx] = value;
    }

    Windows::Foundation::IAsyncAction rename(hstring desiredName);

    static const uint32_t NbDetailImg = 9;
    static Microsoft::UI::Xaml::Media::Brush ColorBrush[5];

  private:
    Windows::Storage::StorageFile m_file;
    int32_t m_rectIdx;
    int32_t m_detail[NbDetailImg];
    Microsoft::UI::Xaml::Window m_detailWindow{ nullptr };

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
