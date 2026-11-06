# MP3 Tag

MP3 Tag is a native Haiku Tracker add-on for viewing and editing ID3v1 and
ID3v2 tags in MP3 files. It works with one file at a time or applies fields and filename
patterns across a multi-file selection.

This maintained port is based on Arvid Norberg's 1999 BeOS add-on. It replaces
the unavailable `liblayout.so` dependency with Haiku's native Layout Kit,
updates file and MIME handling for current Haiku systems, uses TagLib for
modern MP3 metadata, and adds portable tests for legacy tag serialisation and
filename pattern matching.

## Features

- Read and write both modern ID3v2 tags and legacy ID3v1 tags through TagLib
- Edit title, artist, album, year, comment and genre
- Apply selected fields to several MP3 files at once
- Extract fields from filenames using `/a`, `/t`, `/n`, `/y` and `/c`
- Ignore arbitrary filename text with `/*`
- Read existing tags or create metadata when none exists
- Set successfully updated files to the current `audio/mpeg` MIME type

The default pattern `/a - /t.mp3` reads filenames such as
`Artist - Song title.mp3`.

## Building and testing

Run the portable tests:

```sh
make -C tests test
```

Build the Tracker add-on on Haiku:

```sh
pkgman install taglib_devel
make -C src
```

Install it for the current user:

```sh
make -C src install
```

The add-on is installed as `MP3 Tag` in the current user's non-packaged
Tracker add-ons directory.

## Packaging

Create an installable HPKG on Haiku:

```sh
./build-package.sh
```

The output is `dist/mp3tag-1.1.0-1-<architecture>.hpkg`.

## Version and copyright

- Version: 1.1.0
- Original author: Arvid Norberg
- Copyright status: the original source did not include a software licence;
  see `COPYRIGHT` before redistributing it
