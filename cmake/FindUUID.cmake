
# For other systems but native Windows
find_path (UUID_INCLUDE_DIR 
	NAMES
	uuid.h
	PATH_SUFFIXES 
	include
	uuid
	PATHS
	/usr/local/include
	/usr/include
	/opt/local/include
	/opt/include
	/usr
	)

find_library (UUID_LIBRARIES
	NAMES
	uuid
	ossp-uuid
	PATH_SUFFIXES 
	lib64 
	lib
	PATHS
	/usr/local
	/usr
	/opt/local
	/opt
	)

include (FindPackageHandleStandardArgs)
mark_as_advanced (CLEAR UUID_LIBRARIES, UUID_INCLUDE_DIR)
find_package_handle_standard_args (Uuid DEFAULT_MSG UUID_LIBRARIES UUID_INCLUDE_DIR)

