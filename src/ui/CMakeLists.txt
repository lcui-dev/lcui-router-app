aux_source_directory(. DIR_UI_SRC)
add_subdirectory(components)
add_subdirectory(views)
add_library(AppUI ${DIR_UI_SRC})
