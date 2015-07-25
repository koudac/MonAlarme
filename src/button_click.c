#include <pebble.h>

static Window *window;
static TextLayer *bp_activer;
static TextLayer *bp_passage;
static TextLayer *bp_desactiver;

static void passage_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text_color(bp_activer, GColorBlack); 
  text_layer_set_text(bp_activer, "Activer");
	text_layer_set_text_color(bp_desactiver, GColorBlack);
	text_layer_set_text(bp_desactiver, "Desactiver");	
  text_layer_set_text_color(bp_passage, GColorRed);
  text_layer_set_text(bp_passage, "-> Passage <-");
}

static void activer_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text_color(bp_passage, GColorBlack);
  text_layer_set_text(bp_passage, "Passage");
	text_layer_set_text_color(bp_desactiver, GColorBlack);
	text_layer_set_text(bp_desactiver, "Desactiver");	
  text_layer_set_text_color(bp_activer, GColorRed);
  text_layer_set_text(bp_activer, "-> Activer <-");
}

static void desactiver_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text_color(bp_desactiver, GColorBlack); 
  text_layer_set_text(bp_activer, "Activer");
	text_layer_set_text_color(bp_passage, GColorBlack);
	text_layer_set_text(bp_passage, "Passage");	
  text_layer_set_text_color(bp_desactiver, GColorRed);
  text_layer_set_text(bp_desactiver, "-> Desactiver <-");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, passage_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, activer_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, desactiver_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  bp_activer = text_layer_create((GRect) { .origin = { 0, 24 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_activer, "Activer");
  text_layer_set_text_alignment(bp_activer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_activer));
	
	bp_passage = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_passage, "Passage");
  text_layer_set_text_alignment(bp_passage, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_passage));
	
	bp_desactiver = text_layer_create((GRect) { .origin = { 0, 120 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_desactiver, "Desactiver");
  text_layer_set_text_alignment(bp_desactiver, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_desactiver));
}

static void window_unload(Window *window) {
  text_layer_destroy(bp_activer);
	text_layer_destroy(bp_passage);
	text_layer_destroy(bp_desactiver);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	  .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}