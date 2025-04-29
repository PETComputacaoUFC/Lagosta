#include <cctype>
#include "gui/gui.hpp"
// #include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "rapidcsv.h"
#include "pugixml.hpp"
#include <iostream>

int main() {
    SetTraceLogLevel(LOG_WARNING);
    int screenWidth = 1280;
    int screenHeight = 720;
    // do not set the FLAG_WINDOW_HIGHDPI flag, that scales a low res framebuffer up to the native
    // resolution. use the native resolution and scale your geometry.
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Lagosta");
    SetTargetFPS(144);
    SetExitKey(KEY_NULL);

    Reader r{.data_table = rapidcsv::Document("resources/data/participantes_teste.csv",
                                              rapidcsv::LabelParams(0, 0))};
    //


    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("resources/SVGgabarito_automatico.svg");

    if (!result) {
        std::cerr << "XML parsed with errors: " << result.description() << std::endl;
        return 1;
    }

    // Specify the id you are looking for
    const char* target_id = "image1";

    // Find the node with the specific id
    pugi::xml_node node = doc.find_node([&](const pugi::xml_node& n) {
        return std::string(n.attribute("id").value()) == target_id;
    });

    if (node) {
        std::cout << "Node found: " << node.child("tspan").child_value() << std::endl;

        std::string new_base64_data = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAAAAACIM/FCAAADBElEQVR4XmP8zzA8ABPDqEcGVwiMxshgS5GjMTIaIzQKgdGkNZq0RpMW/hAYzSODLY+woDqIkQj3wdvLcMWYInBj8CgmLygwLYWYM5q0Rovf0eJ3tPhlGFJgtNQa5BUiZkUGF4HXRLiqJGSvEVNpYg5E4amOCY5ajSat0QpxtEIcrRBHK8QBCYFhU/yi9RCJmS0hqdNH4fQL8XaNVoijFeJohThaIY5WiKMVIiUhwIJDM2Y3ELNqwzNSisdJeOo48gyE2DVaIY5WiKMV4miFOFohjlaIlIQAwRV0FM4qwt1GzEArMWpweXa0QhytEEcrxNEKcbRCHK0QqVghEjOLx0jHEB8dMmUYsmC0iTLaRBltoow2UYZWAcZCtHMpWfGC3xJMk8lYkzNa/I4Wv6PF72jxO7SK39FSa7DFFxmLavDUjCR1Hslbb4NL12jSGq0QRyvE0QpxtEIckBAYrqtM8WwthNdExCx9wVPZkVdpEpxeHK0QRyvE0QpxtEIcrRBHK0RKQoCMHiJJ1pG0ToakBahode5ohThaIY5WiKMV4miFOFohUrFChBtFTFeRGHtJGhelxNLRCnG0QhytEEcrxNEKcbRCpEWFiFmRETNSilmfkuQ2SgZjRyvE0QpxtEIcrRBHK8TRCpEWFSIxnTU8q0PJcxJJy03RrBitEEcrxNEKcbRCHK0QRytESkKAkqttGAc08Y3OIQ7yvD/aRBltoow2UUabKEOriYKrQqRwDhHPWhqq9yshAT5a/I4Wv6PF72jxO9pDHJAQGDbFLxk9RPIGNvHUsPAYJGmzxmgPcZDn/dEmymgTZbSJMtpEGVpNFOIP9ybGX5Tsm0A2H08Hc3Rj/lBJYKMV4miFOFohjlaIw6NCxPQFMV08YtQQYzJcDWZvFFddOVr8jha/o8XvaPE7tIrf0VJrsMUXJYtqSPILnhU4mJ0+MsZXR5PWaIU4WiGOVoijFeKAhMBInkMcnElutEIcrRBHK8TRCnG0QhytECkJgdF6ZLQeGa1HRuuRoVWPAABy7WCN6KIXawAAAABJRU5ErkJggg==";

        // Change the value of the node
        node.attribute("xlink:href").set_value(new_base64_data);

        // Save the modified document back to a file
        if (doc.save_file("modified_file.svg")) {
            std::cout << "File saved successfully." << std::endl;
        } else {
            std::cerr << "Failed to save the file." << std::endl;
        }
    } else {
        std::cout << "Node with id '" << target_id << "' not found." << std::endl;
    }

    // rlImGuiSetup(true);
    // UIReader reader = UIReader();
    // reader.reader = r;
    // UIWindow ui = UIWindow();
    // ui.add_tab(&reader);

    // while (!WindowShouldClose()) {
    //     BeginDrawing();
    //     rlImGuiBegin();

    //     ui.draw();
    //     // ImGui::ShowDemoWindow();

    //     rlImGuiEnd();
    //     EndDrawing();
    // }

    // // delete reader;
    // rlImGuiShutdown();
    CloseWindow();
    return 0;
}