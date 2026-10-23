#pragma once

namespace wheel_of_fear {
  class GameClock;
  class Input;
  class Renderer;
  class SoundPlayer;
  class Art;
  class World;
  class Lighting;
  class Player;
  class Mobs;
  class Generators;
  class Effects;
  class Tracker;
  class Hud;
  class Noise;
  class Throwables;
  class Distractions;
  class Camera;

  struct GameContextDto {
      GameClock    &clock;
      Input        &input;
      Renderer     &renderer;
      SoundPlayer  &sound_player;
      Art          &art;
      World        &world;
      Lighting     &lighting;
      Player       &player;
      Mobs         &mobs;
      Generators   &generators;
      Effects      &effects;
      Tracker      &tracker;
      Hud          &hud;
      Noise        &noise;
      Throwables   &throwables;
      Distractions &distractions;
      Camera       &camera;
  };
} // namespace wheel_of_fear
