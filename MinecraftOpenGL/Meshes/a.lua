-- Lua script.
p=tetview:new()
p:load_plc("F:/Spel/Programming/C++/MinecraftOpenGL/MinecraftOpenGL/Meshes/example.poly")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
