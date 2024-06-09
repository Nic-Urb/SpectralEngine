Player = {}

function Player.OnCreate(self)
	print("Created Player Script")
	self.time = 0
	print("Time : ", self.time)
end

require "keyboard_keys" -- include keyboard key table

function Player.OnUpdate(self, ts)
	
	local speed = 10
	local transform = self.owner:get(Transform)

	if inputKeyDown(KEY.W) then
		transform.translation.z = transform.translation.z - speed * ts
	end

	if inputKeyDown(KEY.S) then
		transform.translation.z = transform.translation.z + speed * ts
	end

	if inputKeyDown(KEY.A) then
		transform.translation.x = transform.translation.x - speed * ts
	end

	if inputKeyDown(KEY.D) then
		transform.translation.x = transform.translation.x + speed * ts
	end

end

return Player