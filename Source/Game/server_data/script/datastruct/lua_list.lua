---------------------------------------------------------
--	脚本数据结构实现
---------------------------------------------------------

---------------------------------------------------------
-- List
---------------------------------------------------------
List = {}
function List.new()
	return {first = 0, last = -1}
end


function List.pushfirst(list, value)
	list.first = list.first - 1
	list[list.first] = value
end


function List.pushlast(list, value)
	list.last = list.last + 1
	list[list.last] = value
end


function List.popfirst(list)
	if list.first > list.last then		-- 空表返回nil
		return nil
	end

	local value = list[list.first]
	list.first = list.first + 1
	return value
end


function List.poplast(list)
	if list.first > list.last then		-- 空表返回nil
		return nil
	end

	local value = list[list.last]
	list.last = list.last - 1
	return value
end