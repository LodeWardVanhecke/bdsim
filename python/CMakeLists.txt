find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
# glob python binding sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

pybind11_add_module(aperture ${CMAKE_CURRENT_SOURCE_DIR}/aperturePybind.cc)
target_link_libraries(aperture PUBLIC ${GMAD_LIB_NAME} Python::Module)
set_property(TARGET aperture PROPERTY CXX_VISIBILITY_PRESET "default")

pybind11_add_module(array ${CMAKE_CURRENT_SOURCE_DIR}/arrayPybind.cc)
target_link_libraries(array PUBLIC ${GMAD_LIB_NAME} Python::Module)
set_property(TARGET array PROPERTY CXX_VISIBILITY_PRESET "default")

pybind11_add_module(atom ${CMAKE_CURRENT_SOURCE_DIR}/atomPybind.cc)
target_link_libraries(atom PUBLIC ${GMAD_LIB_NAME} Python::Module)
set_property(TARGET atom PROPERTY CXX_VISIBILITY_PRESET "default")

pybind11_add_module(beam ${CMAKE_CURRENT_SOURCE_DIR}/beamPybind.cc)
target_link_libraries(beam PUBLIC ${GMAD_LIB_NAME} Python::Module)
set_property(TARGET beam PROPERTY CXX_VISIBILITY_PRESET "default")

pybind11_add_module(element ${CMAKE_CURRENT_SOURCE_DIR}/elementPybind.cc)
target_link_libraries(element PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(elementtype ${CMAKE_CURRENT_SOURCE_DIR}/elementtypePybind.cc)
target_link_libraries(elementtype PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(fastlist ${CMAKE_CURRENT_SOURCE_DIR}/fastlistPybind.cc)
target_link_libraries(fastlist PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(parser ${CMAKE_CURRENT_SOURCE_DIR}/parserPybind.cc)
target_link_libraries(parser PUBLIC ${GMAD_LIB_NAME} Python::Module)

pybind11_add_module(sym_table ${CMAKE_CURRENT_SOURCE_DIR}/sym_tablePybind.cc)
target_link_libraries(sym_table PUBLIC ${GMAD_LIB_NAME} Python::Module)