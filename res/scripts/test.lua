cube = nil

function OnStart()
    print("Script started!")
    cube = Object.new("../res/cube.obj")
    print("Test After")
end

function OnUpdate(dt)
    cube:Update(dt)
end

function OnDraw()
    cube:Draw()
end
