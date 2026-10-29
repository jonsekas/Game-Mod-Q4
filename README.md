# Twisted: a Quake 4 Mod

Twisted turns Quake 4 into a stamina-based melee and black-powder game. You start with a sword, earn XP by killing Strogg, and spend skill points across six skill trees to unlock new weapons, combat techniques, and passive bonuses. Quake 4's sci-fi guns are removed from your arsenal; every weapon you get comes from the skill tree.

---

## Installing

1. Install Quake 4 (Steam version tested).
2. Copy the `Twisted` folder into your Quake 4 install folder, next to `q4base`:
   ```
   Quake 4\
   ├── q4base\
   ├── Twisted\
   │   ├── game000.pk4   (mod code: Gamex86.dll)
   │   └── pak001.pk4    (all custom assets: defs, GUIs, sounds, effects, default binds)
   └── Quake4.exe
   ```
3. Copy `Launch Twisted.bat` into the Quake 4 install folder (next to `Quake4.exe`).

## Launching

- **Double-click `Launch Twisted.bat`** in the Quake 4 folder, or
- use the **Twisted** desktop shortcut, or
- run manually: `Quake4.exe +set fs_game Twisted`

Then start a **New Game** (saves from unmodded Quake 4 are not compatible).

**Press F1 at any time in game to open the Help screen.**

---

## Controls

| Key | Action |
|---|---|
| Mouse 1 | Attack / swing |
| Mouse 2 (hold) | Block (melee weapons) |
| Q | Parry |
| V | Evade (dash), requires the Evade skill |
| R | Reload |
| Shift | Sprint (uses stamina) |
| Space | Jump (uses stamina) |
| K | Open / close the Skill Tree |
| F1 | Open / close the Help screen |

| Key | Weapon | Key | Weapon |
|---|---|---|---|
| 1 | Sword (starting weapon) | 7 | Greatsword |
| 2 | Crossbow | 8 | Spear |
| 3 | Flintlock | 9 | Musket |
| 4 | Repeating Crossbow | 0 | Pilum |
| 5 | Dual Swords | - | Longsword |
| 6 | Dual Daggers | | |

Parry, Evade, Skill Tree and all weapon keys can be rebound in **Settings > Controls > Weapons**.

---

## Features

### Stamina
A stamina bar on the HUD. Sprinting drains it, and jumping, swinging, blocking, parrying and evading cost it. It regenerates after a short pause.

### Melee combat
- **Swords:** single, timed swings with a procedural slash animation. Each weapon has its own swing arc, speed, damage, reach and stamina cost.
- **Block (hold Mouse 2):** stops melee hits for a stamina cost.
- **Perfect Block:** start blocking just before a hit lands to take no cost and regain stamina.
- **Parry (Q):** a short timed window. A parried hit does no damage and your next swing deals 2x damage.
- **Dual weapons** (Dual Daggers, Dual Swords) show a weapon in each hand.

### Guns and throwables
- **Firearms:** Musket (1 powerful shot), Flintlock (6-shot pistol).
- **Throwables:** Crossbow (1 bolt), Repeating Crossbow (3 bolts), Pilum (a thrown iron rod). Bolts and pilums arc with gravity and stick in walls.
- **Timed reloads** with an on-screen reload bar. Click to cancel a reload.

### Progression
- Killing enemies gives XP. Each level gives 1 skill point. Each level needs more XP than the last.
- Press **K** to open the Skill Tree. Hover over a skill to read what it does, and click to unlock it. Each skill requires the one above it.
- **Reset Skills** refunds every point (and removes weapons granted by skills).

### The six skill trees

| Tree | Skills |
|---|---|
| **Prowess** | Parry, Fighter (+25 stamina), Block (Perfect Block), Riposte (longer parry, 2.5x riposte damage), Warrior (+25 stamina), Sidestep (better perfect blocks, shorter block cooldown) |
| **Fortitude** | Unburdened (+5% speed, cheaper evade), Brave (+25 health), Outlasting (25% less sprint drain), Vigor (+25 health) |
| **Light** | Greatsword, Blessed Powder (+25% firearm damage), Spear, The Light (+30% throwable damage), Paladin (+10% XP), The Manifesto (+25% more firearm damage) |
| **Dark** | Longsword, Dual Daggers, Heretic (+5% XP), Encore (10% faster swings), Impossibly Skilled, Dual Swords |
| **Ingenuity** | Crossbow, Musket, Pilum, Repeating Crossbow, Marksman (firearms 2x accurate standing still, 4x crouched), Flintlock |
| **Litheness** | Dexterity (30% faster reloads), Evade (unlocks the dash), Nimble (+5% speed), Swift (+5% speed) |

**Light and Dark are rival faiths:** owning any skill in one locks the other.

**Impossibly Skilled** (Dark capstone): dual weapons can block bullets, and a parry or perfect block sends bullets and projectiles back at the shooter.

---

## Seeing every feature quickly (for testing)

Open the console (`Ctrl + Alt + ~`) and type:

```
give xp 5000
```

This grants enough levels to unlock most of the skill tree. Then press **K** and unlock skills to test them. Remember Light and Dark cannot be mixed; use **Reset Skills** to switch faiths.

Suggested test path:
1. **Combat:** fight a Strogg with the Sword: block (Mouse 2), perfect block (block just before the hit), parry (Q) then swing for double damage.
2. **Stamina:** sprint and jump until the stamina bar empties.
3. **Ingenuity:** unlock the Crossbow and shoot a wall (the bolt sticks), then the Musket and Marksman (crouch for accuracy).
4. **Litheness:** unlock Evade and dash with V.
5. **Dark:** unlock Dual Daggers (two-handed model) and Impossibly Skilled, then parry enemy gunfire to reflect it.

---

## Source code

All gameplay code is in `game/`:

| File | Contents |
|---|---|
| `Player_Stamina.cpp` | stamina drain and regen, evade |
| `Player_Melee.cpp` | blocking and perfect blocks |
| `Player_SkillTree.cpp` | skill tree GUI, unlock/refund, XP |
| `PlayerSkills.cpp`, `SkillTree.h` | skill data table and rules |
| `Player_Prowess.cpp`, `Player_Fortitude.cpp`, `Player_Litheness.cpp`, `Player_Ingenuity.cpp`, `Player_Light.cpp`, `Player_Dark.cpp` | effects of each tree's skills |
| `Player_Help.cpp` | help screen |
| `Weapon_Reload.cpp` | timed reloads and the procedural swing motion |
| `weapon/WeaponSword.cpp` | all melee weapons, including dual-wield |

Mod data (defs and GUIs) is in `melee/`, `skilltree/`, `stamina/` and `controls/`.

## Known limitations
- Melee weapons and guns use existing Quake 4 models as placeholders.
- The Help screen does not open automatically; press F1.
