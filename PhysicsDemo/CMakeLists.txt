cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMakeGenerated)
set_property(GLOBAL PROPERTY CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set_property(GLOBAL PROPERTY CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY TRUE)

project(PhysicsDemo VERSION 0.0.1)

message(CHECK_START "Checking compiler...")
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	message(CHECK_PASS "using MSVC compiler!")
	set(USING_COMPILER "${CMAKE_CXX_COMPILER_ID}")
else()
	message(CHECK_FAIL "Unsupported!")
endif()

message(CHECK_START "Checking platform...")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(CHECK_PASS "detected x64!")
	set(USING_MACHINE "x64")
else()
	message(CHECK_FAIL "Unsupported!")
endif()

set(OUTPUT_BINARY_PATH "${CMAKE_BINARY_DIR}/../PhysicsDemo_${USING_COMPILER}_${USING_MACHINE}")
set(EXT_LIB_PATH "${PROJECT_SOURCE_DIR}/Source/ExternalLibraries")
set(LIB_PATH "${PROJECT_SOURCE_DIR}/Source/Libraries")
set(EXECUTABLES_PATH "${PROJECT_SOURCE_DIR}/Source/Executables")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BINARY_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINARY_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINARY_PATH})

#=======================#==EXTERNAL LIBS=====#=======================
#=======================eigen


