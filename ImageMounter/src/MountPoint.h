#ifndef MOUNT_POINT_H
#define MOUNT_POINT_H

#include <string>


std::string MountNameForImage(const std::string& path);
std::string NumberedMountPoint(const std::string& name, unsigned int number);


#endif
