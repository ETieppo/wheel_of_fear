#pragma once

#include <vector>

#include "core/math/Vec2.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/tracker/GhostEchoProps.hpp"
#include "mods/tracker/SignalProps.hpp"

namespace wheel_of_fear {

class Tracker {
public:
    Tracker();

    bool isOpen() const { return is_open_; }
    float openAmount() const { return open_amount_; }
    void forceOpen(bool is_open) { is_open_ = is_open; }
    void close();
    void update(GameContextDto& context);
    void draw(GameContextDto& context) const;

private:
    static constexpr std::size_t signal_capacity = 48;
    static constexpr std::size_t ghost_capacity = 6;
    static constexpr float scan_range = 760;
    static constexpr float radar_radius = 215;
    static constexpr Vec2 radar_center{RenderConfig::ui_width / 2, RenderConfig::ui_height / 2 + 14};
    static constexpr float sweep_speed = 2.4F;
    static constexpr float opening_speed = 9;
    static constexpr float blip_fade_seconds = 1.6F;
    static constexpr float beep_noise_radius = 110;

    static bool crossedAngle(float previous, float current, float target);
    static Vec2 worldToRadar(Vec2 offset);
    void addSignal(const SignalProps& signal);
    void collectSignals(const GameContextDto& context);
    void updateGhosts(GameContextDto& context);
    void sweep(GameContextDto& context);
    void drawWallEchoes(GameContextDto& context) const;
    void drawSignal(GameContextDto& context, const SignalProps& signal, float last_ping) const;

    bool is_open_ = false;
    float open_amount_ = 0;
    float sweep_angle_ = 0;
    std::vector<SignalProps> signals_;
    std::vector<float> last_ping_seconds_;
    std::vector<GhostEchoProps> ghosts_;
};

}
