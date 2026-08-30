cube = nil
suzanne = nil
house = nil
terrain = nil
rotationY = 0
time = 0

cubePosX = 0

function OnStart()
    print("Script started!")
	FEngine.Camera:SetPosition(-1, 5, 7)
    FEngine.Camera:SetRotation(-156, -18)
	
	-- Initialize objects
    cube = FEngine.NewObject()
	suzanne = FEngine.NewObject()
	house = FEngine.NewObject()
	terrain = FEngine.NewObject()

	local testObj = FEngine.NewObject()

	cube:AddModels("cube.obj")
	suzanne:AddModels("DamagedHelmet.gltf")
	house:AddModels("house.obj")
	terrain:AddModels("plane.obj")
	
	-- Cube Texture Setup
	cube:GetModel(0):SetTexture("diffuse.dds")
	cube:GetModel(0):SetNormalMap("normals.dds")
	cube:GetModel(0):SetSpecularMap("specular.dds")

	house:GetModel(0):SetTexture("house.dds")
	terrain:GetModel(0):SetTexture("dirtDiffuse.dds")
	
	suzanne:SetPosition(-3,2,0)
	suzanne:SetRotation(0,35,0)
	
	house:SetPosition(-10,0,0)
	house:SetRotation(0,65,0)
	
	terrain:SetScale(3,1,3)

	testObj:Destroy()
end

function OnUpdate(dt)
    -- accumulate time
    time = time + dt

    -- smooth up/down motion
    local amplitude = 1.0
    local speed = 1.0
    local offsetY = math.sin(time * speed) * amplitude + 2
	local suzanneRotY = math.sin(time * speed * 0.5) * 180

    -- rotation
    rotationY = rotationY + 45 * dt

    
    if FEngine.Inputs:IsKeyDown(INPUT_KEY_LEFT) then
		cubePosX = cubePosX + 15 * dt
	end

    if FEngine.Inputs:IsKeyDown(INPUT_KEY_RIGHT) then
		cubePosX = cubePosX - 15 * dt
	end

    -- apply transforms
    cube:SetPosition(cubePosX, offsetY, 0)
    cube:SetRotation(0, rotationY, 0)
	suzanne:SetRotation(0,suzanneRotY,0)

    
    cube:Update(dt)
	suzanne:Update(dt)
	house:Update(dt)
	terrain:Update(dt)
end

function OnDraw()
    cube:Draw()
	suzanne:Draw()
	house:Draw()
	terrain:Draw()
end
