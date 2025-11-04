#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "EventMap.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "SpellAuras.h"

enum Spells
{
    // Hakkari Bloodkeeper
    SPELL_BLOOD_AURA                = 700101,
    SPELL_TIDES_OF_BLOOD            = 700102,
    SPELL_SPIRIT_CHAINS             = 700103,
    SPELL_SPIRIT_CHAINS_VISUAL      = 700104,
    SPELL_FRIGHTSOME_HOWL           = 700105,
    SPELL_BUBBLING_BLOOD            = 700106,
    SPELL_BUBBLING_BLOOD_TRIGGER    = 700108,
    SPELL_BUBBLING_BLOOD_DAMAGE     = 700109,

    // Avatar of Hakkar
    SPELL_BLOOD_SIPHON              = 700110,
    SPELL_CURSE_OF_TONGUES          = 700111,
    SPELL_CORRUPTED_BLOOD           = 700112,
    SPELL_INSANITY                  = 700113,
    SPELL_BLOOD_NOVA                = 700114,
    SPELL_DRAIN_BLOOD               = 700115,
    SPELL_DRAIN_BLOOD_DAMAGE        = 700116,
    SPELL_DRAIN_BLOOD_VISUAL        = 700117,
    SPELL_SKELETAL                  = 700118,
};

enum NPCs
{
    NPC_HEROIC_HAKKARI_BLOODKEEPER     = 78438,
    NPC_HEROIC_ATAL_AI_RITUALIST       = 170003,
    NPC_HEROIC_AVATAR_OF_HAKKAR        = 78443,
};

enum Events
{
    // Hakkari Bloodkeeper
    EVENT_BLOODKEEPER_SPIRIT_CHAINS         = 1,
    EVENT_BLOODKEEPER_FRIGHTSTOME_HOWL      = 2,
    EVENT_BLOODKEEPER_BUBBLING_BLOOD        = 3,

    // Avatar of Hakkar
    EVENT_HAKKAR_BLOOD_SIPHON               = 1,
    EVENT_HAKKAR_PHASE_ONE                  = 2,
    EVENT_HAKKAR_CURSE_OF_TONGUES           = 3,
    EVENT_HAKKAR_CORRUPTED_BLOOD            = 4,
    EVENT_HAKKAR_INSANITY                   = 5,
    EVENT_HAKKAR_BLOOD_NOVA                 = 6,
    EVENT_HAKKAR_DRAIN_BLOOD                = 7,
};

enum Phases
{
    // Hakkari Bloodkeeper
    BLOODKEEPER_PHASE_NONE  = 1,
    BLOODKEEPER_PHASE_ONE   = 2,

    // Avatar of Hakkar
    HAKKAR_PHASE_NONE       = 1,
    HAKKAR_PHASE_ONE        = 2,
};

enum Data
{
    DATA_BOSS_AVATAR_OF_HAKKAR = 9
};

enum Texts
{
    HAKKAR_SAY_1    = 0,
    HAKKAR_SAY_2    = 1,
    HAKKAR_SAY_3    = 2,
    HAKKAR_SAY_4    = 3,
};

class boss_avatar_of_hakkar : public BossAI
{
public:
    boss_avatar_of_hakkar(Creature* creature) : BossAI(creature, DATA_BOSS_AVATAR_OF_HAKKAR) { }

