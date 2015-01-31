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
	radius = 7.0
	return radius * radius - ( x * x + y * y + z * z )
end
function BlobFunc( x, y, z )
	radius = 3.9
	return radius * radius - ( x * x + y * y * y + z * z )
end

transform = {{1,0,0},{0,1,0},{0,0,1},{0,0,0}}

function AddTri( v1,v2,v3, normal, u1,u2,u3 )
	n = normal
	ax=v2[1]-v1[1] ay=v2[2]-v1[2] az=v2[3]-v1[3]
	bx=v3[1]-v1[1] by=v3[2]-v1[2] bz=v3[3]-v1[3]
	n = {ay*bz-az*by, az*bx-ax*bz, ax*by-ay*bx}
	l = Sqrt(n[1]*n[1]+n[2]*n[2]+n[3]*n[3])
	n = {n[1]/l, n[2]/l, n[3]/l}

	dist = normal[1]*n[1]+normal[2]*n[2]+normal[3]*n[3]
	if dist < 0 then
		n = {-n[1],-n[2],-n[3]}
	end

	AddVertex( v1, n, u1 )
	AddVertex( v2, n, u2 )
	AddVertex( v3, n, u3 )
end

function GatherSolidsAndMids( func, r )
	solid = {}
	r2 = r*2
	r22 = r*2*r*2
	for z=-r,r do
		for y=-r,r do
			for x=-r,r do
				index = x+y*r2+z*r22
				if func( x, y, z ) > 0 then
					solid[index] = 1
				else
					solid[index] = 0
				end
			end
		end
	end

	xmid = {}
	ymid = {}
	zmid = {}

	for z=-r,r-1 do
		for y=-r,r-1 do
			for x=-r,r-1 do
				index = x+y*r2+z*r22
				-- check for x pos
				if solid[index] + solid[index+1] == 1 then
					dir = solid[index]
					e = dir
					s = 1-dir
					while math.abs(e-s) > 0.01 do
						m = (s+e)/2
						mv = func(x+m,y,z)
						if mv < 0 then
							e = m
						else
							s = m
						end
					end
					xmid[index] = m
				end
				-- check for y pos
				if solid[index] + solid[index+r2] == 1 then
					dir = solid[index]
					e = dir
					s = 1-dir
					while math.abs(e-s) > 0.01 do
						m = (s+e)/2
						mv = func(x,y+m,z)
						if mv < 0 then
							e = m
						else
							s = m
						end
					end
					ymid[index] = m
				end
				-- check for z pos
				if solid[index] + solid[index+r22] == 1 then
					dir = solid[index]
					e = dir
					s = 1-dir
					while math.abs(e-s) > 0.01 do
						m = (s+e)/2
						mv = func(x,y,z+m)
						if mv < 0 then
							e = m
						else
							s = m
						end
					end
					zmid[index] = m
				end
			end
		end
	end
	return solid, xmid,ymid,zmid
end

function ImplicitByMarchingCube( func, r )
	solid, xmid,ymid,zmid = GatherSolidsAndMids( func, r )

	d = 0.5
	D = d+d
	for z=-r,r-1 do
		for y=-r,r-1 do
			for x=-r,r-1 do
				uv1 = {(x+0)/r,(z+0)/r}
				uv2 = {(x+1)/r,(z+0)/r}
				uv3 = {(x+0)/r,(z+1)/r}
				uv4 = {(x+1)/r,(z+1)/r}
				-- for this cell, see what the marching cube data is
				index = x+y*r2+z*r22
				--                      x y    z
				xyz =   0 + solid[index+0+0*r2+0*r22] * 1
				xyz = xyz + solid[index+1+0*r2+0*r22] * 2
				xyz = xyz + solid[index+1+1*r2+0*r22] * 4
				xyz = xyz + solid[index+0+1*r2+0*r22] * 8
				xyz = xyz + solid[index+0+0*r2+1*r22] * 16
				xyz = xyz + solid[index+1+0*r2+1*r22] * 32
				xyz = xyz + solid[index+1+1*r2+1*r22] * 64
				xyz = xyz + solid[index+0+1*r2+1*r22] * 128
	
				tris,numtris = MCLookup2(xyz)
				if numtris  > 0 then
					--Log("From "..xyz.." Make "..numtris.." triangle")
					--for k,v in pairs(tris) do
						--Log( "K:"..k.." V:"..v )
					--end
				
					origin = solid[index+0+0*r2+0*r22] * 1
					xpos = solid[index+1+0*r2+0*r22] * 1
					ypos = solid[index+0+1*r2+0*r22] * 1
					zpos = solid[index+0+0*r2+1*r22] * 1
					n = { origin - xpos, origin-ypos, origin-zpos }
					
					mxyz={x+(xmid[index]or 0),y+0,z+0}
					mxYz={x+(xmid[index+r2]or 0),y+D,z+0}
					mxyZ={x+(xmid[index+r22]or 0),y+0,z+D}
					mxYZ={x+(xmid[index+r2+r22]or 0),y+D,z+D}
					myxz={x+0,y+(ymid[index]or 0),z+0}
					myXz={x+D,y+(ymid[index+1]or 0),z+0}
					myxZ={x+0,y+(ymid[index+r22]or 0),z+D}
					myXZ={x+D,y+(ymid[index+1+r22]or 0),z+D}
					mzxy={x+0,y+0,z+(zmid[index]or 0)}
					mzXy={x+D,y+0,z+(zmid[index+1]or 0)}
					mzxY={x+0,y+D,z+(zmid[index+r2]or 0)}
					mzXY={x+D,y+D,z+(zmid[index+1+r2]or 0)}

					vertlist = {
						mxyz,myXz,mxYz,myxz, -- 0,1,2,3
						mxyZ,myXZ,mxYZ,myxZ, -- 4,5,6,7
						mzxy,mzXy,mzXY,mzxY  -- 8,9,10,11
					}

					for t=1,numtris do
						v1 = tris[t*3-2]
						v2 = tris[t*3-1]
						v3 = tris[t*3-0]
						--Log("Verts "..v1.." "..v2.." "..v3)
						v1 = vertlist[v1+1]
						v2 = vertlist[v2+1]
						v3 = vertlist[v3+1]
						--Log("V1 "..v1[1]-x.." "..v1[2]-y.." "..v1[3]-z)
						--Log("V2 "..v2[1]-x.." "..v2[2]-y.." "..v2[3]-z)
						--Log("V3 "..v3[1]-x.." "..v3[2]-y.." "..v3[3]-z)
						AddTri( v1,v2,v3, n, uv1, uv2, uv3 )
					end
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

	ImplicitByMarchingCube( SphereFunc, 7 )
	--ImplicitByMarchingCube( BlobFunc, 7 )
	--ImplicitByBlocks( BlobFunc, 4 )

	Log("FIXUP VCount = "..vertCount)
end
Fixup()
