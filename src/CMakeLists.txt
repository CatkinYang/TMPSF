add_library(
  tmpsf STATIC
  ${PROJECT_SOURCE_DIR}/src/fpga.cc ${PROJECT_SOURCE_DIR}/src/task.cc
  ${PROJECT_SOURCE_DIR}/src/task_dag.cc
  ${PROJECT_SOURCE_DIR}/src/task_manager.cc)
