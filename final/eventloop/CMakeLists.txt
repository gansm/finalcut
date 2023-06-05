cmake_minimum_required(VERSION 3.18)

project(eventloop)

add_executable(
    eventloop
    monitor.h
    monitor.cpp
    timer_monitor.h
    timer_monitor.cpp
    signal_monitor.h
    signal_monitor.cpp
    io_monitor.h
    io_monitor.cpp
    eventloop.h
    eventloop.cpp
    main.cpp )

#target_compile_options(eventloop PUBLIC -std=c++14 -Wall -Wextra -Wpedantic -Werror )
target_compile_options(eventloop PUBLIC -std=c++14 -Wall -Wextra -Weffc++ -pedantic -pedantic-errors -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -fstack-protector -Wstrict-aliasing -Wstrict-aliasing=3 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -fext-numeric-literals -Wreorder -Wnoexcept -Wnarrowing -Wliteral-suffix -Wctor-dtor-privacy -ftree-loop-distribute-patterns -Wmemset-transposed-args -Wno-format-nonliteral )

target_link_libraries(
    eventloop
    rt
    c )

install(TARGETS eventloop RUNTIME DESTINATION bin)
