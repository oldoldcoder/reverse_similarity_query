# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /root/heqi/encryption_algorithm/reverse_similarity_query

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/RSQ.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/RSQ.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/RSQ.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RSQ.dir/flags.make

CMakeFiles/RSQ.dir/include/hello.c.o: CMakeFiles/RSQ.dir/flags.make
CMakeFiles/RSQ.dir/include/hello.c.o: /root/heqi/encryption_algorithm/reverse_similarity_query/include/hello.c
CMakeFiles/RSQ.dir/include/hello.c.o: CMakeFiles/RSQ.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/RSQ.dir/include/hello.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/RSQ.dir/include/hello.c.o -MF CMakeFiles/RSQ.dir/include/hello.c.o.d -o CMakeFiles/RSQ.dir/include/hello.c.o -c /root/heqi/encryption_algorithm/reverse_similarity_query/include/hello.c

CMakeFiles/RSQ.dir/include/hello.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/RSQ.dir/include/hello.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/heqi/encryption_algorithm/reverse_similarity_query/include/hello.c > CMakeFiles/RSQ.dir/include/hello.c.i

CMakeFiles/RSQ.dir/include/hello.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/RSQ.dir/include/hello.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/heqi/encryption_algorithm/reverse_similarity_query/include/hello.c -o CMakeFiles/RSQ.dir/include/hello.c.s

# Object files for target RSQ
RSQ_OBJECTS = \
"CMakeFiles/RSQ.dir/include/hello.c.o"

# External object files for target RSQ
RSQ_EXTERNAL_OBJECTS =

RSQ: CMakeFiles/RSQ.dir/include/hello.c.o
RSQ: CMakeFiles/RSQ.dir/build.make
RSQ: /usr/lib/aarch64-linux-gnu/libssl.so
RSQ: /usr/lib/aarch64-linux-gnu/libcrypto.so
RSQ: CMakeFiles/RSQ.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable RSQ"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RSQ.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RSQ.dir/build: RSQ
.PHONY : CMakeFiles/RSQ.dir/build

CMakeFiles/RSQ.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RSQ.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RSQ.dir/clean

CMakeFiles/RSQ.dir/depend:
	cd /root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/heqi/encryption_algorithm/reverse_similarity_query /root/heqi/encryption_algorithm/reverse_similarity_query /root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug /root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug /root/heqi/encryption_algorithm/reverse_similarity_query/cmake-build-debug/CMakeFiles/RSQ.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/RSQ.dir/depend

