// MainWindow.xaml.h
#pragma once

#include "MainWindow.g.h"

namespace winrt::ImageSorter::implementation
{
  struct MainWindow : MainWindowT<MainWindow>
  {
    //MainWindow()
    //{
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    //}
    MainWindow::MainWindow() :
      m_images(winrt::single_threaded_observable_vector<IInspectable>())
    {
      InitializeComponent();
      GetItemsAsync();
    }

    Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Images() const
    {
      return m_images;
    }

    void ImageGridView_ContainerContentChanging(
      Microsoft::UI::Xaml::Controls::ListViewBase const& sender,
      Microsoft::UI::Xaml::Controls::ContainerContentChangingEventArgs const& args);

  private:
    Windows::Foundation::Collections::IVector<IInspectable> m_images{ nullptr };

    Windows::Foundation::IAsyncAction GetItemsAsync();
    Windows::Foundation::IAsyncOperation<ImageSorter::ImageFileInfo> LoadImageInfoAsync(Windows::Storage::StorageFile);
    fire_and_forget ShowImage(
      Microsoft::UI::Xaml::Controls::ListViewBase const& sender,
      Microsoft::UI::Xaml::Controls::ContainerContentChangingEventArgs const& args);
    fire_and_forget ShowFolderPickerAsync(HWND hWnd);
  public:
      void RadioButtons_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
      void OpenClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
      void ExitClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
  };
}

namespace winrt::ImageSorter::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
