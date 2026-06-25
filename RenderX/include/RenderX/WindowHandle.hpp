#pragma once

namespace RenderX {
	/**
	 * @brief Which windowing library does the window use
	 */
	enum class WindowPlatform {
		Unknown = 0,
		Win32,
		X11,
		Wayland,
		Cocoa
	};

	/**
	 * @brief Pointer to the window handle based on the windowing library
	 */
	struct WindowHandle {
		WindowPlatform platform = WindowPlatform::Unknown;

		void* window = nullptr;
		void* display = nullptr;
	};
}