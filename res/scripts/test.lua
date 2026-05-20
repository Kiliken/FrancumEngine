cube = nil
suzanne = nil
house = nil
terrain = nil
rotationY = 0
time = 0

cubePosX = 0

function OnStart()
    print("Script started!")
	
	-- Initialize objects
    cube = Object.new()
	suzanne = Object.new()
	house = Object.new()
	terrain = Object.new()

	cube:AddModels("../res/cube.obj")
	suzanne:AddModels("../res/DamagedHelmet.gltf")
	house:AddModels("../res/house.obj")
	terrain:AddModels("../res/plane.obj")
	
	-- Cube Texture Setup
	cube:GetModel(0):SetTexture("../res/diffuse.dds")
	cube:GetModel(0):SetNormalMap("../res/normals.dds")
	cube:GetModel(0):SetSpecularMap("../res/specular.dds")

	house:GetModel(0):SetTexture("../res/house.dds")
	terrain:GetModel(0):SetTexture("../res/dirtDiffuse.dds")
	-- terrain:GetModel(0):SetColor("0xFF0000")
	
	suzanne:SetPosition(-3,2,0)
	suzanne:SetRotation(0,35,0)
	
	house:SetPosition(-10,0,0)
	house:SetRotation(0,65,0)
	
	terrain:SetScale(3,1,3)
	
	-- FEngine.Camera:SetProjMode("ORTHO")
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
	house:Update(dt)
	terrain:Update(dt)
end

function OnDraw()
    cube:Draw()
	suzanne:Draw()
	house:Draw()
	terrain:Draw()
end
