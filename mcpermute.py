#!/usr/bin/python

# a -> b increase in x
# a -> c increase in y
# a -> e increase in z

base = [0,1,2,3,4,5,6,7]
def RotateZ( config ):
	a,b,c,d = config[0:4]
	e,f,g,h = config[4:8]
	return [c,a,d,b, g,e,h,f]
def RotateX( config ):
	a,b = config[0:2]
	c,d = config[2:4]
	e,f = config[4:6]
	g,h = config[6:8]
	return [c,d,g,h,a,b,e,f]
def RotateY( config ):
	a,b,c,d = config[0:4]
	e,f,g,h = config[4:8]
	return [e,a,g,c,f,b,h,d]

print base
n = base
print "Rotate around Z"
for i in range(4):
	n = RotateZ( n )
	print n
print "Rotate around Y"
for i in range(4):
	n = RotateY( n )
	print n
print "Rotate around X"
for i in range(4):
	n = RotateX( n )
	print n

validConfigs = [
	[0,0,0,0,0,0,0,0], # 0
	[1,0,0,0,0,0,0,0], # 1 single corner triangle
	[1,1,0,0,0,0,0,0], # 2 single edge quad
	[1,0,0,1,0,0,0,0], # 3 corners on the same face
	[1,1,1,0,0,0,0,0], # 4 three element corner (one quad one tri)

	[1,1,1,1,0,0,0,0], # 5 full face (one quad)
	[1,1,1,0,0,0,0,1], # 6 three element corner (one quad one tri) plus opposite corner
	[1,0,0,1,0,1,1,0], # 7 four corners
	[1,1,1,0,1,0,0,0], # 8 diagonal half cut
	[1,1,0,0,1,0,1,0], # 9 clockwise screw

	[1,0,0,0,0,0,0,1], # 10 opposing corners
	[1,1,0,0,0,0,1,0], # 11 edge and opposing corner
	[1,0,0,1,0,1,0,0], # 12 three corners
	[1,1,0,0,0,0,1,1], # 13 opposing edges
	[1,1,0,0,0,1,0,1], # 14 anticlock screw
]

def GetListFromCubeID( cubeID ):
	return [1 if cubeID&(1<<x) else 0 for x in range(8)]

def FindRotations( cubeID ):
	baseCube = GetListFromCubeID( cubeID )
	for xRot in range(4):
		yrotCube = baseCube[:]
		for yRot in range(4):
			zrotCube = yrotCube[:]
			for zRot in range(4):
				for i in range(15):
					if zrotCube == validConfigs[i]:
						return i,zRot+4*yRot+16*xRot
				zrotCube = RotateZ(zrotCube)
			yrotCube = RotateY(yrotCube)
		baseCube = RotateX(baseCube)
	baseCube = GetListFromCubeID( 255-cubeID )
	for xRot in range(4):
		yrotCube = baseCube[:]
		for yRot in range(4):
			zrotCube = yrotCube[:]
			for zRot in range(4):
				for i in range(15):
					if zrotCube == validConfigs[i]:
						return i,zRot+4*yRot+16*xRot
				zrotCube = RotateZ(zrotCube)
			yrotCube = RotateY(yrotCube)
		baseCube = RotateX(baseCube)
	return -1,0
		
orientations = {}
r = {}
for cubeID in range(256):
	mesh, ori = FindRotations( cubeID )
	r[cubeID] = (mesh,ori)
	orientations[ori] = 1
	print "CUBE {} -> {}".format(cubeID,FindRotations( cubeID ))

f = open("mc.h","w")
f.write("#ifndef _MC_H_\n")
f.write("#define _MC_H_\n")
f.write("static const char mc_mesh[] = {\n")
for key, value in r.iteritems():
	f.write("{},".format( value[0] ) )
f.write("};\n")
f.write("static const char mc_orientation[] = {\n")
for key, value in r.iteritems():
	f.write("{},".format( value[1] ) )
f.write("};\n")
f.write("#endif\n")
f.close()

used = [x for x in orientations]
print len(used)
print used
