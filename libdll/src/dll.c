#include "dll.h"

void* dll_handle_open(char* path)
{
    void *handle;

    handle = dlopen(path, RTLD_LAZY);
    if(!handle)
    {
        fprintf(stderr, "%s\n", dlerror());
    }

    dlerror();

   return handle;
}

void* dll_handle_lookup(void* handle, char* path)
{
    void* resource = dlsym(handle, path);
    char* error;

    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "Failed to load resource \"%s\" from handle %p : %s\n", path, handle, error);
    }

   return resource;
}

void dll_handle_close(void* handle)
{
    int status = dlclose(handle);

    if(status != 0)
    {
        fprintf(stderr, "Failed to unload %p : error %d occured\n", handle, status);
    }
}
