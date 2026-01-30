player = nil

function on_start()
    player = Entity.create()
    player:set_position(100, 100)
    player:set_size(32, 32)
end

function on_update(dt)
    local speed = 200

    if Input.is_down("A") then
        player:move(-speed * dt, 0)
    end
    if Input.is_down("D") then
        player:move(speed * dt, 0)
    end
end

function on_draw()
    Graphics.clear(0, 0, 0)
    Graphics.rect(player:x(), player:y(), 32, 32)
end