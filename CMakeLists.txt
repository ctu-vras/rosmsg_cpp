cmake_minimum_required(VERSION 3.0.2)
project(rosmsg_cpp)

find_package(catkin REQUIRED COMPONENTS roslib)

find_package(PythonLibs $ENV{ROS_PYTHON_VERSION} REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)
add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${PYTHON_LIBRARIES})

add_executable(rosmsgpp src/cli.cpp)
add_dependencies(rosmsgpp ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(rosmsgpp ${catkin_LIBRARIES} ${PROJECT_NAME})
set_target_properties(rosmsgpp PROPERTIES OUTPUT_NAME "rosmsg++")

# install to global bin, too
file(MAKE_DIRECTORY "${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_BIN_DESTINATION}")
add_custom_command(
  TARGET rosmsgpp POST_BUILD VERBATIM
  COMMAND ${CMAKE_COMMAND} -E create_symlink "$<TARGET_FILE:rosmsgpp>" ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_BIN_DESTINATION}/$<TARGET_FILE_NAME:rosmsgpp>
)

install(TARGETS rosmsgpp
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(TARGETS rosmsgpp
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
)

if (CATKIN_ENABLE_TESTING)
  catkin_add_gtest(${PROJECT_NAME}-test test/test.cpp)
  target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
endif()