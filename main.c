#include <pebble.h>

Window *window;
TextLayer *text_layer;

void display(char *text)
{
    text_layer_set_text(text_layer, text);
}

Tuple *tuple[100];

char *text[100];

int writecounter = -1;
int readcounter = -1;
int current = 0;

static void in_received_handler(DictionaryIterator *received, void *context) 
{
  writecounter++;
	tuple[writecounter] = dict_find(received, 0);  
  text[writecounter] = malloc(tuple[writecounter] -> length);
  memcpy(text[writecounter], (char*)tuple[writecounter] -> value -> cstring, tuple[writecounter] -> length);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%i", tuple[writecounter] -> length);
}

static void in_dropped_handler(AppMessageResult reason, void *context) 
{	
  text_layer_set_text(text_layer, "Dropped");
}

void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
    display(text[++readcounter]);
}

void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if(readcounter - 1 >= 0)
      display(text[--readcounter]);
    else
      text_layer_set_text(text_layer, "Nothing before this :\(");
}

void click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void send_message(void)
{
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_int8(iter, 0, 0);
	dict_write_end(iter);
  app_message_outbox_send();
}


void handle_init(void)
{
	  window = window_create();
	  text_layer = text_layer_create(GRect(0, 0, 144, 154));
    window_set_fullscreen(window, true);
    text_layer_set_text(text_layer, "Upload the slides while on this screen; press up to begin");
	  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
    window_stack_push(window, true);
    app_message_register_inbox_received(in_received_handler); 
	  app_message_register_inbox_dropped(in_dropped_handler); 
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    window_set_click_config_provider(window, click_config_provider);
}

void handle_deinit(void)
{
	  text_layer_destroy(text_layer);
    app_message_deregister_callbacks();
	  window_destroy(window);
}

int main(void)
{
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}