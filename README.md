procgen
=======

Testbed for procedural generation ideas: allows procedural textures, meshes, and scenes. A constant work in progress.

Only tested to work on debian based linuxes (Mint and Ubuntu are common test distributions)

To see how things work, check out the procedural.cpp, and the procscene-stest.lua in the data folder. I've not even split the code into multiple files yet, so it's quite easy to read in a single web page and with search at the moment.

Plans:
----
Here's a current list of things I'm planning to do with this project.

* make a marching square and marching cube thing so that Lua can do implicit surfaces
* add loading from file in the lua so lua can load arbitrary data as part of the generation
* move the file handling, timestamp checking, and reloading all into one place to make it nice to use
* add ticking game rules things
* add spawning new obejcts
* add persistence (registering variables from C++ and lua, and being able to save/load them)
* add events, and being able to publish, invoke, and subscribe to them.
* add materials (with more slots) ready for shaders with more slots
* add shader selection
