
#ifndef NPCBOTMETHODS_H
#define NPCBOTMETHODS_H

#include "bot_ai_eluna.h"

/***
 * Player controlled bot party members, relies on latest changes to
 * NPCBots fork of AzerothCore
 *
 * Inherits all methods from: [Object], [WorldObject], [Unit]
 */
namespace LuaNpcBot
{
    /**
     * Return `true` if the [Creature] is a NPCBot,
     * @return bool isNPCBot
     */
    int IsNPCBot(lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsNPCBot());
        return 1;
    }

    /**
     * Returns the [Creature]'s bot owner.
     * @return [Player] botOwner
     */
    int GetBotOwner(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        if (ai) {
            Eluna::Push(L, ai->GetBotOwner());
            return 1;
        }
        else {
            return 0;
        }
    }

    int GetBotOwnerGUID(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        if (ai) {
            Eluna::Push(L, ai->GetBotOwnerGuid());
            return 1;
        }
        else {
            return 0;
        }
    }

    /**
     * Returns the [Creature]'s bot class.
     * @return unint8 botClassID
     */
    int GetBotClass(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        Eluna::Push(L, creature->GetBotClass());
        return 1;
    }

    /**
     * Get the [Creature]'s bot roles Tank, Healer, Damage that are enabled as a mask.
     * @return uint32 botRoles
     */
    int GetBotRoles(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        Eluna::Push(L, creature->GetBotRoles());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is has an assigned role as tank.
     * @return bool isTank
     */
    int IsBotTank(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        Eluna::Push(L, ai->IsTank());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is has an assigned role as off-tank.
     * @return bool isTank
     */
    int IsBotOffTank(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        Eluna::Push(L, ai->IsOffTank());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is free and not assigned to a player
     * @return bool isFree
     */
    int IsFreeBot(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        Eluna::Push(L, creature->IsFreeBot());
        return 1;
    }

    /**
     * Get the [Creature]'s bot average item level of equipped items.
     * @return float botAverageItemLevel
     */
    int GetBotAverageItemLevel(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        Eluna::Push(L, creature->GetBotAverageItemLevel());
        return 1;
    }

    int GetBotEquipment(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        if(creature->IsFreeBot())
            return 0;

        uint32 slot = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, creature->GetBotEquips(slot));
        return 1;
    }

    int GetBotStat(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        uint8 botstat = Eluna::CHECKVAL<uint8>(L, 2);
        /**
         * @fix this to be optional for the bot owner to see the stats
         */

        bot_ai* ai = creature->GetBotAI();
        Eluna::Push(L, ai->GetTotalBotStat(BotStatMods(botstat)));

        return 1;
    }

    int GetTalentSpec(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        Eluna::Push(L, ai->GetSpec());
        return 1;
    }

    int BotEquipItem(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        // If this bot is not owned by a player return.
        if(creature->IsFreeBot())
            return 0;

        // if the entry passed in was an item object
        Item* item = Eluna::CHECKOBJ<Item>(L, 2, false);
        uint32 slot = Eluna::CHECKVAL<uint32>(L, 3);
        Player* owner = creature->GetBotOwner();

        // If an item entry was passed in instead
        if(!item)
        {
            uint32 itemid = Eluna::CHECKVAL<uint32>(L, 2);
            item = owner->GetItemByEntry(itemid);
        }

        if(slot > EQUIPMENT_SLOT_END)
            return 0;

        bot_ai* ai = creature->GetBotAI();
        BotAIEluna elunaAi(ai);
        bool result = elunaAi.Equip(slot, item, owner->GetGUID());

        Eluna::Push(L, result);
        return 1;
    }

    int BotCanEquipItem(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        // If this bot is not owned by a player return.
        if(creature->IsFreeBot())
            return 0;

        // if the entry passed in was an item object
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 slot = Eluna::CHECKVAL<uint32>(L, 3);

        // If an item entry was passed in instead
        if(entry)
        {
            const ItemTemplate* proto = eObjectMgr->GetItemTemplate(entry);
            if(!proto)
                return luaL_argerror(L, 1, "valid ItemEntry expected in BotCanEquipItem");

            if(slot > EQUIPMENT_SLOT_END)
                return luaL_argerror(L, 1, "value slot expected in BotCanEquipItem");

            bot_ai* ai = creature->GetBotAI();
            bool result = BotAIEluna(ai).CanEquip(proto, slot, true);
            Eluna::Push(L, result);

        }
        return 1;
    }

    int BotUnequipItem(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        // If this bot is not owned by a player return.
        if(creature->IsFreeBot())
            return 0;

        uint32 slot = Eluna::CHECKVAL<uint32>(L, 2);
        Player* owner = creature->GetBotOwner();

        bot_ai* ai = creature->GetBotAI();
        BotAIEluna elunaAi(ai);
        bool result = elunaAi.Unequip(slot, owner->GetGUID());
        Eluna::Push(L, result);
        return 1;
    }

    int GetBotDump(lua_State* L, Creature* creature)
    {
        if(!creature->IsNPCBot())
            return 0;

        bot_ai* ai = creature->GetBotAI();
        Player* owner = creature->GetBotOwner();
        BotAIEluna elunaAi(ai);
        std::string dump = elunaAi.BotDump(owner, creature);

        if(dump.empty())
            return luaL_argerror(L, 1, "BotDump failed for bot.");

        Eluna::Push(L, dump);
        return 1;
    }

}

#endif // NPCBOTMETHODS_H
