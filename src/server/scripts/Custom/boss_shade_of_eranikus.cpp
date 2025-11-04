#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

enum Spells
{
    SPELL_BELLOWING_ROAR            = 700000,
    SPELL_LETHARGIC_POISON          = 700001,
    SPELL_THRASH                    = 3391,
    SPELL_TAIL_SWEEP                = 700002,
    SPELL_CORROSIVE_BREATH          = 700003,
    SPELL_DEEP_SLUMBER              = 700004,
    SPELL_DEEP_SLUMBER_TRIGGER      = 700005,
    SPELL_DEEP_SLUMBER_SELF         = 700006,
    SPELL_WAKING_NIGHTMARE          = 700007,
    SPELL_DEEP_SLUMBER_CLOUD        = 700008
};

enum NPCs
{
    NPC_SHADE_OF_ERANIKUS       = 75709,
    NPC_LUMBERING_DREAMWALKER   = 170000,
    NPC_NIGHTMARE_SCALEBANE     = 170001,
    NPC_NIGHTMARE_WHELPLING     = 170002
};

enum Events
{
    EVENT_BELLOWING_ROAR            = 1,
    EVENT_LETHARGIC_POISON          = 2,
    EVENT_THRASH                    = 3,
    EVENT_TAIL_SWEEP                = 4,
    EVENT_CORROSIVE_BREATH          = 5,
    EVENT_DEEP_SLUMBER              = 6,
    EVENT_WAKING_NIGHTMARE          = 7,
    EVENT_NIGHTMARE_WHELPLINGS      = 8,
    EVENT_NIGHTMARE_SCALEBANES      = 9
};

enum Phases
{
    PHASE_ONE       = 1,
    PHASE_TWO       = 2,
    PHASE_THREE     = 3
};

enum Data
{
    DATA_BOSS_ERENIKUS = 7
};

class boss_shade_of_eranikus : public BossAI
{
public:
    boss_shade_of_eranikus(Creature* creature) : BossAI(creature, DATA_BOSS_ERENIKUS), _wakingNightmare(false) { }

