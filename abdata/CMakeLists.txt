project (abdata CXX)

file(GLOB ABDATA_SOURCES      abdata/*.cpp abdata/*.h)

add_executable(
    abdata
    ${ABDATA_SOURCES}
)

target_precompile_headers(abdata PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/abdata/stdafx.h)
set_property(TARGET abdata PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_link_libraries(abdata abdb abscommon EASTL)
if (UNIX)
    target_link_libraries(abdata DeathHandler)
endif()

install(TARGETS abdata
    RUNTIME DESTINATION bin
    COMPONENT runtime
)
