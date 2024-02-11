// ImageFileInfo.cpp
#include "pch.h"
#include "ImageFileInfo.h"
#include "ImageFileInfo.g.cpp"
#include "ImagesRepository.g.cpp"
#include <random>
#include <MainWindow.xaml.h>
#include <regex>

using namespace std;
namespace winrt
{
  using namespace Microsoft::UI::Xaml;
  using namespace Microsoft::UI::Xaml::Media::Imaging;
  using namespace Windows::Foundation;
  using namespace Windows::Storage;
  using namespace Windows::Storage::FileProperties;
  using namespace Windows::Storage::Streams;
}

namespace winrt::ImageSorter::implementation
{
  Media::Brush ImageFileInfo::ColorBrush[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

  int32_t ImageFileInfo::Class()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
    if (m_class != -1)
      return m_class;
    const std::regex base_regex("_[0-9a-f]{5}\\.png$");
    std::smatch base_match;
    auto fname = to_string(Path());
    if (std::regex_search(fname, base_match, base_regex))
    {
      std::ssub_match base_sub_match = base_match[0];
      std::string base = base_sub_match.str();
      std::string s = base.substr(1, 5);
      uint32_t v = std::stoul(s, nullptr, 16);
      int cnt[4] = { 0 };
      for (int i = 0; i < NbDetailImg; i++)
      {
        uint32_t t = v & 3;
        v >>= 2;
        cnt[t] += 1;
      }
      if (cnt[0] > 0)
      {
        m_class = 0;
        return 0;
      }
      if (cnt[3] > 0)
      {
        m_class = 3;
        return 3;
      }
      if (cnt[2] > 0)
      {
        m_class = 2;
        return 2;
      }
      m_class = 1;
      return 1;
    }
    const std::regex base_regex2("_[egm]\\.png$");
    if (std::regex_search(fname, base_match, base_regex2))
    {
      std::ssub_match base_sub_match = base_match[0];
      std::string base = base_sub_match.str();
      char c = base[1];
      switch (c)
      {
      case 'e':
        m_class = 1;
        return 1;
      case 'g':
        m_class = 2;
        return 2;
      case 'm':
        m_class = 3;
        return 3;
      }
    }
    m_class = 0;
    return 0;
  }

  void ImageFileInfo::Class(int32_t value)
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering set method " __FUNCTION__ "\n\n");
#endif
    if (value < 0 || value > 3) return;
    if (value == Class()) return;
    if (m_file.Name().size() <= 10) return;
    auto sName = to_string(m_file.Name());
    auto end = sName.substr(sName.length() - 6);
    std::string start;
    if (end[0] == '_')
    {
      start = sName.substr(0, sName.length() - 6);
    }
    else
    {
      end = sName.substr(sName.length() - 10);
      if (end[0] == '_')
      {
        start = sName.substr(0, sName.length() - 10);
      }
      else
      {
        start = sName.substr(0, sName.length() - 4);
      }
    }
    std::string code = "";
    switch (value)
    {
    case 1:
      code = "_e";
      break;
    case 2:
      code = "_g";
      break;
    case 3:
      code = "_m";
      break;
    }
    hstring desiredName = to_hstring(start + code + ".png");
    rename(desiredName);
  }

  hstring ImageFileInfo::ClassColor()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
    if (!m_classColor.empty())
      return m_classColor;
    switch (Class())
    {
    case 1:
      m_classColor = L"White";
      return m_classColor;
    case 2:
      m_classColor = L"SpringGreen";
      return m_classColor;
    case 3:
      m_classColor = L"Red";
      return m_classColor;
    }
    m_classColor = L"Gold";
    return m_classColor;
  }

  int32_t ImageFileInfo::ClassFromDetails()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
    int cnt[4] = { 0 };
    for (int i = 0; i < NbDetailImg; i++)
    {
      cnt[m_detail[i]] += 1;
    }
    if (cnt[0] > 0) return 0;
    if (cnt[3] > 0) return 3;
    if (cnt[2] > 0) return 2;
    return 1;
  }

  int32_t ImageFileInfo::RectIdx()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
    return m_rectIdx;
  }

  void ImageFileInfo::RectIdx(int32_t value)
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering set method " __FUNCTION__ "\n\n");
#endif
    m_rectIdx = value;
    OnPropertyChanged(L"RectIdx");
  }

  int32_t ImageFileInfo::RectLeft()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering get method " __FUNCTION__ "\n\n");
