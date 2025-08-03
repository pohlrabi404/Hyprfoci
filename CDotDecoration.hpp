#pragma once
#include <cstdint>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprutils/math/Box.hpp>
#include <string>

class CDotDecoration : public IHyprWindowDecoration {
public:
  CDotDecoration(PHLWINDOW);
  virtual ~CDotDecoration();
  virtual SDecorationPositioningInfo getPositioningInfo();
  virtual void draw(PHLMONITOR pMonitor, float const &a);
  virtual eDecorationType getDecorationType();
  virtual eDecorationLayer getDecorationLayer();
  virtual void updateWindow(PHLWINDOW pWindow);
  virtual void damageEntire();
  virtual uint64_t getDecorationFlags();
  virtual std::string getDisplayName();
  virtual CBox getSquareBox();
  virtual void onPositioningReply(const SDecorationPositioningReply &reply);
  PHLWINDOW getOwner();
  PHLWINDOWREF m_pWindow;
};
