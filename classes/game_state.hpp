#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#define ADJUST_ARROW 0
#define ROLLING 1
#define LOWER_ARM 2
#define RESET 3
#define RAISE_ARM  4

class GameState{
	private:
		int game_state = ADJUST_ARROW;
	public:
		void change_state(int state);
		int get_state();
};
#endif // GAME_STATE_HPP

