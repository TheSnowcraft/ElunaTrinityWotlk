#include "CreatureData.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"

std::map<ObjectGuid, uint32> _PlayersModelReplaceMap;

class player_model_replace : PlayerScript
{
public:
    player_model_replace() : PlayerScript("player_model_replace") { }

    void OnPlayerInitDisplayID(Player* player) override
    {
        auto itr = _PlayersModelReplaceMap.find(player->GetGUID());
        if (itr == _PlayersModelReplaceMap.end())
            return;

        player->SetDisplayId(itr->second);
        player->SetNativeDisplayId(itr->second);
    }
};

class player_model_replace_worldscript : WorldScript
{
public:
    player_model_replace_worldscript() : WorldScript("player_model_replace_worldscript") { }

    void OnConfigLoad(bool /*reload*/) override
    {
        uint32 oldMSTime = getMSTime();

        _PlayersModelReplaceMap.clear();

        QueryResult result = CharacterDatabase.Query("SELECT guid, displayID FROM character_model_replace");
        if (!result)
            return;

        uint32 count = 0;
        do
        {
            Field* fields = result->Fetch();

            ObjectGuid guid     = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            uint32 displayID    = fields[1].GetUInt32();

            _PlayersModelReplaceMap[guid] = displayID;
            ++count;
        }
        while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded {} Player Model Replace entries in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
    }
};

void AddSC_player_model_replace()
{
    new player_model_replace();
    new player_model_replace_worldscript();
}
