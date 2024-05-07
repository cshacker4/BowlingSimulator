#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#define VELOCITY_ARROW 0
#define ROTATION_ARROW 1
#define ROLLING 2
#define LOWER_ARM 3
#define RESET 4
#define RAISE_ARM  5

class GameState{
	private:
		int game_state = VELOCITY_ARROW;
	public:
		void change_state(int state);
		int get_state();
};
#endif // GAME_STATE_HPP

