// MainWindow.xaml.cpp
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "ImageFileInfo.h"
#include <regex>

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
  //IAsyncOperation<ImageSorter::ImageFileInfo> MainWindow::LoadImageInfoAsync(StorageFile file)
  //{
  //  auto &properties = co_await file.Properties().GetImagePropertiesAsync();
  //  ImageSorter::ImageFileInfo info(properties,
  //    file, file.DisplayName(), file.DisplayType(), properties.Width(), properties.Height(), UIQueue());
  //  std::string line = "LoadImage: " + to_string(properties.Title()) + ":" + to_string(file.DisplayName()) + "\n";
  //  OutputDebugStringA(line.c_str());
  //  co_return info;
  //}

  IAsyncAction MainWindow::LoadImages(hstring folderPath)
  {
    co_await ImagesRepository().GetImages(folderPath, UIQueue());
    auto numImages = ImagesRepository().Images().Size();
    ImageInfoBar().Message(to_hstring(numImages) + L" have loaded from " + folderPath);
    ImageInfoBar().IsOpen(true);
  }

  void MainWindow::CreateOrUpdateSpringAnimation(float finalValue)
  {
    if (_springAnimation == nullptr)
    {
      auto compositor = this->Compositor();
      if (compositor != nullptr)
      {
        _springAnimation = this->Compositor().CreateSpringVector3Animation();
        _springAnimation.Target(L"Scale");
      }
    }

    if (_springAnimation != nullptr) _springAnimation.FinalValue(Numerics::float3(finalValue));
  }

  void winrt::ImageSorter::implementation::MainWindow::Button_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
  {
    CreateOrUpdateSpringAnimation(1.05f);
    sender.try_as<UIElement>().StartAnimation(_springAnimation);
  }

  void winrt::ImageSorter::implementation::MainWindow::Button_PointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
  {
    CreateOrUpdateSpringAnimation(1.0f);
    sender.try_as<UIElement>().StartAnimation(_springAnimation);
  }

  void MainWindow::Canvas_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
  {
  }

  void MainWindow::Window_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args)
  {
    auto window = sender.try_as<Window>();
    auto canvas = window.Content().try_as<Canvas>();
    auto imageInfo = canvas.DataContext().as<ImageSorter::ImageFileInfo>();
    imageInfo.DetailWindow(nullptr);
    //Debug.WriteLine("### Set DetailWindow to null");
  }

  IAsyncAction MainWindow::AppBarButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    auto windowNative{ this->m_inner.as<::IWindowNative>() };
    HWND hWnd{ 0 };
    windowNative->get_WindowHandle(&hWnd);
    auto folderPicker = Windows::Storage::Pickers::FolderPicker();
    auto initializeWithWindow{ folderPicker.as<::IInitializeWithWindow>() };
    initializeWithWindow->Initialize(hWnd);
    folderPicker.FileTypeFilter().Append(L"*");
    Windows::Storage::StorageFolder folder{ nullptr };
    folder = co_await folderPicker.PickSingleFolderAsync();
    if (folder != nullptr)
    {
      LoadImages(folder.Path());
    }
  }

  //void MainWindow::ExitClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  //{
  //  this->Close();
  //}

  IAsyncAction MainWindow::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    auto imageInfo = sender.try_as<Button>().DataContext().as<ImageSorter::ImageFileInfo>();
    if (imageInfo == nullptr) co_return;
    if (imageInfo.DetailWindow() != nullptr)
    {
      imageInfo.DetailWindow().Activate();
      co_return;
    }
    // initialize the details if we already have some info
    const std::regex base_regex("_[0-9a-f]{5}\\.png$");
    std::smatch base_match;
    auto fname = to_string(imageInfo.Path());
    if (std::regex_search(fname, base_match, base_regex))
    {
      std::ssub_match base_sub_match = base_match[0];
      std::string base = base_sub_match.str();
      std::string s = base.substr(1, 5);
      uint32_t v = std::stoul(s, nullptr, 16);
      for (int i = 0; i < ImageFileInfo::NbDetailImg; i++)
      {
        imageInfo.setDetail(ImageFileInfo::NbDetailImg - 1 - i, v & 3);
        v >>= 2;
      }
    }
    // Create the binding description.
    auto b = Data::Binding();
    b.Mode(Data::BindingMode::OneWay);
    PropertyPath propertyPath(L"RectLeft");
    b.Path(propertyPath);
    auto canvas = Canvas();
    canvas.Width(900);
    canvas.Height(250);
    canvas.DataContext(imageInfo);
    canvas.KeyUp({ this, &MainWindow::Canvas_KeyUp });
    auto image = Image();
    image.Source(Media::Imaging::BitmapImage{ Uri{ imageInfo.Path()}});
    image.SetValue(Canvas::TopProperty(), box_value(4));
    image.SetValue(Canvas::LeftProperty(), box_value(4));
    canvas.Children().Append(image);
    auto rect = Shapes::Rectangle();
    rect.Stroke(ImageFileInfo::ColorBrush[4]);
    rect.StrokeThickness(4.0);
    rect.Width(104.0);
    rect.Height(104.0);
    rect.SetValue(Canvas::TopProperty(), box_value(0));
    rect.SetBinding(Canvas::LeftProperty(), b);
    canvas.Children().Append(rect);
    for (int i = 0; i < ImageFileInfo::NbDetailImg; i++)
    {
      auto r = Shapes::Rectangle();
      r.Fill(ImageFileInfo::ColorBrush[imageInfo.getDetail(i)]);
      r.Width(88.0);
      r.Height(12.0);
      r.SetValue(Canvas::LeftProperty(), box_value(i * 88 + 4));
      r.SetValue(Canvas::TopProperty(), box_value(108));
      canvas.Children().Append(r);
    }
    auto r1 = Shapes::Rectangle();
    r1.Fill(ImageFileInfo::ColorBrush[imageInfo.Class()]);
    r1.Width(12.0);
    r1.Height(12.0);
    r1.SetValue(Canvas::LeftProperty(), box_value(812));
    r1.SetValue(Canvas::TopProperty(), box_value(30));
    canvas.Children().Append(r1);
    auto r2 = Shapes::Rectangle();
    r2.Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
    r2.Width(12.0);
    r2.Height(12.0);
    r2.SetValue(Canvas::LeftProperty(), box_value(812));
    r2.SetValue(Canvas::TopProperty(), box_value(50));
    canvas.Children().Append(r2);
    auto t1 = TextBlock();
    t1.Text(L"Orig value");
    t1.Width(70.0);
    t1.SetValue(Canvas::LeftProperty(), box_value(830));
    t1.SetValue(Canvas::TopProperty(), box_value(25));
    canvas.Children().Append(t1);
    auto t2 = TextBlock();
    t2.Text(L"New value");
    t2.Width(70.0);
    t2.SetValue(Canvas::LeftProperty(), box_value(830));
    t2.SetValue(Canvas::TopProperty(), box_value(45));
    canvas.Children().Append(t2);
    auto window = Window();
    window.Title(imageInfo.Name());
    window.Content(canvas);
    Microsoft::UI::Windowing::AppWindow appWindow = window.AppWindow();
    auto size = Windows::Graphics::SizeInt32();
    size.Width = 1300;
    size.Height = 250;
    appWindow.Resize(size);
    window.Activate();
    window.Closed({ this, &MainWindow::Window_Closed });
    imageInfo.DetailWindow(window);
  }

  void MainWindow::Button_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
  {
    //var imageInfo = (sender as Button) ? .DataContext as ImageInfo;
    //            if (imageInfo != null)
    //            {
    //                var options = new FindNextElementOptions()
    //                {
    //                    SearchRoot = ImageScrollViewer,
    //                    XYFocusNavigationStrategyOverride = XYFocusNavigationStrategyOverride.Projection
    //                };
    //                DependencyObject candidate = null;
    //                if (e.Key == Windows.System.VirtualKey.A)
    //                {
    //                    imageInfo.ImageClass = 1;
    //                    candidate = FocusManager.FindNextElement(FocusNavigationDirection.Down, options);
    //                }
    //                if (e.Key == Windows.System.VirtualKey.E) { imageInfo.ImageClass = 1; }
    //                if (e.Key == Windows.System.VirtualKey.S)
    //                {
    //                    imageInfo.ImageClass = 2;
    //                    candidate = FocusManager.FindNextElement(FocusNavigationDirection.Down, options);
    //                }
    //                if (e.Key == Windows.System.VirtualKey.G) { imageInfo.ImageClass = 2; }
    //                if (e.Key == Windows.System.VirtualKey.D)
    //                {
    //                    imageInfo.ImageClass = 3;
    //                    candidate = FocusManager.FindNextElement(FocusNavigationDirection.Down, options);
    //                }
    //                if (e.Key == Windows.System.VirtualKey.M) { imageInfo.ImageClass = 3; }
    //                if (e.Key == Windows.System.VirtualKey.U) { imageInfo.ImageClass = 0; }
    //                if (candidate != null && candidate is Control)
    //                {
    //                    (candidate as Control).Focus(FocusState.Keyboard);
    //                }
    //            }
  }

  void MainWindow::AppBarButton_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    //            ContentDialog dialog = new ContentDialog();
    //
    //            // XamlRoot must be set in the case of a ContentDialog running in a Desktop app
    //            dialog.XamlRoot = (sender as Button).XamlRoot;
    //            dialog.Style = Application.Current.Resources["DefaultContentDialogStyle"] as Style;
    //            dialog.Title = "Image Sorter";
    //            dialog.CloseButtonText = "Close";
    //            dialog.DefaultButton = ContentDialogButton.Primary;
    //            dialog.Content = "Some interesting text";
    //
    //            var result = await dialog.ShowAsync();
  }
