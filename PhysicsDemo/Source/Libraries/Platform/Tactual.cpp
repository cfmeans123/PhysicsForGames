#include "Tactual.h"

#include "PlatformDebug.h"

#include "OS.h"

#include <strsafe.h>
#include <windowsx.h>

//#include <vector>
//#include <variant>
#include <unordered_map>

#if JM_DEBUG
#define JM_VERIFY_NOERROR(fxnCall) JM_PLATFORM_ASSERT(fxnCall == 0)
#else
#define JM_VERIFY_NOERROR(fxnCall) fxnCall
#endif

std::atomic_flag jm::singleton<jm::Tactual::System>::s_instance_flag;

namespace jm::Tactual
{

	struct Device
	{
		Device(const RAWINPUTDEVICELIST& rawDeviceInfo)
			: Name()
			, Handle(rawDeviceInfo.hDevice)
		{

			UINT nameStringSize = 0;
			JM_VERIFY_NOERROR(GetRawInputDeviceInfo(Handle, RIDI_DEVICENAME, NULL, &nameStringSize));

			Name.resize(nameStringSize);
			const UINT stringLength = GetRawInputDeviceInfo(Handle, RIDI_DEVICENAME, Name.data(), &nameStringSize);
			JM_PLATFORM_ASSERT(stringLength >= 0);

			Name = Name.substr(0, stringLength);
		}

		std::string Name;
		HANDLE Handle;
	};


	struct MouseDevice : Device
	{
		MouseDevice(const RAWINPUTDEVICELIST& rawDeviceInfo)
			:Device(rawDeviceInfo)
		{
			UINT bufferSize = 0;
			JM_VERIFY_NOERROR(GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, NULL, &bufferSize));

			std::unique_ptr<BYTE[]> dataBuffer(new BYTE[bufferSize]);
			
			GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, dataBuffer.get(), &bufferSize);
			JM_PLATFORM_ASSERT(bufferSize == sizeof(RID_DEVICE_INFO));

			RID_DEVICE_INFO deviceInfo = *(RID_DEVICE_INFO*)dataBuffer.get();
			deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
			JM_PLATFORM_ASSERT(deviceInfo.dwType == RIM_TYPEMOUSE);

