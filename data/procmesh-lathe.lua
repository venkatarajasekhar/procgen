function ScriptName()
	return "lathe"
end

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

function Warp( invert )
	return invert
	--bend = invert[2] * 0.3
	--bend = bend * bend
	--outvert = {invert[1] + bend, invert[2], invert[3] }
	--return outvert
end

function Fixup()
	vertCount = 0
	vertices = {}
	normals = {}
	uvs = {}

	if true then
		n = { 0,1,0 }
		radii = {1,1,1,1,1,1}
		p = {0,1,2,3,4,5,6}
		for i=1,5 do
			l,h = (i)/5,(i+1)/5
			p1 = p[i]
			p2 = p[i+1]
			widthl = radii[i]
			widthh = radii[i+1]
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

				AddVertex( Warp( v1l ), n1l, uv1 )
				AddVertex( Warp( v2l ), n2l, uv2 )
				AddVertex( Warp( v1h ), n1h, uv3 )
				AddVertex( Warp( v1h ), n1h, uv3 )
				AddVertex( Warp( v2l ), n2l, uv2 )
				AddVertex( Warp( v2h ), n2h, uv4 )
			end

		end
	end
	Log("FIXUP VCount = "..vertCount)
end
Fixup()
