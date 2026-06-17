#include "CDotDecoration.hpp"
#include "cairo.h"
#include "globals.hpp"

#include <GLES2/gl2ext.h>
#include <filesystem>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/desktop/state/FocusState.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>
#include <hyprland/src/render/Renderer.hpp>

static CDotDecoration *current = nullptr;
static bool isTextureLoaded = false;

// TODO this is messy as hell lol
void loadTexture(fs::path parentPath, bool animated) {
  if (!animated) {
    g_pTexture = nullptr;
    g_pTexture = g_pHyprRenderer->createTexture();
    g_pTexture->allocate(Vector2D());
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

      pair.second = g_pHyprRenderer->createTexture();
      pair.second->allocate(Vector2D());
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
  static auto ImgPaths = vars.imgs->value();
  static auto ImgPath = vars.img->value();

  // make sure only load img or imgs
  if (ImgPaths != "none") {
    g_pTexture = nullptr;
    const auto path = expandTilde(ImgPaths);
    loadTexture(path, true);
  } else if (ImgPath != "none") {
    for (auto &t : g_pTextures) {
      t.second = nullptr;
    }
    const auto path = expandTilde(ImgPath);
    loadTexture(path, false);
  } else {
    for (auto &t : g_pTextures) {
      t.second = nullptr;
    }
    g_pTexture = nullptr;
  }
}

void onCloseWindow(const PHLWINDOW& PWINDOW) {
  auto square = current;

  if (current && current->getOwner() == PWINDOW) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, square);
    current = nullptr;
  }
}

void onActiveWindow(const PHLWINDOW& PWINDOW) {

  if (!isTextureLoaded)
    initialLoad();

  if (current) {
    HyprlandAPI::removeWindowDecoration(PHANDLE, current);
  }

  if (PWINDOW) {
    std::string excluded = vars.exclude->value();

    std::string WindowTitle = PWINDOW->m_class;
    std::string className;
    for(char c : excluded + ',') {
      if (c == ' ' || c == ',') {
        if(!className.empty() && WindowTitle == className) {
          current = nullptr;
          return;
        }
        className = "";
      }
      else className += c;
    }

    auto square = makeUnique<CDotDecoration>(PWINDOW);
    current = square.get();
    HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, std::move(square));
  }
}

void onConfigReload() {
  const auto PWINDOW = Desktop::focusState()->window();

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

  if (HASH.find(GIT_COMMIT_HASH) != 0) {
    HyprlandAPI::addNotification(
        PHANDLE,
        "[Hyprfoci] Failure in initialization: Version mismatch (headers ver "
        "is not equal to running hyprland ver)",
        CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
    throw std::runtime_error("[Hyprfoci] Version mismatch");
  }

  // config variables
  vars.size     = makeShared<Config::Values::CVec2Value>("plugin:hyprfoci:size",     "Size of dot", Config::VEC2{20, 20});
  vars.pos      = makeShared<Config::Values::CVec2Value>("plugin:hyprfoci:pos",      "Position offset", Config::VEC2{10, 10});
  vars.origin   = makeShared<Config::Values::CVec2Value>("plugin:hyprfoci:origin",   "Origin (0: left/top, 1: middle, 2: down/right)", Config::VEC2{0, 0});
  vars.color    = makeShared<Config::Values::CColorValue>("plugin:hyprfoci:color",   "Colour of dot", 0x8833ff11); // rgba(11ff3388)
  vars.rounding = makeShared<Config::Values::CFloatValue>("plugin:hyprfoci:rounding","Rounding of dot", 4.0f);
  vars.exclude  = makeShared<Config::Values::CStringValue>("plugin:hyprfoci:exclude","Excluded windows", "");
  vars.img      = makeShared<Config::Values::CStringValue>("plugin:hyprfoci:img",    "Image path"," none");
  vars.imgs     = makeShared<Config::Values::CStringValue>("plugin:hyprfoci:imgs",   "Image path", "none");

  HyprlandAPI::addConfigValueV2(PHANDLE, vars.size);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.pos);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.origin);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.color);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.rounding);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.exclude);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.img);
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.imgs);

  static auto closeWindowListener = Event::bus()->m_events.window.close.listen(
      [](const PHLWINDOW& window) {
          onCloseWindow(window);
      });

  static auto activeWindowListener = Event::bus()->m_events.window.active.listen(
      [](const PHLWINDOW& window, Desktop::eFocusReason reason) {
          onActiveWindow(window);
      });

  static auto configReloadListener = Event::bus()->m_events.config.reloaded.listen(
      []() {
          onConfigReload();
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