    void Reset() override
    {
        _events.Reset();
        _events.SetPhase(HAKKAR_PHASE_NONE);

        _events.ScheduleEvent(EVENT_HAKKAR_BLOOD_SIPHON, 300ms);
        _events.ScheduleEvent(EVENT_HAKKAR_PHASE_ONE, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_HAKKAR_BLOOD_SIPHON:
                if (Creature* bloodkeeper = GetClosestCreatureWithEntry(me, NPC_HEROIC_HAKKARI_BLOODKEEPER, 100.f))
                    DoCast(bloodkeeper, SPELL_BLOOD_SIPHON, true);
                _events.ScheduleEvent(EVENT_HAKKAR_BLOOD_SIPHON, 300ms);
                break;
            case EVENT_HAKKAR_PHASE_ONE:
                BeginPhaseOne();
                break;
            case EVENT_HAKKAR_CURSE_OF_TONGUES:
                DoCast(SPELL_CURSE_OF_TONGUES);
                _events.ScheduleEvent(EVENT_HAKKAR_CURSE_OF_TONGUES, 9s, 14s);
                break;
            case EVENT_HAKKAR_CORRUPTED_BLOOD:
                DoCast(SPELL_CORRUPTED_BLOOD);
                _events.ScheduleEvent(EVENT_HAKKAR_CORRUPTED_BLOOD, 6s, 9s);
                break;
            case EVENT_HAKKAR_INSANITY:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.f, true, false))
                {
                    DoCast(target, SPELL_INSANITY);
                    Talk(HAKKAR_SAY_3);
                }
                _events.ScheduleEvent(EVENT_HAKKAR_INSANITY, 10s, 13s);
                break;
            case EVENT_HAKKAR_BLOOD_NOVA:
                DoCast(SPELL_BLOOD_NOVA);
                _events.ScheduleEvent(EVENT_HAKKAR_BLOOD_NOVA, 3s, 6s);
                break;
            case EVENT_HAKKAR_DRAIN_BLOOD:
                DoCastSelf(SPELL_DRAIN_BLOOD_VISUAL, true);
                DoCastSelf(SPELL_DRAIN_BLOOD);
                Talk(HAKKAR_SAY_4);
                _events.ScheduleEvent(EVENT_HAKKAR_DRAIN_BLOOD, 12s, 15s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;

    void BeginPhaseOne()
    {
        _events.CancelEvent(EVENT_HAKKAR_BLOOD_SIPHON);
        _events.SetPhase(HAKKAR_PHASE_ONE);

        me->SetImmuneToAll(false);
        me->SetControlled(false, UNIT_STATE_STUNNED);

        _events.ScheduleEvent(EVENT_HAKKAR_CURSE_OF_TONGUES, 9s, 14s);
        _events.ScheduleEvent(EVENT_HAKKAR_CORRUPTED_BLOOD, 5s, 9s);
        _events.ScheduleEvent(EVENT_HAKKAR_INSANITY, 10s, 13s);
        _events.ScheduleEvent(EVENT_HAKKAR_BLOOD_NOVA, 2s, 4s);
        _events.ScheduleEvent(EVENT_HAKKAR_DRAIN_BLOOD, 12s, 16s);

        Talk(HAKKAR_SAY_2);
    }
};

class npc_hakkari_bloodkeeper : public ScriptedAI
{
public:
    npc_hakkari_bloodkeeper(Creature* creature) : ScriptedAI(creature), _checkManaTimer(0) { }

    void BeginPhaseOne()
    {
        _events.SetPhase(BLOODKEEPER_PHASE_ONE);

        ResetManaPool();
        me->RemoveAura(SPELL_BLOOD_AURA);
        me->AddAura(SPELL_TIDES_OF_BLOOD, me);

        me->SetControlled(false, UNIT_STATE_STUNNED);

        _events.ScheduleEvent(EVENT_BLOODKEEPER_SPIRIT_CHAINS, 5s, 8s);
        _events.ScheduleEvent(EVENT_BLOODKEEPER_FRIGHTSTOME_HOWL, 8s, 10s);
        _events.ScheduleEvent(EVENT_BLOODKEEPER_BUBBLING_BLOOD, 1s, 3s);
    }

    void Reset() override
    {
        _events.Reset();
        _events.SetPhase(BLOODKEEPER_PHASE_NONE);

        me->AddAura(SPELL_BLOOD_AURA, me);
        me->SetControlled(true, UNIT_STATE_STUNNED);

        ResetManaPool();
    }

