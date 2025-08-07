#include "CDotDecoration.hpp"
#include "globals.hpp"

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/pass/RectPassElement.hpp>
#include <hyprland/src/render/pass/TexPassElement.hpp>
#include <hyprlang.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>

CDotDecoration::CDotDecoration(PHLWINDOW pWindow)
    : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
  if (g_pTextures["both.png"]) {
    m_pTexture = g_pTextures["both.png"];
    m_pKeypressCallback = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "keyPress",
        [&](void *self, SCallbackInfo &info, std::any data) {
          onKeypress(info, data);
        });
  } else if (g_pTexture) {
    m_pTexture = g_pTexture;
  }

  const auto PMONITOR = pWindow->m_monitor.lock();
}

void CDotDecoration::onKeypress(SCallbackInfo &info, std::any data) {
  auto const keyEvent =
      std::any_cast<std::unordered_map<std::string, std::any>>(data);
  auto const event = std::any_cast<IKeyboard::SKeyEvent>(keyEvent.at("event"));
  auto const hand = getHandForKeyEvent(event);

  std::string textureName;
  if (event.state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    textureName = hand;
  } else {
    textureName = "both.png";
  }

  const auto PMONITOR = m_pWindow->m_monitor.lock();
  damageEntire();
  m_pTexture = g_pTextures[textureName];
}

std::string CDotDecoration::getHandForKeyEvent(IKeyboard::SKeyEvent event) {
  uint32_t keycode = event.keycode;

  // Left hand keys on a standard QWERTY keyboard
  const std::vector<uint32_t> leftHandKeys = {// Left side of number row
                                              KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,

                                              // Left side of top row
                                              KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,

                                              // Left side of home row
                                              KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,

                                              // Left side of bottom row
                                              KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,

                                              // Left modifiers and special keys
                                              KEY_ESC, KEY_TAB, KEY_CAPSLOCK,
                                              KEY_LEFTSHIFT, KEY_LEFTCTRL,
                                              KEY_LEFTALT, KEY_LEFTMETA};

  // Check if keycode is for a left hand key
  if (std::find(leftHandKeys.begin(), leftHandKeys.end(), keycode) !=
      leftHandKeys.end()) {
    return "left.png"; // Left hand
  } else {
    return "right.png"; // Everything else is right hand?
  }
}

CDotDecoration::~CDotDecoration() {
  m_pTexture.reset();
  m_pKeypressCallback.reset();
  damageEntire();
}

SDecorationPositioningInfo CDotDecoration::getPositioningInfo() {
  SDecorationPositioningInfo info;
  info.policy = DECORATION_POSITION_ABSOLUTE;
  info.edges = DECORATION_EDGE_BOTTOM;
  info.priority = 10000;
  return info;
}

