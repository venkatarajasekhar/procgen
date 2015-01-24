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

function SurfaceFunction( x, y, z )
	radius = 2.0
	return x * x + y * y + z * z - radius * radius
end

function Fixup()
	vertCount = 0
	vertices = {}
	normals = {}
	uvs = {}

	for z=-4,4 do
		for y=-4,4 do
			for x=-4,4 do
				-- for this cell, see what the marching cube data is
			end
		end
	end

	if true then
		n = { 0,1,0 }
		radii = {0,0.2,0.8,0.9,1,1.2,1,0.9,0.8,0.1,0.1,0}
		p = {-4.1,-4,-3,-2,-1,0,1,2,3,4,5,5}
		for i=-4,6 do
			l,h = (i+4)/11,(i+5)/11
			p1 = p[i+5]
			p2 = p[i+6]
			widthl = radii[i+5]
			widthh = radii[i+6]
			pi2 = 6.284
			delta = pi2/8
			rep = 3
			for theta = 0,pi2,delta do
				ta = theta
				tb = theta+delta
				sa = Sin(ta)
				ca = Cos(ta)
				sb = Sin(tb)
				cb = Cos(tb)
				v1l = { sa*widthl,p1,ca*widthl }
				v2l = { sb*widthl,p1,cb*widthl }
				v1h = { sa*widthh,p2,ca*widthh }
				v2h = { sb*widthh,p2,cb*widthh }
				uv1 = { ta*rep/pi2,l }
				uv2 = { tb*rep/pi2,l }
				uv3 = { ta*rep/pi2,h }
				uv4 = { tb*rep/pi2,h }
				n1l = { sa,0,ca }
				n2l = { sb,0,cb }
				n1h = { sa,0,ca }
				n2h = { sb,0,cb }

				AddVertex( v1l, n1l, uv1 )
				AddVertex( v2l, n2l, uv2 )
				AddVertex( v1h, n1h, uv3 )
				AddVertex( v1h, n1h, uv3 )
				AddVertex( v2l, n2l, uv2 )
				AddVertex( v2h, n2h, uv4 )
			end

		end
	end
	Log("FIXUP VCount = "..vertCount)
end
Fixup()