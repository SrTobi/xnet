cmake_minimum_required( VERSION 3.0 )

project(xnet VERSION 0.1)
set(Project_SHORTCUT "xnet")
set(Project_PREFIX "xnet")

include(GenerateExportHeader)

add_subdirectory("extras/buildx")
buildx_enable_debug()
buildx_version_dir(XNET_VERSION_DIR)
buildx_set_default_prefixes()
buildx_set_default_output_dirs()
buildx_activate_cpp11()


set(XNET_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
buildx_scan(XNET_PUBLIC_INCLUDES ${XNET_PUBLIC_INCLUDE_DIR} "hpp")
buildx_debug("public includes: ${XNET_PUBLIC_INCLUDES}" proj)

add_subdirectory("src")
add_subdirectory("samples")


buildx_install_package(	EXPORT xnetTargets
						NAMESPACE x::)