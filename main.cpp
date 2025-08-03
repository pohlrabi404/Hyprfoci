#include "CDotDecoration.hpp"
#include "cairo.h"
#include "globals.hpp"
#include "src/Compositor.hpp"

#include <GLES2/gl2ext.h>
#include <filesystem>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>

static CDotDecoration *current = nullptr;

void loadTexture() {
  static auto *const PIMGPATH =
      (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:img")
          ->getDataStaticPtr();
  if (std::string{*PIMGPATH} == "none")
    return;
  PTEXTURE = makeShared<CTexture>();
  PTEXTURE->allocate();

  std::string path = std::string{*PIMGPATH};

  if (!std::filesystem::exists(path)) {
    HyprlandAPI::addNotification(
        PHANDLE, std::format("[Hyprfoci] no image exist at {}", path.c_str()),
        CHyprColor{0.0, 0.0, 1.0, 1.0}, 5000);
  }

  const auto CAIROSURFACE = cairo_image_surface_create_from_png(path.c_str());
  const auto CAIRO = cairo_create(CAIROSURFACE);

  const Vector2D textureSize = {
      static_cast<double>(cairo_image_surface_get_width(CAIROSURFACE)),
      static_cast<double>(cairo_image_surface_get_height(CAIROSURFACE))};

  PTEXTURE->m_size = textureSize;

  const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
  glBindTexture(GL_TEXTURE_2D, PTEXTURE->m_texID);
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

void onCloseWindow(void *self, std::any data) {
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);
  auto square = current;

  if (current && current->m_pWindow.lock() == PWINDOW) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, square);
    current = nullptr;
  }
}

void onActiveWindow(void *self, std::any data) {
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

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

  if (PTEXTURE) {
    PTEXTURE = nullptr;
  }

  if (PWINDOW) {
    loadTexture();
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

  // TODO: PNG support
  HyprlandAPI::addConfigValue(PHANDLE, "plugin:hyprfoci:img",
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

  HyprlandAPI::addNotification(PHANDLE, "[Hyprfoci] init successful",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {"hyprfoci", "A plugin to add a dot focus indicator", "Pohlrabi",
          "0.2.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
