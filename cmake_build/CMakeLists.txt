message("********** INSTALL D-A-S **********")
cmake_minimum_required(VERSION 3.15)
project(das VERSION 1.0)


add_custom_target(${PROJECT_NAME}
  ALL
  COMMAND bash build.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "install DAS")