    void Reset() override
    {
        summons.DespawnAll();
        summons.clear();

        _events.Reset();
        _events.SetPhase(PHASE_ONE);

        _events.ScheduleEvent(EVENT_TAIL_SWEEP, 4s, 7s);
        _events.ScheduleEvent(EVENT_THRASH, 6s, 11s);
        _events.ScheduleEvent(EVENT_CORROSIVE_BREATH, 5s, 9s);
        _events.ScheduleEvent(EVENT_LETHARGIC_POISON, 2s, 7s);
        _events.ScheduleEvent(EVENT_BELLOWING_ROAR, 8s, 15s);
        //_events.ScheduleEvent(EVENT_DEEP_SLUMBER, 4s, 7s);
        //_events.ScheduleEvent(EVENT_WAKING_NIGHTMARE, 12s, 18s);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPctDamaged(70, damage) && _events.IsInPhase(PHASE_ONE))
        {
            _events.SetPhase(PHASE_TWO);
            me->CastStop();
            DoCastSelf(SPELL_DEEP_SLUMBER_SELF);
            Talk(0);
            _events.ScheduleEvent(EVENT_NIGHTMARE_WHELPLINGS, 2s, 5s);
            DoSummonLumberingDreamwalkers();
        }
        else if (me->HealthBelowPctDamaged(40, damage) && _events.IsInPhase(PHASE_TWO))
        {
            _events.SetPhase(PHASE_THREE);
            me->CastStop();
            DoCastSelf(SPELL_DEEP_SLUMBER_SELF);
            Talk(0);
            _events.ScheduleEvent(EVENT_NIGHTMARE_SCALEBANES, 2s, 5s);
            DoSummonLumberingDreamwalkers();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_BELLOWING_ROAR:
                DoCast(SPELL_BELLOWING_ROAR);
                _events.ScheduleEvent(EVENT_BELLOWING_ROAR, 11s, 18s);
                break;
            case EVENT_LETHARGIC_POISON:
                DoCast(SPELL_LETHARGIC_POISON);
                _events.ScheduleEvent(EVENT_LETHARGIC_POISON, 6s, 11s);
                break;
            case EVENT_THRASH:
                DoCastSelf(SPELL_THRASH);
                _events.ScheduleEvent(EVENT_THRASH, 7s, 11s);
                break;
            case EVENT_TAIL_SWEEP:
                DoCastVictim(SPELL_TAIL_SWEEP);
                _events.ScheduleEvent(EVENT_TAIL_SWEEP, 3s, 7s);
                break;
            case EVENT_CORROSIVE_BREATH:
                DoCastVictim(SPELL_CORROSIVE_BREATH);
                _events.ScheduleEvent(EVENT_CORROSIVE_BREATH, 5s, 10s);
                break;
            case EVENT_DEEP_SLUMBER:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0, true, false))
                    if (DoCast(target, SPELL_DEEP_SLUMBER) == SpellCastResult::SPELL_CAST_OK)
                        _wakingNightmare = true;
                _events.ScheduleEvent(EVENT_DEEP_SLUMBER, 6s, 9s);
                break;
            case EVENT_WAKING_NIGHTMARE:
                if (_wakingNightmare)
                    if (DoCast(SPELL_WAKING_NIGHTMARE) == SpellCastResult::SPELL_CAST_OK)
                        _wakingNightmare = false;
                _events.ScheduleEvent(EVENT_WAKING_NIGHTMARE, 11s, 18s);
                break;
            case EVENT_NIGHTMARE_WHELPLINGS:
                DoSummonNightmareWhelplings();
                _events.ScheduleEvent(EVENT_NIGHTMARE_WHELPLINGS, 15s, 20s);
                break;
            case EVENT_NIGHTMARE_SCALEBANES:
                DoSummonNightmareScalebanes();
                _events.ScheduleEvent(EVENT_NIGHTMARE_SCALEBANES, 15s, 20s, 0, PHASE_THREE);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

protected:
    bool _wakingNightmare;

private:
    EventMap _events;

    void DoSummonLumberingDreamwalkers()
    {
        for (uint8 i = 0; i < 2; ++i)
            if (Creature* summon = DoSummon(NPC_LUMBERING_DREAMWALKER, me, frand(-15.f, 15.f)))
                summons.Summon(summon);
    }

    void DoSummonNightmareWhelplings()
    {
        uint8 amount = _events.IsInPhase(PHASE_TWO) ? 3 : 3;
        for (uint8 i = 0; i < amount; ++i)
            if (Creature* summon = DoSummon(NPC_NIGHTMARE_WHELPLING, me, frand(-15.f, 15.f)))
                summons.Summon(summon);
    }

    void DoSummonNightmareScalebanes()
    {
        for (uint8 i = 0; i < 2; ++i)
            if (Creature* summon = DoSummon(NPC_NIGHTMARE_SCALEBANE, me, frand(-15.f, 15.f)))
                summons.Summon(summon);
    }
};

