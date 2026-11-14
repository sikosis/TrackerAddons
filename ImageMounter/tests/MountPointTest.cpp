#include "MountPoint.h"

#include <cstdlib>
#include <iostream>
#include <string>


namespace {


void Expect(bool condition, const char* message)
{
	if (!condition) {
		std::cerr << "FAIL: " << message << '\n';
		std::exit(1);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


} // namespace


int main()
{
	Expect(MountNameForImage("/boot/home/Haiku.iso") == "Haiku",
		"known image extension removed");
	Expect(MountNameForImage("/boot/home/archive.tar") == "archive.tar",
		"unknown extension retained");
	Expect(MountNameForImage("/boot/home/My Disk.IMG") == "My Disk",
		"extension matching is case insensitive");
	Expect(MountNameForImage("   .iso") == "ImageMounter",
		"empty name uses fallback");
	Expect(NumberedMountPoint("Haiku", 0) == "/Haiku",
		"first mount point is unnumbered");
	Expect(NumberedMountPoint("Haiku", 3) == "/Haiku 3",
		"conflicting mount point is numbered");
	Expect(MountNameForImage(std::string(130, 'a') + ".image").size() == 120,
		"mount name is bounded");
	std::cout << "All MountPoint tests passed.\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
