function ScriptName()
	return "branch"
end

sides = 6
quads = sides
vertcount = quads * 6

function GetMeshVertCount()
	return vertcount
end

function GetCubeV( vert )
	sub = vert % 6
	side = vert / 6
	side = side - (side%1)
	side = side+1
	sub = sub+1

	ds = 1.0

	
	afx = { -ds, ds, 0,0,0,0 }
	afy = { 0,0, -ds, ds, 0,0 }
	afz = { 0,0,0,0,  -ds, ds }
	adx = { 0,0,ds,ds,ds,ds }
	ady = { ds,ds,0,0,ds,ds }
	adz = { ds,ds,ds,ds,0,0 }
	fx = afx[side]
	fy = afy[side]
	fz = afz[side]
	dx = adx[side]
	dy = ady[side]
	dz = adz[side]

	if side < 3 then
		xs = { -dx+fx, dx+fx, -dx+fx, dx+fx }
		ys = { -dy+fy, -dy+fy, dy+fy, dy+fy }
		zs = { -dz+fz, dz+fz, -dz+fz, dz+fz }
	else if side < 5 then
		xs = { -dx+fx, dx+fx, -dx+fx, dx+fx }
		ys = { -dy+fy, -dy+fy, dy+fy, dy+fy }
		zs = { -dz+fz, -dz+fz, dz+fz, dz+fz }
	else
		xs = { -dx+fx, dx+fx, -dx+fx, dx+fx }
		ys = { -dy+fy, -dy+fy, dy+fy, dy+fy }
		zs = { -dz+fz, dz+fz, -dz+fz, dz+fz }
	end
	end

	indices = { 1, 2, 3, 2, 3, 4 }
	vi = indices[sub];
	x = xs[vi]
	y = ys[vi]
	z = zs[vi]
	return x, 0.5*y+0.5, z
end
function GetMeshVert( vert )
	--Log( "vert count " .. vertcount)
	x, y, z = GetCubeV( vert )
	return x,y,z
	--return 10*xs[vi], 10*ys[vi], 0
end

function GetMeshNormal( vert )
	sub = vert % 6
	side = vert / 6
	side = side - (side%1)
	side = side+1
	sub = sub+1

	ds = 1
	fx = { -ds, ds, 0,0,0,0 }
	fy = { 0,0, -ds, ds, 0,0 }
	fz = { 0,0,0,0,  -ds, ds }
	fx = fx[side]
	fy = fy[side]
	fz = fz[side]

	return fx,fy,fz
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
