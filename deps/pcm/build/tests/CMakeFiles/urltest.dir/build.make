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
include tests/CMakeFiles/urltest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/urltest.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/urltest.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/urltest.dir/flags.make

tests/CMakeFiles/urltest.dir/urltest.cpp.o: tests/CMakeFiles/urltest.dir/flags.make
tests/CMakeFiles/urltest.dir/urltest.cpp.o: ../tests/urltest.cpp
tests/CMakeFiles/urltest.dir/urltest.cpp.o: tests/CMakeFiles/urltest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/urltest.dir/urltest.cpp.o"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/urltest.dir/urltest.cpp.o -MF CMakeFiles/urltest.dir/urltest.cpp.o.d -o CMakeFiles/urltest.dir/urltest.cpp.o -c /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/tests/urltest.cpp

tests/CMakeFiles/urltest.dir/urltest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/urltest.dir/urltest.cpp.i"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/tests/urltest.cpp > CMakeFiles/urltest.dir/urltest.cpp.i

tests/CMakeFiles/urltest.dir/urltest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/urltest.dir/urltest.cpp.s"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/tests/urltest.cpp -o CMakeFiles/urltest.dir/urltest.cpp.s

tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o: tests/CMakeFiles/urltest.dir/flags.make
tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o: ../src/simdjson_wrapper.cpp
tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o: tests/CMakeFiles/urltest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o -MF CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o.d -o CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o -c /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/simdjson_wrapper.cpp

tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.i"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/simdjson_wrapper.cpp > CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.i

tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.s"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/src/simdjson_wrapper.cpp -o CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.s

# Object files for target urltest
urltest_OBJECTS = \
"CMakeFiles/urltest.dir/urltest.cpp.o" \
"CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o"

# External object files for target urltest
urltest_EXTERNAL_OBJECTS =

bin/tests/urltest: tests/CMakeFiles/urltest.dir/urltest.cpp.o
bin/tests/urltest: tests/CMakeFiles/urltest.dir/__/src/simdjson_wrapper.cpp.o
bin/tests/urltest: tests/CMakeFiles/urltest.dir/build.make
bin/tests/urltest: src/libpcm.a
bin/tests/urltest: tests/CMakeFiles/urltest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../bin/tests/urltest"
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/urltest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/urltest.dir/build: bin/tests/urltest
.PHONY : tests/CMakeFiles/urltest.dir/build

tests/CMakeFiles/urltest.dir/clean:
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/urltest.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/urltest.dir/clean

tests/CMakeFiles/urltest.dir/depend:
	cd /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/tests /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests /nethome/mvemmou3/caladan_sender_zsim/caladan/deps/pcm/build/tests/CMakeFiles/urltest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/urltest.dir/depend

