-- resorce.lua
InstanceTable = {}

function Luacall(sp, obj)
    if InstanceTable[sp]==nil then
        local str=""
        InstanceTable[sp], str = obj:new(sp)
        return str
    else
        return InstanceTable[sp]:Update()
    end
end


-- class Test
Test = {
    sp = 0,
    x = 0,
    y = 0,
    time = 0,

    new = function(self, sp)
        self.sp = sp
        self.x = 50
        self.y = 50
        SpriteXY(self.sp, self.x, self.y)
        return self, "created Test\n"
    end,

    Update = function(self)
        self.x = self.x + 0.5
        self.y = self.y + 0.5
        self.time = self.time+1
        SpriteXY(self.sp, self.x, self.y)
        SpriteUV(self.sp, math.floor((self.time%60)/15)*32, 0)
        return "x = ".. self.x .. ",y = " .. self.y .."\n"
    end,
}

-- テストオブジェクト呼び出し
function LuacallTest(sp)
    return Luacall(sp, Test)
end








