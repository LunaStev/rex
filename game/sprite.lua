player_tex = nil

function on_start()
    player_tex = Texture.load("assets/player.png")
end

function on_draw()
    Graphics.clear(0, 0, 0)
    Graphics.sprite(player_tex, player:x(), player:y())
end