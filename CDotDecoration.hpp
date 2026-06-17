#pragma once
#include <hyprland/src/devices/Keyboard.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

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
  virtual void onKeypress(Event::SCallbackInfo& info, const IKeyboard::SKeyEvent& event);
  virtual std::string getHandForKeyEvent(IKeyboard::SKeyEvent event);

private:
  CHyprSignalListener m_pKeypressCallback;
  PHLWINDOWREF m_pWindow;
  SP<Render::ITexture> m_pTexture;
};