    void UpdateAI(uint32 diff) override
    {
        if (_events.IsInPhase(BLOODKEEPER_PHASE_NONE))
            return;

        if (!UpdateVictim())
            return;

        _checkManaTimer += diff;
        if (_checkManaTimer >= 1000)
        {
            if (me->GetPowerPct(POWER_MANA) >= 100.f)
                HandleFullMana();
            else
                _checkManaTimer = 0;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_BLOODKEEPER_SPIRIT_CHAINS:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.f, true, false))
                    DoCast(target, SPELL_SPIRIT_CHAINS);
                _events.ScheduleEvent(EVENT_BLOODKEEPER_SPIRIT_CHAINS, 5s, 8s);
                break;
            case EVENT_BLOODKEEPER_FRIGHTSTOME_HOWL:
                DoCast(SPELL_FRIGHTSOME_HOWL);
                _events.ScheduleEvent(EVENT_BLOODKEEPER_FRIGHTSTOME_HOWL, 8s, 10s);
                break;
            case EVENT_BLOODKEEPER_BUBBLING_BLOOD:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.f, true, false))
                    DoCast(target, SPELL_BUBBLING_BLOOD);
                _events.ScheduleEvent(EVENT_BLOODKEEPER_BUBBLING_BLOOD, 3s, 5s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        me->RemoveAura(SPELL_TIDES_OF_BLOOD);

        if (why != EvadeReason::EVADE_REASON_NO_HOSTILES)
            return;

        if (me->FindNearestCreature(NPC_HEROIC_ATAL_AI_RITUALIST, 100.f))
            return;

        me->DespawnOrUnsummon(1s);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (_events.IsInPhase(BLOODKEEPER_PHASE_NONE))
            damage = 0;
    }

protected:
    uint32 _checkManaTimer;

private:
    EventMap _events;

    void HandleFullMana()
    {
        _events.Reset();
        _events.SetPhase(BLOODKEEPER_PHASE_NONE);
        me->SetControlled(true, UNIT_STATE_STUNNED);

        Position pos(*me);
        const float randPos = frand(-7.f, 7.f);
        pos.m_positionX += randPos;
        pos.m_positionY += randPos;

        if (Creature* hakkar = DoSummon(NPC_HEROIC_AVATAR_OF_HAKKAR, pos, 10min))
        {
            if (CreatureAI* _ai = hakkar->AI())
                _ai->Talk(HAKKAR_SAY_1);

            hakkar->SetImmuneToAll(true);
            hakkar->SetControlled(true, UNIT_STATE_STUNNED);
        }

        me->DespawnOrUnsummon(7s);
    }

    void ResetManaPool() { me->SetPower(POWER_MANA, 0, true, true); }
};

class npc_atal_ai_ritualist : public ScriptedAI
{
public:
    npc_atal_ai_ritualist(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetControlled(true, UNIT_STATE_ROOT);
    }

    void JustEngagedWith(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->FindNearestCreature(NPC_HEROIC_ATAL_AI_RITUALIST, 100.f))
            return;

        if (Creature* bloodkeeper = me->FindNearestCreature(NPC_HEROIC_HAKKARI_BLOODKEEPER, 100.f))
        {
            npc_hakkari_bloodkeeper* AI = static_cast<npc_hakkari_bloodkeeper*>(bloodkeeper->AI());
            if (!AI)
            {
                bloodkeeper->DespawnOrUnsummon();
                return;
            }

            AI->BeginPhaseOne();
        }
    }
};

// 700103 - Spirit Chains
class spell_spirit_chains : public AuraScript
{
    PrepareAuraScript(spell_spirit_chains);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SPIRIT_CHAINS_VISUAL });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->CastSpell(target, SPELL_SPIRIT_CHAINS_VISUAL, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->RemoveAura(SPELL_SPIRIT_CHAINS_VISUAL);
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* dispelledUnit = GetUnitOwner())
        {
            std::vector<Player*> players;
            dispelledUnit->GetPlayerListInGrid(players, 10.f);

            for (Player* player : players)
            {
                if (player == dispelledUnit)
                    continue;

                player->AddAura(m_scriptSpellId, player);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_spirit_chains::OnApply, EFFECT_1, SPELL_AURA_MOD_ATTACKSPEED, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_spirit_chains::OnRemove, EFFECT_1, SPELL_AURA_MOD_ATTACKSPEED, AURA_EFFECT_HANDLE_REAL);
        AfterDispel += AuraDispelFn(spell_spirit_chains::HandleDispel);
    }
};

