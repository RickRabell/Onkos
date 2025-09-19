#pragma once

/**
 * @file Prerequisites.h
 * @brief A precompiled header file for the engine.
 * @author Ricardo Rabell
 * @version 1.0
 * @date 2025-09-05
 *
 * This file includes commonly used headers from the Standard Library, Windows API,
 * and DirectX 11. It also defines several utility macros and data structures
 * that are used throughout the project. It is intended to be included in nearly
 * all source files to reduce compilation times and simplify dependency management.
 */

// Librerias STD
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <thread>

// Librerias DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "Resource.h"
#include "resource.h"

// Third Party Libraries

// MACROS
/**
* @def SAFE_RELEASE(x)
* @brief Safely releases a COM object pointer and sets it to nullptr.
* * Checks if the pointer `x` is not null, calls its Release() method,
* and then nullifies the pointer to prevent dangling references.
*/
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

/**
* @def MESSAGE(classObj, method, state)
* @brief Logs a debug message indicating the state of a resource creation.
* @param classObj The name of the class where the message is logged.
* @param method The name of the method where the message is logged.
* @param state A string describing the status (e.g., "SUCCESS", "FAILURE").
*/
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

/**
* @def ERROR(classObj, method, errorMSG)
* @brief Logs a formatted error message to the debug output.
* @param classObj The name of the class where the error occurred.
* @param method The name of the method where the error occurred.
* @param errorMSG A detailed message describing the error.
*/
#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    try {                                                     \
        std::wostringstream os_;                              \
        os_ << L"ERROR : " << classObj << L"::" << method     \
            << L" : " << errorMSG << L"\n";                   \
        OutputDebugStringW(os_.str().c_str());                \
    } catch (...) {                                           \
        OutputDebugStringW(L"Failed to log error message.\n");\
    }                                                         \
}

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
/**
 * @struct SimpleVertex
 * @brief Defines the vertex structure for simple geometry.
 *
 * Contains position and texture coordinates, which is a common layout
 * for textured models.
 */
struct 
SimpleVertex {
  XMFLOAT3 Pos;
  XMFLOAT2 Tex;
};

/**
 * @struct CBNeverChanges
 * @brief Constant buffer structure for data that is updated once per view.
 *
 * Typically holds the view matrix. This buffer is set once and remains
 * constant for all objects drawn from that camera's perspective.
 */
struct 
CBNeverChanges {
  XMMATRIX mView;
};

/**
 * @struct CBChangeOnResize
 * @brief Constant buffer structure for data that changes when the window is resized.
 *
 * Typically holds the projection matrix, which depends on the window's aspect ratio.
 */
struct 
CBChangeOnResize {
  XMMATRIX mProjection;
};

/**
 * @struct CBChangesEveryFrame
 * @brief Constant buffer for data that is updated for each object drawn in a frame.
 *
 * Holds per-object data like the world matrix and material color.
 */
struct 
CBChangesEveryFrame {
  XMMATRIX mWorld;
  XMFLOAT4 vMeshColor;
};

enum 
ExtensionType {
  DDS = 0,
  PNG = 1,
  JPG = 2
};