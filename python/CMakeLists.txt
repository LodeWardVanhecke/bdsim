find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
# glob python binding sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

list(APPEND pythonBindingSrcs
        aperture array atom beam blmplacement cavitymodel
        crystal element fastlist field material modulator
        newcolour options parameters parser physicsbiasing placement
        query region samplerplacement scorer scorermesh
        sym_table tunnel)

list(APPEND rootBindingSrcs
        bdsimclass bdsexecoptions bdsparser)

foreach(pythonBindingSrc IN ZIP_LISTS pythonBindingSrcs)
    MESSAGE(STATUS ${pythonBindingSrc_0})
    pybind11_add_module(${pythonBindingSrc_0} ${CMAKE_CURRENT_SOURCE_DIR}/${pythonBindingSrc_0}Pybind.cc)
    target_link_libraries(${pythonBindingSrc_0} PUBLIC ${GMAD_LIB_NAME} Python::Module)
    set_property(TARGET ${pythonBindingSrc_0} PROPERTY CXX_VISIBILITY_PRESET "default")
endforeach()

foreach(rootBindingSrc IN ZIP_LISTS rootBindingSrcs)
    pybind11_add_module(${rootBindingSrc_0} ${CMAKE_CURRENT_SOURCE_DIR}/${rootBindingSrc_0}Pybind.cc)
    target_link_libraries(${rootBindingSrc_0} PUBLIC ${GMAD_LIB_NAME} ${BDSIM_LIB_NAME} Python::Module)
    target_include_directories(${rootBindingSrc_0} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
endforeach()
