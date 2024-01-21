// MainWindow.xaml.cpp
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "ImageFileInfo.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>

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
  IAsyncAction MainWindow::LoadImages(hstring folderPath)
  {
    co_await ImagesRepository().GetImages(folderPath, this->DispatcherQueue());
    //ImageInfoBar().Message(to_hstring(numImages) + L" have loaded from " + folderPath);
    //ImageInfoBar().IsOpen(true);
    //auto dispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    co_await wil::resume_foreground(this->DispatcherQueue());
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
    auto canvas = sender.try_as<Canvas>();
    auto imageInfo = canvas.DataContext().as<ImageSorter::ImageFileInfo>();
    if (imageInfo == nullptr) return;
    if (e.Key() == Windows::System::VirtualKey::Tab)
    {
      imageInfo.NextRect();
      e.Handled(true);
      canvas.SetLeft(canvas.Children().GetAt(1), imageInfo.RectLeft());
    }
    if (e.Key() == Windows::System::VirtualKey::Enter)
    {
      std::string start;
      const std::regex base_regex("_[0-9a-f]{5}\\.png$");
      std::smatch base_match;
      auto fname = to_string(imageInfo.Name());
      if (std::regex_search(fname, base_match, base_regex))
      {
        start = fname.substr(0, base_match.position());
      }
      else
      {
        const std::regex r2("_[egm]\\.png$");
        if (std::regex_search(fname, base_match, r2))
        {
          start = fname.substr(0, base_match.position());
        }
        else
        {
          start = fname.substr(0, fname.length() - 4);
        }
      }
      uint32_t v = 0;
      for (int i = 0; i < ImageFileInfo::NbDetailImg; i++)
      {
        v <<= 2;
        v |= imageInfo.getDetail(i);
      }
      std::ostringstream o;
      o << std::hex << std::setfill('0') << std::setw(5) << v;
      //std::string code = std::format("_{:06x}", v);
      imageInfo.rename(to_hstring(start + "_" + o.str() + ".png"));
      imageInfo.DetailWindow().Close();
    }
    if (e.Key() == Windows::System::VirtualKey::Escape)
    {
      imageInfo.DetailWindow().Close();
    }
    if (e.Key() == Windows::System::VirtualKey::A)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 1);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[1]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      imageInfo.NextRect();
      e.Handled(true);
      canvas.SetLeft(canvas.Children().GetAt(1), imageInfo.RectLeft());
    }
    if (e.Key() == Windows::System::VirtualKey::E)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 1);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[1]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      e.Handled(true);
    }
    if (e.Key() == Windows::System::VirtualKey::S)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 2);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[2]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      imageInfo.NextRect();
      e.Handled(true);
      canvas.SetLeft(canvas.Children().GetAt(1), imageInfo.RectLeft());
    }
    if (e.Key() == Windows::System::VirtualKey::G)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 2);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[2]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      e.Handled(true);
    }
    if (e.Key() == Windows::System::VirtualKey::D)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 3);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[3]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      imageInfo.NextRect();
      e.Handled(true);
      canvas.SetLeft(canvas.Children().GetAt(1), imageInfo.RectLeft());
    }
    if (e.Key() == Windows::System::VirtualKey::M)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 3);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[3]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      e.Handled(true);
    }
    if (e.Key() == Windows::System::VirtualKey::U)
    {
      imageInfo.setDetail(imageInfo.RectIdx(), 0);
      canvas.Children().GetAt(imageInfo.RectIdx() + 2).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[0]);
      canvas.Children().GetAt(12).try_as<Shapes::Rectangle>().Fill(ImageFileInfo::ColorBrush[imageInfo.ClassFromDetails()]);
      e.Handled(true);
    }
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
      co_await wil::resume_foreground(this->DispatcherQueue());
      ImagesRepository().ClearImages();
      co_await LoadImages(folder.Path());
    }
  }

  IAsyncAction MainWindow::AppBarButton_Click_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    aboutContentDialog().XamlRoot(sender.try_as<Button>().XamlRoot());
    auto result = co_await aboutContentDialog().ShowAsync();
  }

  void MainWindow::Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
  {
    auto imageInfo = sender.try_as<Button>().DataContext().as<ImageSorter::ImageFileInfo>();
    if (imageInfo == nullptr) return;
    if (imageInfo.DetailWindow() != nullptr)
    {
      imageInfo.DetailWindow().Activate();
      return;
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
    size.Width = 1400;
    size.Height = 250;
    appWindow.Resize(size);
    window.Activate();
    window.Closed({ this, &MainWindow::Window_Closed });
    imageInfo.DetailWindow(window);
  }

  void MainWindow::Button_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
  {
    auto imageInfo = sender.try_as<Button>().DataContext().as<ImageSorter::ImageFileInfo>();
    if (imageInfo == nullptr) return;
    auto options = Input::FindNextElementOptions::FindNextElementOptions();
    options.SearchRoot(ImageScrollViewer());
    options.XYFocusNavigationStrategyOverride(Input::XYFocusNavigationStrategyOverride::Projection);
    DependencyObject candidate{ nullptr };
    if (e.Key() == Windows::System::VirtualKey::A)
    {
      imageInfo.Class(1);
      candidate = Input::FocusManager::FindNextElement(Input::FocusNavigationDirection::Down, options);
    }
    if (e.Key() == Windows::System::VirtualKey::E) { imageInfo.Class(1); }
    if (e.Key() == Windows::System::VirtualKey::S)
    {
      imageInfo.Class(2);
      candidate = Input::FocusManager::FindNextElement(Input::FocusNavigationDirection::Down, options);
    }
    if (e.Key() == Windows::System::VirtualKey::G) { imageInfo.Class(2); }
    if (e.Key() == Windows::System::VirtualKey::D)
    {
      imageInfo.Class(3);
      candidate = Input::FocusManager::FindNextElement(Input::FocusNavigationDirection::Down, options);
    }
    if (e.Key() == Windows::System::VirtualKey::M) { imageInfo.Class(3); }
    if (e.Key() == Windows::System::VirtualKey::U) { imageInfo.Class(0); }
    if (Control theControl{ candidate.try_as<Control>() })
    {
      theControl.Focus(FocusState::Keyboard);
    }
  }
}
