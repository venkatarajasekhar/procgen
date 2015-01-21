sceneNodePos = { }
sceneNodeRotation = { }
sceneNodeScale = { }
sceneNodeTexture = { }
sceneNodeOrientation = { }
sceneNodeMesh = { "end" }

function GetSceneNodeCount() 
	count = 0
	while sceneNodeMesh[count+1] do
		if sceneNodeMesh[count+1] == "end" then
			return count;
		end
		count = count + 1
	end
	return count
end
function GetSceneNodePos( n ) 
	x = sceneNodePos[n+1][1]
	y = sceneNodePos[n+1][2]
	z = sceneNodePos[n+1][3]
	return x,y,z
end
function GetSceneNodeRotation( n )
	x = sceneNodeRotation[n+1][1]
	y = sceneNodeRotation[n+1][2]
	z = sceneNodeRotation[n+1][3]
	return x,y,z
end
function GetSceneNodeScale( n )
	x = sceneNodeScale[n+1][1]
	y = sceneNodeScale[n+1][2]
	z = sceneNodeScale[n+1][3]
	return x,y,z
end
function GetSceneNodeMesh( n ) return sceneNodeMesh[n+1] end
function GetSceneNodeTexture( n ) return sceneNodeTexture[n+1] end
function GetSceneNodeOrientation( n ) return sceneNodeOrientation[n+1] end

function AddNode( mesh, texture, pos, rot, scale )
	current = GetSceneNodeCount() + 1
	-- Log( "Adding node at " .. current )
	sceneNodePos[current] = pos
	sceneNodeRotation[current] = rot
	sceneNodeScale[current] = scale
	sceneNodeTexture[current] = texture
	sceneNodeMesh[current] = mesh
end

function AddAnimNode( mesh, texture, orientationFunction )
	current = GetSceneNodeCount() + 1
	-- Log( "Adding node at " .. current )
	sceneNodePos[current] = {0,0,0}
	sceneNodeRotation[current] = {0,0,0}
	sceneNodeScale[current] = {1,1,1}
	sceneNodeTexture[current] = texture
	sceneNodeMesh[current] = mesh
	sceneNodeOrientation[current] = orientationFunction
end

function AddScene( scene, pos, rot, scale )
	current = GetSceneNodeCount() + 1
	sceneNodePos[current] = pos
	sceneNodeRotation[current] = rot
	sceneNodeScale[current] = scale
	sceneNodeTexture[current] = ""
	sceneNodeMesh[current] = scene
end
function AddCamera( pos, rot, scale )
	current = GetSceneNodeCount() + 1
	sceneNodePos[current] = pos
	sceneNodeRotation[current] = rot
	sceneNodeScale[current] = scale
	sceneNodeTexture[current] = ""
	sceneNodeMesh[current] = ""
end
function AddAnimCamera( orientationFunction )
	current = GetSceneNodeCount() + 1
	-- Log( "Adding node at " .. current )
	sceneNodePos[current] = {0,0,0}
	sceneNodeRotation[current] = {0,0,0}
	sceneNodeScale[current] = {1,1,1}
	sceneNodeTexture[current] = ""
	sceneNodeMesh[current] = ""
	sceneNodeOrientation[current] = orientationFunction
end


function TestShapes()
	AddNode( "cylinder:panels=7 r=1 h=1 Fixup()", "sand", { 0,0,8 }, { 0,0,0 }, { 4,1,4 } )
	AddNode( "cube", "checker:xstripes=256 ystripes=256", { 4,2,0 }, { 0,0,0 }, { 1,1,1 } )
	AddNode( "cube", "wavychecker", { 0,3,-8 }, { 0,1,0 }, { 1,1,1 } )
	for i=3,9 do
		AddNode( "cylinder:panels="..i.." r=1 h=1 Fixup()", "mess:stripes="..(i-2), { 10,1,-30+i*5 }, { 0,0,0 }, { 2,1,2 } )
	end
end

function LandAndTrees()
	for i=0,5 do
		AddScene( "tree:seed=" .. 100+i .." branchDepth=".. 3+i%5 .." Fixup()",
		{-8,0,-8+4*i}, {0,i,0}, {1,1,1} )
	end
	--AddScene( "tree:seed=11 Fixup()", {2,0,2}, {0,0,0}, {1,1,1} )
	AddScene( "terrain", {0,-2,0}, {0,0,0}, {4,2,4} )
end

function Village()
	points = 7
	for i=0,points-1 do
		rad = 2.5
		theta = i*6.284/points
		x = Sin(theta) * rad
		y = Cos(theta) * rad
		AddScene( "house:wallTexture=\"checker\" Fixup()", {0.5+x,1.25,8+y}, {0,i,0}, {0.2,0.2,0.2} )
	end
	--for i=0,4 do
		--for j=0,4 do
			--AddScene( "house:wallTexture=\"checker\" Fixup()", {0+i,2,5+j}, {0,0,0}, {0.2,0.2,0.2} )
		--end
	--end
end

function CubeAnim()
	theta = 0.1 * GetTime()
	rad = 4
	st = Sin(theta)
	ct = Cos(theta)
	x = 0
	y = 3
	z = 0
	return ct,st,0,0, -st,ct,0,0, 0,0,1,0, x,y,z,1
end
function BananaAnim()
	theta = 0.05 * GetTime()
	rad = 12
	st = Sin(theta)
	ct = Cos(theta)
	x = st * rad
	y = 2
	z = ct * rad
	return 0,1,0,0, ct,0,-st,0, st,0,ct,0, x,y,z,1
end
function CameraAnim()
	theta = 0.5 * GetTime()
	rad = 12
	st = Sin(theta)
	ct = Cos(theta)
	x = st * rad
	y = 2
	z = ct * rad
	--return 0,1,0,0, ct,0,-st,0, st,0,ct,0, x,y,z,1
	return ct,0,-st,0, 0,1,0,0, st,0,ct,0, x,y,z,1
end
function Fixup()
	AddNode( "implicit", "checker", {0,0,0}, {0,0,0}, {1,1,1} )
end

Fixup()
