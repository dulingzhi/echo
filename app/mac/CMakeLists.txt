# module
SET(MODULE_NAME app)

# Set Module Path
SET(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Include Direoctories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${ECHO_ROOT_PATH})

# Link Directories
LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
LINK_DIRECTORIES(${ECHO_LIB_PATH})
LINK_DIRECTORIES(${ECHO_ROOT_PATH}/thirdparty/live2d/Cubism31SdkNative-EAP5/Core/lib/macos/)

# AddFrameWork Macro
MACRO(ADD_FRAMEWORK fwname)
	SET(FRAMEWORKS "${FRAMEWORKS} -framework ${fwname}")
ENDMACRO(ADD_FRAMEWORK)

# Get all project files recursively
FILE(GLOB_RECURSE HEADER_FILES *.h *.inl)
FILE(GLOB_RECURSE SOURCE_FILES *.cpp *.m *.mm)
FILE(GLOB_RECURSE STORYBOARD_FILES *.storyboard)

SET(ALL_FILES ${HEADER_FILES} ${SOURCE_FILES} ${STORYBOARD_FILES})

# group source files
GROUP_FILES(ALL_FILES ${CMAKE_CURRENT_SOURCE_DIR})

# Mac platorm resources
SET(MAC_RESOURCE_FILES
	${MODULE_PATH}/resources/mac/App.icns
	${MODULE_PATH}/resources/data
)
SET_SOURCE_FILES_PROPERTIES(${MAC_RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
SOURCE_GROUP("Resources" FILES ${MAC_RESOURCE_FILES})

# add framework
ADD_FRAMEWORK(AppKit)
ADD_FRAMEWORK(QuartzCore)
ADD_FRAMEWORK(Metal)
ADD_FRAMEWORK(MetalKit)
ADD_FRAMEWORK(OpenAL)
ADD_FRAMEWORK(OpenCL)

SET(FRAMEWORKS "${FRAMEWORKS} -ObjC")
SET(CMAKE_EXE_LINKER_FLAGS ${FRAMEWORKS})
SET(CMAKE_OSX_ARCHITECTURES "${ARCHS_STANDARD}")

ADD_EXECUTABLE(${MODULE_NAME} MACOSX_BUNDLE ${ALL_FILES} ${MAC_RESOURCE_FILES} CMakeLists.txt)

# Link Library
TARGET_LINK_LIBRARIES(${MODULE_NAME} engine)
TARGET_LINK_LIBRARIES(${MODULE_NAME} pugixml physx spine recast lua freeimage freetype zlib box2d)
TARGET_LINK_LIBRARIES(${MODULE_NAME} glslang spirv-cross)
TARGET_LINK_LIBRARIES(${MODULE_NAME} Live2DCubismCore)

# https://help.apple.com/xcode/mac/current/#/itcaec37c2a6
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_COPY_PHASE_STRIP No)
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep")
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/Info.plist)
SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES RESOURCE ${STORYBOARD_FILES})

# Messages
MESSAGE(STATUS "Configure Mac App success!")
