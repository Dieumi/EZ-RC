#ifndef LIBDLL_H

    #define LIBDLL_H

    #include <stdio.h>
    #include <dlfcn.h>

    void* dll_handle_open(char* path);
    void* dll_handle_lookup(void* handle, char* path);
    void dll_handle_close(void* handle);

#endif // LIBDLL_H
