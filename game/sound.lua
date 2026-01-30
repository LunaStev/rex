jump = nil

function on_start()
    jump = Audio.load("assets/jump.wav")
end

function on_key_down(key)
    if key == "SPACE" then
        Audio.play(jump)
    end
end