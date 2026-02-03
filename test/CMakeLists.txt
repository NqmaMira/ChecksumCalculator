# Executable target for the unit tests
add_executable(unit-tests)

target_link_libraries(
    unit-tests
    PRIVATE
        checksumlib
        Catch2::Catch2WithMain
)

target_sources(
    unit-tests
    PRIVATE
        "test-checksum.cpp"
        "test-builder.cpp"
        "test-visitor.cpp"
        "test-memento.cpp"
        "test-report.cpp"
)

# Automatically register all tests
catch_discover_tests(unit-tests ADD_TAGS_AS_LABELS)
