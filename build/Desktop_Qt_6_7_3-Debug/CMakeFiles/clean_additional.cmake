# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/pcb_production_file_generator_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/pcb_production_file_generator_autogen.dir/ParseCache.txt"
  "pcb_production_file_generator_autogen"
  )
endif()
