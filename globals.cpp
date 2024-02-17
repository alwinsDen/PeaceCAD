//
// Created by alwint on 16/2/24.
//

#include "globals.h"
bool draw_grid_flag = false;
//set the flag state
void set_grid_state(bool state){
    draw_grid_flag = state;
}
bool get_grid_state(){
    return draw_grid_flag;
}