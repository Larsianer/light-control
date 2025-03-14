from os.path import join, isfile

Import("env")

LIB_DIR = env["PROJECT_LIBDEPS_DIR"]
patchflag_path = join(LIB_DIR, ".patching-done")

# patch file only if we didn't do it before
if not isfile(join(LIB_DIR, ".patching-done")):
    original_file_1 = join(LIB_DIR, "d1_mini", "PubSubClient", "src", "PubSubClient.cpp")
    original_file_2 = join(LIB_DIR, "d1_mini_ota", "PubSubClient", "src", "PubSubClient.cpp")
    patched_file = join("patches", "1-yield-in-PubSub.patch")

    assert isfile(original_file_1) and isfile(original_file_2) and isfile(patched_file)

    env.Execute("patch %s %s" % (original_file_1, patched_file))
    env.Execute("patch %s %s" % (original_file_2, patched_file))
    # env.Execute("touch " + patchflag_path)


    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_path))
