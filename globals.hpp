#pragma once
#include <filesystem>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <wordexp.h>

inline HANDLE PHANDLE = nullptr;

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
inline std::unordered_map<std::string, SP<CTexture>> g_pTextures = {
    {"both.png", nullptr},
    {"left.png", nullptr},
    {"right.png", nullptr},
};

inline SP<CTexture> g_pTexture = nullptr;
