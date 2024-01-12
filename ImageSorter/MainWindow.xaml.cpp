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

  void MainWindow::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    //            var imageInfo = (sender as Button)?.DataContext as ImageInfo;
    //            if (imageInfo != null)
    //            {
    //                if (imageInfo.DetailWindow != null)
    //                {
    //                    imageInfo.DetailWindow.Activate();
    //                    return;
    //                }
    //                // initialize the details if we already have some info
    //                string pattern = @"_[0-9a-f]{5}\.png$";
    //                Match m = Regex.Match(imageInfo.FullName, pattern, RegexOptions.IgnoreCase);
    //                if (m.Success)
    //                {
    //                    var v = Convert.ToUInt32(m.Value.Substring(1, 5),16);
    //                    for (int i = 0; i < ImageInfo.NbDetailImg; i++)
    //                    {
    //                        imageInfo.detail[ImageInfo.NbDetailImg-1-i] = v & 3;
    //                        v >>= 2;
    //                    }
    //                }
    //                // Create the binding description.
    //                Binding b = new Binding();
    //                b.Mode = BindingMode.OneWay;
    //                b.Source = imageInfo.RectLeft;
    //                Canvas canvas = new();
    //                canvas.Width = 900;
    //                canvas.Height = 250;
    //                canvas.DataContext = imageInfo;
    //                canvas.KeyUp += Canvas_KeyUp;
    //                Image image = new();
    //                image.Source = new BitmapImage(new Uri(imageInfo.FullName, UriKind.Absolute));
    //                image.SetValue(Canvas.TopProperty, 4);
    //                image.SetValue(Canvas.LeftProperty, 4);
    //                canvas.Children.Add(image);
    //                var rect = new Rectangle();
    //                rect.Stroke = ImageInfo.ColorBrush[4];
    //                rect.StrokeThickness = 4;
    //                rect.Width = 104;
    //                rect.Height = 104;
    //                rect.SetValue(Canvas.TopProperty, 0);
    //                rect.SetBinding(Canvas.LeftProperty, b);
    //                canvas.Children.Add(rect);
    //                for (int i = 0; i < ImageInfo.NbDetailImg; i++)
    //                {
    //                    var r = new Rectangle();
    //                    r.Fill = ImageInfo.ColorBrush[imageInfo.detail[i]];
    //                    r.Width = 88;
    //                    r.Height = 12;
    //                    r.SetValue(Canvas.LeftProperty, i*88 + 4);
    //                    r.SetValue(Canvas.TopProperty, 108);
    //                    canvas.Children.Add(r);
    //                }
    //                var r1 = new Rectangle();
    //                r1.Fill = ImageInfo.ColorBrush[imageInfo.ImageClass];
    //                r1.Width = 12;
    //                r1.Height = 12;
    //                r1.SetValue(Canvas.LeftProperty, 812);
    //                r1.SetValue(Canvas.TopProperty, 30);
    //                canvas.Children.Add(r1);
    //                var r2 = new Rectangle();
    //                r2.Fill = ImageInfo.ColorBrush[imageInfo.ClassFromDetail];
    //                r2.Width = 12;
    //                r2.Height = 12;
    //                r2.SetValue(Canvas.LeftProperty, 812);
    //                r2.SetValue(Canvas.TopProperty, 50);
    //                canvas.Children.Add(r2);
    //                var t1 = new TextBlock();
    //                t1.Text = "Orig value";
    //                t1.Width = 70;
    //                t1.SetValue(Canvas.LeftProperty, 830);
    //                t1.SetValue(Canvas.TopProperty, 25);
    //                canvas.Children.Add(t1);
    //                var t2 = new TextBlock();
    //                t2.Text = "New value";
    //                t2.Width = 70;
    //                t2.SetValue(Canvas.LeftProperty, 830);
    //                t2.SetValue(Canvas.TopProperty, 45);
    //                canvas.Children.Add(t2);
    //                Window window = new()
    //                {
    //                    Title = imageInfo.Name,
    //                    Content = canvas
    //                };
    //                SetWindowSize(window, 1300, 250);
    //                window.Activate();
    //                window.Closed += Window_Closed;
    //                imageInfo.DetailWindow = window;
    //            }
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
//
//        private async void AppBarButton_Click_1(object sender, RoutedEventArgs e)
//        {

//        }
//
//        private void Btn_KeyUp(object sender, KeyRoutedEventArgs e)
//        {
//            
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
