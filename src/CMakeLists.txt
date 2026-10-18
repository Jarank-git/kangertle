# In ESP-IDF, the app itself is a "component". This registers it with the build system.
# SRCS:         every .c file to compile. Add new files here or they won't be built.
# INCLUDE_DIRS: folders searched for #include "..." headers.
# PRIV_REQUIRES: other ESP-IDF components this code uses (esp_driver_i2c = driver/i2c_master.h).
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "." "../include"
                       PRIV_REQUIRES esp_driver_i2c)
