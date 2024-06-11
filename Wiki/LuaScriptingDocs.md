# Lua Scripting Documentation

## Basics

Create a .lua file and put it into a directory relative to your executable, in our case it would be `SpectralEditor/assets/`

The script must return a table. 
**You need to use this template to ensure the script works correctly !**

```lua
-- player script
Player = {}

function Player.OnCreate(self)
{
    -- OnCreate logic
}

function Player.OnUpdate(self, ts)
{
    -- OnUpdate logic
}

return Player
```

## Usage

### Entity

Each script does know their entity `self.owner` and can interact with them.
You can interact with the entity components by calling `has`, `get` or `remove`.

``` lua
-- example
function Player.OnCreate(self)
{
    -- check if we have Transform component and print something
    if self.owner:has(Transform) then
        print("player has Transform component")
    end
}

```

**For now we only support on component:**
- TransformComponent as `Transform`
- RigidBody2DComponent as `RigidBody2D`

But you can easily add more by including them in the `ScriptGlue` class.

### Physics 2D

You can access the rigid body by getting it from an entity's components using `owner.self:get(RigidBody2D)`

**exposed variables:**
 - `isAwake`
 - `gravityScale`

**exposed functions:**
- `applyImpulse(impulse, point, wake)`

``` lua
-- example
function Player.OnUpdate(self, ts)
    if self.owner:has(RigidBody2D) then
		local rigid = self.owner:get(RigidBody2D)
		if rigid.isAwake then
			rigid:applyImpulse(vector2(500.0, 0.0), vector2(0.0, 0.0), true)
		end
	end
end
```

### Input

To check the key detection you need to add `require "keyboard_keys"` package.

``` lua
-- example

require "keyboard_keys" -- include keyboard key table

function Player.OnUpdate(self, ts)
	if inputKeyPressed(KEY.A) then
		print("Key pressed")
	end

    if inputKeyDown(KEY.A) then
        print("Key down")
    end
end
```

You can go to `SpectralEditor/assets/lua_package/keyboard_keys.lua` to see available keys.