# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build

# Include any dependencies generated for this target.
include src/CMakeFiles/daemon.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/daemon.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/daemon.dir/flags.make

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o: src/CMakeFiles/daemon.dir/flags.make
src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o: ../src/daemon/daemon.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon.dir/daemon/daemon.cpp.o -c /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/daemon.cpp

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon.dir/daemon/daemon.cpp.i"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/daemon.cpp > CMakeFiles/daemon.dir/daemon/daemon.cpp.i

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon.dir/daemon/daemon.cpp.s"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/daemon.cpp -o CMakeFiles/daemon.dir/daemon/daemon.cpp.s

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.requires:

.PHONY : src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.requires

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.provides: src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/daemon.dir/build.make src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.provides.build
.PHONY : src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.provides

src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.provides.build: src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o


src/CMakeFiles/daemon.dir/daemon/main.cpp.o: src/CMakeFiles/daemon.dir/flags.make
src/CMakeFiles/daemon.dir/daemon/main.cpp.o: ../src/daemon/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/daemon.dir/daemon/main.cpp.o"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon.dir/daemon/main.cpp.o -c /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/main.cpp

src/CMakeFiles/daemon.dir/daemon/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon.dir/daemon/main.cpp.i"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/main.cpp > CMakeFiles/daemon.dir/daemon/main.cpp.i

src/CMakeFiles/daemon.dir/daemon/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon.dir/daemon/main.cpp.s"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src/daemon/main.cpp -o CMakeFiles/daemon.dir/daemon/main.cpp.s

src/CMakeFiles/daemon.dir/daemon/main.cpp.o.requires:

.PHONY : src/CMakeFiles/daemon.dir/daemon/main.cpp.o.requires

src/CMakeFiles/daemon.dir/daemon/main.cpp.o.provides: src/CMakeFiles/daemon.dir/daemon/main.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/daemon.dir/build.make src/CMakeFiles/daemon.dir/daemon/main.cpp.o.provides.build
.PHONY : src/CMakeFiles/daemon.dir/daemon/main.cpp.o.provides

src/CMakeFiles/daemon.dir/daemon/main.cpp.o.provides.build: src/CMakeFiles/daemon.dir/daemon/main.cpp.o


# Object files for target daemon
daemon_OBJECTS = \
"CMakeFiles/daemon.dir/daemon/daemon.cpp.o" \
"CMakeFiles/daemon.dir/daemon/main.cpp.o"

# External object files for target daemon
daemon_EXTERNAL_OBJECTS =

bin/pcm-daemon: src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o
bin/pcm-daemon: src/CMakeFiles/daemon.dir/daemon/main.cpp.o
bin/pcm-daemon: src/CMakeFiles/daemon.dir/build.make
bin/pcm-daemon: src/libpcm.a
bin/pcm-daemon: src/CMakeFiles/daemon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../bin/pcm-daemon"
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/daemon.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/daemon.dir/build: bin/pcm-daemon

.PHONY : src/CMakeFiles/daemon.dir/build

src/CMakeFiles/daemon.dir/requires: src/CMakeFiles/daemon.dir/daemon/daemon.cpp.o.requires
src/CMakeFiles/daemon.dir/requires: src/CMakeFiles/daemon.dir/daemon/main.cpp.o.requires

.PHONY : src/CMakeFiles/daemon.dir/requires

src/CMakeFiles/daemon.dir/clean:
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src && $(CMAKE_COMMAND) -P CMakeFiles/daemon.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/daemon.dir/clean

src/CMakeFiles/daemon.dir/depend:
	cd /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/src /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src /nethome/mvemmou3/caladan_prashant/caladan/deps/pcm/build/src/CMakeFiles/daemon.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/daemon.dir/depend

