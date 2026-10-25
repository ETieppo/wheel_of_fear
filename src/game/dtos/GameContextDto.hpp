#pragma once

namespace wheel_of_fear {
  class GameClock;
  class InputController;
  class Renderer;
  class SoundPlayer;
  class ArtLibrary;
  class WorldSystem;
  class LightingSystem;
  class PlayerEntity;
  class MobSystem;
  class GeneratorSystem;
  class EffectSystem;
  class TrackerSystem;
  class HudSystem;
  class NoiseSystem;
  class ThrowableSystem;
  class DistractionSystem;
  class CameraSystem;
  class ExtraLifeSystem;
  class LetterSystem;

  struct GameContextDto {
      GameClock         &clock;
      InputController   &input;
      Renderer          &renderer;
      SoundPlayer       &sound_player;
      ArtLibrary        &art;
      WorldSystem       &world;
      LightingSystem    &lighting;
      PlayerEntity      &player;
      MobSystem         &mobs;
      GeneratorSystem   &generators;
      EffectSystem      &effects;
      TrackerSystem     &tracker;
      HudSystem         &hud;
      NoiseSystem       &noise;
      ThrowableSystem   &throwables;
      DistractionSystem &distractions;
      CameraSystem      &camera;
      ExtraLifeSystem   &extra_lives;
      LetterSystem      &letters;
  };
} // namespace wheel_of_fear