set(EIGEN_MODULE_DIR "${EXT_LIB_PATH}/eigen")
file(GLOB_RECURSE EIGEN_MODULE_FILE_GLOB ${EIGEN_MODULE_DIR}/*.h ${EIGEN_MODULE_DIR}/*.cpp  ${EIGEN_MODULE_DIR}/*.natvis)
file(GLOB_RECURSE EIGEN_MODULE_FILE_GLOB ${EIGEN_MODULE_DIR}/Eigen/*)

add_library(eigen INTERFACE ${EIGEN_MODULE_FILE_GLOB})
target_include_directories(eigen INTERFACE "${EIGEN_MODULE_DIR}/Eigen")
target_compile_features(eigen INTERFACE cxx_std_20)
target_compile_options(eigen INTERFACE $<IF:$<CXX_COMPILER_ID:MSVC>, /W4 /WX /wd4100 /wd4244 /wd4267 /wd4456 /wd4459 /wd4996 /wd5054,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${EIGEN_MODULE_DIR}" FILES ${EIGEN_MODULE_FILE_GLOB})
set_target_properties(eigen PROPERTIES
	FOLDER "ExternalLibraries"
	LINKER_LANGUAGE CXX
)

#=======================glm


set(GLM_MODULE_DIR "${EXT_LIB_PATH}/glm")
file(GLOB_RECURSE GLM_MODULE_FILE_GLOB ${GLM_MODULE_DIR}/*.hpp ${GLM_MODULE_DIR}/*.cpp ${GLM_MODULE_DIR}/*.inl  ${GLM_MODULE_DIR}/*.natvis)

add_library(glm ${GLM_MODULE_FILE_GLOB})
target_include_directories(glm PUBLIC "${GLM_MODULE_DIR}/..")
target_compile_features(glm PUBLIC cxx_std_20)
target_compile_options(glm PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>, /W4 /WX /wd4201, -Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${GLM_MODULE_DIR}" FILES ${GLM_MODULE_FILE_GLOB})
set_target_properties(glm PROPERTIES
	FOLDER "ExternalLibraries"
)

#=======================ImGui

set(IMGUI_MODULE_DIR "${EXT_LIB_PATH}/DearImGui")
set( ImGuiSourceList
"${IMGUI_MODULE_DIR}/imgui.cpp"
"${IMGUI_MODULE_DIR}/imgui.h"
"${IMGUI_MODULE_DIR}/imgui_demo.cpp"
"${IMGUI_MODULE_DIR}/imgui_draw.cpp"
"${IMGUI_MODULE_DIR}/imgui_internal.h"
"${IMGUI_MODULE_DIR}/imgui_tables.cpp"
"${IMGUI_MODULE_DIR}/imgui_widgets.cpp"
"${IMGUI_MODULE_DIR}/imstb_rectpack.h"
"${IMGUI_MODULE_DIR}/imstb_textedit.h"
"${IMGUI_MODULE_DIR}/imstb_truetype.h"
"${IMGUI_MODULE_DIR}/backends/imgui_impl_win32.cpp"
"${IMGUI_MODULE_DIR}/backends/imgui_impl_win32.h"
"${IMGUI_MODULE_DIR}/backends/imgui_impl_opengl3.cpp"
"${IMGUI_MODULE_DIR}/backends/imgui_impl_opengl3.h"
"${IMGUI_MODULE_DIR}/backends/imgui_impl_opengl3_loader.h"
)

add_library(ImGui ${ImGuiSourceList})
target_include_directories(ImGui PUBLIC "${IMGUI_MODULE_DIR}")
source_group(TREE "${IMGUI_MODULE_DIR}" FILES ${ImGuiSourceList})
target_include_directories(ImGui PRIVATE "${IMGUI_MODULE_DIR}")
set_target_properties(ImGui PROPERTIES
	FOLDER "ExternalLibraries"
)

#=======================EnTT

set(ENTT_MODULE_DIR "${EXT_LIB_PATH}/entt/src/entt")
file(GLOB_RECURSE ENTT_MODULE_FILE_GLOB ${ENTT_MODULE_DIR}/*.hpp ${ENTT_MODULE_DIR}/*.cpp ${ENTT_MODULE_DIR}/*.inl)

source_group(TREE "${ENTT_MODULE_DIR}" FILES ${ENTT_MODULE_FILE_GLOB})
add_library(entt INTERFACE ${ENTT_MODULE_FILE_GLOB})
set_target_properties(entt PROPERTIES
	FOLDER "ExternalLibraries"
)

add_subdirectory("Source/ExternalLibraries/entt")

#=======================#=====LIBRARIES======#=======================
#=======================Platform

set(PLATFORM_MODULE_DIR "${LIB_PATH}/Platform")
set( PlatformSourceList
"${PLATFORM_MODULE_DIR}/Application.cpp"
"${PLATFORM_MODULE_DIR}/Application.h"
"${PLATFORM_MODULE_DIR}/Debugger.cpp"
"${PLATFORM_MODULE_DIR}/Debugger.h"
"${PLATFORM_MODULE_DIR}/Modal.cpp"
"${PLATFORM_MODULE_DIR}/Modal.h"
"${PLATFORM_MODULE_DIR}/OS.h"
"${PLATFORM_MODULE_DIR}/OSWindow.cpp"
"${PLATFORM_MODULE_DIR}/OSWindow.h"
"${PLATFORM_MODULE_DIR}/PlatformCore.h"
"${PLATFORM_MODULE_DIR}/PlatformDebug.h"
"${PLATFORM_MODULE_DIR}/Singleton.h"
"${PLATFORM_MODULE_DIR}/Tactual.cpp"
"${PLATFORM_MODULE_DIR}/Tactual.h"
"${PLATFORM_MODULE_DIR}/Timer.cpp"
"${PLATFORM_MODULE_DIR}/Timer.h"
"${PLATFORM_MODULE_DIR}/Window.cpp"
"${PLATFORM_MODULE_DIR}/Window.h"
"${PLATFORM_MODULE_DIR}/WindowedApplication.cpp"
"${PLATFORM_MODULE_DIR}/WindowedApplication.h"
)

add_library(Platform ${PlatformSourceList})
target_include_directories(Platform PUBLIC "${PLATFORM_MODULE_DIR}")
target_compile_features(Platform PUBLIC cxx_std_20)
target_compile_options(Platform PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${PLATFORM_MODULE_DIR}" FILES ${PlatformSourceList})
set_target_properties(Platform PROPERTIES
	FOLDER "Libraries"
)

#=======================Math

set(MATH_MODULE_DIR "${LIB_PATH}/Math")
set( MathSourceList
"${MATH_MODULE_DIR}/MathTypes.h"
"${MATH_MODULE_DIR}/MathDebug.h"
"${MATH_MODULE_DIR}/Graph.h"
"${MATH_MODULE_DIR}/Camera.h"
"${MATH_MODULE_DIR}/Random.cpp"
)

add_library(Math ${MathSourceList})
target_include_directories(Math PUBLIC "${MATH_MODULE_DIR}" "${LIB_PATH}" "${EIGEN_MODULE_DIR}/..")
target_compile_features(Math PUBLIC cxx_std_20)
target_compile_options(Math PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>, /W4 /WX,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${MATH_MODULE_DIR}" FILES ${MathSourceList})
set_target_properties(Math PROPERTIES
	FOLDER "Libraries"
)
target_link_libraries(Math PUBLIC Platform glm)

#=======================Visual

set(VISUAL_MODULE_DIR "${LIB_PATH}/Visual")
set( VisualSourceList
"${VISUAL_MODULE_DIR}/DearImGui/ImGuiContext.cpp"
"${VISUAL_MODULE_DIR}/DearImGui/ImGuiContext.h"
"${VISUAL_MODULE_DIR}/OpenGL/KHR/khrplatform.h"
"${VISUAL_MODULE_DIR}/OpenGL/KHR/wglext.h"
"${VISUAL_MODULE_DIR}/OpenGL/KHR/glext.h"
"${VISUAL_MODULE_DIR}/OpenGL/OpenGL.cpp"
"${VISUAL_MODULE_DIR}/OpenGL/OpenGL.h"
"${VISUAL_MODULE_DIR}/OpenGL/FunctionBindings.h"
"${VISUAL_MODULE_DIR}/MeshData.h"
"${VISUAL_MODULE_DIR}/RenderingContext.cpp"
"${VISUAL_MODULE_DIR}/RenderingContext.h"
"${VISUAL_MODULE_DIR}/Visual.cpp"
"${VISUAL_MODULE_DIR}/VisualGeometry.h"
"${VISUAL_MODULE_DIR}/VisualGeometry.cpp"
"${VISUAL_MODULE_DIR}/Visual.h"
"${VISUAL_MODULE_DIR}/VisualDebug.h"
"${VISUAL_MODULE_DIR}/MeshData.cpp"
)


add_library(Visual ${VisualSourceList})
target_include_directories(Visual 
	PRIVATE "${EXT_LIB_PATH}" "${VISUAL_MODULE_DIR}"
	PUBLIC "${LIB_PATH}"
)
target_compile_features(Visual PUBLIC cxx_std_20)
target_compile_options(Visual PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${VISUAL_MODULE_DIR}" FILES ${VisualSourceList})
set_target_properties(Visual PROPERTIES
	FOLDER "Libraries"
)
target_link_libraries(Visual PUBLIC Math Platform ImGui)

#=======================Systems

set(SYSTEMS_MODULE_DIR "${LIB_PATH}/Systems")
set( SystemsSourceList
"${SYSTEMS_MODULE_DIR}/Graphics.cpp"
"${SYSTEMS_MODULE_DIR}/Graphics.h"
"${SYSTEMS_MODULE_DIR}/Entity.cpp"
"${SYSTEMS_MODULE_DIR}/Entity.h"
)

add_library(Systems ${SystemsSourceList})
target_include_directories(Systems PUBLIC "${SYSTEMS_MODULE_DIR}")
target_compile_features(Systems PUBLIC cxx_std_20)
target_compile_options(Systems PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>, /W4 /WX,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${SYSTEMS_MODULE_DIR}" FILES ${SystemsSourceList})
set_target_properties(Systems PROPERTIES
	FOLDER "Libraries"
)
target_link_libraries(Systems PUBLIC Math Platform Visual entt)

#=======================#====EXECUTABLES=====#=======================
#=======================PhysicsDemo

set(PHYSICSDEMO_MODULE_DIR "${EXECUTABLES_PATH}/PhysicsDemo")
set( PhysicsDemoSourceList
	"${PHYSICSDEMO_MODULE_DIR}/PhysicsDemo.cpp"
)

add_executable(PhysicsDemo ${PhysicsDemoSourceList})
target_include_directories(PhysicsDemo PRIVATE "${PHYSICSDEMO_MODULE_DIR}")
target_compile_features(PhysicsDemo PUBLIC cxx_std_20)
target_compile_options(PhysicsDemo PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/W4 /WX,-Wall -Wextra -Wpedantic -Werror>)
source_group(TREE "${PHYSICSDEMO_MODULE_DIR}" FILES ${PhysicsDemoSourceList})
set_target_properties(PhysicsDemo PROPERTIES
	FOLDER "Executables"
)
target_link_libraries(PhysicsDemo
	PRIVATE Platform Math Visual Systems
)

if ( MSVC )
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PhysicsDemo)
endif ()
