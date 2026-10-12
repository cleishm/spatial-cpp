include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(spatial_tests
    spatial_test.cpp
)

target_link_libraries(spatial_tests PRIVATE
    spatial::spatial
    Catch2::Catch2WithMain
)

target_compile_options(spatial_tests PRIVATE -Wall)

include(CTest)
include(Catch)
catch_discover_tests(spatial_tests)

# The formatter must also build with exceptions disabled, as on embedded
# targets. Catch2 requires exceptions, so this is a standalone target. MSVC
# spells the option differently and still declares __cpp_exceptions, so the
# check is limited to compilers taking -fno-exceptions.
if(NOT MSVC)
    add_executable(spatial_no_exceptions_test no_exceptions_test.cpp)
    target_link_libraries(spatial_no_exceptions_test PRIVATE spatial::spatial)
    target_compile_options(spatial_no_exceptions_test PRIVATE -fno-exceptions)
    add_test(NAME no_exceptions COMMAND spatial_no_exceptions_test)
endif()
