function ScriptName()
	return "s_test"
end


sceneNodePos = {
	{ 0,-2,0 },
	{ 0,0,8 },
	{ 4,2,0 },
	{ 0,3,-8 },
	{ 10,1,0 },
	{ 10,1,5 },
	{ 10,1,10 } 
}
sceneNodeRotation = {
	{ 0,0,0 },
	{ 0,0,0 },
	{ 0,0,0 },
	{ 0,1,0 },
	{ 0,0,0 },
	{ 0,0,0 },
	{ 0,0,0 }
}
sceneNodeScale = {
	{ 4,2,4 },
	{ 4,1,4 },
	{ 1,1,1 },
	{ 1,1,1 },
	{ 2,1,2 },
	{ 2,1,2 },
	{ 2,1,2 }
}
sceneNodeTexture = {
	"grass",
	"sand",
	"checker",
	"wavychecker",
	"mess",
	"mess",
	"mess"
}
sceneNodeParams = {
	"",
	"panels=7 r=1 h=1 Fixup()",
	"",
	"",
	"panels=3 r=1 h=1 Fixup()",
	"panels=4 r=1 h=1 Fixup()",
	"panels=5 r=1 h=1 Fixup()",
	"end"
}

sceneNodeMesh = {
	"land",
	"cylinder",
	"cube",
	"cube",
	"cylinder",
	"cylinder",
	"cylinder",
	"end"
}

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
function GetSceneNodeParams( n ) return sceneNodeParams[n+1] end
function GetSceneNodeMesh( n ) return sceneNodeMesh[n+1] end
function GetSceneNodeTexture( n ) return sceneNodeTexture[n+1] end

