print("Hello Rex!")

function on_start()
    print("Game started")
end

function on_update(dt)
    -- Update game logic here
end

function on_draw()
    Graphics.clear(0.1, 0.1, 0.15)
    Graphics.text(20, 20, "Rex Game!", 24)
end