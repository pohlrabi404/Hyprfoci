# Hyprfoci (Hyprland Focus Indicator)
A tiny Hyprland plugin that draws a single, always-visible dot in the corner of the screen whenever any window has input focus.

## Why?
I'm not a fan of border/dim/opacity in general, so having an a dot as indicator helps my case.
So the overall use cases: 
- You run a borderless or minimal border setup.
- You are blind like me and lose track of which window is currently focused.
- Subtle, non-intrusive visual cue without all the giant borders, window titles, just a dot.

## Demo
[📽️ Demo video (v0.1)](https://github.com/user-attachments/assets/dc8d3c55-17ad-4cdb-b2d8-a7f83b900121)

## Installation
### Requirement
- Hyprland v0.40.0 or newer (needs the plugin ABI).
- Meson, cmake and a C++ compiler. 

### Build from source
```bash
git clone https://github.com/pohlrabi404/Hyprfoci
cd Hyprfoci
make all
```
then add it to hyprland:
```bash
hyprctl plugin load /dir/to/hyprfoci.so
```
change load to unload to remove the plugin

## Config
Add the following to your Hyprland.conf
```hyprland.conf
plugin {
    hyprfoci {
		size = 10, 10 # width, height 
		pos = 10, 10 # distance from the top left corner
		rounding = 2.0 # usually 2.0 will make it a circle
                       # high number can make it square-er
		color = rgba(aaff0088)
		origin = 2, 1 #0: left/top, 1: middle, 2: down/right
    }
}
```

TODO
- [x] main functionalities
- [x] add config options
    - [x] size, color, position
    - [x] more position origins (top left, top middle, etc)
    - [ ] shape (maybe just svg instead)
    - [ ] option to disable indicator when there is only 1 window
    - [ ] change dot to svg
- [ ] maybe something else

## Contribute
PRs are welcome~
