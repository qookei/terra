project('terra', 'cpp',
	default_options: [
		'cpp_std=c++2a',
		'warning_level=3'
	])

client_sources = files(
	'src/world/light.cpp',
	'src/client/main.cpp',
	'src/client/render/chunk_view.cpp',
	'src/client/render/window.cpp')

includes = include_directories('src')
client_includes = include_directories('src/client')

# Silence volatile assignment warnings coming from GLM
if meson.get_compiler('cpp').has_argument('-Wno-volatile')
	add_project_arguments('-Wno-volatile', language: 'cpp')
endif

sdl2_dep = dependency('SDL2')
epoxy_dep = dependency('epoxy')

subproject_inc = include_directories('subprojects/', 'subprojects/glm')
subprojects_lib = static_library(
	'subprojects',
	'src/subproject_impls.cpp',
	include_directories: subproject_inc,
	override_options: ['warning_level=1'])

executable('terra-client', client_sources,
	include_directories: [subproject_inc, includes, client_includes],
	dependencies: [sdl2_dep, epoxy_dep],
	link_with: [subprojects_lib])
