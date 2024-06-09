Entity = {}

function Entity.OnCreate()
	print("Created Entity Script")
end

function Entity.OnUpdate(ts)
	print("Updating Entity")
	print("[Lua] Updated! ", ts)
end

return Entity