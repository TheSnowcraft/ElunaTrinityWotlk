#include "CreatureData.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
 
namespace
{
    std::unordered_map<ObjectGuid, uint32> _PlayersModelReplaceMap;
}
 
namespace
{
    void UpdateDisplayIds(Player* player, const uint32 displayId)
    {
        player->SetDisplayId(displayId);
        player->SetNativeDisplayId(displayId);
    }
}
 
class player_model_replace : PlayerScript
{
public:
    player_model_replace() : PlayerScript("player_model_replace") {}
 
    void OnPlayerInitDisplayID(Player* player) override
    {
        auto itr = _PlayersModelReplaceMap.find(player->GetGUID());
        if (itr == _PlayersModelReplaceMap.end())
            return;
 
        UpdateDisplayIds(player, itr->second);
    }
};
 
class player_model_replace_worldscript : WorldScript
{
public:
    player_model_replace_worldscript() : WorldScript("player_model_replace_worldscript") {}
 
    void OnConfigLoad(bool reload) override
    {
        _PlayersModelReplaceMap.clear();
 
        const uint32 oldMSTime = getMSTime();
 
        QueryResult result = CharacterDatabase.Query("SELECT guid, displayID FROM character_model_replace");
        if (!result)
            return;
 
        do
        {
            const Field* fields = result->Fetch();
 
            const ObjectGuid guid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            const uint32 displayID = fields[1].GetUInt32();
 
            _PlayersModelReplaceMap[guid] = displayID;
 
            // when reloading send a displayId update to player
            if (reload)
            {
                if (Player* player = ObjectAccessor::FindPlayer(guid))
                {
                    UpdateDisplayIds(player, displayID);
                }
            }
        } while (result->NextRow());
 
        TC_LOG_INFO("server.loading", ">> Loaded {} Player Model Replace entries in {} ms", _PlayersModelReplaceMap.size(), GetMSTimeDiffToNow(oldMSTime));
    }
};
 
void AddSC_player_model_replace()
{
    new player_model_replace();
    new player_model_replace_worldscript();
}
