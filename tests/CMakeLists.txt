file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests PRIVATE shapr_lib Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
