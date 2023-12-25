// ImageFileInfo.h
#pragma once
#include "ImageFileInfo.g.h"

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
}
namespace winrt::ImageSorter::factory_implementation
{
  struct ImageFileInfo : ImageFileInfoT<ImageFileInfo, implementation::ImageFileInfo>
  {
  };
}
