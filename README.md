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
		# can be from -1 to 1, will be
			# dynamic ratio with the window instead.
			# recommend fixed size for image as dynamic can look weird
			# in some cases.
			# leave one as 0 to keep original image ratio (image only)
		# width, height
		size = 100, 0 			

		# shift down right (can be from -1 to 1 similarly)
		pos = 0, 0

		# rounding of 3~4 usually results in circle
			# depends on monitor type
			# higher and lower will make it look square-ly
			# doesn't affect image
		rounding = 4.0

		# color = rgba(FFDD33FF)

		#0: left/top, 1: middle, 2: down/right
		origin = 1, 0 

		# Absolute path is needed
		img = /path/to/your/image.png
	}
}
```

TODO
- [x] main functionalities
- [x] add config options
    - [x] size, color, position
    - [x] more position origins (top left, top middle, etc)
    - [x] support for png
    - [ ] animation
    - [ ] option to disable indicator when there is only 1 window
- [ ] maybe something else

## Contribute
PRs are welcome~
