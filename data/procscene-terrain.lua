function ScriptName()
	return "terrain"
end

nodePos = {
	{ 0,0,0 }
}
nodeRot = {
	{ 0,0,0 }
}
nodeScale = {
	{ 1,1,1 }
}
sceneNodeTexture = {
	"grass"
}
sceneNodeParams = {
	""
}
sceneNodeMesh = {
	"land",
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

function GetSceneNodePos( n ) return table.unpack(nodePos[n+1]) end
function GetSceneNodeRotation( n ) return table.unpack(nodeRot[n+1]) end
function GetSceneNodeScale( n ) return table.unpack(nodeScale[n+1]) end
function GetSceneNodeParams( n ) return sceneNodeParams[n+1] end
function GetSceneNodeMesh( n ) return sceneNodeMesh[n+1] end
function GetSceneNodeTexture( n ) return sceneNodeTexture[n+1] end

