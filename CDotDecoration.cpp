#include "CDotDecoration.hpp"
#include "globals.hpp"
#include "src/render/pass/TexPassElement.hpp"

#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/pass/RectPassElement.hpp>
#include <hyprlang.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <string>

CDotDecoration::CDotDecoration(PHLWINDOW pWindow)
    : IHyprWindowDecoration(pWindow) {
  m_pWindow = pWindow;
  const auto PMONITOR = pWindow->m_monitor.lock();
}

CDotDecoration::~CDotDecoration() { damageEntire(); }

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
  if (PTEXTURE) {
    CTexPassElement::SRenderData renderData;
    renderData.box = squareBox;
    renderData.clipBox = squareBox;
    renderData.a = 1.F;
    renderData.tex = PTEXTURE;
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

  if (PTEXTURE && size_x == 0)
    size_x = size_y * PTEXTURE->m_size.x / PTEXTURE->m_size.y;
  if (size_x != 0 && size_y == 0 && PTEXTURE)
    size_y = size_x * PTEXTURE->m_size.y / PTEXTURE->m_size.x;

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
  box.translate(m_pWindow->m_floatingOffset);
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
