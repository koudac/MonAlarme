#include <pebble.h>	
#define TAP_NOT_DATA true

static Window *window;
static TextLayer *bp_activer;
static TextLayer *bp_passage;
static TextLayer *bp_desactiver;
static TextLayer *s_debugtext;
	
enum AlarmeKey {
  ETAT_ALARME_KEY = 0x0,        // TUPLE_INT
	PRESSED_KEY = 0x1,						// TUPLE_INT	
	READ_STATE = 0x2,							// TUPLE_INT
	ACTION_MSG = 0x3,							// TUPLE_INT
	MSG_TYPE = 0x4								// TUPLE_INT
};

static AppSync s_sync;
static uint8_t s_sync_buffer[74];

static void data_handler(AccelData *data, uint32_t num_samples) {
  // Long lived buffer
  static char s_buffer[128];

  // Compose string of all data
  snprintf(s_buffer, sizeof(s_buffer), 
    "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
    data[0].x, data[0].y, data[0].z, 
    data[1].x, data[1].y, data[1].z, 
    data[2].x, data[2].y, data[2].z
  );

  //Show the data
  //console.log( s_buffer);
}

static void Send_Action_To_JS(int ActionID, int ActionValue){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }
	
	// Definit que le message a envoyer est une action
	int Msg_Type_Value = ACTION_MSG;
	
	// Ajout des valeurs de l'action a envoyer dans le dictionnaire
	dict_write_int(iter, MSG_TYPE, &Msg_Type_Value, sizeof(int), true);
  dict_write_int(iter, ActionID, &ActionValue, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	
	switch (key) {
    
    case ETAT_ALARME_KEY:
		  
		  if(strcmp(new_tuple->value->cstring,"1")==0){
				  text_layer_set_text_color(bp_passage, GColorBlack); 
  				text_layer_set_text(bp_passage, "Passage");
					text_layer_set_text_color(bp_desactiver, GColorBlack);
					text_layer_set_text(bp_desactiver, "Desactiver");	
  				text_layer_set_text_color(bp_activer, GColorRed);
  				text_layer_set_text(bp_activer, "-> Activer <-");
			}else if(strcmp(new_tuple->value->cstring,"2")==0){
				  text_layer_set_text_color(bp_activer, GColorBlack); 
  				text_layer_set_text(bp_activer, "Activer");
					text_layer_set_text_color(bp_desactiver, GColorBlack);
					text_layer_set_text(bp_desactiver, "Desactiver");	
  				text_layer_set_text_color(bp_passage, GColorRed);
  				text_layer_set_text(bp_passage, "-> Passage <-");
			}else if(strcmp(new_tuple->value->cstring,"3")==0){
				  text_layer_set_text_color(bp_passage, GColorBlack); 
  				text_layer_set_text(bp_passage, "Passage");
					text_layer_set_text_color(bp_activer, GColorBlack);
					text_layer_set_text(bp_activer, "Activer");	
  				text_layer_set_text_color(bp_desactiver, GColorRed);
  				text_layer_set_text(bp_desactiver, "-> Désactiver <-");
			}else {
					text_layer_set_text(s_debugtext, new_tuple->value->cstring);
			} 
		
      break;
		
		default:
				text_layer_set_text(bp_passage, "Erreur !!!");
			break;
  }
}

static void request_etat_alarme(void) {
	// Demande au JS un refresh de l'etat de l'alarme affiché
	Send_Action_To_JS(READ_STATE,0);
}

static void passage_click_handler(ClickRecognizerRef recognizer, void *context) {
	Send_Action_To_JS(PRESSED_KEY,3);
	request_etat_alarme();
}

static void activer_click_handler(ClickRecognizerRef recognizer, void *context) {
	Send_Action_To_JS(PRESSED_KEY,1);
	request_etat_alarme();
}

static void desactiver_click_handler(ClickRecognizerRef recognizer, void *context) {
	Send_Action_To_JS(PRESSED_KEY,2);
	request_etat_alarme();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, desactiver_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, activer_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, passage_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  bp_activer = text_layer_create((GRect) { .origin = { 0, 24 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_activer, "Activer");
  text_layer_set_text_alignment(bp_activer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_activer));
	
	bp_desactiver = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_desactiver, "Desactiver");
  text_layer_set_text_alignment(bp_desactiver, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_desactiver));
	
	bp_passage = text_layer_create((GRect) { .origin = { 0, 120 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(bp_passage, "Passage");
  text_layer_set_text_alignment(bp_passage, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bp_passage));
	
	s_debugtext = text_layer_create((GRect) { .origin = { 0, 144 }, .size = { bounds.size.w, 200 } });
  text_layer_set_text(s_debugtext, "---");
  text_layer_set_text_alignment(s_debugtext, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_debugtext));
	
	Tuplet initial_values[] = {
    TupletInteger(ETAT_ALARME_KEY, (int16_t) -1),
  };
	
	app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
  				    								sync_tuple_changed_callback, sync_error_callback, NULL );
}

static void window_unload(Window *window) {
  text_layer_destroy(bp_activer);
	text_layer_destroy(bp_passage);
	text_layer_destroy(bp_desactiver);
	text_layer_destroy(s_debugtext);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
      text_layer_set_text(s_debugtext, "X axis positive.");
    } else {
      text_layer_set_text(s_debugtext, "X axis negative.");
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
      text_layer_set_text(s_debugtext, "Y axis positive.");
    } else {
      text_layer_set_text(s_debugtext, "Y axis negative.");
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
      text_layer_set_text(s_debugtext, "Z axis positive.");
    } else {
      text_layer_set_text(s_debugtext, "Z axis negative.");
    }
    break;
  }
	
	// Mise a jour de l'etat de l'alarme affiché
	//request_etat_alarme();
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
	
	// Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler);
  } else {
    // Subscribe to the accelerometer data service
    int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  }
	
	app_message_open(64, 64);
}

static void deinit(void) {
  window_destroy(window);
	
	if (TAP_NOT_DATA) {
    accel_tap_service_unsubscribe();
  } else {
    accel_data_service_unsubscribe();
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}