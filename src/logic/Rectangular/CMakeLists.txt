cmake_minimum_required(VERSION 3.0.0)

add_library(RectangularProductions P1.hpp P2.hpp P3.hpp P4.hpp P5.hpp P6.hpp P1.cpp P2.cpp P3.cpp P4.cpp P5.cpp P6.cpp Rectangular.hpp Rectangular.cpp)
target_link_libraries(RectangularProductions LogicUtils)
target_link_libraries(RectangularProductions Models)
target_link_libraries(RectangularProductions Interpolations)
target_link_libraries(RectangularProductions IO)
target_include_directories(RectangularProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(RectangularProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
target_include_directories(RectangularProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/logic/Rectangular")
target_include_directories(RectangularProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/logic/Interpolation")
target_include_directories(RectangularProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/io")