pub const SpriteStripProps = struct {
    bytes: []const u8,
    frame_count: u32,
    frame_width: u32,
    frame_height: u32,
    pixels_per_unit: f32 = pixels_per_unit,
};

pub const TextureFileProps = struct {
    bytes: []const u8,
    width: u32,
    height: u32,
};

pub const pixels_per_unit: f32 = 8;

pub const generated = @import("generated.zig");

pub const sprites = struct {
    pub const barrel = SpriteStripProps{ .bytes = @embedFile("sprites/barrel.png"), .frame_count = 1, .frame_width = 96, .frame_height = 96 };
    pub const crate = SpriteStripProps{ .bytes = @embedFile("sprites/crate.png"), .frame_count = 1, .frame_width = 112, .frame_height = 112 };
    pub const decoy_can = SpriteStripProps{ .bytes = @embedFile("sprites/decoy_can.png"), .frame_count = 1, .frame_width = 48, .frame_height = 48 };
    pub const door_frame = SpriteStripProps{ .bytes = @embedFile("sprites/door_frame.png"), .frame_count = 1, .frame_width = 288, .frame_height = 192 };
    pub const door_left = SpriteStripProps{ .bytes = @embedFile("sprites/door_left.png"), .frame_count = 1, .frame_width = 128, .frame_height = 160 };
    pub const door_right = SpriteStripProps{ .bytes = @embedFile("sprites/door_right.png"), .frame_count = 1, .frame_width = 128, .frame_height = 160 };
    pub const eye_monster = SpriteStripProps{ .bytes = @embedFile("sprites/eye_monster.png"), .frame_count = 8, .frame_width = 192, .frame_height = 192 };
    pub const generator = SpriteStripProps{ .bytes = @embedFile("sprites/generator.png"), .frame_count = 1, .frame_width = 208, .frame_height = 192 };
    pub const generator_gear = SpriteStripProps{ .bytes = @embedFile("sprites/generator_gear.png"), .frame_count = 1, .frame_width = 104, .frame_height = 104 };
    pub const icon_battery = SpriteStripProps{ .bytes = @embedFile("sprites/icon_battery.png"), .frame_count = 1, .frame_width = 80, .frame_height = 80 };
    pub const icon_fuse = SpriteStripProps{ .bytes = @embedFile("sprites/icon_fuse.png"), .frame_count = 1, .frame_width = 80, .frame_height = 80 };
    pub const icon_gear = SpriteStripProps{ .bytes = @embedFile("sprites/icon_gear.png"), .frame_count = 1, .frame_width = 80, .frame_height = 80 };
    pub const icon_valve = SpriteStripProps{ .bytes = @embedFile("sprites/icon_valve.png"), .frame_count = 1, .frame_width = 80, .frame_height = 80 };
    pub const junction_box = SpriteStripProps{ .bytes = @embedFile("sprites/junction_box.png"), .frame_count = 1, .frame_width = 96, .frame_height = 96 };
    pub const locker_closed = SpriteStripProps{ .bytes = @embedFile("sprites/locker_closed.png"), .frame_count = 1, .frame_width = 96, .frame_height = 160 };
    pub const locker_open = SpriteStripProps{ .bytes = @embedFile("sprites/locker_open.png"), .frame_count = 1, .frame_width = 96, .frame_height = 160 };
    pub const machine = SpriteStripProps{ .bytes = @embedFile("sprites/machine.png"), .frame_count = 1, .frame_width = 160, .frame_height = 176 };
    pub const neon_sign_0 = SpriteStripProps{ .bytes = @embedFile("sprites/neon_sign_0.png"), .frame_count = 1, .frame_width = 128, .frame_height = 80 };
    pub const neon_sign_1 = SpriteStripProps{ .bytes = @embedFile("sprites/neon_sign_1.png"), .frame_count = 1, .frame_width = 128, .frame_height = 80 };
    pub const neon_sign_2 = SpriteStripProps{ .bytes = @embedFile("sprites/neon_sign_2.png"), .frame_count = 1, .frame_width = 128, .frame_height = 80 };
    pub const neon_tube_core = SpriteStripProps{ .bytes = @embedFile("sprites/neon_tube_core.png"), .frame_count = 1, .frame_width = 144, .frame_height = 48 };
    pub const neon_tube_housing = SpriteStripProps{ .bytes = @embedFile("sprites/neon_tube_housing.png"), .frame_count = 1, .frame_width = 144, .frame_height = 48 };
    pub const particle_circle = SpriteStripProps{ .bytes = @embedFile("sprites/particle_circle.png"), .frame_count = 1, .frame_width = 128, .frame_height = 128 };
    pub const particle_dirt = SpriteStripProps{ .bytes = @embedFile("sprites/particle_dirt.png"), .frame_count = 3, .frame_width = 128, .frame_height = 128 };
    pub const particle_flare = SpriteStripProps{ .bytes = @embedFile("sprites/particle_flare.png"), .frame_count = 1, .frame_width = 128, .frame_height = 128 };
    pub const particle_light = SpriteStripProps{ .bytes = @embedFile("sprites/particle_light.png"), .frame_count = 3, .frame_width = 128, .frame_height = 128 };
    pub const particle_magic = SpriteStripProps{ .bytes = @embedFile("sprites/particle_magic.png"), .frame_count = 5, .frame_width = 128, .frame_height = 128 };
    pub const particle_scorch = SpriteStripProps{ .bytes = @embedFile("sprites/particle_scorch.png"), .frame_count = 3, .frame_width = 128, .frame_height = 128 };
    pub const particle_smoke = SpriteStripProps{ .bytes = @embedFile("sprites/particle_smoke.png"), .frame_count = 5, .frame_width = 128, .frame_height = 128 };
    pub const particle_spark = SpriteStripProps{ .bytes = @embedFile("sprites/particle_spark.png"), .frame_count = 7, .frame_width = 128, .frame_height = 128 };
    pub const particle_twirl = SpriteStripProps{ .bytes = @embedFile("sprites/particle_twirl.png"), .frame_count = 1, .frame_width = 128, .frame_height = 128 };
    pub const pipes = SpriteStripProps{ .bytes = @embedFile("sprites/pipes.png"), .frame_count = 1, .frame_width = 128, .frame_height = 128 };
    pub const player_feet = SpriteStripProps{ .bytes = @embedFile("sprites/player_feet.png"), .frame_count = 20, .frame_width = 208, .frame_height = 162 };
    pub const player_hug = SpriteStripProps{ .bytes = @embedFile("sprites/player_hug.png"), .frame_count = 15, .frame_width = 208, .frame_height = 162 };
    pub const player_idle = SpriteStripProps{ .bytes = @embedFile("sprites/player_idle.png"), .frame_count = 20, .frame_width = 208, .frame_height = 162 };
    pub const player_move = SpriteStripProps{ .bytes = @embedFile("sprites/player_move.png"), .frame_count = 20, .frame_width = 208, .frame_height = 162 };
    pub const teeth_monster = SpriteStripProps{ .bytes = @embedFile("sprites/teeth_monster.png"), .frame_count = 8, .frame_width = 192, .frame_height = 192 };
    pub const vent = SpriteStripProps{ .bytes = @embedFile("sprites/vent.png"), .frame_count = 1, .frame_width = 112, .frame_height = 80 };
    pub const zombie_attack = SpriteStripProps{ .bytes = @embedFile("sprites/zombie_attack.png"), .frame_count = 9, .frame_width = 176, .frame_height = 188 };
    pub const zombie_move = SpriteStripProps{ .bytes = @embedFile("sprites/zombie_move.png"), .frame_count = 17, .frame_width = 176, .frame_height = 188 };
};

