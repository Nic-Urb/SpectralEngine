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
- TransformComponent (`Transform`)

But you can easily add more by including them in the `ScriptGlue` class.


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