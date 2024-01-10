#include "Modal.h"

#include "OS.h"

#include <cassert>
#include <sstream>

namespace jm::Platform
{
	bool ShouldPerformActionModal(cstring project, cstring title, cstring message, cstring verb)
	{
		std::ostringstream modalTitle;
		modalTitle << project << " - " << title;

		std::ostringstream text;
		text << message << "\n\nDo you want to " << verb << "?";

#ifdef JM_ON_WINDOWS
		UINT type = MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON1 | MB_APPLMODAL;
		INT affirmation = MessageBoxA(NULL, text.str().c_str(), title, type);
		assert((affirmation == IDYES) ^ (affirmation == IDNO));
		return (affirmation == IDYES);
#endif
	}

	bool ShouldInspectModal(cstring project, cstring title, cstring message)
	{
		return ShouldPerformActionModal(project, title, message, "inspect");
	}
}