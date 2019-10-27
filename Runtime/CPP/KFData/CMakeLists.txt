CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT (KFData)

message("==================KFData===================")
include_directories(./)


#如果想增加一个子模块，只需要在这里增加名字即可，后面的代码都不要改动
set(ModuleList 
	Format 
	Utils)

message("${ModuleList}")

foreach(Module ${ModuleList})
	message(-)
	message(Module:${Module})

	file(GLOB_RECURSE ${Module}_HPP ${Module}/*.h)
	file(GLOB_RECURSE ${Module}_CPP ${Module}/*.cpp)
	#aux_source_directory(Private/${Module} ${Module}_CPP)
	list(APPEND AllModuleFiles ${${Module}_HPP} ${${Module}_CPP})

	message(HPP:${${Module}_HPP})
	message(CPP:${${Module}_CPP})

	file(GLOB Group_HPP ${Module}/*.h)
	file(GLOB Group_CPP ${Module}/*.cpp)
	source_group("${Module}" FILES ${Group_CPP} ${Group_HPP})

	foreach(GroupName ${${Module}_Groups})
		message(Group:${GroupName})
		file(GLOB Group_HPP ${GroupName}/*.h)
		file(GLOB Group_CPP ${GroupName}/*.cpp)

		source_group("${GroupName}" FILES ${Group_CPP} ${Group_HPP})
	endforeach()

endforeach()

file(GLOB Project_HPP ./*.h)
file(GLOB Project_CPP ./*.cpp)
list(APPEND AllModuleFiles ${Project_HPP} ${Project_CPP})

add_library(KFData STATIC ${AllModuleFiles})
