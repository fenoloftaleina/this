void handle_input(const sapp_event* ev, const float t)
{
#ifdef GUI
  simgui_handle_event(ev);
#endif

  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
      case SAPP_KEYCODE_Q:
        sapp_quit();
        break;

      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:
        in_data.v = IN_UP;
        break;

      case SAPP_KEYCODE_S:
      case SAPP_KEYCODE_DOWN:
        in_data.v = IN_DOWN;
        break;

      case SAPP_KEYCODE_A:
      case SAPP_KEYCODE_LEFT:
        in_data.h = IN_LEFT;
        break;

      case SAPP_KEYCODE_D:
      case SAPP_KEYCODE_RIGHT:
        in_data.h = IN_RIGHT;
        break;

      default:
        break;
    }
  } else if (ev->type == SAPP_EVENTTYPE_KEY_UP) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:
        if (in_data.v == IN_UP) {
          in_data.v = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_S:
      case SAPP_KEYCODE_DOWN:
        if (in_data.v == IN_DOWN) {
          in_data.v = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_A:
      case SAPP_KEYCODE_LEFT:
        if (in_data.h == IN_LEFT) {
          in_data.h = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_D:
      case SAPP_KEYCODE_RIGHT:
        if (in_data.h == IN_RIGHT) {
          in_data.h = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_H:
        if (in_data.editor) {
          in_data.editor = false;
        } else {
          reload_current_map();
          reset_editor();
          in_data.editor = true;
        }
        break;

      case SAPP_KEYCODE_R:
        reload_current_map();
        break;

      case SAPP_KEYCODE_U:
        undo(t);
        break;

      case SAPP_KEYCODE_J:
        if (in_data.editor) {
          next_spot_type();
        }
        break;

      case SAPP_KEYCODE_N:
        if (in_data.editor) {
          clear_spot();
        }
        break;

      case SAPP_KEYCODE_G:
        if (in_data.editor) {
          start_or_add_to_path();
        }
        break;

      case SAPP_KEYCODE_F:
        if (in_data.editor) {
          finish_editing_path();
        }
        break;

      case SAPP_KEYCODE_L:
        if (in_data.editor) {
          toggle_path_looped();
        }
        break;

      case SAPP_KEYCODE_Y:
        if (in_data.editor) {
          toggle_path_self_only();
        }
        break;

      case SAPP_KEYCODE_T:
        if (in_data.editor) {
          inc_path_step();
        }
        break;

      case SAPP_KEYCODE_O:
        if (in_data.editor) {
          set_player_start_position();
        }
        break;

      case SAPP_KEYCODE_P:
        if (in_data.editor) {
          save_current_map();
        }
        break;

      case SAPP_KEYCODE_C:
        if (in_data.editor) {
          duplicate_current_map();
        }
        break;

      case SAPP_KEYCODE_V:
        run_map(cur_map_i - 1);
        break;

      case SAPP_KEYCODE_B:
        run_map(cur_map_i + 1);
        break;

      default:
        break;
    }
  }
}
