#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(TrackFinder)

#******************************************************
#ROOT
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake${CMAKE_MODULE_PATH})

# Find ROOT
find_package(ROOT)

# Add FOOs header paths
include_directories(${ROOT_INCLUDE_DIR})

# Build your executable
#add_executable(MyExe MyExe.cpp)

# Link executable to FOO libraries
#target_link_libraries(SBS ${ROOT_LIBRARIES})
#*****************************************************

include_directories(${PROJECT_SOURCE_DIR}/include)
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.C)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(TrackFinder track_finder.C ${sources} ${headers})
target_link_libraries(TrackFinder ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build N03. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(TRACKFINDER_SCRIPTS
	chain.ini	 
)	

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/visTutor)

foreach(_script ${TRACKFINDER_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS TrackFinder DESTINATION bin)


