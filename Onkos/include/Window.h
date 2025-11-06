#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Manages the creation and lifecycle of the application's main window.
 * @author Ricardo Rabell
 * @date 2025-09-05
 *
 * This class encapsulates the functionality related to a Win32 window,
 * including its initialization, message loop handling (via update), and destruction.
 * It holds the window handle (HWND) and its dimensions.
 */
class 
Window {
public:
	/**
	 * @brief Default constructor.
	 */
	Window() = default;
	/**
	 * @brief Default destructor.
	 */
	~Window() = default;

	/**
	 * @brief Initializes the window.
	 * * Creates and registers the window class, and then creates the window itself.
	 * @param hInstance Handle to the instance of the application.
	 * @param nCmdShow Controls how the window is to be shown.
	 * @param wndproc A pointer to the application-defined window procedure.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc);

	/**
	 * @brief Updates the window state.
	 * * This method should be called once per frame to process window messages.
	 */
	void
	update();

	/**
	 * @brief Handles window-specific rendering tasks.
	 */
	void
	render();

	/**
	 * @brief Destroys the window and unregisters its class.
	 */
	void
	destroy();

public:
	/**
	 * @brief The handle to the window. This is the primary identifier for the window in the OS.
	 */
	HWND m_hWnd = nullptr;
	/**
	 * @brief The current width of the window's client area in pixels.
	 */
	unsigned int m_width;
	/**
	 * @brief The current height of the window's client area in pixels.
	 */
	unsigned int m_height;

private:
	/**
	 * @brief The handle to the instance of the application.
	 */
	HINSTANCE m_hInst = nullptr;
	/**
	 * @brief A RECT structure that holds the client area coordinates of the window.
	 */
	RECT m_rect;
	/**
	 * @brief The name/title of the window that appears in the title bar.
	 */
	std::string m_windowName = "Onkos Engine";
};