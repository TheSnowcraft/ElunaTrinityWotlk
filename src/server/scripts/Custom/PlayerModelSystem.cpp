#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "PlayerModelSystem.h"
#include "Log.h"


namespace
{
    std::unordered_map<ObjectGuid, uint32> playersModelReplaceMap_;
}

namespace
{
    void SetDisplayId(Player* player, const uint32 displayId)
    {
        player->SetDisplayId(displayId);
        player->SetNativeDisplayId(displayId);
    }
    
    void UpdateAllModels()
    {
        for (const auto& [guid, displayId] : playersModelReplaceMap_)
        {
            if (Player* player = ObjectAccessor::FindPlayer(guid))
            {
                SetDisplayId(player, displayId);
            }
        }
    }
}

void PlayerModelSystem::LoadFromDB(bool reload /*= false*/)
{
    playersModelReplaceMap_.clear();
    
    const uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT guid, displayID FROM character_model_replace");
    if (!result)
        return;

    do
    {
        const Field* fields = result->Fetch();

        const ObjectGuid guid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
        const uint32 displayID = fields[1].GetUInt32();

        playersModelReplaceMap_[guid] = displayID;
    } while (result->NextRow());

    if (reload)
    {
        UpdateAllModels();
    }
    
    TC_LOG_INFO("server.loading", ">> Loaded {} Player Model Replace entries in {} ms", playersModelReplaceMap_.size(), GetMSTimeDiffToNow(oldMSTime));
}

class PlayerModelScript : PlayerScript
{
public:
    PlayerModelScript() : PlayerScript("PlayerModelScript") {}

    void OnLogin(Player* player, bool /*= firstLogin*/) override
    {
        auto itr = playersModelReplaceMap_.find(player->GetGUID());
        if (itr == playersModelReplaceMap_.end())
            return;

        SetDisplayId(player, itr->second);
    }
};

void AddSC_PlayerModelScript()
{
    new PlayerModelScript();
}
