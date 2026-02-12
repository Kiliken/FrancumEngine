cube = nil
rotationY = 0
time = 0

cubePosX = 0

function OnStart()
    print("Script started!")
    cube = Object.new("../res/cube.obj")
	suzanne = Object.new("../res/suzanne.obj")
	
	suzanne:SetPosition(-3,0,0)
	suzanne:SetRotation(0,35,0)

    print("Engine =", FEngine)	
    print("Inputs =", FEngine.Inputs)
	print("IsKeyDown =", FEngine.Inputs.IsKeyDown)
end

function OnUpdate(dt)
    -- accumulate time
    time = time + dt

    -- smooth up/down motion
    local amplitude = 1.0      -- how high it moves
    local speed = 2.0          -- how fast it moves
    local offsetY = math.sin(time * speed) * amplitude
	local suzanneRotY = math.sin(time * speed) * 180

    -- rotation
    rotationY = rotationY + 1.5

    
    if FEngine.Inputs:IsKeyDown(INPUT_KEY_LEFT) then
		cubePosX = cubePosX + 1
	end

    if FEngine.Inputs:IsKeyDown(INPUT_KEY_RIGHT) then
		cubePosX = cubePosX - 1
	end

    -- apply transforms
    cube:SetPosition(cubePosX, offsetY, 0)
    cube:SetRotation(0, rotationY, 0)
	suzanne:SetRotation(0,suzanneRotY,0)

    

    cube:Update(dt)
	suzanne:Update(dt)
end

function OnDraw()
    cube:Draw()
	suzanne:Draw()
end
