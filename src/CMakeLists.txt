# Sample static library
add_library(checksumlib STATIC)

set_target_properties(checksumlib PROPERTIES OUTPUT_NAME "checksumcalc")

target_sources(
    checksumlib
    PRIVATE
        "IChecksumCalculator.h"
        "MD5Calculator.h"        
        
        "FileSystemComponent.h"
        "FileNode.h"
        "FileNode.cpp"
        "DirectoryNode.h"
        "DirectoryNode.cpp"
        "DirectoryTreeBuilder.h"
        "DirectoryTreeBuilder.cpp"
)


# Executable target for the application
add_executable(application)

set_target_properties(application PROPERTIES OUTPUT_NAME "calc")

target_link_libraries(
    application
    PRIVATE
        checksumlib
)
target_link_libraries(
    checksumlib
    PRIVATE
        external_hashes
)

target_include_directories(
    checksumlib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

target_sources(
    application
    PRIVATE
        "application.cpp"
)