# Hyprfoci (Hyprland Focus Indicator)
A tiny Hyprland plugin that draws a single, always-visible dot in the corner of the screen whenever any window has input focus.

## Why?
I'm not a fan of border/dim/opacity in general, so having an a dot as indicator helps my case.
So the overall use cases: 
- You run a borderless or minimal border setup.
- You are blind like me and lose track of which window is currently focused.
- Subtle, non-intrusive visual cue without all the giant borders, window titles, just a dot.

## Installation
### Requirement
- Hyprland v0.40.0 or newer (needs the plugin ABI).
- Meson >= 0.60 and a C++ compiler. 

### Build from source
```bash
git clone https://github.com/pohlrabi404/Hyprfoci
cd Hyprfoci
meson setup build
meson compile -C build
```
then add it to hyprland:
```bash
hyprctl plugin load /dir/to/build/libhyprfoci.so
```
change load to unload to remove the plugin

TODO
- [x] main functionalities
- [ ] config
    - [ ] size
    - [ ] color
    - [ ] position
    - [ ] shape
    - [ ] option to disable indicator when there is only 1 window
- [ ] maybe something else

## Contribute
PRs are welcome~
