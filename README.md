# Hyprfoci (Hyprland Focus Indicator)

A tiny Hyprland plugin that draws a single, always-visible dot in the corner of the screen whenever any window has input focus.

## Why?

I'm not a fan of border/dim/opacity in general, so having an a dot as indicator helps my case.
So the overall use cases: 
- You run a borderless or minimal border setup.
- You are blind like me and lose track of which window is currently focused.
- Subtle, non-intrusive visual cue without all the giant borders, window titles, just a dot.
- Now also support png images.

**Bongocat tho, that thing invalidated everything I said above.**

## Demo
[📽️ Demo video (v0.2.0)](https://github.com/user-attachments/assets/677dee35-c2f8-4fef-8014-55c800b47688)


## Installation
### Requirement
- Newest Hyprland.
- Meson, cmake and a C++ compiler. (usually already builtin if you use hyprland) 

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

### Typing Bongocat! [**NEW**]

Now you can have cute bongocat! Copy the bongo dir that has images in there
to your prefered location and set plugin:hyprfoci:imgs accordingly

```hyprland.conf
plugin {
	hyprfoci {
		# can be from -1 to 1, will be
			# dynamic ratio with the window instead.
			# recommend fixed size for image as dynamic can look weird
			# leave one as 0 to keep original image ratio (image only)
		# width, height
		size = 100, 0 			

		# shift down right (can be from -1 to 1, similarly)
		pos = 0, 0

		#0: left/top, 1: middle, 2: down/right
		origin = 1, 0 

        # path to a directory with both.png, left.png, right.png
		# Absolute path is needed (~ for home directory is fine)
		imgs = /path/to/your/imgs
	}
}
```

### Simple dot or png

Add the following to your Hyprland.conf for normal configuration with dot or png

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

		# Absolute path is needed (~ for home directory is fine)
		img = /path/to/your/image.png
	}
}
```


TODO
- [x] main functionalities
- [x] can now have a cute bongocat typing with you!
- [x] add config options
    - [x] size, color, position
    - [x] more position origins (top left, top middle, etc)
    - [x] support for png
    - [ ] animation
    - [ ] window title as focus indicator
    - [ ] option to disable indicator when there is only 1 window (not sure if I want it lol)
- [ ] maybe something else

## Contribute
PRs are welcome~
