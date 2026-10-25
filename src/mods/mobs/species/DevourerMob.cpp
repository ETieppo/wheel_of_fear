#include "mods/mobs/species/DevourerMob.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void DevourerMob::think(GameContextDto &context) {
    if(!canAct()) return;

    const std::optional<Vec2Props> tool
      = context.throwables.nearestTool(position(), tool_scent_range);

    if(!tool) {
      brain().hunt(context);
      return;
    }

    huntTool(context, *tool);
  }

  void DevourerMob::huntTool(
    GameContextDto &context,
    Vec2Props       tool
  ) {
    if(distanceTo(tool) <= bite_reach) {
      if(context.throwables.devourAt(context.world, tool, bite_reach))
        voice().vocalize(context, 1.0F);
      return;
    }

    const bool is_reachable
      = locomotion().navigateTo(context, tool, locomotion().currentSpeed());

    if(is_reachable) brain().changeIntent(MobIntentEnum::investigating, tool);
    else brain().hunt(context);
  }

  void DevourerMob::onContact(GameContextDto &context) {
    brain().strikeAndRetreat(context, strike_retreat_seconds);
  }

  void DevourerMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t       frame
      = animationFrame(isMoving() ? 9.0F : 4.0F, sprites.devourer_chew.size());

    appearance().drawBody(context, sprites.devourer_chew[frame], MobLookDto {});
  }
} // namespace wheel_of_fear
