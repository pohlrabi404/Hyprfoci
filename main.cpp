#include <any>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <hyprutils/memory/UniquePtr.hpp>

#include "CDotDecoration.hpp"

inline HANDLE PHANDLE = nullptr;

static CDotDecoration *current = nullptr;

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

  HyprlandAPI::addNotification(PHANDLE, "[Hyprfoci] init successful",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {"hyprfoci", "A plugin to add a dot focus indicator", "Pohlrabi",
          "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
