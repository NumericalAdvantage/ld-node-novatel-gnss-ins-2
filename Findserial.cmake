find_path(serial_INCLUDE_DIRS serial/serial.h /usr/include 
          /usr/local/include "$ENV{NAMER_ROOT}")

find_library(serial_RELEASE_LIB serial   PATHS /usr/lib /usr/local/lib
             "$ENV{NAMER_ROOT}")

if(WIN32)
	 find_library(serial_DEBUG_LIB  seriald   PATHS /usr/lib /usr/local/lib
             "$ENV{NAMER_ROOT}")


	set(serial_LIBRARIES optimized ${serial_RELEASE_LIB} debug ${serial_DEBUG_LIB})
else()
	set(serial_LIBRARIES ${serial_RELEASE_LIB})
endif(WIN32)
set(serial_FOUND TRUE)

if (NOT serial_INCLUDE_DIRS)
  set(serial_FOUND FALSE)
endif (NOT serial_INCLUDE_DIRS)

if (NOT serial_LIBRARIES)
  set(serial_FOUND FALSE)
else (NOT serial_LIBRARIES)
  if ("${CMAKE_SYSTEM}" MATCHES "Linux")
    list(APPEND serial_LIBRARIES rt)
  endif ("${CMAKE_SYSTEM}" MATCHES "Linux")
endif (NOT serial_LIBRARIES)