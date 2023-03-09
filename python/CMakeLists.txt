find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
# glob python binding sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Object library, will be used by both static and shared library
add_library(python-binding-lib OBJECT ${GMAD_PARSER_SOURCES})

# headers
target_include_directories(python-binding-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../parser/)

# Shared libraries need PIC
set_target_properties(python-binding-lib PROPERTIES POSITION_INDEPENDENT_CODE 1)

target_link_libraries(python-binding-lib PUBLIC pybind11::module Python::Module)
