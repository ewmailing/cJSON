CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(cJSON)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckFunctionExists)
#SET(CPACK_SET_DESTDIR TRUE)
#INCLUDE(CPack)

IF(APPLE)
	IF(IOS)
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" OFF)
	ELSE()
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
		CMAKE_DEPENDENT_OPTION(WANTS_BUILD_FRAMEWORK "Set to ON to build framework instead of dylib. Only valid if BUILD_SHARED_LIBRARY is ON an is OS X." ON "WANTS_BUILD_SHARED_LIBRARY" ON)
	ENDIF()
ELSE()
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ENDIF()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})


IF(WANTS_BUILD_SHARED_LIBRARY)
# RPATH stuff. This MUST be set before any targets that use it are declared.
IF("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)
	# RPATH $ORIGIN is only for Linux and Solaris
	# FIXME: match for Solaris too

	# use, i.e. don't skip the full RPATH for the build tree
	#	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

	#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	#	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


	# the RPATH to be used when installing, but only if it's not a system directory
	#	LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
	#	IF("${isSystemDir}" STREQUAL "-1")
	#   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\\$$ORIGIN")
	#	ENDIF("${isSystemDir}" STREQUAL "-1")
	SET(CMAKE_INSTALL_RPATH "\$ORIGIN")
	#    SET(CMAKE_INSTALL_RPATH "/usr/lib${CMAKE_INSTALL_RPATH}:\\\$ORIGIN")
ELSEIF(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	# use, i.e. don't skip the full RPATH for the build tree
	#	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

	#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)	
ENDIF()
ENDIF(WANTS_BUILD_SHARED_LIBRARY)

SET(CJSON_C_CFLAGS "")

IF(WANTS_BUILD_SHARED_LIBRARY)
	IF("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
		SET(CJSON_C_CFLAGS "${CJSON_C_CFLAGS} -fvisibility=hidden")
	ELSEIF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
		SET(CJSON_C_CFLAGS "${CJSON_C_CFLAGS} -fvisibility=hidden")
	ENDIF()
	
	SET(CJSON_C_CFLAGS "${CJSON_C_CFLAGS} -DBUILD_CJSON_AS_DLL")

ENDIF(WANTS_BUILD_SHARED_LIBRARY)

IF(APPLE)

	IF(WANTS_BUILD_FRAMEWORK)
#		SET(CMAKE_FRAMEWORK_INSTALL_DIR "/Library/Frameworks" CACHE STRING "Directory to install frameworks to.")
#		SET(LUA_FRAMEWORK_NAME "${LUA_LIBRARY_NAME}.framework")
#		SET(LUA_FRAMEWORK_VERSION_NUMBER "5.3")
#		SET(LUA_FRAMEWORK_VERSIONED_EXECUTABLE_DIR "Versions/${LUA_FRAMEWORK_VERSION_NUMBER}/MacOS")
#		SET(LUA_FRAMEWORK_VERSIONED_LIB_DIR "Versions/${LUA_FRAMEWORK_VERSION_NUMBER}/lib")
#		SET(LUA_FRAMEWORK_CURRENT_EXECUTABLE_DIR "Versions/Current/MacOS")

		# For Apple install_name, is it better to detect if Xcode vs Makefile?
		# Xcode default=1, Makefile=0? Or detect if Framework vs. dylib,
		# Framework=1, dylib=0?
		#OPTION(CMAKE_BUILD_WITH_INSTALL_RPATH "Set to YES to set the rpath or install_name on build instead of install." ON)
	ENDIF(WANTS_BUILD_FRAMEWORK)
ENDIF(APPLE)


SET(PUBLIC_HEADERS
	cJSON.h
)
SET(SOURCE_FILES 
	cJSON.c	
)

IF(WANTS_BUILD_SHARED_LIBRARY)
	ADD_LIBRARY(cJSON SHARED 
		${SOURCE_FILES}
		${PUBLIC_HEADERS}
)
ELSE()
	ADD_LIBRARY(cJSON STATIC 
		${SOURCE_FILES}
		${PUBLIC_HEADERS}
	)
ENDIF()

TARGET_LINK_LIBRARIES(cJSON 
	${ADDITIONAL_LIBRARIES}
)

IF(APPLE)
	INCLUDE(XcodeDefaults)
	BLURRR_CONFIGURE_XCODE_DEFAULTS(cJSON)
ENDIF()



# Set Dynamic Library and Framework properties
IF(WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)

	SET_TARGET_PROPERTIES(cJSON PROPERTIES
		FRAMEWORK TRUE
#		FRAMEWORK_VERSION "${LUA_FRAMEWORK_VERSION_NUMBER}"
		#  PRIVATE_HEADER "fooPrivate.h;fooBoth.h"
		PUBLIC_HEADER "${PUBLIC_HEADERS}"
#		RESOURCE "${RESOURCE_FILES}"
		# INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}
		# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
#		VERSION ${LUA_VERSION}
#		SOVERSION ${LUA_COMPATIBILITY_VERSION}
		COMPILE_FLAGS "${CJSON_C_CFLAGS}"
	)
	
	# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
	# SET_TARGET_PROPERTIES will fail if the variable is empty.
	#	SET_PROPERTY(TARGET lua_library_dynamic PROPERTY INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR})
	#	SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)
	# If the user deletes the install_name path, use the gcc default
	# and disable the option completely. But CMake by default places
	# the name of the library in the install_name if 
	# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
	# disable the switch.
#	IF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
		#		MESSAGE("got dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})

#	ELSE(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
		#		MESSAGE("no dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)

#	ENDIF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)



	# Short Version is the "marketing version". It is the version
	# the user sees in an information panel.
#	SET(MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.${LUA_PATCH_VERSION}")
	# Bundle version is the version the OS looks at.
#	SET(MACOSX_FRAMEWORK_BUNDLE_VERSION "${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.${LUA_PATCH_VERSION}")
	SET(MACOSX_FRAMEWORK_IDENTIFIER "org.davegamble.cjson")
	#	SET(MACOSX_BUNDLE_BUNDLE_NAME "lua" )

ELSEIF(WANTS_BUILD_SHARED_LIBRARY)
	IF(APPLE)
		SET_TARGET_PROPERTIES(cJSON PROPERTIES
			FRAMEWORK FALSE
			# INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR}
			# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
#			VERSION ${LUA_VERSION}
#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
			COMPILE_FLAGS "${CJSON_C_CFLAGS}"
		)
		# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
		# SET_TARGET_PROPERTIES will fail if the variable is empty.
#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR})
#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)
		# If the user deletes the install_name path, use the gcc default
		# and disable the option completely. But CMake by default places
		# the name of the library in the install_name if 
		# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
		# disable the switch.
#		IF(CMAKE_DYLIB_INSTALL_NAME_DIR)
#			SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})
#		ELSE(CMAKE_DYLIB_INSTALL_NAME_DIR)
#			SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH OFF)
#		ENDIF(CMAKE_DYLIB_INSTALL_NAME_DIR)



	ELSEIF(UNIX)
		IF(NOT ANDROID)
			SET_TARGET_PROPERTIES(cJSON PROPERTIES
				INSTALL_RPATH ${CMAKE_INSTALL_RPATH}
				BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
#				VERSION ${LUA_VERSION}
#				SOVERSION ${LUA_SOVERSION}
				COMPILE_FLAGS "${CJSON_C_CFLAGS}"
			)
		ENDIF(NOT ANDROID)
	
	ELSEIF(WIN32)
		SET_TARGET_PROPERTIES(cJSON PROPERTIES
#			VERSION ${LUA_VERSION}
#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
			COMPILE_FLAGS "${CJSON_C_CFLAGS}"
		)

	ELSE(APPLE)
		SET_TARGET_PROPERTIES(cJSON PROPERTIES
#			VERSION ${LUA_VERSION}
#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
			COMPILE_FLAGS "${CJSON_C_CFLAGS}"
		)

	ENDIF(APPLE)


ENDIF(WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)


# test app
# To do anything on Android requires we start from a Java app and must call into Lua via JNI.
# So in addition to the normal liblua.so, we also must build our C test program as a library.
# Then both libraries will be loaded using System.loadLibrary in our Java program.
# The Java program can then call out to our C test program, which can then start calling Lua API functions.
# TODO: We could extend this idea to test on other non-commandline GUI systems, such as iOS.
# In theory, we could carve out a common test C app. Each platform has some minimal driver that gets through the native boilerplate to call our test program.
if(ANDROID)
	# This is a hack to get around Android Studio/Gradle deficiencies.
	# Being able to build an AAR, and then using it in the same project is horribly broken.
	# After some hackery, I got the testapp target to refer to the AAR, but it only looks at the Java side.
	# The CMake/NDK side can't find the libraries.
	# So the hack is to force the NDK side to rebuild all the libraries, plus the libBIAPNativeProgram.so library.
	# But we don't want the libMyIupProgram.so built when just building the AAR, otherwise it goes into the AAR.
	# So the build.gradle is required to set -DBIAP_ANDROID_BUILD_TEST_LIBRARY=1 when building the testapp.
	IF(CJSON_ANDROID_BUILD_TEST_LIBRARY)
		FIND_LIBRARY( ANDROID_LOG_LIBRARY log )
		include_directories(${CMAKE_SOURCE_DIR}/src)
		add_library(TestAppLib SHARED EXCLUDE_FROM_ALL
			${CMAKE_SOURCE_DIR}/Android/testapp/src/main/cpp/TestAppLib.c
		)
	target_link_libraries(TestAppLib cJSON ${ANDROID_LOG_LIBRARY})
	ENDIF()
endif()




IF(NOT WANTS_BUILD_FRAMEWORK)
	INSTALL(TARGETS
		cJSON
		RUNTIME DESTINATION lib		
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include
	)
	# Hmmm...install TARGETS is not installing headers
	INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION include)
ELSE()
	INSTALL(TARGETS
		cJSON
		FRAMEWORK DESTINATION Frameworks
	)


ENDIF()

