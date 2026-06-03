if(CONFIG_OUR_DRIVER)
    # Always compile the main driver
    target_sources(app PRIVATE our_driver.c)
    
    # Add shell support if shell is enabled
    if(CONFIG_SHELL)
        target_sources(app PRIVATE sensor_shell.c)
        target_sources(app PRIVATE our_driver_shell.c)
        message(STATUS "Shell support enabled for our_driver (sensor + custom API)")
    endif()
endif()
