#include "module.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

static void ModuleFileName(char *out, size_t outLen, const char *name);

static void EntryFunctionName(char *out, size_t outLen, const char *name);

bool SS4S_ModuleOpen(const char *name, SS4S_Module *module, const SS4S_LibraryContext *context) {
    if (name == NULL || name[0] == '\0') {
        return false;
    }
    char tmp[128];
    ModuleFileName(tmp, sizeof(tmp), name);
    void *lib = dlopen(tmp, RTLD_NOW);
    if (lib == NULL) {
        return false;
    }
    EntryFunctionName(tmp, sizeof(tmp), name);
    SS4S_ModuleOpenFunction *fn = (SS4S_ModuleOpenFunction *) dlsym(lib, tmp);
    if (fn == NULL) {
        context->Log(SS4S_LogLevelError, "Module", "Module `%s` is not valid!", name);
        return false;
    }
    memset(module, 0, sizeof(SS4S_Module));
    return fn(module, context);
}

bool SS4S_ModuleAvailable(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return false;
    }
    char tmp[128];
    ModuleFileName(tmp, sizeof(tmp), name);
    void *lib = dlopen(tmp, RTLD_NOW);
    if (lib == NULL) {
        return false;
    }
    dlclose(lib);
    return true;
}

static void ModuleFileName(char *out, size_t outLen, const char *name) {
    snprintf(out, outLen, "libss4s-%s.so", name);
}

static void EntryFunctionName(char *out, size_t outLen, const char *name) {
    int strLen = snprintf(out, outLen, "SS4S_ModuleOpen_%s", name);
    for (int i = 16; i < strLen; i++) {
        char ch = out[i];
        if (ch == '\0') {
            break;
        } else if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z')) {
            continue;
        } else if (ch >= 'a' && ch <= 'z') {
            out[i] = ch - 'a' + 'A';
        } else {
            out[i] = '_';
        }
    }
}