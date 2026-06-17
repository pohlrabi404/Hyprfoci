#pragma once
#include <filesystem>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <wordexp.h>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprland/src/config/values/ConfigValues.hpp>

inline HANDLE PHANDLE = nullptr;

template <typename T>
using SP = Hyprutils::Memory::CSharedPointer<T>;

struct SVars {
    SP<Config::Values::CVec2Value>   size;
    SP<Config::Values::CVec2Value>   pos;
    SP<Config::Values::CVec2Value>   origin;
    SP<Config::Values::CColorValue>  color;
    SP<Config::Values::CFloatValue>  rounding;
    SP<Config::Values::CStringValue> exclude;
    SP<Config::Values::CStringValue> img;
    SP<Config::Values::CStringValue> imgs;
};

inline SVars vars = {};

// debug
template <typename... Args>
inline void noti(std::format_string<Args...> fmt, Args &&...args) {
  std::string msg = std::format(fmt, std::forward<Args>(args)...);
  HyprlandAPI::addNotification(PHANDLE, msg, CHyprColor(1.0, 0.0, 0.0, 1.0),
                               2000);
}

// to get pngs location
namespace fs = std::filesystem;
inline fs::path expandTilde(std::string_view raw) {
  if (!raw.starts_with('~'))
    return fs::path(raw);
  wordexp_t p{};
  if (wordexp(std::string{raw}.c_str(), &p, 0) != 0 || p.we_wordc == 0) {
    throw std::runtime_error("bad path: " + std::string{raw});
  }
  fs::path result{p.we_wordv[0]};
  wordfree(&p);
  return result;
}

// pngs pointers
inline std::unordered_map<std::string, SP<Render::ITexture>> g_pTextures = {
    {"both.png", nullptr},
    {"left.png", nullptr},
    {"right.png", nullptr},
};

inline SP<Render::ITexture> g_pTexture = nullptr;
