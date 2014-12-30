function ScriptName()
	return "land"
end

mapwidth = 128
mapcells = mapwidth * mapwidth
vertcount = mapcells * 6

function LandHeight( x, y ) 
	return Perlin( x+100, y+100, 1, 16, 100 ) * 0.5
end

function GetMeshVertCount()
	return vertcount
end

function GetLandV( vert )
	sub = vert % 6
	cell = vert / 6

	cellx = cell / mapwidth
	celly = cell % mapwidth
	cellx = cellx - (cellx%1)
	celly = celly - (celly%1)

	ds = 0.5
	xs = { -ds, ds, -ds, ds }
	ys = { -ds, -ds, ds, ds }
	indices = { 1, 2, 3, 2, 3, 4 }
	vi = indices[sub+1];
	x = xs[vi] + cellx - mapwidth/2
	y = ys[vi] + celly - mapwidth/2
	return x, y
end
function GetMeshVert( vert )
	--Log( "vert count " .. vertcount)
	x, y = GetLandV( vert )
	h = LandHeight( x, y )
	return 10/mapwidth*x,h*2,10/mapwidth*y
	--return 10*xs[vi], 10*ys[vi], 0
end

function GetMeshNormal( vert )
	x, y = GetLandV( vert )
	d = 1
	h1 = LandHeight( x-d, y-d )
	h2 = LandHeight( x+d, y-d )
	h3 = LandHeight( x-d, y+d )
	h4 = LandHeight( x+d, y+d )
	h2w = 1
	dx = h2w*((h2+h4)-(h1+h3))
	dy = h2w*((h3+h4)-(h1+h2))
	idx = Sqrt( 1 - ( dx*dx ) )
	idy = Sqrt( 1 - ( dy*dy ) )
	-- left  = idx, dx, 0
	-- right =  0, dy, idy
	--  up   =  dx*idy-0*dy, 0*0-idx*idy, idx*dy-0*dx
	--  up   =  dx*idy, -idx*idy, idx*dy
	--return idx,dx,0
	return -dx*idy,idx*idy,-dy*idx
end

function GetMeshUV( vert )
	sub = vert % 6
	ds = 0.5
	xs = { -ds, ds, -ds, ds }
	ys = { -ds, -ds, ds, ds }
	indices = { 1, 2, 3, 2, 3, 4 }
	vi = indices[sub+1];
	cell = vert / 6

	cellx = cell / mapwidth
	celly = cell % mapwidth
	cellx = cellx - (cellx%1)
	celly = celly - (celly%1)

	x = xs[vi] + cellx
	y = ys[vi] + celly
	return y/mapwidth,x/mapwidth
	--return 10*xs[vi], 10*ys[vi], 0
end
