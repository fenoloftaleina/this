static char cur_map_name[255];

void run_map(const char* map_name)
{
  strcpy(cur_map_name, map_name);
  load_map(cur_map_name);
  reload_logic();
}


void reload_current_map()
{
  load_map(cur_map_name);
  reload_logic();
}


void save_current_map()
{
  save_map(cur_map_name);
}


void handle_input(const sapp_event* ev)
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
          in_data.editor = true;
        }
        break;

      case SAPP_KEYCODE_R:
        reload_current_map();
        break;

      case SAPP_KEYCODE_J:
        next_spot_type();
        break;

      case SAPP_KEYCODE_N:
        clear_spot();
        break;

      case SAPP_KEYCODE_P:
        save_current_map();
        break;

      default:
        break;
    }
  }
}
