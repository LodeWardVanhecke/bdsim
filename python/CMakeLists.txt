find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
# glob python binding sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

pybind11_add_module(element ${CMAKE_CURRENT_SOURCE_DIR}/elementPybind.cc)
target_link_libraries(element PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(elementtype ${CMAKE_CURRENT_SOURCE_DIR}/elementtypePybind.cc)
target_link_libraries(elementtype PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(fastlist ${CMAKE_CURRENT_SOURCE_DIR}/fastlistPybind.cc)
target_link_libraries(fastlist PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(parser ${CMAKE_CURRENT_SOURCE_DIR}/parserPybind.cc)
target_link_libraries(parser PUBLIC ${GMAD_LIB_NAME} Python::Module)