void CDotDecoration::draw(PHLMONITOR pMonitor, float const &a) {
  if (!validMapped(m_pWindow)) {
    return;
  }
  const auto PWINDOW = m_pWindow.lock();
  if (!PWINDOW->m_windowData.decorate.valueOrDefault()) {
    return;
  }

  CBox squareBox = getSquareBox();
  squareBox.translate(pMonitor->m_position * -1)
      .scale(pMonitor->m_scale)
      .round();
  if (squareBox.width < 1 || squareBox.height < 1) {
    return;
  }

  static auto *const PSIZE =
      (Hyprlang::VEC2 *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:size")
          ->getDataStaticPtr();
  static auto *const PCOLOR =
      (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:color")
          ->getDataStaticPtr();
  static auto *const PROUND =
      (Hyprlang::FLOAT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:rounding")
          ->getDataStaticPtr();

  // handle float size case
  float size_x = (**PSIZE).x;
  if (size_x < 1) {
    size_x = PWINDOW->m_size.x * size_x;
  }
  float size_y = (**PSIZE).y;
  if (size_y < 1) {
    size_y = PWINDOW->m_size.y * size_y;
  }

  // render data
  if (m_pTexture) {
    CTexPassElement::SRenderData renderData;
    renderData.box = squareBox;
    renderData.clipBox = squareBox;
    renderData.a = 1.F;
    renderData.tex = m_pTexture;
    g_pHyprRenderer->m_renderPass.add(makeUnique<CTexPassElement>(renderData));
  } else {
    CRectPassElement::SRectData rectData;
    rectData.color = CHyprColor(**PCOLOR);
    rectData.box = squareBox;
    rectData.clipBox = squareBox;
    rectData.round = std::min(size_x, size_y);
    rectData.roundingPower = **PROUND;
    g_pHyprRenderer->m_renderPass.add(makeUnique<CRectPassElement>(rectData));
  }
}

CBox CDotDecoration::getSquareBox() {
  const auto PWINDOW = m_pWindow.lock();

  static auto *const PSIZE =
      (Hyprlang::VEC2 *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:size")
          ->getDataStaticPtr();
  static auto *const PPOS =
      (Hyprlang::VEC2 *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:pos")
          ->getDataStaticPtr();

  // handle float size case
  float size_x = (**PSIZE).x;
  if ((**PSIZE).x < 1) {
    size_x = PWINDOW->m_realSize->value().x * (**PSIZE).x;
  }
  float size_y = (**PSIZE).y;
  if ((**PSIZE).y < 1) {
    size_y = PWINDOW->m_realSize->value().y * (**PSIZE).y;
  }
  float pos_x = (**PPOS).x;
  if (pos_x < 1 && pos_x > -1) {
    pos_x = PWINDOW->m_realSize->value().x * pos_x;
  }
  float pos_y = (**PPOS).y;
  if (pos_y < 1 && pos_y > -1) {
    pos_y = PWINDOW->m_realSize->value().y * pos_y;
  }

  if (m_pTexture && size_x == 0)
    size_x = size_y * m_pTexture->m_size.x / m_pTexture->m_size.y;
  if (size_x != 0 && size_y == 0 && m_pTexture)
    size_y = size_x * m_pTexture->m_size.y / m_pTexture->m_size.x;

  static auto *const PORIGIN =
      (Hyprlang::VEC2 *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:hyprfoci:origin")
          ->getDataStaticPtr();

  double originX =
      PWINDOW->m_realPosition->value().x +
      (**PORIGIN).x * (PWINDOW->m_realSize->value().x - size_x) / 2;
  double originY =
      PWINDOW->m_realPosition->value().y +
      ((**PORIGIN).y * (PWINDOW->m_realSize->value().y - size_y) / 2);

  // position at top
  double squareX = originX + pos_x;
  double squareY = originY + pos_y;

  CBox box = {squareX, squareY, size_x, size_y};

  // handle workspace offset
  const auto PWORKSPACE = PWINDOW->m_workspace;
  const auto WORKSPACEOFFSET = PWORKSPACE && !PWINDOW->m_pinned
                                   ? PWORKSPACE->m_renderOffset->value()
                                   : Vector2D();
  return box.translate(WORKSPACEOFFSET);
}

eDecorationType CDotDecoration::getDecorationType() {
  return DECORATION_CUSTOM;
}

void CDotDecoration::updateWindow(PHLWINDOW pWindow) { damageEntire(); }

void CDotDecoration::damageEntire() {
  auto box = getSquareBox();
  g_pHyprRenderer->damageBox(box);
}

eDecorationLayer CDotDecoration::getDecorationLayer() {
  return DECORATION_LAYER_OVER;
}

uint64_t CDotDecoration::getDecorationFlags() {
  return DECORATION_PART_OF_MAIN_WINDOW;
}

std::string CDotDecoration::getDisplayName() { return "Simple Dot"; }

PHLWINDOW CDotDecoration::getOwner() { return m_pWindow.lock(); }

void CDotDecoration::onPositioningReply(
    const SDecorationPositioningReply &reply) {
  return;
}
