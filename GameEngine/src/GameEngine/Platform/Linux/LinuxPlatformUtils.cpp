#include "gepch.h"
#include "GameEngine/Utils/PlatformUtils.h"
#include "GameEngine/Core/Log.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

namespace GameEngine
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		GE_CORE_WARN("File dialog not implemented on Linux yet");
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		GE_CORE_WARN("File dialog not implemented on Linux yet");
		return std::string();
	}
}