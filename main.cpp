#include "CDotDecoration.hpp"
#include "cairo.h"
#include "globals.hpp"

#include <GLES2/gl2ext.h>
#include <filesystem>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>

static CDotDecoration *current = nullptr;
static bool isTextureLoaded = false;

void loadTexture(fs::path parentPath, bool animated) {
  if (!animated) {
    g_pTexture = nullptr;
    g_pTexture = makeShared<CTexture>();
    g_pTexture->allocate();
    fs::path path = parentPath;

    if (!fs::exists(path))
      noti("[hyprfoci] {} not found", path.generic_string());
    const auto CAIROSURFACE = cairo_image_surface_create_from_png(path.c_str());
    const auto CAIRO = cairo_create(CAIROSURFACE);

    const Vector2D textureSize = {
        static_cast<double>(cairo_image_surface_get_width(CAIROSURFACE)),
        static_cast<double>(cairo_image_surface_get_height(CAIROSURFACE))};

    g_pTexture->m_size = textureSize;

    const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
    glBindTexture(GL_TEXTURE_2D, g_pTexture->m_texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef GLES2
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.x, textureSize.y, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, DATA);
    cairo_surface_destroy(CAIROSURFACE);
    cairo_destroy(CAIRO);
  } else {
    for (auto &pair : g_pTextures) {
      pair.second = nullptr;

      pair.second = makeShared<CTexture>();
      pair.second->allocate();
      fs::path path = parentPath / pair.first;

      if (!fs::exists(path))
        noti("[hyprfoci] {} not found", pair.first, path.generic_string());
      const auto CAIROSURFACE =
          cairo_image_surface_create_from_png(path.c_str());
      const auto CAIRO = cairo_create(CAIROSURFACE);

      const Vector2D textureSize = {
          static_cast<double>(cairo_image_surface_get_width(CAIROSURFACE)),
          static_cast<double>(cairo_image_surface_get_height(CAIROSURFACE))};

      pair.second->m_size = textureSize;

      const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
      glBindTexture(GL_TEXTURE_2D, pair.second->m_texID);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef GLES2
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
#endif
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.x, textureSize.y, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, DATA);
      cairo_surface_destroy(CAIROSURFACE);
      cairo_destroy(CAIRO);
    }
  }
}

void initialLoad() {
  static const auto m_pImgPaths =
      (const Hyprlang::STRING *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:imgs")
          ->getDataStaticPtr();
  static const auto m_pImgPath =
      (const Hyprlang::STRING *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:img")
          ->getDataStaticPtr();

  // make sure only load img or imgs
  if (std::string{*m_pImgPaths} != "none") {
    g_pTexture = nullptr;
    const auto path = expandTilde(std::string{*m_pImgPaths});
    loadTexture(path, true);
  } else if (std::string{*m_pImgPath} != "none") {
    for (auto &t : g_pTextures) {
      t.second = nullptr;
    }
    const auto path = expandTilde(std::string{*m_pImgPath});
    loadTexture(path, false);
  } else {
    for (auto &t : g_pTextures) {
      t.second = nullptr;
    }
    g_pTexture = nullptr;
  }
}

void onCloseWindow(void *self, std::any data) {
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);
  auto square = current;

  if (current && current->getOwner() == PWINDOW) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, square);
    current = nullptr;
  }
}

void onActiveWindow(void *self, std::any data) {
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

  if (!isTextureLoaded)
    initialLoad();

  if (current) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, current);
  }

  if (PWINDOW) {
    auto square = makeUnique<CDotDecoration>(PWINDOW);
    current = square.get();

    HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, std::move(square));
  }
}

void onConfigReload(void *self, std::any data) {
  const auto PWINDOW = g_pCompositor->m_lastWindow.lock();

  if (current) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, current);
  }

  if (PWINDOW) {
    initialLoad();
    auto square = makeUnique<CDotDecoration>(PWINDOW);
    current = square.get();
    HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, std::move(square));
  }
}

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string HASH = __hyprland_api_get_hash();

  if (HASH != GIT_COMMIT_HASH) {
    HyprlandAPI::addNotification(
        PHANDLE,
        "[Hyprfoci] Failure in initialization: Version mismatch (headers ver "
        "is not equal to running hyprland ver)",
        CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
    throw std::runtime_error("[Hyprfoci] Version mismatch");
  }

  // config variables
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:size",
                              Hyprlang::VEC2{20, 20});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:pos",
                              Hyprlang::VEC2{10, 10});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:origin",
                              Hyprlang::VEC2{0, 0});
  HyprlandAPI::addConfigValue(
      PHANDLE, "plugin:hyprfoci:color",
      Hyprlang::INT{*configStringToInt("rgba(11ff3388)")});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:rounding",
                              Hyprlang::FLOAT{4.0});

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:img",
                              Hyprlang::STRING{"none"});
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:imgs",
                              Hyprlang::STRING{"none"});

  static auto P = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "closeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onCloseWindow(self, data);
      });

  static auto P1 = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "activeWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onActiveWindow(self, data);
      });
  static auto P2 = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "configReloaded",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onConfigReload(self, data);
      });

  // generate a deco for current window if exists
  for (auto &w : g_pCompositor->m_windows) {
    if (g_pCompositor->isWindowActive(w)) {
      auto deco = makeUnique<CDotDecoration>(w);
      current = deco.get();
      HyprlandAPI::addWindowDecoration(PHANDLE, w, std::move(deco));
    }
  }

  HyprlandAPI::addNotification(PHANDLE, "[Hyprfoci] init successful",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {"hyprfoci", "A plugin to add a dot focus indicator", "Pohlrabi",
          "0.2.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  HyprlandAPI::addNotification(PHANDLE, "[Hyprfoci] unload successful",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
}
