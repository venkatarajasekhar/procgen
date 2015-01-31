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
function AddQuad( v1,v2,v3,v4, normal, u1,u2,u3,u4 )
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
	AddVertex( v3, n, u3 )
	AddVertex( v2, n, u2 )
	AddVertex( v4, n, u4 )
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

function RotateBitsZ(xyz,p)
	i = p
	while i > 0 do
		a = band(xyz,1+16)
		b = band(xyz,2+32)
		c = band(xyz,4+64)
		d = band(xyz,8+128)
		xyz = a*2 + b*4 + c/4 +d/2
		i = i - 1
	end
	return xyz
end
function RotateBitsX(xyz,p)
	i = p
	while i > 0 do
		a = band(xyz,1+2)
		b = band(xyz,4+8)
		c = band(xyz,16+32)
		d = band(xyz,64+128)
		xyz = a*4 + b*16 +c/16 + d/4
		i = i - 1
	end
	return xyz
end
function SetupOrientation( o, X,Y,Z )
	x = o % 4
	o = (o-x)/4
	y = o%4
	o = (o-y)/4
	z = o%4
	Identity()
	Log( "SetupOrientation "..x..","..y..","..z )
	m = -0.5
	b = 0.5
	for i=1,y do
		Translate(m,m,m)
		Rotate90( 1 )
		Rotate90( 1 )
		Rotate90( 1 )
		Translate(b,b,b)
	end
	for i=1,x do
		Translate(m,m,m)
		Rotate90( 2 )
		Rotate90( 2 )
		Rotate90( 2 )
		Translate(b,b,b)
	end
	for i=1,z do
		Translate(m,m,m)
		Rotate90( 0 )
		Rotate90( 0 )
		Rotate90( 0 )
		Translate(b,b,b)
	end
	Translate(X,Y,Z)
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
				xyz = xyz + solid[index+0+1*r2+0*r22] * 4
				xyz = xyz + solid[index+1+1*r2+0*r22] * 8
				xyz = xyz + solid[index+0+0*r2+1*r22] * 16
				xyz = xyz + solid[index+1+0*r2+1*r22] * 32
				xyz = xyz + solid[index+0+1*r2+1*r22] * 64
				xyz = xyz + solid[index+1+1*r2+1*r22] * 128
	
				mesh,orientation = MCLookup(xyz)
				
				if mesh > 0 then
					origin = solid[index+0+0*r2+0*r22] * 1
					xpos = solid[index+1+0*r2+0*r22] * 1
					ypos = solid[index+0+1*r2+0*r22] * 1
					zpos = solid[index+0+0*r2+1*r22] * 1
					n = { origin - xpos, origin-ypos, origin-zpos }
					
					xd = xmid[index]
					yd = ymid[index]
					zd = zmid[index]

					Log("Add "..mesh.." @ "..orientation.." @ "..x..","..y..","..z)
					SetupOrientation(orientation,x,y,z)

					mxyz=Transform({(xmid[index]or 0.5),0,0})
					mxYz=Transform({(xmid[index+r2]or 0.5),D,0})
					mxyZ=Transform({(xmid[index+r22]or 0.5),0,D})
					mxYZ=Transform({(xmid[index+r2+r22]or 0),D,D})
					myxz=Transform({0,(ymid[index]or 0.5),0})
					myXz=Transform({D,(ymid[index+1]or 0.5),0})
					myxZ=Transform({0,(ymid[index+r22]or 0.5),D})
					myXZ=Transform({D,(ymid[index+1+r22]or 0.5),D})
					mzxy=Transform({0,0,(zmid[index]or 0.5)})
					mzXy=Transform({D,0,(zmid[index+1]or 0.5)})
					mzxY=Transform({0,D,(zmid[index+r2]or 0.5)})
					mzXY=Transform({D,D,(zmid[index+1+r2]or 0.5)})

					--test = {1,2,3}
					--for k,v in pairs(test) do
						--Log("KV"..k.."=>"..v)
					--end
					--test = Transform(test)
					--Log("After")
					--for k,v in pairs(test) do
						--Log("KV"..k.."=>"..v)
					--end

					-- case 1
					if mesh == 1 then
						AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
						PrintCurrentMatrix()
						for k,v in pairs(mxyz) do
							Log("V"..k..":"..v)
						end
						for k,v in pairs(myxz) do
							Log("V"..k..":"..v)
						end
						for k,v in pairs(mzxy) do
							Log("V"..k..":"..v)
						end
					end
					
					-- case 2
					if mesh == 2 then
						AddQuad( myxz,myXz,mzxy,mzXy, n, uv1,uv2,uv3,uv4 )
					end

					-- case 3
					if mesh == 3 then
						AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
						AddTri( mxYz, myXz, mzXY, n, uv1,uv2,uv3 )
						AddTri( mxYZ, myxZ, mzxY, n, uv1,uv2,uv3 )
					end

					-- case 4
					if mesh == 4 then
						AddQuad( mxYz,myXz,mzxY,mzXy, n, uv1,uv2,uv3,uv4 )
						AddTri( mzxY,mzXy,mzxy, n, uv1,uv2,uv3 )
					end
					
					-- case 5
					if mesh == 5 then
						AddQuad( mzxy, mzXy, mzxY, mzXY, n, uv1,uv2,uv3,uv4 )
					end

					-- case 6
					if mesh == 6 then
						AddQuad( mxYz,myXz,mzxY,mzXy, n, uv1,uv2,uv3,uv4 )
						AddTri( mzxY,mzXy,mzxy, n, uv1,uv2,uv3 )
						AddTri( mxYZ, myXZ, mzXY, n, uv1,uv2,uv3 )
					end

					-- case 7
					if mesh == 7 then
						AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
						AddTri( mxYz, myXz, mzXY, n, uv1,uv2,uv3 )
						AddTri( mxyZ, myXZ, mzXy, n, uv1,uv2,uv3 )
						AddTri( mxYZ, myxZ, mzxY, n, uv1,uv2,uv3 )
					end

					-- case 8
					if mesh == 8 then
						AddQuad( mxYz,myXz,mxyZ,mzXy, n, uv1,uv2,uv3,uv4 )
						AddQuad( mxYz,mxyZ,mzxY,myxZ, n, uv1,uv2,uv3,uv4 )
					end

					-- case 9
					if mesh == 9 then
						AddTri( mxYz, mzxy, mzxY, n, uv1,uv2,uv3 )
						AddTri( mxYz, mzxy, myXz, n, uv1,uv2,uv3 )
						AddTri( myXz, mzxy, myXZ, n, uv1,uv2,uv3 )
					end

					-- case 10
					if mesh == 10 then
						AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
						AddTri( mxYZ, myXZ, mzXY, n, uv1,uv2,uv3 )
					end

					-- case 11
					if mesh == 11 then
						AddQuad( myxz,myXz,mzxy,mzXy, n, uv1,uv2,uv3,uv4 )
						AddTri( mxYZ, myXZ, mzXY, n, uv1,uv2,uv3 )
					end

					-- case 12
					if mesh == 12 then
						AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
						AddTri( mxYz, myXz, mzXY, n, uv1,uv2,uv3 )
						AddTri( mxyZ, myXZ, mzXY, n, uv1,uv2,uv3 )
					end

					-- case 13
					if mesh == 13 then
						AddQuad( myxz,myXz,mzxy,mzXy, n, uv1,uv2,uv3,uv4 )
						AddQuad( myxZ,myXZ, mzxY,mzXY, n, uv1,uv2,uv3,uv4 )
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

	ImplicitByMarchingCube( SphereFunc, 4 )
	--ImplicitByMarchingCube( BlobFunc, 4 )
	--ImplicitByBlocks( BlobFunc, 4 )

	Log("FIXUP VCount = "..vertCount)
end
Fixup()