//
//        private void Window_Closed(object sender, WindowEventArgs args)
//        {
//            var window = sender as Window;
//            var canvas = window.Content as Canvas;
//            var imageInfo = canvas?.DataContext as ImageInfo;
//            imageInfo.DetailWindow = null;
//            //Debug.WriteLine("### Set DetailWindow to null");
//        }
//
//        private static void SetWindowSize(Window window, int width, int height)
//        {
//            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(window);
//            var windowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(hwnd);
//            var appWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);
//            appWindow.Resize(new Windows.Graphics.SizeInt32(width, height));
//        }
//
//        // Option 2 - Implement Mica with codebehind.
//        // Allows for toggling backdrops as shown in sample.
//        bool TrySetMicaBackdrop(bool useMicaAlt)
//        {
//            if (Microsoft.UI.Composition.SystemBackdrops.MicaController.IsSupported())
//            {
//                Microsoft.UI.Xaml.Media.MicaBackdrop micaBackdrop = new Microsoft.UI.Xaml.Media.MicaBackdrop();
//                micaBackdrop.Kind = useMicaAlt ? Microsoft.UI.Composition.SystemBackdrops.MicaKind.BaseAlt : Microsoft.UI.Composition.SystemBackdrops.MicaKind.Base;
//                this.SystemBackdrop = micaBackdrop;
//
//                return true; // Succeeded.
//            }
//
//            return false; // Mica is not supported on this system.
//        }
//        private void Canvas_KeyUp(object sender, KeyRoutedEventArgs e)
//        {
//            var canvas = (Canvas)sender;
//            var imageInfo = canvas?.DataContext as ImageInfo;
//            if (imageInfo != null)
//            {
//                if (e.Key == Windows.System.VirtualKey.Tab)
//                {
//                    imageInfo.NextRect();
//                    e.Handled = true;
//                    Canvas.SetLeft(canvas.Children[1], imageInfo.RectLeft);
//                }
//                if (e.Key == Windows.System.VirtualKey.Enter)
//                {
//                    string start;
//                    string pattern = @"_[0-9a-f]{5}\.png$";
//                    Match m = Regex.Match(imageInfo.FullName, pattern, RegexOptions.IgnoreCase);
//                    if (m.Success)
//                    {
//                        start = imageInfo.FullName.Substring(0, m.Index);
//                    }
//                    else
//                    {
//                        string p2 = @"_[egm]\.png$";
//                        Match m2 = Regex.Match(imageInfo.FullName, p2, RegexOptions.IgnoreCase);
//                        if (m2.Success)
//                        {
//                            start = imageInfo.FullName.Substring(0, m2.Index);
//                        }
//                        else
//                        {
//                            start = imageInfo.FullName.Substring(0, imageInfo.FullName.Length - 4);
//                        }
//                    }
//                    uint v = 0;
//                    for (int i = 0; i < ImageInfo.NbDetailImg; i++)
//                    {
//                        v <<= 2;
//                        v |= imageInfo.detail[i];
//                    }
//                    string code = $"_{v:x5}";
//                    string desiredName = start + code + ".png";
//                    File.Move(imageInfo.FullName, desiredName);
//                    imageInfo.FullName = desiredName;
//                    imageInfo.DetailWindow.Close();
//                }
//                if (e.Key == Windows.System.VirtualKey.Escape)
//                {
//                    imageInfo.DetailWindow.Close();
//                }
//                if (e.Key == Windows.System.VirtualKey.A)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 1;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[1];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    imageInfo.NextRect();
//                    e.Handled = true;
//                    Canvas.SetLeft(canvas.Children[1], imageInfo.RectLeft);
//                }
//                if (e.Key == Windows.System.VirtualKey.E)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 1;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[1];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    e.Handled = true;
//                }
//                if (e.Key == Windows.System.VirtualKey.S)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 2;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[2];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    imageInfo.NextRect();
//                    e.Handled = true;
//                    Canvas.SetLeft(canvas.Children[1], imageInfo.RectLeft);
//                }
//                if (e.Key == Windows.System.VirtualKey.G)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 2;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[2];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    e.Handled = true;
//                }
//                if (e.Key == Windows.System.VirtualKey.D)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 3;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[3];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    imageInfo.NextRect();
//                    e.Handled = true;
//                    Canvas.SetLeft(canvas.Children[1], imageInfo.RectLeft);
//                }
//                if (e.Key == Windows.System.VirtualKey.M)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 3;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[3];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    e.Handled = true;
//                }
//                if (e.Key == Windows.System.VirtualKey.U)
//                {
//                    imageInfo.detail[imageInfo.RectIdx] = 0;
//                    (canvas.Children[imageInfo.RectIdx + 2] as Rectangle).Fill = ImageInfo.ColorBrush[0];
//                    (canvas.Children[12] as Rectangle).Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
//                    e.Handled = true;
//                }
//            }
//        }
}
