function ScriptName()
	return "tree"
end

nodePos = { }
nodeRot = { }
nodeScale = { }
sceneNodeTexture = { }
sceneNodeParams = { }
sceneNodeMesh = { "end" }
seed=100
branchDepth = 7

function Clear()
	nodePos = { }
	nodeRot = { }
	nodeScale = { }
	sceneNodeTexture = { }
	sceneNodeParams = { }
	sceneNodeMesh = { "end" }
end

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

function copy(obj)
  if type(obj) ~= 'table' then return obj end
  if seen and seen[obj] then return seen[obj] end
  local s = seen or {}
  local res = setmetatable({}, getmetatable(obj))
  s[obj] = res
  for k, v in pairs(obj) do res[copy(k, s)] = copy(v, s) end
  return res
end

function AddNode( mesh, texture, params, pos, rot, scale )
	current = GetSceneNodeCount() + 1
	-- Log( "Adding node at " .. current )
	nodePos[current] = pos
	nodeRot[current] = rot
	nodeScale[current] = scale
	sceneNodeParams[current] = params
	sceneNodeTexture[current] = texture
	sceneNodeMesh[current] = mesh
end

function GetSceneNodePos( n ) return table.unpack(nodePos[n+1]) end
function GetSceneNodeRotation( n ) return table.unpack(nodeRot[n+1]) end
function GetSceneNodeScale( n ) return table.unpack(nodeScale[n+1]) end
function GetSceneNodeParams( n ) return sceneNodeParams[n+1] end
function GetSceneNodeMesh( n ) return sceneNodeMesh[n+1] end
function GetSceneNodeTexture( n ) return sceneNodeTexture[n+1] end

function AddTree( pos, rot, scale, remaining, branch )
	if remaining > 1 then
		AddNode( "branch", "wood", "", pos, rot, scale )
		x,y,z = table.unpack(pos)

		pos = copy(pos)
		rot = copy(rot)
		scale = copy(scale)

		Identity()
		Translate(table.unpack(pos))
		Rotate(table.unpack(rot))
		newx, newy, newz = Transform( 0,scale[2], 0 )

		move = {dx,dy,dz}
		pos = { newx, newy, newz }

		rot[2] = rot[2] + Noise( branch, remaining, seed ) * 1.4
		rot[3] = rot[3] + (Noise( branch, remaining, seed+1 )-0.5) * 1.9
		factor = 0.8
		scale[1] = scale[1] * factor
		scale[2] = scale[2] * factor
		scale[3] = scale[3] * factor
		if true then -- ( remaining % 2 ) == 1 then
			rot[1] = rot[1] + 1.0
			AddTree( pos, rot, scale, remaining - 1, branch * 2 )
			rot = copy( rot )
			rot[1] = rot[1] - 2.0
			AddTree( pos, rot, scale, remaining - 1, branch * 2 + 1 )
		else
			AddTree( pos, rot, scale, remaining - 1, branch * 2 )
		end

	else
		leafSize = 0.2
		AddNode( "cube", "grass", "", pos, rot, {leafSize,leafSize,leafSize} )
	end
end

pos = { 0,0,0 }
rot = { 0,0,0 }
scale = { 0.2,2,0.2 }
AddTree( pos, rot, scale, branchDepth, 1 )

function Fixup() 
	Clear()
	AddTree( pos, rot, scale, branchDepth, 1 )
end