class npc_lumbering_dreamwalker : public ScriptedAI
{
public:
    npc_lumbering_dreamwalker(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();

        _events.ScheduleEvent(EVENT_LETHARGIC_POISON, 2s, 4s);
        _events.ScheduleEvent(EVENT_DEEP_SLUMBER, 3s, 7s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_LETHARGIC_POISON:
                DoCast(SPELL_LETHARGIC_POISON);
                _events.ScheduleEvent(EVENT_LETHARGIC_POISON, 6s, 12s);
                break;
            case EVENT_DEEP_SLUMBER:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random))
                    DoCast(target, SPELL_DEEP_SLUMBER);
                _events.ScheduleEvent(EVENT_DEEP_SLUMBER, 8s, 14s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

enum NightmareScalebane
{
    SPELL_ACID_RAIN = 700010,

    EVENT_ACID_RAIN = 1
};

class npc_nightmare_scalebane : public ScriptedAI
{
public:
    npc_nightmare_scalebane(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();

        _events.ScheduleEvent(EVENT_ACID_RAIN, 2s, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ACID_RAIN:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random))
                    DoCast(target, SPELL_ACID_RAIN);
                _events.ScheduleEvent(EVENT_ACID_RAIN, 5s, 12s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

enum NightmareWhelpling
{
    SPELL_ACID_SHOT = 700009,

    EVENT_ACID_SHOT = 1
};

class npc_nightmare_whelpling : public ScriptedAI
{
public:
    npc_nightmare_whelpling(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();

        _events.ScheduleEvent(EVENT_ACID_SHOT, 2s, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ACID_SHOT:
                DoCastVictim(SPELL_ACID_SHOT);
                _events.ScheduleEvent(EVENT_ACID_SHOT, 3s, 7s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

// 700004 - Deep Slumber
class spell_deep_slumber : public SpellScript
{
    PrepareSpellScript(spell_deep_slumber);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DEEP_SLUMBER, SPELL_DEEP_SLUMBER_CLOUD });
    }

    void ApplyCloud()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_DEEP_SLUMBER_CLOUD, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_deep_slumber::ApplyCloud);
    }
};

// 700005 - Deep Slumber (Trigger)
class spell_deep_slumber_trigger : public AuraScript
{
    PrepareAuraScript(spell_deep_slumber_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DEEP_SLUMBER_TRIGGER });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->SetControlled(true, UNIT_STATE_STUNNED);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->SetControlled(false, UNIT_STATE_STUNNED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_deep_slumber_trigger::OnApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_deep_slumber_trigger::OnRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 700006 - Deep Slumber (Self)
class spell_deep_slumber_self : public AuraScript
{
    PrepareAuraScript(spell_deep_slumber_self);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DEEP_SLUMBER_SELF });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->SetControlled(true, UNIT_STATE_STUNNED);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->SetControlled(false, UNIT_STATE_STUNNED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_deep_slumber_self::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_deep_slumber_self::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 700008 - Deep Slumber (Cloud)
class spell_deep_slumber_cloud : public AuraScript
{
    PrepareAuraScript(spell_deep_slumber_cloud);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DEEP_SLUMBER_TRIGGER, SPELL_DEEP_SLUMBER_CLOUD });
    }

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* target = GetTarget())
            if (!target->HasAura(SPELL_DEEP_SLUMBER_TRIGGER))
                target->CastSpell(target, SPELL_DEEP_SLUMBER_TRIGGER, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_deep_slumber_cloud::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 700007 - Waking Nightmare
class spell_waking_nightmare : public SpellScript
{
    PrepareSpellScript(spell_waking_nightmare);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WAKING_NIGHTMARE, SPELL_DEEP_SLUMBER_TRIGGER });
    }

    void RemoveDeepSlumber()
    {
        if (Unit* target = GetHitUnit())
            if (target->HasAura(SPELL_DEEP_SLUMBER_TRIGGER))
                target->RemoveAura(SPELL_DEEP_SLUMBER_TRIGGER);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_waking_nightmare::RemoveDeepSlumber);
    }
};

void AddSC_boss_shade_of_eranikus()
{
    RegisterCreatureAI(boss_shade_of_eranikus);
    RegisterCreatureAI(npc_lumbering_dreamwalker);
    RegisterCreatureAI(npc_nightmare_scalebane);
    RegisterCreatureAI(npc_nightmare_whelpling);
    RegisterSpellScript(spell_deep_slumber);
    RegisterSpellScript(spell_deep_slumber_trigger);
    RegisterSpellScript(spell_deep_slumber_self);
    RegisterSpellScript(spell_deep_slumber_cloud);
    RegisterSpellScript(spell_waking_nightmare);
}