// 700108 - Bubbling Blood
class spell_bubbling_blood : public SpellScript
{
    PrepareSpellScript(spell_bubbling_blood);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BUBBLING_BLOOD_DAMAGE });
    }

    void HandleDummyHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            Unit* caster = target->FindNearestCreature(NPC_HEROIC_HAKKARI_BLOODKEEPER, 100.f);
            if (!caster)
                caster = target->FindNearestCreature(NPC_HEROIC_AVATAR_OF_HAKKAR, 100.f);

            if (caster)
                caster->CastSpell(target, SPELL_BUBBLING_BLOOD_DAMAGE, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bubbling_blood::HandleDummyHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 700112 - Corrupted Blood
class spell_corrupted_blood : public AuraScript
{
    PrepareAuraScript(spell_corrupted_blood);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();
        if (!caster || caster->isDead())
        {
            GetAura()->Remove();
            return;
        }

        if (!roll_chance_f(30.f))
            return;

        if (Unit* target = GetTarget())
        {
            std::vector<Player*> players;
            target->GetPlayerListInGrid(players, 10.f);

            for (Player* player : players)
                caster->AddAura(SPELL_CORRUPTED_BLOOD, player);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_corrupted_blood::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 700115 - Drain Blood
class spell_drain_blood : public SpellScript
{
    PrepareSpellScript(spell_drain_blood);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRAIN_BLOOD_VISUAL });
    }

    void HandleDummyHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_DRAIN_BLOOD_VISUAL))
                caster->RemoveAura(SPELL_DRAIN_BLOOD_VISUAL);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_drain_blood::HandleDummyHitTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 700116 - Drain Blood (Damage)
class spell_drain_blood_damage : public SpellScript
{
    PrepareSpellScript(spell_drain_blood_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CORRUPTED_BLOOD, SPELL_SKELETAL });
    }

    void HandleDummyHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (target->HasAura(SPELL_CORRUPTED_BLOOD))
                {
                    target->RemoveAura(SPELL_CORRUPTED_BLOOD);
                    caster->CastSpell(target, SPELL_SKELETAL, true);
                }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_drain_blood_damage::HandleDummyHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class gameobject_pool_of_blood : public GameObjectAI
{
public:
    gameobject_pool_of_blood(GameObject* go) : GameObjectAI(go) { }

    void InitializeAI() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_TRIGGER_BUBBLING_BLOOD, 1s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_TRIGGER_BUBBLING_BLOOD:
                me->CastSpell(nullptr, SPELL_BUBBLING_BLOOD_TRIGGER);
                _events.ScheduleEvent(EVENT_TRIGGER_BUBBLING_BLOOD, 1s);
                break;
            default:
                break;
            }
        }
    }

private:
    EventMap _events;

    enum Events
    {
        EVENT_TRIGGER_BUBBLING_BLOOD = 1,
    };
};

void AddSC_boss_avatar_of_hakkar()
{
    RegisterCreatureAI(boss_avatar_of_hakkar);
    RegisterCreatureAI(npc_hakkari_bloodkeeper);
    RegisterCreatureAI(npc_atal_ai_ritualist);
    RegisterSpellScript(spell_spirit_chains);
    RegisterSpellScript(spell_bubbling_blood);
    RegisterSpellScript(spell_corrupted_blood);
    RegisterSpellScript(spell_drain_blood);
    RegisterSpellScript(spell_drain_blood_damage);
    RegisterGameObjectAI(gameobject_pool_of_blood);
}
