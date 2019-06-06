--怪物相关全局函数定义
--天地龙脉传送人(3011065)     1		用以记录刷出他的玩家的ID


--同步周围怪物仇恨
function g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, EnmityValue, Radius, High)
    --获取怪物周围的其它怪物ID
    local AroundCreatureID = {}
    AroundCreatureID = cre.GetAroundCreature(MapID, InstanceID, CreatureID, Radius, High)
    for n in pairs(AroundCreatureID) do
        --得到怪物和目标相互间的敌我关系
        local Relation = unit.FriendEnemy(MapID, InstanceID, TargetID, AroundCreatureID[n])
        --判断怪物和目标为敌方
        if Relation and Relation == 2 then
            --添加仇恨
            cre.AddEnmity(MapID, InstanceID, AroundCreatureID[n], TargetID, EnmityValue)
        end    
    end    
end

--[[-- 应用实例(进入战斗时触发）
function c1008121_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为40个格子，高度为20的有方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 40, 20)
end

aux.RegisterCreatureEvent(1008121, 2, "c1008121_OnEnterCombat")]]
























