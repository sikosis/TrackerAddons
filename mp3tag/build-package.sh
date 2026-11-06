#!/bin/sh

set -eu

version="1.1.0"
revision="1"

project_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
source_directory="$project_directory/src"
test_directory="$project_directory/tests"
template="$project_directory/packaging/PackageInfo.in"
output_directory="$project_directory/dist"

if [ "$(uname -s)" != "Haiku" ]; then
	echo "Error: packages must be built on Haiku." >&2
	exit 1
fi

for command_name in copyattr getarch make package sed; do
	if ! command -v "$command_name" >/dev/null 2>&1; then
		echo "Error: required command '$command_name' was not found." >&2
		exit 1
	fi
done

architecture=$(getarch)
package_name="mp3tag-$version-$revision-$architecture.hpkg"
output="$output_directory/$package_name"

if [ -e "$output" ]; then
	echo "Error: $output already exists." >&2
	echo "Increase the package revision or version before creating another artifact." >&2
	exit 1
fi

make -C "$test_directory" clean
make -C "$test_directory" test
make -C "$source_directory" clean
make -C "$source_directory"

binary="$source_directory/mp3tag"
if [ ! -f "$binary" ]; then
	echo "Error: the expected add-on binary was not produced at $binary." >&2
	exit 1
fi

staging_directory=$(mktemp -d "${TMPDIR:-/tmp}/mp3tag-package.XXXXXX")
trap 'rm -rf -- "$staging_directory"' EXIT HUP INT TERM

mkdir -p "$staging_directory/add-ons/Tracker"
mkdir -p "$staging_directory/documentation/packages/mp3tag"
mkdir -p "$staging_directory/data/licenses"
mkdir -p "$output_directory"

copyattr --data "$binary" "$staging_directory/add-ons/Tracker/MP3 Tag"
cp "$project_directory/README.md" \
	"$staging_directory/documentation/packages/mp3tag/README.md"
cp "$project_directory/COPYRIGHT" \
	"$staging_directory/data/licenses/Copyrighted"

sed \
	-e "s/@VERSION@/$version/g" \
	-e "s/@REVISION@/$revision/g" \
	-e "s/@ARCHITECTURE@/$architecture/g" \
	"$template" > "$staging_directory/.PackageInfo"

package create -C "$staging_directory" "$output"
if [ ! -s "$output" ]; then
	echo "Error: package creation did not produce $output." >&2
	exit 1
fi

echo
echo "Created $output"
