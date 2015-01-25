vertCount = 0
vertices = {}
normals = {}
uvs = {}

function AddVertex( pos, normal, uv )
	vertCount = vertCount + 1
	vertices[vertCount] = pos
	normals[vertCount] = normal
	uvs[vertCount] = uv	
end

function GetMeshVertCount()  return vertCount end
function GetMeshVert( vert ) return table.unpack(vertices[vert+1]) end
function GetMeshNormal( vert ) return table.unpack(normals[vert+1]) end
function GetMeshUV( vert ) return table.unpack(uvs[vert+1]) end

function SphereFunc( x, y, z )
	radius = 3.5
	return radius * radius - ( x * x + y * y + z * z )
end
function BlobFunc( x, y, z )
	radius = 3.5
	return radius * radius - ( x * x + y * y * y + z * z )
end

function AddQuad( v1,v2,v3,v4, normal, u1,u2,u3,u4 )
	AddVertex( v1, normal, u1 )
	AddVertex( v2, normal, u2 )
	AddVertex( v3, normal, u3 )
	AddVertex( v3, normal, u3 )
	AddVertex( v2, normal, u2 )
	AddVertex( v4, normal, u4 )
end
function ImplicitByBlocks( func, r )
	for z=-r,r do
		for y=-r,r do
			for x=-r,r do
				-- for this cell, see what the marching cube data is
				if func( x, y, z ) > 0 then
					d = 0.5
					vl1 = {x-d,y-d,z-d}
					vl2 = {x+d,y-d,z-d}
					vl3 = {x-d,y-d,z+d}
					vl4 = {x+d,y-d,z+d}

					vh1 = {x-d,y+d,z-d}
					vh2 = {x+d,y+d,z-d}
					vh3 = {x-d,y+d,z+d}
					vh4 = {x+d,y+d,z+d}
					n = {0,0,1}
					uv1 = {(x+0)/r,(z+0)/r}
					uv2 = {(x+1)/r,(z+0)/r}
					uv3 = {(x+0)/r,(z+1)/r}
					uv4 = {(x+1)/r,(z+1)/r}
					AddQuad( vl1,vl2,vl3,vl4, {0,-1,0}, uv1,uv2,uv3,uv4 )
					AddQuad( vh1,vh2,vh3,vh4, {0,1,0}, uv1,uv2,uv3,uv4 )

					AddQuad( vl1,vl2,vh1,vh2, {0,0,-1}, uv1,uv2,uv1,uv2 )
					AddQuad( vl3,vl4,vh3,vh4, {0,0,1}, uv3,uv4,uv3,uv4 )
					AddQuad( vl1,vh1,vl3,vh3, {-1,0,0}, uv1,uv1,uv3,uv3 )
					AddQuad( vl2,vh2,vl4,vh4, {1,0,0}, uv2,uv2,uv4,uv4 )
				end
			end
		end
	end
end
function Fixup()
	vertCount = 0
	vertices = {}
	normals = {}
	uvs = {}

	ImplicitByBlocks( BlobFunc, 4 )

	Log("FIXUP VCount = "..vertCount)
end
Fixup()
