from glob import glob

env = SConscript(
    "godot-cpp/SConstruct",
    {"custom_tools": []}
)

env.Append(CPPPATH=["include"])

sources = glob("src/*.cpp") + glob("godot/*.cpp")

library = env.SharedLibrary(
    "bin/libCPPTEMPLATE",
    source=sources
)

Default(library)
