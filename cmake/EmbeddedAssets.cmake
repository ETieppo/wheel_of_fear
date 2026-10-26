function(wheel_of_fear_add_embedded_assets target asset_directory shader_directory)
    file(GLOB_RECURSE asset_files CONFIGURE_DEPENDS RELATIVE "${asset_directory}" "${asset_directory}/*")
    file(GLOB_RECURSE shader_files CONFIGURE_DEPENDS RELATIVE "${shader_directory}" "${shader_directory}/*")

    set(asset_keys "")
    foreach(asset_file IN LISTS asset_files)
        list(APPEND asset_keys "assets/${asset_file}")
    endforeach()
    foreach(shader_file IN LISTS shader_files)
        list(APPEND asset_keys "shaders/${shader_file}")
    endforeach()
    list(SORT asset_keys)

    set(array_definitions "")
    set(catalog_entries "")
    set(asset_index 0)
    foreach(asset_key IN LISTS asset_keys)
        if(asset_key MATCHES "^assets/(.*)$")
            set(source_path "${asset_directory}/${CMAKE_MATCH_1}")
        else()
            string(REGEX REPLACE "^shaders/" "" shader_file "${asset_key}")
            set(source_path "${shader_directory}/${shader_file}")
        endif()

        string(APPEND array_definitions
            "    const std::uint8_t embedded_asset_${asset_index}[] = {\n"
            "#embed \"${source_path}\"\n"
            "    };\n"
        )
        string(APPEND catalog_entries
            "      {.path = \"${asset_key}\", .bytes = embedded_asset_${asset_index}},\n"
        )
        math(EXPR asset_index "${asset_index} + 1")
    endforeach()

    list(LENGTH asset_keys asset_count)
    string(CONCAT generated_source
        "#include \"core/assets/EmbeddedAssetCatalog.hpp\"\n"
        "#include <array>\n"
        "\n"
        "namespace wheel_of_fear {\n"
        "  namespace {\n"
        "${array_definitions}"
        "\n"
        "    const std::array<EmbeddedAssetProps, ${asset_count}> embedded_assets {{\n"
        "${catalog_entries}"
        "    }};\n"
        "  }\n"
        "\n"
        "  std::span<const EmbeddedAssetProps> EmbeddedAssetCatalog::entries() {\n"
        "    return embedded_assets;\n"
        "  }\n"
        "}\n"
    )

    set(generated_directory "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(generated_path "${generated_directory}/EmbeddedAssetEntries.cpp")
    file(WRITE "${generated_path}.pending" "${generated_source}")
    file(COPY_FILE "${generated_path}.pending" "${generated_path}" ONLY_IF_DIFFERENT)
    file(REMOVE "${generated_path}.pending")

    set_source_files_properties("${generated_path}" PROPERTIES COMPILE_OPTIONS
        "$<$<CXX_COMPILER_ID:GNU>:-Wno-c++26-extensions>;$<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-c23-extensions>"
    )
    target_sources(${target} PRIVATE "${generated_path}")
endfunction()
