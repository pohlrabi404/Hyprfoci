#include "CDotDecoration.hpp"

#include <cstdint>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/pass/RectPassElement.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
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

  // render square
  CRectPassElement::SRectData rectData;
  rectData.color = m_cSquareColor;
  rectData.box = squareBox;
  rectData.clipBox = squareBox;
  rectData.round = 10;
  rectData.roundingPower = 2.0;

  g_pHyprRenderer->m_renderPass.add(makeUnique<CRectPassElement>(rectData));
}

CBox CDotDecoration::getSquareBox() {
  const auto PWINDOW = m_pWindow.lock();

  // position at top
  double squareX = PWINDOW->m_realPosition->value().x + 10;
  double squareY = PWINDOW->m_realPosition->value().y + 10;

  CBox box = {squareX, squareY, 20, 20};

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