			mouseInfo = deviceInfo.mouse;
		}

		RID_DEVICE_INFO_MOUSE mouseInfo;
	};


	struct KeyboardDevice : Device
	{
		KeyboardDevice(const RAWINPUTDEVICELIST& rawDeviceInfo)
			:Device(rawDeviceInfo)
		{
			UINT bufferSize = 0;
			JM_VERIFY_NOERROR(GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, NULL, &bufferSize));

			std::unique_ptr<BYTE[]> dataBuffer(new BYTE[bufferSize]);

			GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, dataBuffer.get(), &bufferSize);
			JM_PLATFORM_ASSERT(bufferSize == sizeof(RID_DEVICE_INFO));

			RID_DEVICE_INFO deviceInfo = *(RID_DEVICE_INFO*)dataBuffer.get();
			deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
			JM_PLATFORM_ASSERT(deviceInfo.dwType == RIM_TYPEKEYBOARD);

			keyboardInfo = deviceInfo.keyboard;
		}

		RID_DEVICE_INFO_KEYBOARD keyboardInfo;
	};

	struct UnknownDevice : Device
	{
		UnknownDevice(const RAWINPUTDEVICELIST& rawDeviceInfo)
			:Device(rawDeviceInfo)
		{
			UINT bufferSize = 0;
			JM_VERIFY_NOERROR(GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, NULL, &bufferSize));

			std::unique_ptr<BYTE[]> dataBuffer(new BYTE[bufferSize]);

			GetRawInputDeviceInfo(Handle, RIDI_DEVICEINFO, dataBuffer.get(), &bufferSize);
			JM_PLATFORM_ASSERT(bufferSize == sizeof(RID_DEVICE_INFO));

			RID_DEVICE_INFO deviceInfo = *(RID_DEVICE_INFO*)dataBuffer.get();
			deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
			JM_PLATFORM_ASSERT(deviceInfo.dwType == RIM_TYPEHID);

			hidInfo = deviceInfo.hid;
		}


		//vendors lookup on http://www.linux-usb.org/usb-ids.html
		//usage on https://www.freebsddiary.org/APC/usb_hid_usages.php

		RID_DEVICE_INFO_HID hidInfo;
	};





	InputHandler::InputHandler(System& state)
		:stateSystem(state)
	{

	}

	void SetKeyPressed(KeyboardState& state, USHORT key, bool pressed)
	{
		switch (key)
		{
		case VK_SPACE:
			state.SpacePressed = pressed;
			break;
		case VK_CONTROL:
			state.CtrlPressed = pressed;
			break;
		case VK_SHIFT:
			state.ShiftPressed = pressed;
			break;
		case VK_ESCAPE:
			state.EscPressed = pressed;
			break;


		case 0x41: //A key
			state.APressed = pressed;
			break;
		case 0x44: //D key
			state.DPressed = pressed;
			break;
		case 0x46: //F key
			state.FPressed = pressed;
			break;
		case 0x52: //R key
			state.RPressed = pressed;
			break;
		case 0x53: //S key
			state.SPressed = pressed;
			break;
		case 0x57: //W key
			state.WPressed = pressed;
			break;


		case VK_UP:
			state.UpPressed = pressed;
			break;
		case VK_DOWN:
			state.DownPressed = pressed;
			break;
		case VK_LEFT:
			state.LeftPressed = pressed;
			break;
		case VK_RIGHT:
			state.RightPressed = pressed;
			break;
		}
	}

	void SetKeyPressedPrefixed(KeyboardState& state, USHORT key, bool pressed)
	{
		switch (key)
		{
		case VK_UP:
			state.UpPressed = pressed;
			break;
		case VK_DOWN:
			state.DownPressed = pressed;
			break;
		case VK_LEFT:
			state.LeftPressed = pressed;
			break;
		case VK_RIGHT:
			state.RightPressed = pressed;
			break;
		}
	}

	void UpdateState(const RAWKEYBOARD& raw, KeyboardState& state)
	{
		if (raw.Flags & RI_KEY_E0)
		{
			SetKeyPressedPrefixed(state, raw.VKey, !(raw.Flags & RI_KEY_BREAK));
		}
		else
		{
			SetKeyPressed(state, raw.VKey, !(raw.Flags & RI_KEY_BREAK));
		}
	}

	void UpdateState(const MSG& systemInput, MouseState& state)
	{
		state.relative = {0, 0};
		switch (systemInput.message)
		{
		case WM_LBUTTONDOWN:
			state.LeftPressedPrevious = false;
			state.LeftPressed = true;
			break;
		case WM_LBUTTONUP:
			state.LeftPressedPrevious = true;
			state.LeftPressed = false;
			break;
		case WM_RBUTTONDOWN:
			state.RightPressedPrevious = false;
			state.RightPressed = true;
			break;
		case WM_RBUTTONUP:
			state.RightPressedPrevious = true;
			state.RightPressed = false;
			break;
		case WM_MOUSEMOVE:
			Platform::ScreenPosition intPosition{ GET_X_LPARAM(systemInput.lParam), GET_Y_LPARAM(systemInput.lParam) };
			state.relative = intPosition.offset(state.absolute);
			state.absolute = intPosition;
		}
	}

	bool InputHandler::ProcessSystemMessage(MSG& systemInput)
	{
		UpdateState(systemInput, stateSystem.GetMouse());

		switch (systemInput.message)
		{


		//raw input (mouse and keyboard)
		case WM_INPUT:
		{
			char buffer[1024];
			STRSAFE_LPSTR szTempOutput = buffer;
			UINT dwSize;
			HRAWINPUT rawInputHandle = (HRAWINPUT)systemInput.lParam;

			GetRawInputData(rawInputHandle, RID_INPUT, NULL, &dwSize,
				sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL)
			{
				return 0;
			}

			if (GetRawInputData(rawInputHandle, RID_INPUT, lpb, &dwSize,
				sizeof(RAWINPUTHEADER)) != dwSize)
			{
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
			}

			RAWINPUT* rawInput = (RAWINPUT*)lpb;

			if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
			{
				HRESULT hResult = StringCchPrintf(szTempOutput, STRSAFE_MAX_CCH, TEXT(" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n"),
					rawInput->data.keyboard.MakeCode,
					rawInput->data.keyboard.Flags,
					rawInput->data.keyboard.Reserved,
					rawInput->data.keyboard.ExtraInformation,
					rawInput->data.keyboard.Message,
					rawInput->data.keyboard.VKey);
				if (FAILED(hResult))
				{
					// TODO: write error handler
				}
				OutputDebugString(szTempOutput);


				UpdateState(rawInput->data.keyboard, stateSystem.GetKeyboard());

			}

			delete[] lpb;
			return true;
		}

		//raw input (general HID)
		//case MSG: // Private message
		//{
		//	UINT cbSize;
		//	Sleep(1000);

		//	JM_VERIFY_NOERROR(GetRawInputBuffer(NULL, &cbSize, sizeof(RAWINPUTHEADER)));
		//	cbSize *= 16;            // this is a wild guess
		//	Log(_T("Allocating %d bytes"), cbSize);
		//	PRAWINPUT pRawInput = (PRAWINPUT)malloc(cbSize);
		//	if (pRawInput == NULL)
		//	{
		//		Log(_T("Not enough memory"));
		//		return;
		//	}
		//	for (;;)
		//	{
		//		UINT cbSizeT = cbSize;
		//		UINT nInput = GetRawInputBuffer(pRawInput, &cbSizeT, /*0,
		//					  */sizeof(RAWINPUTHEADER));
		//		Log(_T("nInput = %d"), nInput);
		//		if (nInput == 0)
		//		{
		//			break;
		//		}
		//		JM_CORE_ASSERT(nInput > 0);
		//		PRAWINPUT* paRawInput = (PRAWINPUT*)malloc(sizeof(PRAWINPUT) *
		//			nInput);
		//		if (paRawInput == NULL)
		//		{
		//			Log(_T("paRawInput NULL"));
		//			break;
		//		}
		//		PRAWINPUT pri = pRawInput;
		//		for (UINT i = 0; i < nInput; ++i)
		//		{
		//			Log(_T(" input[%d] = @%p"), i, pri);
		//			paRawInput[i] = pri;
		//			pri = NEXTRAWINPUTBLOCK(pri);
		//		}
		//		// to clean the buffer
		//		DefRawInputProc(paRawInput, nInput, sizeof(RAWINPUTHEADER));

		//		free(paRawInput);
		//	}
		//	free(pRawInput);
		//}
		}

		return false;
	}

	System::System()
		: MessageHandler(*this)
		, Keyboard()
		, Mouse()
	{
		UINT numberOfDevices = 0;
		const UINT deviceListSize = sizeof(RAWINPUTDEVICELIST);

		UINT foundDevices = GetRawInputDeviceList(NULL, &numberOfDevices, deviceListSize);
		JM_PLATFORM_ASSERT(foundDevices != (UINT)-1);


		std::unique_ptr<RAWINPUTDEVICELIST[]> rawInputDeviceList(new RAWINPUTDEVICELIST[numberOfDevices]);
		JM_PLATFORM_ASSERT(rawInputDeviceList != NULL);

		foundDevices = GetRawInputDeviceList(rawInputDeviceList.get(), &numberOfDevices, deviceListSize);
		JM_PLATFORM_ASSERT(foundDevices != (UINT)-1);

		std::vector<MouseDevice> mouseDevices;
		std::vector<KeyboardDevice> keyboardDevices;
		std::vector<UnknownDevice> unknownDevices;

		bool hasMouse = false;
		bool hasKeyboard = false;
		bool hasGamepad = false;

		for (UINT deviceIndex = 0; deviceIndex < foundDevices; ++deviceIndex)
		{
			RAWINPUTDEVICELIST& currentDevice = rawInputDeviceList[deviceIndex];
			switch (currentDevice.dwType)
			{
			case RIM_TYPEMOUSE:
			{
				MouseDevice device(currentDevice);
				hasMouse = true;


				mouseDevices.push_back(device);
				break;
			}
			case RIM_TYPEKEYBOARD:
			{
				KeyboardDevice device(currentDevice);
				hasKeyboard = true;

				keyboardDevices.push_back(device);
				break;
			}
			case RIM_TYPEHID:
			{
				UnknownDevice device(currentDevice);

				if (device.hidInfo.usUsagePage == 0x1 && device.hidInfo.usUsage == 0x5)
				{
					hasGamepad = true;
				}


				unknownDevices.push_back(device);
				break;
			}
			default:
				JM_PLATFORM_LOG("Unexpected device type: %d", currentDevice.dwType);
			}
		}

		std::vector<RAWINPUTDEVICE> rawDevices;

		if (hasMouse)
		{
			RAWINPUTDEVICE deviceInfo;
			deviceInfo.usUsagePage = 0x01;
			deviceInfo.usUsage = 0x02;
			deviceInfo.dwFlags = 0;// RIDEV_NOLEGACY;//ignore legacy mouse messages
			deviceInfo.hwndTarget = 0;
			rawDevices.push_back(deviceInfo);
		}

		if (hasKeyboard)
		{
			RAWINPUTDEVICE deviceInfo;
			deviceInfo.usUsagePage = 0x01;
			deviceInfo.usUsage = 0x06;
			deviceInfo.dwFlags = RIDEV_NOLEGACY;//ignore legacy keyboard messages
			deviceInfo.hwndTarget = 0;
			rawDevices.push_back(deviceInfo);
		}

		if (hasGamepad)
		{
			RAWINPUTDEVICE deviceInfo;
			deviceInfo.usUsagePage = 0x01;
			deviceInfo.usUsage = 0x05;
			deviceInfo.dwFlags = 0;
			deviceInfo.hwndTarget = 0;
			rawDevices.push_back(deviceInfo);
		}

		if (RegisterRawInputDevices(rawDevices.data(), UINT(rawDevices.size()), sizeof(RAWINPUTDEVICE)) == FALSE)
		{
			DWORD error = GetLastError();
			error; //error on register devices
		}

	}

	System::~System()
	{

	}
}