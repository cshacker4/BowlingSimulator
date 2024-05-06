#include "game_state.hpp"

void GameState::change_state(int state){
	game_state = state;
}
int GameState::get_state(){
	return game_state;
}
