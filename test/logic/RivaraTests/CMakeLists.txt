cmake_minimum_required(VERSION 3.0.0)

add_library(RivaraTest RivaraProduction0test.cpp RivaraProduction1test.cpp RivaraProduction2test.cpp RivaraProduction3test.cpp RivaraProduction4test.cpp RivaraProduction5test.cpp RivaraProduction5test.cpp)
target_link_libraries(RivaraTest RivaraProductions)
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/googletest-src/googletest/include")
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/src/logic/Rivara")
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/test/logic/RivaraTests")
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/test/logic")
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
target_include_directories(RivaraTest PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
