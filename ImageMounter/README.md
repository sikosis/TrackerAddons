# ImageMounter

ImageMounter is a native Haiku Tracker add-on and application for mounting
filesystem image files. It can be launched from Tracker's add-ons menu, chosen
from **Open With…**, or opened directly as a drag-and-drop target.

This maintained port is based on ImageMounter 0.3 by Maurice Michalski. It
updates the source, interface, build and packaging for current Haiku systems,
handles several selected files, mounts images read-only, and reports failures
instead of silently ignoring them.

## Supported filesystems

ImageMounter asks Haiku to identify images as BFS, HFS, ISO 9660 or FAT. Actual
support depends on the filesystem add-ons installed in the running Haiku
system. Mount points are created at the filesystem root using the image's
filename, with a number appended when the name is already in use.

## Usage

- Select one or more image files in Tracker and choose **ImageMounter** from
  the add-ons menu.
- Choose ImageMounter from Tracker's **Open With…** menu.
- Launch ImageMounter and drop image files on its window.

Mounted images are read-only to prevent accidental modification.

## Building and testing

Run the portable mount-point tests:

```sh
make -C tests test
```

Build and install on Haiku:

```sh
make -C src
make -C src install
```

The development install is placed in the current user's non-packaged Tracker
add-ons directory as `ImageMounter`.

## Packaging

Create an installable package on Haiku:

```sh
./build-package.sh
```

The output is `dist/imagemounter-1.0.0-1-<architecture>.hpkg`.

## Version and licence

- Version: 1.0.0
- Original version: 0.3
- Licence: MIT
