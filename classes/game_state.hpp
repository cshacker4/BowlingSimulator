//Macros
#define ADJUST_ARROW 0
#define ROLLING 1
#define ROLL_COMPLETE  2
#define LOWER_ARM 3
#define RAISE_ARM  4
#define RESET  5

class GameState{
	private:
		int game_state = ADJUST_ARROW;
	public:
		void change_state(int state);
		int get_state();
};