pub const textures = struct {
    pub const face_color = TextureFileProps{ .bytes = @embedFile("textures/face_color.png"), .width = 1024, .height = 1024 };
    pub const face_normal = TextureFileProps{ .bytes = @embedFile("textures/face_normal.png"), .width = 1024, .height = 1024 };
    pub const floor_color = TextureFileProps{ .bytes = @embedFile("textures/floor_color.png"), .width = 1024, .height = 1024 };
    pub const floor_normal = TextureFileProps{ .bytes = @embedFile("textures/floor_normal.png"), .width = 1024, .height = 1024 };
    pub const wall_color = TextureFileProps{ .bytes = @embedFile("textures/wall_color.png"), .width = 1024, .height = 1024 };
    pub const wall_normal = TextureFileProps{ .bytes = @embedFile("textures/wall_normal.png"), .width = 1024, .height = 1024 };
};

pub const sounds = struct {
    pub const acid_sizzle = [_][]const u8{ @embedFile("audio/sfx/acid_sizzle_0.opus"), @embedFile("audio/sfx/acid_sizzle_1.opus") };
    pub const bloater_gurgle = [_][]const u8{ @embedFile("audio/sfx/bloater_gurgle_0.opus"), @embedFile("audio/sfx/bloater_gurgle_1.opus"), @embedFile("audio/sfx/bloater_gurgle_2.opus"), @embedFile("audio/sfx/bloater_gurgle_3.opus"), @embedFile("audio/sfx/bloater_gurgle_4.opus"), @embedFile("audio/sfx/bloater_gurgle_5.opus") };
    pub const breathing = [_][]const u8{ @embedFile("audio/sfx/breathing_0.opus"), @embedFile("audio/sfx/breathing_1.opus"), @embedFile("audio/sfx/breathing_2.opus") };
    pub const crumble = [_][]const u8{ @embedFile("audio/sfx/crumble_0.opus"), @embedFile("audio/sfx/crumble_1.opus"), @embedFile("audio/sfx/crumble_2.opus"), @embedFile("audio/sfx/crumble_3.opus"), @embedFile("audio/sfx/crumble_4.opus") };
    pub const decoy_impact = [_][]const u8{ @embedFile("audio/sfx/decoy_impact_0.opus"), @embedFile("audio/sfx/decoy_impact_1.opus"), @embedFile("audio/sfx/decoy_impact_2.opus"), @embedFile("audio/sfx/decoy_impact_3.opus"), @embedFile("audio/sfx/decoy_impact_4.opus") };
    pub const decoy_throw = [_][]const u8{ @embedFile("audio/sfx/decoy_throw_0.opus"), @embedFile("audio/sfx/decoy_throw_1.opus") };
    pub const distant_scream = [_][]const u8{ @embedFile("audio/sfx/distant_scream_0.opus"), @embedFile("audio/sfx/distant_scream_1.opus"), @embedFile("audio/sfx/distant_scream_2.opus"), @embedFile("audio/sfx/distant_scream_3.opus"), @embedFile("audio/sfx/distant_scream_4.opus"), @embedFile("audio/sfx/distant_scream_5.opus"), @embedFile("audio/sfx/distant_scream_6.opus") };
    pub const door_open = [_][]const u8{ @embedFile("audio/sfx/door_open_0.opus"), @embedFile("audio/sfx/door_open_1.opus"), @embedFile("audio/sfx/door_open_2.opus") };
    pub const electric_buzz = [_][]const u8{ @embedFile("audio/sfx/electric_buzz_0.opus"), @embedFile("audio/sfx/electric_buzz_1.opus"), @embedFile("audio/sfx/electric_buzz_2.opus"), @embedFile("audio/sfx/electric_buzz_3.opus") };
    pub const explosion = [_][]const u8{ @embedFile("audio/sfx/explosion_0.opus"), @embedFile("audio/sfx/explosion_1.opus"), @embedFile("audio/sfx/explosion_2.opus"), @embedFile("audio/sfx/explosion_3.opus"), @embedFile("audio/sfx/explosion_4.opus") };
    pub const footstep = [_][]const u8{ @embedFile("audio/sfx/footstep_0.opus"), @embedFile("audio/sfx/footstep_1.opus"), @embedFile("audio/sfx/footstep_2.opus"), @embedFile("audio/sfx/footstep_3.opus"), @embedFile("audio/sfx/footstep_4.opus") };
    pub const generator_breakdown = [_][]const u8{ @embedFile("audio/sfx/generator_breakdown_0.opus"), @embedFile("audio/sfx/generator_breakdown_1.opus"), @embedFile("audio/sfx/generator_breakdown_2.opus"), @embedFile("audio/sfx/generator_breakdown_3.opus"), @embedFile("audio/sfx/generator_breakdown_4.opus") };
    pub const generator_online = [_][]const u8{ @embedFile("audio/sfx/generator_online_0.opus"), @embedFile("audio/sfx/generator_online_1.opus"), @embedFile("audio/sfx/generator_online_2.opus") };
    pub const generator_request = [_][]const u8{ @embedFile("audio/sfx/generator_request_0.opus"), @embedFile("audio/sfx/generator_request_1.opus"), @embedFile("audio/sfx/generator_request_2.opus"), @embedFile("audio/sfx/generator_request_3.opus") };
    pub const generator_tick = [_][]const u8{ @embedFile("audio/sfx/generator_tick_0.opus"), @embedFile("audio/sfx/generator_tick_1.opus"), @embedFile("audio/sfx/generator_tick_2.opus"), @embedFile("audio/sfx/generator_tick_3.opus"), @embedFile("audio/sfx/generator_tick_4.opus") };
    pub const glass_shatter = [_][]const u8{ @embedFile("audio/sfx/glass_shatter_0.opus"), @embedFile("audio/sfx/glass_shatter_1.opus"), @embedFile("audio/sfx/glass_shatter_2.opus"), @embedFile("audio/sfx/glass_shatter_3.opus"), @embedFile("audio/sfx/glass_shatter_4.opus") };
    pub const heartbeat = [_][]const u8{@embedFile("audio/sfx/heartbeat_0.opus")};
    pub const heavy_footstep = [_][]const u8{ @embedFile("audio/sfx/heavy_footstep_0.opus"), @embedFile("audio/sfx/heavy_footstep_1.opus"), @embedFile("audio/sfx/heavy_footstep_2.opus"), @embedFile("audio/sfx/heavy_footstep_3.opus"), @embedFile("audio/sfx/heavy_footstep_4.opus") };
    pub const jaws = [_][]const u8{ @embedFile("audio/sfx/jaws_0.opus"), @embedFile("audio/sfx/jaws_1.opus"), @embedFile("audio/sfx/jaws_2.opus"), @embedFile("audio/sfx/jaws_3.opus"), @embedFile("audio/sfx/jaws_4.opus") };
    pub const locker_door = [_][]const u8{ @embedFile("audio/sfx/locker_door_0.opus"), @embedFile("audio/sfx/locker_door_1.opus"), @embedFile("audio/sfx/locker_door_2.opus"), @embedFile("audio/sfx/locker_door_3.opus") };
    pub const lure_whistle = [_][]const u8{ @embedFile("audio/sfx/lure_whistle_0.opus"), @embedFile("audio/sfx/lure_whistle_1.opus"), @embedFile("audio/sfx/lure_whistle_2.opus"), @embedFile("audio/sfx/lure_whistle_3.opus") };
    pub const machine_work = [_][]const u8{ @embedFile("audio/sfx/machine_work_0.opus"), @embedFile("audio/sfx/machine_work_1.opus"), @embedFile("audio/sfx/machine_work_2.opus") };
    pub const maze_grind = [_][]const u8{ @embedFile("audio/sfx/maze_grind_0.opus"), @embedFile("audio/sfx/maze_grind_1.opus"), @embedFile("audio/sfx/maze_grind_2.opus") };
    pub const metal_clang = [_][]const u8{ @embedFile("audio/sfx/metal_clang_0.opus"), @embedFile("audio/sfx/metal_clang_1.opus"), @embedFile("audio/sfx/metal_clang_2.opus"), @embedFile("audio/sfx/metal_clang_3.opus"), @embedFile("audio/sfx/metal_clang_4.opus") };
    pub const mob_burn = [_][]const u8{ @embedFile("audio/sfx/mob_burn_0.opus"), @embedFile("audio/sfx/mob_burn_1.opus"), @embedFile("audio/sfx/mob_burn_2.opus") };
    pub const moth_flutter = [_][]const u8{ @embedFile("audio/sfx/moth_flutter_0.opus"), @embedFile("audio/sfx/moth_flutter_1.opus"), @embedFile("audio/sfx/moth_flutter_2.opus"), @embedFile("audio/sfx/moth_flutter_3.opus") };
    pub const neon_pop = [_][]const u8{ @embedFile("audio/sfx/neon_pop_0.opus"), @embedFile("audio/sfx/neon_pop_1.opus"), @embedFile("audio/sfx/neon_pop_2.opus"), @embedFile("audio/sfx/neon_pop_3.opus"), @embedFile("audio/sfx/neon_pop_4.opus") };
    pub const part_pickup = [_][]const u8{ @embedFile("audio/sfx/part_pickup_0.opus"), @embedFile("audio/sfx/part_pickup_1.opus"), @embedFile("audio/sfx/part_pickup_2.opus"), @embedFile("audio/sfx/part_pickup_3.opus") };
    pub const phase_start = [_][]const u8{@embedFile("audio/sfx/phase_start_0.opus")};
    pub const player_death = [_][]const u8{@embedFile("audio/sfx/player_death_0.opus")};
    pub const player_hurt = [_][]const u8{ @embedFile("audio/sfx/player_hurt_0.opus"), @embedFile("audio/sfx/player_hurt_1.opus"), @embedFile("audio/sfx/player_hurt_2.opus"), @embedFile("audio/sfx/player_hurt_3.opus"), @embedFile("audio/sfx/player_hurt_4.opus") };
    pub const reinforcement_whistle = [_][]const u8{@embedFile("audio/sfx/reinforcement_whistle_0.opus")};
    pub const shepherd_call = [_][]const u8{@embedFile("audio/sfx/shepherd_call_0.opus")};
    pub const skitter = [_][]const u8{ @embedFile("audio/sfx/skitter_0.opus"), @embedFile("audio/sfx/skitter_1.opus"), @embedFile("audio/sfx/skitter_2.opus"), @embedFile("audio/sfx/skitter_3.opus"), @embedFile("audio/sfx/skitter_4.opus") };
    pub const switch_click = [_][]const u8{ @embedFile("audio/sfx/switch_click_0.opus"), @embedFile("audio/sfx/switch_click_1.opus"), @embedFile("audio/sfx/switch_click_2.opus"), @embedFile("audio/sfx/switch_click_3.opus"), @embedFile("audio/sfx/switch_click_4.opus") };
    pub const tracker_blip = [_][]const u8{ @embedFile("audio/sfx/tracker_blip_0.opus"), @embedFile("audio/sfx/tracker_blip_1.opus"), @embedFile("audio/sfx/tracker_blip_2.opus"), @embedFile("audio/sfx/tracker_blip_3.opus") };
    pub const tracker_close = [_][]const u8{ @embedFile("audio/sfx/tracker_close_0.opus"), @embedFile("audio/sfx/tracker_close_1.opus"), @embedFile("audio/sfx/tracker_close_2.opus"), @embedFile("audio/sfx/tracker_close_3.opus") };
    pub const tracker_open = [_][]const u8{ @embedFile("audio/sfx/tracker_open_0.opus"), @embedFile("audio/sfx/tracker_open_1.opus"), @embedFile("audio/sfx/tracker_open_2.opus"), @embedFile("audio/sfx/tracker_open_3.opus") };
    pub const whisper = [_][]const u8{ @embedFile("audio/sfx/whisper_0.opus"), @embedFile("audio/sfx/whisper_1.opus") };
    pub const zombie_groan = [_][]const u8{ @embedFile("audio/sfx/zombie_groan_0.opus"), @embedFile("audio/sfx/zombie_groan_1.opus"), @embedFile("audio/sfx/zombie_groan_2.opus"), @embedFile("audio/sfx/zombie_groan_3.opus"), @embedFile("audio/sfx/zombie_groan_4.opus") };
};

pub const music = struct {
    pub const ambience = @embedFile("audio/music/ambience.opus");
    pub const tension = @embedFile("audio/music/tension.opus");
    pub const title = @embedFile("audio/music/title.opus");
};

pub const fonts = struct {
    pub const body_font = @embedFile("fonts/body_font.ttf");
    pub const display_font = @embedFile("fonts/display_font.ttf");
};
