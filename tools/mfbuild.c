#include <mf_lib.h>
MF_LIB_LINUX("m")
#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_file.h>
#include <mf_string.h>

#define DEFAULT_OUT "./a.out"

typedef struct SourceDef {
    const char *cc;
    const char *source;
} SourceDef;

SourceDef defaultSources[] = {
    {"gcc", "main.c"},
    {"gcc", "src/main.c"},
};

void compile(SourceDef *def) {
    MF_String cmd = MF_StringNewFormat("%s %s", def->cc, def->source);
    system(cmd);
    printf("Compiled %s\n", DEFAULT_OUT);
}

void run(SourceDef *def) {
    system(DEFAULT_OUT);
}

int main(int argc, char **argv) {
    const char *action = NULL;
    SourceDef *def = NULL;

    if (argc > 1) {
        action = argv[1];
    } else {
        action = "run";
    }

    if (argc > 2) {
        def = MF_MallocStruct(SourceDef);
        def->source = argv[2];
        def->cc = "gcc";
    } else {
        MF_ArrayFor(defaultSources) {
            if (MFF_IsFile(it->source)) {
                def = it;
                break;
            }
        }
    }

    if (strcmp(action, "build") == 0) {
        compile(def);
    } else if (strcmp(action, "run") == 0) {
        if (MFF_IsFile(DEFAULT_OUT)) {
            u64 binTime = MFF_FileGetWriteTime(DEFAULT_OUT);
            u64 srcTime = MFF_FileGetWriteTime(def->source);
            if (srcTime > binTime) {
                compile(def);
            }
        }
        run(def);
    } else {
        fprintf(stderr, "Error: unknown action %s\n", action);
    }
}
