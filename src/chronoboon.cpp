/*
 * This file is part of mod-chronoboon module for AzerothCore project.
 */

#include "Player.h"
#include "SharedDefines.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include <map>

enum PlayerSpecializationIconicSpells
{
    /* Warrior - Protection */
    SPELL_VIGILANCE                         = 50720,
    SPELL_DEVASTATE                         = 20243,
    SPELL_SHOCKWAVE                         = 46968,

    /* Paladin - Holy*/
    PASSIVE_ILLUMINATION                    = 20215,
    SPELL_HOLY_SHOCK                        = 20473,
    SPELL_BEACON_OF_LIGHT                   = 53563,

    /* Paladin - Protection */
    SPELL_BLESS_OF_SANC                     = 20911,
    SPELL_HOLY_SHIELD                       = 20925,
    SPELL_HAMMER_OF_RIGHTEOUS               = 53595,

    /* Priest - Holy */
    PASSIVE_SPIRIT_REDEMPTION               = 20711,
    SPELL_DESPERATE_PRAYER                  = 19236,
    SPELL_GUARDIAN_SPIRIT                   = 47788,

    /* Death Knight - Blood */
    SPELL_RUNE_TAP                          = 48982,
    SPELL_HYSTERIA                          = 49016,
    SPELL_HEART_STRIKE                      = 55050,

    /* Shaman - Enhancement */
    PASSIVE_SPIRIT_WEAPONS                  = 16268,
    SPELL_LAVA_LASH                         = 60103,
    SPELL_FERAL_SPIRIT                      = 51533,

    /* Mage - Arcane */
    SPELL_FOCUS_MAGIC                       = 54646,
    SPELL_ARCANE_POWER                      = 12042,
    SPELL_ARCANE_BARRAGE                    = 44425,

    /* Druid - Feral */
    SPELL_SURVIVAL_INSTINCTS                = 61336,
    SPELL_MANGLE                            = 33917,
    SPELL_BERSERK                           = 50334
};

enum WellFedBuffs
{
    SPELL_WELL_FED_2_STAMINA_2_SPIRIT       = 19705,
    SPELL_WELL_FED_4_STAMINA_4_SPIRIT       = 19706,
    SPELL_WELL_FED_6_STAMINA_6_SPIRIT       = 19708,
    SPELL_WELL_FED_8_STAMINA_8_SPIRIT       = 19709,
    SPELL_WELL_FED_12_STAMINA_12_SPIRIT     = 19710,
    SPELL_WELL_FED_30_STAMINA_20_SPIRIT     = 33257,
    SPELL_WELL_FED_80_AP_40_SP_40_STAMINA   = 57399
};

enum WorldBuffs
{
    SPELL_WARCHIEFS_BLESSING                = 90004,
    SPELL_MIGHT_OF_STORMWIND                = 90005,
    SPELL_RALLYING_CRY_OF_THE_DRAGONSLAYER  = 90006,
    SPELL_SPIRIT_OF_ZANDALAR                = 90007
};

enum ElixirBuffs
{
    SPELL_ELIXIR_OF_EXPERTISE               = 60344,
    SPELL_ELIXIR_OF_PROTECTION              = 53763,
    SPELL_ELIXIR_OF_MIGHTY_THOUGHTS         = 60347,
    SPELL_GURUS_ELIXIR                      = 53749,
    SPELL_GIFT_OF_ARTHAS                    = 11371,
    SPELL_FLASK_OF_ENDLESS_RAGE             = 53760,
    SPELL_FLASK_OF_STONEBLOOD               = 53758,
    SPELL_FLASK_OF_DISTILLED_WISDOM         = 17627,
    SPELL_FLASK_OF_FROST_WYRM               = 53755
};

