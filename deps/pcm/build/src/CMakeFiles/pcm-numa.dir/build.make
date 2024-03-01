# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build

# Include any dependencies generated for this target.
include src/CMakeFiles/pcm-numa.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/pcm-numa.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/pcm-numa.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/pcm-numa.dir/flags.make

src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o: src/CMakeFiles/pcm-numa.dir/flags.make
src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o: ../src/pcm-numa.cpp
src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o: src/CMakeFiles/pcm-numa.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o -MF CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o.d -o CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o -c /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/pcm-numa.cpp

src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pcm-numa.dir/pcm-numa.cpp.i"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/pcm-numa.cpp > CMakeFiles/pcm-numa.dir/pcm-numa.cpp.i

src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pcm-numa.dir/pcm-numa.cpp.s"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/pcm-numa.cpp -o CMakeFiles/pcm-numa.dir/pcm-numa.cpp.s

# Object files for target pcm-numa
pcm__numa_OBJECTS = \
"CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o"

# External object files for target pcm-numa
pcm__numa_EXTERNAL_OBJECTS =

bin/pcm-numa: src/CMakeFiles/pcm-numa.dir/pcm-numa.cpp.o
bin/pcm-numa: src/CMakeFiles/pcm-numa.dir/build.make
bin/pcm-numa: src/libpcm.a
bin/pcm-numa: src/CMakeFiles/pcm-numa.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/pcm-numa"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pcm-numa.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/pcm-numa.dir/build: bin/pcm-numa
.PHONY : src/CMakeFiles/pcm-numa.dir/build

src/CMakeFiles/pcm-numa.dir/clean:
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src && $(CMAKE_COMMAND) -P CMakeFiles/pcm-numa.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/pcm-numa.dir/clean

src/CMakeFiles/pcm-numa.dir/depend:
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/src/CMakeFiles/pcm-numa.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/pcm-numa.dir/depend

