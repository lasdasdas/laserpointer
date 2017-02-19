cmake_minimum_required(VERSION 2.8.3)
project(lasertracking)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  geometry_msgs
  tf



)
find_package(OpenCV )
include_directories(${OpenCV_INCLUDE_DIRS})

generate_messages(
  DEPENDENCIES
  std_msgs
  sensor_msgs
  gps_common
  tf
)

catkin_package(

CATKIN_DEPENDS  tf  dynamic_reconfigure roscpp tf rospy geometry_msgs interactive_markers
)


include_directories(
  ${catkin_INCLUDE_DIRS}


)

add_executable(lasertracking src/lasertracking.cpp)
add_dependencies(lasertracking ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS}
)

target_link_libraries( lasertracking ${OpenCV_LIBS} )
target_link_libraries(lasertracking
  ${catkin_LIBRARIES}

)
