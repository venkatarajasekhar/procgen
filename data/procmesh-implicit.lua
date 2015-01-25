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
	radius = 4.0
	return radius * radius - ( x * x + y * y + z * z )
end
function BlobFunc( x, y, z )
	radius = 3.9
	return radius * radius - ( x * x + y * y * y + z * z )
end

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
function ImplicitByBlocks( func, r )
	for z=-r,r do
		for y=-r,r do
			for x=-r,r do
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
				xyz = xyz + solid[index+0+1*r2+0*r22] * 4
				xyz = xyz + solid[index+1+1*r2+0*r22] * 8
				xyz = xyz + solid[index+0+0*r2+1*r22] * 16
				xyz = xyz + solid[index+1+0*r2+1*r22] * 32
				xyz = xyz + solid[index+0+1*r2+1*r22] * 64
				xyz = xyz + solid[index+1+1*r2+1*r22] * 128

				origin = solid[index+0+0*r2+0*r22] * 1
				xpos = solid[index+1+0*r2+0*r22] * 2
				ypos = solid[index+0+1*r2+0*r22] * 2
				zpos = solid[index+0+0*r2+1*r22] * 2
				n = { origin - xpos, origin-ypos, origin-zpos }
				
				xd = xmid[index]
				yd = ymid[index]
				zd = zmid[index]

				vxyz={x+0,y+0,z+0}
				vXyz={x+D,y+0,z+0}
				vxYz={x+0,y+D,z+0}
				vXYz={x+D,y+D,z+0}
				vxyZ={x+0,y+0,z+D}
				vXyZ={x+D,y+0,z+D}
				vxYZ={x+0,y+D,z+D}
				vXYZ={x+D,y+D,z+D}
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

				--n = {1,1,1}
				if xyz == 1 or xyz == (255-1) then
					AddTri( mxyz, myxz, mzxy, n, uv1,uv2,uv3 )
				end
				if xyz == 2 or xyz == (255-2) then
					AddTri( mxyz, myXz, mzXy, n, uv1,uv2,uv3 )
				end
				if xyz == 4 or xyz == (255-4) then
					AddTri( mxYz, myxz, mzxY, n, uv1,uv2,uv3 )
				end
				if xyz == 8 or xyz == (255-8) then
					AddTri( mxYz, myXz, mzXY, n, uv1,uv2,uv3 )
				end
				if xyz == 16 or xyz == (255-16) then
					AddTri( mxyZ, myxZ, mzxy, n, uv1,uv2,uv3 )
				end
				if xyz == 32 or xyz == (255-32) then
					AddTri( mxyZ, myXZ, mzXy, n, uv1,uv2,uv3 )
				end
				if xyz == 64 or xyz == (255-64) then
					AddTri( mxYZ, myxZ, mzxY, n, uv1,uv2,uv3 )
				end
				if xyz == 128 or xyz == (255-128) then
					AddTri( mxYZ, myXZ, mzXY, n, uv1,uv2,uv3 )
				end

				if xyz == 1+2 or xyz == 255-(1+2) then
					AddQuad( myxz,myXz,mzxy,mzXy, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 4+8 or xyz == 255-(4+8) then
					AddQuad( myxz, myXz, mzxY,mzXY, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 16+32 or xyz == 255-(16+32) then
					AddQuad( myxZ, myXZ, mzxy, mzXy, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 64+128 or xyz == 255-(64+128) then
					AddQuad( myxZ,myXZ, mzxY,mzXY, n, uv1,uv2,uv3,uv4 )
				end

				if xyz == 1+4 or xyz == 255-(1+4) then
					AddQuad( mzxy,mzxY, mxyz, mxYz, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 2+8 or xyz == 255-(2+8) then
					AddQuad( mzXy,mzXY,mxyz,mxYz, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 16+64 or xyz == 255-(16+64) then
					AddQuad( mzxy,mzxY,mxyZ,mxYZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 32+128 or xyz == 255-(32+128) then
					AddQuad( mzXy, mzXY, mxyZ,mxYZ, n, uv1,uv2,uv3,uv4 )
				end

				if xyz == 1+16 or xyz == 255-(1+16) then
					AddQuad( myxz, myxZ, mxyz, mxyZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 2+32 or xyz == 255-(2+32) then
					AddQuad( myXz, myXZ, mxyz, mxyZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 4+64 or xyz == 255-(4+64) then
					AddQuad( myxz,myxZ,mxYz,mxYZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 8+128 or xyz == 255-(8+128) then
					AddQuad( myXz,myXZ,mxYz,mxYZ, n, uv1,uv2,uv3,uv4 )
				end

				if xyz == 1+2+4+8 or xyz == 16+32+64+128 then
					AddQuad( mzxy, mzXy, mzxY, mzXY, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 1+4+16+64 or xyz == 2+8+32+128 then
					AddQuad( mxyz,mxYz,mxyZ,mxYZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 1+2+16+32 or xyz == 4+8+64+128 then
					AddQuad( myxz,myXz,myxZ,myXZ, n, uv1,uv2,uv3,uv4 )
				end

				if xyz == 1+2+4+16 or xyz == 255-(1+2+4+16) then
					AddQuad( mxYz,myXz,mxyZ,mzXy, n, uv1,uv2,uv3,uv4 )
					AddQuad( mxYz,mxyZ,mzxY,myxZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 1+2+8+32 or xyz == 255-(1+2+8+32) then
					AddQuad( mxYz,myxz,mxyZ,mzxy, n, uv1,uv2,uv3,uv4 )
					AddQuad( mxYz,mxyZ,mzXY,myXZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 1+4+8+64 then
					AddQuad( mxyz,myXz,mxYZ,mzXY, n, uv1,uv2,uv3,uv4 )
					AddQuad( mxyz,mxYZ,mzxy,myxZ, n, uv1,uv2,uv3,uv4 )
				end
				if xyz == 2+4+8+128 then
					AddQuad( mxyz,myxz,mxYZ,mzxY, n, uv1,uv2,uv3,uv4 )
					AddQuad( mxyz,mxYZ,mzXy,myXZ, n, uv1,uv2,uv3,uv4 )
				end

				if xyz == 1+2+4 or xyz == 255-(1+2+4) then
					AddQuad( mxYz,myXz,mzxY,mzXy, n, uv1,uv2,uv3,uv4 )
					AddTri( mzxY,mzXy,mzxy, n, uv1,uv2,uv3 )
				end
				if xyz == 1+2+8 or xyz == 255-(1+2+8) then
					AddQuad( mxYz,myxz,mzXY,mzxy, n, uv1,uv2,uv3,uv4 )
					AddTri( mzxy,mzXy,mzXY, n, uv1,uv2,uv3 )
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
