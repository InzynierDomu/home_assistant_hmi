#include "ui.h"

// Placeholder — zastąp plikami wyeksportowanymi z SquareLine Studio
// (Project Export -> src/*.c i include/*.h, target: PlatformIO / Arduino).
void ui_init(void)
{
  lv_obj_t* scr = lv_scr_act();
  lv_obj_t* label = lv_label_create(scr);
  lv_label_set_text(label, "Home Assistant HMI");
  lv_obj_center(label);
}
