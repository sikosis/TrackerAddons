# Copy Name to Clipboard

Copy Name to Clipboard is a native Haiku Tracker add-on that copies the paths
of selected files and folders to the system clipboard.

This maintained port is based on version 1.0.1 by Werner Freytag and Humdinger.
It preserves the original shortcuts and modifier-key behaviour while updating
the source, build and packaging for current Haiku systems.

## Usage

Select one or more items in Tracker and choose **Copy Name to Clipboard** from
the add-ons menu.

- By default, complete paths are copied and separated by spaces.
- With no item selected, the current Tracker folder's path is copied.
- Hold **Shift** while choosing the add-on to copy names without paths.
- Hold **Control** to place each item on a separate line.
- Paths containing spaces are enclosed in double quotation marks.

The modifier keys are intended for mouse invocation because Tracker consumes
them when an add-on is invoked through its keyboard shortcut.

## Building and testing

Run the portable formatting tests on any system with a C++17 compiler:

```sh
make -C tests test
```

Build the native add-on on Haiku:

```sh
make -C src
```

Install an unpackaged development copy:

```sh
make -C src install
```

This installs `Copy Name to Clipboard` in the current user's non-packaged
Tracker add-ons directory. Its Option+Alt+C shortcut is defined in the add-on's
resources and does not form part of the displayed name.

## Packaging

Create an installable package on Haiku:

```sh
./build-package.sh
```

The script runs the portable tests, performs a clean native build and creates
`dist/copynametoclipboard-1.2.1-1-<architecture>.hpkg`.

## Version and licence

- Version: 1.2.1
- Original version: 1.0.1
- Licence: MIT
