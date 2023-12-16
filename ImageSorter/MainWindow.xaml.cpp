// MainWindow.xaml.cpp
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "ImageFileInfo.h"

//using namespace winrt;
//using namespace Microsoft::UI::Xaml;
namespace winrt
{
  using namespace Windows::ApplicationModel;
  using namespace Windows::Foundation;
  using namespace Windows::Foundation::Collections;
  using namespace Windows::Storage;
  using namespace Windows::Storage::Search;
  using namespace Microsoft::UI::Xaml;
  using namespace Microsoft::UI::Xaml::Controls;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ImageSorter::implementation
{
  IAsyncAction MainWindow::GetItemsAsync()
  {
    StorageFolder appInstalledFolder = Package::Current().InstalledLocation();
    StorageFolder picturesFolder = co_await appInstalledFolder.GetFolderAsync(L"Assets\\Samples");

    auto result = picturesFolder.CreateFileQueryWithOptions(QueryOptions());

    IVectorView<StorageFile> imageFiles = co_await result.GetFilesAsync();
    for (auto&& file : imageFiles)
    {
      Images().Append(co_await LoadImageInfoAsync(file));
    }

    ImageGridView().ItemsSource(Images());
  }

  IAsyncOperation<ImageSorter::ImageFileInfo> MainWindow::LoadImageInfoAsync(StorageFile file)
  {
    auto properties = co_await file.Properties().GetImagePropertiesAsync();
    ImageSorter::ImageFileInfo info(properties,
      file, file.DisplayName(), file.DisplayType());
    co_return info;
  }

  void MainWindow::ImageGridView_ContainerContentChanging(ListViewBase const& sender, ContainerContentChangingEventArgs const& args)
  {
    if (args.InRecycleQueue())
    {
      auto templateRoot = args.ItemContainer().ContentTemplateRoot().try_as<Grid>();
      auto image = templateRoot.FindName(L"ItemImage").try_as<Image>();
      image.Source(nullptr);
    }
    if (args.Phase() == 0)
    {
      args.RegisterUpdateCallback({ this, &MainWindow::ShowImage });
      args.Handled(true);
    }
  }

  void MainWindow::RadioButtons_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
  {

  }

  void MainWindow::OpenClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    // show dialog
    // Retrieve the window handle (HWND) of the current WinUI 3 window.
    auto windowNative{ this->m_inner.as<::IWindowNative>() };
    HWND hWnd{ 0 };
    windowNative->get_WindowHandle(&hWnd);
    ShowFolderPickerAsync(hWnd);
  }

  void MainWindow::ExitClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    this->Close();
  }

  fire_and_forget MainWindow::ShowImage(ListViewBase const& sender, ContainerContentChangingEventArgs const& args)
  {
    if (args.Phase() == 1)
    {
      // It's phase 1, so show this item's image.
      auto templateRoot = args.ItemContainer().ContentTemplateRoot().try_as<Grid>();
      auto image = templateRoot.FindName(L"ItemImage").try_as<Image>();
      auto item = args.Item().try_as<ImageSorter::ImageFileInfo>();
      image.Source(co_await get_self<ImageSorter::implementation::ImageFileInfo>(item)->GetImageSourceAsync());
    }
  }

  fire_and_forget MainWindow::ShowFolderPickerAsync(HWND hWnd)
  {
    // Create a folder picker.
    Windows::Storage::Pickers::FolderPicker folderPicker;

    // Initialize the folder picker with the window handle (HWND).
    auto initializeWithWindow{ folderPicker.as<::IInitializeWithWindow>() };
    initializeWithWindow->Initialize(hWnd);

    // Use the folder picker as usual.
    folderPicker.FileTypeFilter().Append(L"*");
    auto picturesFolder{ co_await folderPicker.PickSingleFolderAsync() };
    auto result = picturesFolder.CreateFileQueryWithOptions(QueryOptions());

    Images().Clear();
    IVectorView<StorageFile> imageFiles = co_await result.GetFilesAsync();
    for (auto&& file : imageFiles)
    {
      Images().Append(co_await LoadImageInfoAsync(file));
    }

    //ImageGridView().ItemsSource(Images());
    ImageGridView().UpdateLayout();
  }
}
