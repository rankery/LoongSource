--���ﺰ���ű�

-- �������ս������
function x1004313_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%���ʺ���
             cre.MonsterSay(MapID, InstanceID, CreatureID, 10038)
       end	
end

aux.RegisterCreatureEvent(1004313, 2, "x1004313_OnEnterCombat")
