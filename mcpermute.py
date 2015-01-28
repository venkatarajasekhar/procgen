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
	[1,0,0,0,0,0,0,0], # 3 
]
base = [0,1,2,3,4,5,6,7]
