function ScriptName()
	return "cylinder"
end

panels = 5
captriangles = panels * 2
quads = panels
capverts = captriangles * 3
vertcount = quads * 6 + capverts
h = 2.0
r = 2.0
function Fixup()
	captriangles = panels * 2
	quads = panels
	capverts = captriangles * 3
	vertcount = quads * 6 + capverts
end

function GetMeshVertCount()
	return vertcount
end

function GetCubeV( vert )
	perPanelTheta = 6.284/panels
	if vert < capverts then
		currentH = h
		if vert >= capverts/2 then
			currentH = - h
		end
		seg = (vert/3)%panels
		seg = seg-seg%1

		theta = seg*perPanelTheta
		if vert%3 > 1.5 then
			theta = theta + perPanelTheta
		end
		x = Cos(theta) * r
		y = currentH
		z = Sin(theta) * r
		x=x-(x%0.01)
		z=z-(z%0.01)
		if vert%3 < 0.5 then
			x,z = 0,0
		end
		Log( "GetCubeV("..vert.."),"..h.." t"..theta.." s"..seg.." -> "..x..","..y..","..z )
		return x, y, z
	else
		vert = vert - capverts
		sub = (vert%6)+1
		seg = (vert/6)
		seg = seg-seg%1

		theta = seg*perPanelTheta
		theta2 = theta+perPanelTheta
		x1 = Cos(theta) * r
		z1 = Sin(theta) * r
		x2 = Cos(theta2) * r
		z2 = Sin(theta2) * r

		xs = { x1,x2,x1,x2 }
		ys = { -h, -h, h, h }
		zs = { z1,z2,z1,z2 }

		indices = { 1, 2, 3, 2, 3, 4 }
		vi = indices[sub];
		x = xs[vi]
		y = ys[vi]
		z = zs[vi]

		return x,y,z
	end
end
function GetMeshVert( vert )
	--Log( "vert count " .. vertcount)
	x, y, z = GetCubeV( vert )
	return x,y,z
	--return 10*xs[vi], 10*ys[vi], 0
end

function GetMeshNormal( vert )
	perPanelTheta = 6.284/panels
	if vert < capverts then
		if vert >= capverts/2 then
			return 0,-1,0
		else
			return 0,1,0
		end
	else
		vert = vert - capverts
		sub = (vert%6)+1
		seg = (vert/6)
		seg = seg-seg%1

		theta = seg*perPanelTheta
		theta2 = theta+perPanelTheta
		x1 = Cos(theta)
		z1 = Sin(theta)
		x2 = Cos(theta2)
		z2 = Sin(theta2)

		xs = { x1,x2,x1,x2 }
		zs = { z1,z2,z1,z2 }

		indices = { 1, 2, 3, 2, 3, 4 }
		vi = indices[sub];
		x = xs[vi]
		y = 0
		z = zs[vi]

		return x,y,z
	end
	return 0,1,0
end

function GetMeshUV( vert )
	sub = vert % 6

	xs = { 0,1,0,1 }
	ys = { 0,0,1,1 }

	indices = { 1, 2, 3, 2, 3, 4 }
	vi = indices[sub+1];
	x = xs[vi]
	y = ys[vi]
	return x,y
end
