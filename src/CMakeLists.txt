if(NOT DEFINED DEVKIT_LOCATION)
    message(FATAL_ERROR "Devkit location not provided")
elseif(NOT DEFINED MAYA_VERSION)
    message(FATAL_ERROR "Maya version not provided")
endif()

if(NOT EXISTS "${DEVKIT_LOCATION}/${MAYA_VERSION}")
    message(FATAL_ERROR "Devkit location does not exist: ${DEVKIT_LOCATION}/${MAYA_VERSION}")
endif()


set(DEVKIT_LOCATION "${DEVKIT_LOCATION}/${MAYA_VERSION}/devkitBase")
set(ENV{DEVKIT_LOCATION} ${DEVKIT_LOCATION})

set(MAYA_DOCUMENTS_PATH $ENV{USERPROFILE}/Documents/maya/${MAYA_VERSION} CACHE STRING "Maya documents path")
string(REPLACE "\\" "/" MAYA_DOCUMENTS_PATH "${MAYA_DOCUMENTS_PATH}")

include("${DEVKIT_LOCATION}/cmake/pluginEntry.cmake")

set(LIBRARIES
        OpenMaya Foundation
)

set(SOURCE_FILES
    pluginMain.cpp
    RichPresence.cpp
    RichPresence.h
    RichPresenceCmd.h
    RichPresenceCmd.cpp
)

build_plugin()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)