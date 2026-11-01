# Tracker Add-ons

Native Tracker add-ons for [Haiku](https://www.haiku-os.org/), maintained and
packaged for current Haiku systems.

## Add-ons

| Add-on | Description | Version |
| --- | --- | ---: |
| [CopyNameToClipboard](CopyNameToClipboard/) | Copies the paths or names of selected Tracker items to the system clipboard. | 1.2.0 |

## Building from source

Builds must be performed on Haiku with the development tools installed. Each
maintained add-on has its own build, test and packaging instructions. For
example:

```sh
make -C CopyNameToClipboard/tests test
make -C CopyNameToClipboard/src
```

Create an installable HPKG with the project-owned release script:

```sh
CopyNameToClipboard/build-package.sh
```

## Repository layout

- `CopyNameToClipboard` contains the maintained add-on.
- `CopyNameToClipboard-Original` preserves the 2012 source for reference and
  is not a separate release.

## Licences

Licensing and attribution vary by project. Consult the README and licence file
inside each maintained add-on before redistributing it.
