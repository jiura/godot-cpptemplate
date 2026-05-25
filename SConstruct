from glob import glob

env = SConscript(
    "godot-cpp/SConstruct",
    {"custom_tools": []}
)

env.Append(CPPPATH=["include"])

# sources = glob("src/2d/*.cpp") + glob("godot/2d/*.cpp")
sources = glob("src/3d/*.cpp") + glob("godot/3d/*.cpp")

library = env.SharedLibrary(
    "bin/libCPPTEMPLATE",
    source=sources
)

Default(library)
