#include "../SlimRaster/app.h"
#include "../SlimRaster/core/time.h"
#include "../SlimRaster/viewport/viewport.h"
#include "../SlimRaster/viewport/navigation.h"
#include "../SlimRaster/viewport/manipulation.h"
#include "../SlimRaster/renderer/rasterizer.h"
// Or using the single-header file:
// #include "../SlimRaster.h"


void onMouseButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff = Vec2i(0, 0);
}
void onMouseDoubleClicked(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onMouseButtonDown(mouse_button);
    }
}
void onKeyChanged(u8 key, bool is_pressed) {
    if (!is_pressed) {
        Viewport *viewport = &app->viewport;
        ViewportSettings *settings = &viewport->settings;
        u8 tab = app->controls.key_map.tab;
        if (key == tab) settings->show_hud = !settings->show_hud;
        if (key == '1') {
            settings->show_wire_frame = !settings->show_wire_frame;
            setString(&viewport->hud.lines[1].value.string,settings->show_wire_frame ? (char*)"On" : (char*)"Off");
            viewport->hud.lines[1].value_color = settings->show_wire_frame ? White : Grey;
        }
        if (key == '2') {
            settings->antialias = !settings->antialias;
            setString(&viewport->hud.lines[2].value.string,settings->antialias ? (char*)"On" : (char*)"Off");
            viewport->hud.lines[2].value_color = settings->antialias ? White : Grey;
        }
    }

    NavigationMove *move = &app->viewport.navigation.move;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
}
void updateViewport(Viewport *viewport, Mouse *mouse) {
    if (mouse->is_captured) {
        navigateViewport(viewport, app->time.timers.update.delta_time);
        if (mouse->moved)         orientViewport(viewport, mouse);
        if (mouse->wheel_scrolled)  zoomViewport(viewport, mouse);
    } else {
        if (mouse->wheel_scrolled) dollyViewport(viewport, mouse);
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)  panViewport(viewport, mouse);
            if (mouse->right_button.is_pressed) orbitViewport(viewport, mouse);
        }
    }
}
void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Controls *controls = &app->controls;
    Viewport *viewport = &app->viewport;
    Mouse *mouse = &controls->mouse;
    Scene *scene = &app->scene;

    beginFrame(timer);
        if (!mouse->is_captured) manipulateSelection(scene, viewport, controls);
        if (!controls->is_pressed.alt) updateViewport(viewport, mouse);
        beginDrawing(viewport);
            rasterize(scene, viewport, &app->rasterizer);
            drawSelection(scene, viewport, controls);
            printNumberIntoString((i16)app->time.timers.update.average_frames_per_second, &viewport->hud.lines->value);
        endDrawing(viewport);
    endFrame(timer, mouse);
}
void setupViewport(Viewport *viewport) {
    xform3 *cam_xform = &viewport->camera->transform;
    cam_xform->position = Vec3(0, 15, -15);
    rotateXform3(cam_xform, 0, -0.25f, 0);

    i32 average_fps = app->time.timers.update.average_frames_per_second;

    HUDLine *fps = viewport->hud.lines;
    HUDLine *wireframe = fps + 1;
    HUDLine *msaa = wireframe + 1;
    HUDLine *mode = msaa + 1;
    wireframe->value_color = msaa->value_color = mode->value_color = Grey;

    printNumberIntoString(average_fps, &fps->value);

    setString(&fps->title, (char*)"Fps: ");
    setString(&wireframe->title, (char*)"Wireframe: ");
    setString(&wireframe->value.string, (char*)"Off");
    setString(&msaa->title, (char*)"MSAA: ");
    setString(&msaa->value.string, (char*)"Off");
    setString(&mode->title, (char*)"Mode: ");
    setString(&mode->value.string, (char*)"Beauty");
}
void setupScene(Scene *scene) {
    Material *floor_material  = scene->materials + 0;
    Material *monkey_material1 = scene->materials + 1;
    Material *monkey_material2 = scene->materials + 2;
    Material *monkey_material3 = scene->materials + 3;
    Primitive *floor   = scene->primitives + 0;
    Primitive *monkey1 = scene->primitives + 1;
    Primitive *monkey2 = scene->primitives + 2;
    Primitive *monkey3 = scene->primitives + 3;

    monkey_material1->pixel_shader = shadePixelPosition;
    monkey_material2->pixel_shader = shadePixelUV;
    monkey_material3->pixel_shader = shadePixelNormal;
    floor_material->pixel_shader = shadePixelCheckerboard;

    floor->type = PrimitiveType_Box;
    floor->scale    = Vec3(16, 1, 16);
    floor->position = Vec3(-6, -3, 0);
    floor->material_id = 0;

    monkey1->type = monkey2->type = monkey3->type = PrimitiveType_Mesh;
    monkey1->id = monkey2->id = monkey3->id = 0;
    monkey1->material_id = 1;
    monkey2->material_id = 2;
    monkey3->material_id = 3;
    monkey2->position = Vec3(2, 2, 3);
    monkey3->position = Vec3(-2, 2, -3);
    monkey1->rotation = getRotationAroundAxisBySinCon(Vec3(0, 1, 0), Vec2(sinf(0.5f), cosf(0.5f)));

    floor_material->texture_count = 2;
    floor_material->texture_ids[0] = 0;
    floor_material->texture_ids[1] = 1;
    floor_material->normal_magnitude = 0.4f;
}

void initApp(Defaults *defaults) {
    static char string_buffer[100];
    static String file;
    file.char_ptr = string_buffer;
    char* this_file   = __FILE__;
    char* monkey_mesh = "suzanne.mesh";
    u32 dir_len = getDirectoryLength(this_file);
    mergeString(&file, this_file, monkey_mesh, dir_len);
    defaults->settings.scene.meshes = 1;
    defaults->settings.scene.mesh_files = &file;
    defaults->settings.scene.primitives = 4;
    defaults->settings.scene.materials  = 4;
    defaults->settings.viewport.near_clipping_plane_distance = 1;
    defaults->settings.viewport.hud_line_count = 4;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
    app->on.windowRedraw  = updateAndRender;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onMouseButtonDown;
    app->on.mouseButtonDoubleClicked = onMouseDoubleClicked;
}