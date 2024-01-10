#pragma once

#include "PlatformCore.h"

namespace jm::Platform
{
	extern bool ShouldPerformActionModal(cstring project, cstring title, cstring message, cstring verb);
	extern bool ShouldInspectModal(cstring project, cstring title, cstring message);
}