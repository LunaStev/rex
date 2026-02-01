player = nil

function onEnter()
    player = Entity.create()
    player:set_position(100, 100)
    player:set_size(32, 32)
end

function update(dt)
    local speed = 200

    if Input.is_down("A") then
        player:move(-speed * dt, 0)
    end
    if Input.is_down("D") then
        player:move(speed * dt, 0)
    end
end

function render()
    Graphics.clear(0, 0, 0)
    Graphics.rect(player:x(), player:y(), 32, 32)
end