class spell_item_chronoboon final : public SpellScript
{
    PrepareSpellScript(spell_item_chronoboon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARCHIEFS_BLESSING,
            SPELL_MIGHT_OF_STORMWIND,
            SPELL_RALLYING_CRY_OF_THE_DRAGONSLAYER,
            SPELL_SPIRIT_OF_ZANDALAR,
            SPELL_WELL_FED_2_STAMINA_2_SPIRIT,
            SPELL_WELL_FED_4_STAMINA_4_SPIRIT,
            SPELL_WELL_FED_6_STAMINA_6_SPIRIT,
            SPELL_WELL_FED_8_STAMINA_8_SPIRIT,
            SPELL_WELL_FED_12_STAMINA_12_SPIRIT,
            SPELL_WELL_FED_30_STAMINA_20_SPIRIT,
            SPELL_WELL_FED_80_AP_40_SP_40_STAMINA,
            SPELL_ELIXIR_OF_EXPERTISE,
            SPELL_ELIXIR_OF_PROTECTION,
            SPELL_ELIXIR_OF_MIGHTY_THOUGHTS,
            SPELL_GURUS_ELIXIR,
            SPELL_GIFT_OF_ARTHAS,
            SPELL_FLASK_OF_ENDLESS_RAGE,
            SPELL_FLASK_OF_STONEBLOOD,
            SPELL_FLASK_OF_DISTILLED_WISDOM,
            SPELL_FLASK_OF_FROST_WYRM,
        });
    }

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        Player* target = GetHitPlayer();
        if (!target)
            return;

        CastWorldBuffs(target);
        CastWellFedBuff(target);
        CastElixirs(target);
    }

    static void CastWorldBuffs(Player* player)
    {
        player->CastSpell(player, player->GetTeamId(true) == TEAM_HORDE ? SPELL_WARCHIEFS_BLESSING : SPELL_MIGHT_OF_STORMWIND, true);
        player->CastSpell(player, SPELL_RALLYING_CRY_OF_THE_DRAGONSLAYER, true);
        player->CastSpell(player, SPELL_SPIRIT_OF_ZANDALAR, true);
    }

    static void CastWellFedBuff(Player* player)
    {
        static std::map<uint8, uint32> const levelWellFedBuffMap =
        {
            { 1,    SPELL_WELL_FED_2_STAMINA_2_SPIRIT       },
            { 5,    SPELL_WELL_FED_4_STAMINA_4_SPIRIT       },
            { 12,   SPELL_WELL_FED_6_STAMINA_6_SPIRIT       },
            { 20,   SPELL_WELL_FED_8_STAMINA_8_SPIRIT       },
            { 35,   SPELL_WELL_FED_12_STAMINA_12_SPIRIT     },
            { 55,   SPELL_WELL_FED_30_STAMINA_20_SPIRIT     },
            { 70,   SPELL_WELL_FED_80_AP_40_SP_40_STAMINA   }
        };

        std::map<uint8, uint32>::const_iterator itr = levelWellFedBuffMap.upper_bound(player->GetLevel());
        if (itr == levelWellFedBuffMap.begin())
            return; // Shouldn't happen.

        --itr;
        player->CastSpell(player, itr->second, true);
    }

    static void CastElixirs(Player* player)
    {
        if (player->GetLevel() < DEFAULT_MAX_LEVEL)
            return;

        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                if (player->HasSpell(SPELL_SHOCKWAVE) || player->HasSpell(SPELL_DEVASTATE) || player->HasSpell(SPELL_VIGILANCE)) // Protection
                {
                    player->CastSpell(player, SPELL_ELIXIR_OF_EXPERTISE, true);
                    player->CastSpell(player, SPELL_ELIXIR_OF_PROTECTION, true);
                }
                else
                    player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);

                break;
            case CLASS_PALADIN:
                if (player->HasSpell(SPELL_HAMMER_OF_RIGHTEOUS) || player->HasSpell(SPELL_HOLY_SHIELD) || player->HasSpell(SPELL_BLESS_OF_SANC)) // Protection
                    player->CastSpell(player, SPELL_FLASK_OF_STONEBLOOD, true);
                else if (player->HasSpell(SPELL_BEACON_OF_LIGHT) || player->HasSpell(SPELL_HOLY_SHOCK) || player->HasSpell(PASSIVE_ILLUMINATION)) // Holy
                    player->CastSpell(player, SPELL_FLASK_OF_DISTILLED_WISDOM, true);
                else
                    player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);

                break;
            case CLASS_HUNTER:
            case CLASS_ROGUE:
                player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);
                break;
            case CLASS_PRIEST:
                if (player->HasSpell(SPELL_GUARDIAN_SPIRIT) || player->HasSpell(PASSIVE_SPIRIT_REDEMPTION) || player->HasSpell(SPELL_DESPERATE_PRAYER)) // Holy
                {
                    player->CastSpell(player, SPELL_ELIXIR_OF_MIGHTY_THOUGHTS, true);
                    player->CastSpell(player, SPELL_GURUS_ELIXIR, true);
                }
                else
                    player->CastSpell(player, SPELL_FLASK_OF_FROST_WYRM, true);
                break;
            case CLASS_DEATH_KNIGHT:
                if (player->HasSpell(SPELL_HEART_STRIKE) || player->HasSpell(SPELL_HYSTERIA) || player->HasSpell(SPELL_RUNE_TAP)) // Blood
                    player->CastSpell(player, SPELL_FLASK_OF_STONEBLOOD, true);
                else
                    player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);
                break;
            case CLASS_SHAMAN:
                if (player->HasSpell(SPELL_FERAL_SPIRIT) || player->HasSpell(SPELL_LAVA_LASH) || player->HasSpell(PASSIVE_SPIRIT_WEAPONS)) // Enhancement
                    player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);
                else
                    player->CastSpell(player, SPELL_FLASK_OF_FROST_WYRM, true);
                break;
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                player->CastSpell(player, SPELL_FLASK_OF_FROST_WYRM, true);
                break;
            case CLASS_DRUID:
                if (player->HasSpell(SPELL_BERSERK) || player->HasSpell(SPELL_MANGLE) || player->HasSpell(SPELL_SURVIVAL_INSTINCTS)) // Feral (Tank)
                {
                    if (player->GetShapeshiftForm() == FORM_BEAR || player->GetShapeshiftForm() == FORM_DIREBEAR)
                    {
                        player->CastSpell(player, SPELL_GURUS_ELIXIR, true);
                        player->CastSpell(player, SPELL_GIFT_OF_ARTHAS, true);
                    }
                    else
                        player->CastSpell(player, SPELL_FLASK_OF_ENDLESS_RAGE, true);
                }
                else
                    player->CastSpell(player, SPELL_FLASK_OF_FROST_WYRM, true);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_item_chronoboon::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_chronoboon()
{
    RegisterSpellScript(spell_item_chronoboon);
}
