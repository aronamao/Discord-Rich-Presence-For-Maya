cmake_minimum_required(VERSION 3.31)

project(DRPForMaya VERSION 1.0 DESCRIPTION "My Maya Plug-in" LANGUAGES CXX)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})


add_subdirectory(src)


set(DISCORD_SDK_ROOT "${CMAKE_SOURCE_DIR}/third_party/discord_social_sdk")
set(DISCORD_SDK_LIB_DIR "${DISCORD_SDK_ROOT}/lib/$<CONFIG>")
set(DISCORD_SDK_BIN_DIR "${DISCORD_SDK_ROOT}/bin/$<CONFIG>")
set(DISCORD_SDK_INCLUDE_DIR "${DISCORD_SDK_ROOT}/include")


# Platform-specific Social SDK library paths
if(WIN32)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_BIN_DIR}/discord_partner_sdk.dll")
elseif(APPLE)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
else() # Linux
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
endif()


# Include for Social SDK headers
target_include_directories(${PROJECT_NAME} PRIVATE ${DISCORD_SDK_INCLUDE_DIR})

# Link the Social SDK library
target_link_libraries(${PROJECT_NAME} ${DISCORD_LIB_PATH})


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/mini)


if(NOT DEFINED MODULE_LOCATION)
    set(MODULE_LOCATION "${CMAKE_SOURCE_DIR}/module")
endif()

set(MODULE_PLUGINS "${MODULE_LOCATION}/plug-ins/${MAYA_VERSION}")
set(MODULE_BIN "${MODULE_LOCATION}/bin")



install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${MODULE_PLUGINS})
install(FILES ${DISCORD_SHARED_LIB} DESTINATION ${MODULE_BIN})


# Autodesk's pluginEntry.cmake resets the standard so we force it back and see what happens
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)