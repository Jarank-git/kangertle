# In ESP-IDF, the app itself is a "component". This registers it with the build system.
# SRCS:         every .c file to compile. Add new files here or they won't be built.
# INCLUDE_DIRS: folders searched for #include "..." headers.
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "." "../include")