#endif
    return m_rectIdx * 88;
  }

  void ImageFileInfo::NextRect()
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering method " __FUNCTION__ "\n\n");
#endif
    auto next = RectIdx() + 1;
    if (next >= NbDetailImg) next = 0;
    RectIdx(next);
  }

  Windows::Foundation::IAsyncAction ImageFileInfo::rename(hstring desiredName)
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering method " __FUNCTION__ "\n\n");
#endif
    if (m_file.Path() == desiredName) co_return;
#ifdef _DEBUG
    std::string s = "### --- trying to rename " + to_string(m_file.Path()) + " to " + to_string(desiredName) + "\n";
    OutputDebugStringA(s.c_str());
#endif
    co_await m_file.RenameAsync(desiredName, NameCollisionOption::GenerateUniqueName);
    m_name = L"";
    m_path = L"";
    m_class = -1;
    m_classColor = L"";
    OnPropertyChanged(L"Class");
    OnPropertyChanged(L"ClassColor");
    OnPropertyChanged(L"Path");
    OnPropertyChanged(L"Name");
  }

  IAsyncAction ImagesRepository::GetImages(hstring const& folderPath, Microsoft::UI::Dispatching::DispatcherQueue queue)
  {
#ifdef _DEBUG
    OutputDebugStringA("\n\n########## - Entering method " __FUNCTION__ "\n\n");
#endif
    OutputDebugStringA("\n\n=== Trying to get images\n");
    auto fp{ folderPath }; // try to get a local copy of the thing
    auto dispatcherQueue{ queue };
    co_await winrt::resume_background();
    std::string l1 = "=== GetFolderFromPathAsync " + to_string(fp) + "\n\n";
    OutputDebugStringA(l1.c_str());
    Windows::Storage::StorageFolder folder{ nullptr };
    try
    {
      folder = co_await Windows::Storage::StorageFolder::GetFolderFromPathAsync(fp);
    }
    catch (winrt::hresult_error const& ex)
    {
      winrt::hresult hr = ex.code(); // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
      winrt::hstring message = ex.message(); // The system cannot find the file specified.
      std::string e = "\n\n### Caught exception " + to_string(message) + "\n###\n\n";
      OutputDebugStringA(e.c_str());
    }
    if (folder == nullptr)
    {
      OutputDebugStringA("\n\nfolder is nullptr\n\n");
      co_return;
    }
    OutputDebugStringA("\n\n=== Got the storage folder\n");
    std::vector<hstring> filter{ L".png" };
    Windows::Storage::Search::QueryOptions options = Windows::Storage::Search::QueryOptions(Search::CommonFileQuery::OrderByName, filter);
    auto result = folder.CreateFileQueryWithOptions(options);
    Collections::IVectorView<StorageFile> imageFiles = co_await result.GetFilesAsync();
    OutputDebugStringA("\n\n=== Got the list of files\n");
    // I think we need to go back on the UI thread now...
    co_await wil::resume_foreground(dispatcherQueue);
    for (auto&& file : imageFiles)
    {
      std::string line = "*** Would load image " + to_string(file.Name()) + " from " + to_string(file.Path()) + "\n";
      OutputDebugStringA(line.c_str());
      m_images.Append(ImageSorter::ImageFileInfo(file));
    }
    OnPropertyChanged(L"Images");
  }
}
