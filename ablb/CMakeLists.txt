project (ablb CXX)

file(GLOB ABLB_SOURCES      ablb/*.cpp ablb/*.h)

add_executable(
    ablb
    ${ABLB_SOURCES}
)

target_precompile_headers(ablb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ablb/stdafx.h)
set_property(TARGET ablb PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_link_libraries(ablb abscommon)
if (UNIX)
    target_link_libraries(ablb DeathHandler)
endif()

install(TARGETS ablb
    RUNTIME DESTINATION bin
    COMPONENT runtime
)
