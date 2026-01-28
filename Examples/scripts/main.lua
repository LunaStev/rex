local rex = require("rex")

local player = nil

function onEnter()
  rex.log("enter")
  rex.world.generate_flat(40, 20, 14)
  player = rex.entity.spawn_player(120, 120)
end

function update(dt)
  local vx, vy = 0, 0
  if rex.input.held(rex.key.A) or rex.input.held(rex.key.LEFT) then vx = vx - 300 end
  if rex.input.held(rex.key.D) or rex.input.held(rex.key.RIGHT) then vx = vx + 300 end

  rex.entity.set_velocity(player, vx, vy)

  if rex.input.pressed(rex.key.SPACE) then
    rex.audio.play_sound("assets/jump.wav", 0)
  end
end

function render()
  rex.gfx.clear(0,0,0,255)
  rex.world.render()

  rex.text.draw("assets/fonts/rex_engine.ttf", 24, "hello lua", 20, 20, 255,255,255,255)

  -- 이미지
  rex.gfx.image("assets/textures/player.png", 300, 200)
end

function onExit()
  rex.log("exit")
end
