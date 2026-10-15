jit.off()
new_array = require("table.new")

cube = nil
suzanne = nil
house = nil
terrain = nil
rotationY = 0
time = 0
isOrtho = false
cubePosX = 0

-- Add these at the top of your script with your other variables:
autoSpawnTimer = 0
autoSpawnInterval = 2 -- Spawns a new object automatically every 0.5 seconds

-- New variables for dynamic object management
spawnedObjects = nil
wasTKeyPressed = false
wasOKeyPressed = false
wasGKeyPressed = false

function OnStart()
    print("Script started!")
	
	spawnedObjects = new_array(100, 0)
	
    FEngine.Camera:SetPosition(-1, 5, 7)
    FEngine.Camera:SetRotation(-156, -18)
    
    -- Initialize objects
    cube = FEngine.Object.NewObject()
    suzanne = FEngine.Object.NewObject()
    house = FEngine.Object.NewObject()
    terrain = FEngine.Object.NewObject()

    local testObj = FEngine.Object.NewObject()

    cube:AddModels("cube.obj")
    suzanne:AddModels("models/DamagedHelmet.gltf")
    house:AddModels("models/house.obj")
    terrain:AddModels("models/plane.obj")
    
    -- Cube Texture Setup
    cube:GetModel(0):SetTexture("models/diffuse.dds")
    cube:GetModel(0):SetNormalMap("models/normals.dds")
    cube:GetModel(0):SetSpecularMap("models/specular.dds")

    house:GetModel(0):SetTexture("models/house.dds")
    terrain:GetModel(0):SetTexture("models/dirtDiffuse.dds")
    terrain:GetModel(0):SetNormalMap("models/dirtNormal.dds")
    
    suzanne:SetPosition(-3, 2, 0)
    suzanne:SetRotation(0, 35, 0)
    
    house:SetPosition(-10, 0, 0)
    house:SetRotation(0, 65, 0)
    
    terrain:SetScale(3, 1, 3)

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
    
    -- Camera Toggle (G Key)
    local isGDown = FEngine.Inputs:IsKeyDown(INPUT_KEY_G)
    if isGDown and not wasGKeyPressed then
        isOrtho = not isOrtho
        FEngine.Camera:SetProjMode(isOrtho and "ORTHO" or "PERSP")
        print("Camera Switch")
    end
    wasGKeyPressed = isGDown

-- Automatic Timer Spawner (No keys required)
    autoSpawnTimer = autoSpawnTimer + dt
    if autoSpawnTimer >= autoSpawnInterval then
        autoSpawnTimer = 0 -- Reset the timer
        
        local newObj = FEngine.Object.NewObject()
        newObj:AddModels("models/DamagedHelmet.gltf")
        
        -- Give it a random spread position
        local rx = math.random(-6, 6)
        local rz = math.random(-6, 6)
        newObj:SetPosition(rx, 1, rz)
		
        table.insert(spawnedObjects, newObj)
        print("Auto-created object! Total count: " .. #spawnedObjects)
    end

    -- Deleting an object (O Key)
    local isODown = FEngine.Inputs:IsKeyDown(INPUT_KEY_O)
    if isODown and not wasOKeyPressed then
        if #spawnedObjects > 0 then
            -- Remove and grab the last object from the table
            local objToDestroy = table.remove(spawnedObjects)
            objToDestroy:Destroy()
            print("Destroyed object! Remaining count: " .. #spawnedObjects)
        else
            print("No objects left to destroy!")
        end
    end
    wasOKeyPressed = isODown

    -- apply transforms
    cube:SetPosition(cubePosX, offsetY, 0)
    cube:SetRotation(0, rotationY, 0)
    suzanne:SetRotation(0, suzanneRotY, 0)

    -- Update base objects
    cube:Update(dt)
    suzanne:Update(dt)
    house:Update(dt)
    terrain:Update(dt)

    -- Safe update loop using numeric indexing (bypasses ipairs completely)
    
    for i = 1, #spawnedObjects do
            local obj = spawnedObjects[i]
            if obj then
                obj:Update(dt)
            end
    end
    
end

function OnDraw()
    cube:Draw()
    suzanne:Draw()
    house:Draw()
    terrain:Draw()

    
    for i = 1, #spawnedObjects do
            local obj = spawnedObjects[i]
            if obj then
                obj:Draw()
            end
    end
    
end