local circles = {}
local circle_radius = 18

function update(_dt)
    if rl.mouse_pressed(0) then
        local x = rl.mouse_x()
        local y = rl.mouse_y()
        table.insert(circles, { x = x, y = y, r = circle_radius })
    end
end

function draw()
    for _, circle in ipairs(circles) do
        rl.draw_circle(circle.x, circle.y, circle.r)
    end
end
