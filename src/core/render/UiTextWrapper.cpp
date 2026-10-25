#include "core/render/UiTextWrapper.hpp"

namespace wheel_of_fear {
  std::vector<std::string_view> UiTextWrapper::wrap(
    const Renderer  &renderer,
    std::string_view text,
    float            size,
    float            maximum_width
  ) {
    std::vector<std::string_view> lines;
    std::size_t                   line_start = 0;
    std::size_t                   last_break = std::string_view::npos;
    std::size_t                   cursor     = 0;

    while(cursor <= text.size()) {
      const bool is_end   = cursor == text.size();
      const bool is_space = !is_end && text[cursor] == ' ';

      if(is_end || is_space) {
        const std::string_view candidate
          = text.substr(line_start, cursor - line_start);

        if(
          renderer.measureText(candidate, size)
          > maximum_width
          && last_break
          != std::string_view::npos
        ) {
          lines.push_back(text.substr(line_start, last_break - line_start));
          line_start = last_break + 1;
          last_break = std::string_view::npos;
          continue;
        }

        if(is_end) {
          if(cursor > line_start) lines.push_back(candidate);
          break;
        }

        last_break = cursor;
      }

      cursor += 1;
    }

    return lines;
  }
} // namespace wheel_of_fear
