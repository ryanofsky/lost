MYOBJECTS = ry_alphabeta.o ry_card.o ry_cardstacks.o ry_game.o ry_gameconstants.o ry_main.o ry_turn.o
MYEXEC = ry_main
COMP_OPTIONS = -O3 -DNDEBUG
LINK_OPTIONS = 

$(MYEXEC): $(MYOBJECTS)
	g++ $(LINK_OPTIONS) -o $(MYEXEC) $(MYOBJECTS)

ry_alphabeta.o: ry_alphabeta.cpp ry_alphabeta.h ry_turn.h ry_card.h ry_game.h ry_cardstacks.h ry_gameconstants.h
	g++ $(COMP_OPTIONS) -c -o ry_alphabeta.o ry_alphabeta.cpp

ry_card.o: ry_card.cpp ry_card.h ry_gameconstants.h ry_help.h
	g++ $(COMP_OPTIONS) -c -o ry_card.o ry_card.cpp

ry_cardstacks.o: ry_cardstacks.cpp ry_cardstacks.h ry_gameconstants.h ry_card.h ry_help.h
	g++ $(COMP_OPTIONS) -c -o ry_cardstacks.o ry_cardstacks.cpp

ry_game.o: ry_game.cpp ry_game.h ry_card.h ry_turn.h ry_cardstacks.h ry_gameconstants.h ry_help.h
	g++ $(COMP_OPTIONS) -c -o ry_game.o ry_game.cpp
	
ry_gameconstants.o: ry_gameconstants.cpp ry_gameconstants.h ry_card.h
	g++ $(COMP_OPTIONS) -c -o ry_gameconstants.o ry_gameconstants.cpp

ry_main.o: ry_main.cpp ry_game.h ry_card.h ry_turn.h ry_cardstacks.h ry_gameconstants.h ry_alphabeta.h
	g++ $(COMP_OPTIONS) -c -o ry_main.o ry_main.cpp

ry_turn.o: ry_turn.cpp ry_turn.h ry_card.h ry_gameconstants.h
	g++ $(COMP_OPTIONS) -c -o ry_turn.o ry_turn.cpp

clean:
	@-rm $(MYEXEC) $(MYOBJECTS)